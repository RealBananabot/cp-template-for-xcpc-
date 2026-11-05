#include "../pollard_rho.cpp"
#include "../primitive_root_pollard_rho.cpp"

int main(){
	static_assert(is_same_v<decltype(primitive_root(i64(2))), i64>);
	int small_cases = 0, reference_cases = 0;
	// 仅在小规模中数单位元并直接枚举乘法阶，不使用存在性分类或分解 phi。
	auto brute = [](int d){
		if(d <= 1){
			return -1;
		}
		int phi = 0;
		rep(g, 1, d - 1){
			phi += (gcd(g, d) == 1);
		}
		rep(g, 1, d - 1){
			if(gcd(g, d) != 1){
				continue;
			}
			int r = 1, order = 0;
			do{
				r = int(i64(r) * g % d);
				order++;
			} while(r != 1);
			if(order == phi){
				return g;
			}
		}
		return -1;
	};
	rep(d, -10, 1000){
		assert(primitive_root(d) == brute(d));
		small_cases++;
	}
	mt19937 rng(6091);
	rep(tc, 1, 200){
		int d = int(rng() % 1500) + 1;
		assert(primitive_root(d) == brute(d));
		small_cases++;
	}
	assert(primitive_root(LLONG_MIN) == -1);
	assert(primitive_root(LLONG_MAX) == -1);
	assert(primitive_root(2) == 1);
	assert(primitive_root(4) == 3);
	assert(primitive_root(6) == 5);
	assert(primitive_root(9) == 2);
	assert(primitive_root(18) == 5);
	assert(primitive_root(40487) == 5);
	assert(primitive_root(40487LL * 40487) == 10); // 最小原根不能直接从 p 搬到 p^2。
	assert(primitive_root(2 * 40487LL * 40487) == 13); // 还须在原模数下检验最小值。
	assert(primitive_root(45) == -1);
	assert(primitive_root(1LL << 62) == -1);
	assert(primitive_root(1000000007LL * 1000000009) == -1);
	int cases;
	assert(bool(cin >> cases));
	vector<pair<i64, i64>> references;
	rep(i, 1, cases){
		i64 d, expected;
		assert(bool(cin >> d >> expected));
		assert(primitive_root(d) == expected);
		references.pb(d, expected);
		reference_cases++;
	}
	// 重置随机种子影响分解路径，不应影响存在性和最小值；调用不修改输入。
	for(int seed:vi{0, 1, 2, 3663, 20261105}){
		PR::rnd.seed(seed);
		for(i64 d:vector<i64>{18, 40487LL * 40487, 4052555153018976267LL,
			9223372028264841218LL, LLONG_MAX - 24, LLONG_MAX}){
			auto it = find_if(references.begin(), references.end(), [&](const auto &v){
				return v.fi == d;
			});
			assert(it != references.end());
			i64 before = d;
			assert(primitive_root(d) == it->se);
			assert(d == before);
		}
	}
	// P6091 的规模：原题还要输出所有原根，此接口只计算最小值。
	for(i64 d:vector<i64>{999983, 1000000, 999966, 531441, 781250,
		823543, 524288, 720720, 998244, 999999}){
		auto it = find_if(references.begin(), references.end(), [&](const auto &v){
			return v.fi == d;
		});
		assert(it != references.end());
		assert(primitive_root(d) == it->se);
	}
	cout << "passed " << small_cases << " independent multiplicative-order cases, "
		<< reference_cases << " certified Python cases, five seeds, i64 boundaries and P6091-scale inputs\n";
}
