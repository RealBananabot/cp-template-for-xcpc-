// 仅实际 s.cpp 与本模板，确认没有素性测试、分解或原根依赖。
#include "../mod_sqrt.cpp"

int main(){
	using i128 = __int128;
	static_assert(is_same_v<decltype(mod_sqrt(i64(1), i64(5))), i64>);
	int exhaustive = 0, reference_cases = 0, probability_cases = 0;
	// 第一次调用刻意使首个随机候选 w=0，必须重试而非当作非剩余。
	bool retry_checked = false;
	for(i64 p:vector<i64>{5, 13, 17, 97}){
		mt19937_64 probe(3663);
		i64 a = i64(probe() % p);
		if(a != 0){
			assert(mod_sqrt(a * a % p, p) == min(a, p - a));
			retry_checked = true;
			break;
		}
	}
	assert(retry_checked);
	// 独立试除确认小素数，枚举每个 x 的平方，记录每个余数的最小根。
	rep(p, 2, 1000){
		bool prime = true;
		for(int d = 2; d * d <= p; d++){
			if(p % d == 0){
				prime = false;
				break;
			}
		}
		if(!prime){
			continue;
		}
		vi expected(p, -1);
		rep(x, 0, p - 1){
			int n = x * x % p;
			if(expected[n] == -1){
				expected[n] = x;
			}
		}
		rep(n, 0, p - 1){
			for(i64 a:vector<i64>{n, n - p, n + p}){
				assert(mod_sqrt(a, p) == expected[n]);
				exhaustive++;
			}
			if(p <= 101 && p % 4 == 1 && n != 0 && expected[n] != -1){
				// 精确计数全部 2^64 个输出经 %p 后，成功候选所占的份额。
				i128 N = i128(1) << 64, success = 0;
				rep(a, 0, p - 1){
					int w = (a * a - n + p) % p;
					if(expected[w] == -1){
						success += N / p + (a < N % p);
					}
				}
				assert(4 * success >= N);
				probability_cases++;
			}
		}
	}
	assert(mod_sqrt(LLONG_MIN, 2) == 0);
	assert(mod_sqrt(LLONG_MAX, 2) == 1);
	assert(mod_sqrt(-1, 2) == 1);
	assert(mod_sqrt(2, 7) == 3);
	assert(mod_sqrt(3, 7) == -1);
	assert(mod_sqrt(10, 13) == 6);
	int cases;
	assert(bool(cin >> cases));
	vector<array<i64, 3>> references;
	rep(i, 1, cases){
		i64 n, p, expected;
		assert(bool(cin >> n >> p >> expected));
		i64 r = mod_sqrt(n, p);
		assert(r == expected);
		if(r != -1){
			i64 residue = n % p;
			if(residue < 0){
				residue += p;
			}
			assert(0 <= r && r <= p / 2);
			assert(i128(r) * r % p == residue);
		}
		references.pb(array<i64, 3>{n, p, expected});
		reference_cases++;
	}
	// 每次重新构造引擎；逆序重复调用仍须返回同一最小值。
	reverse(references.begin(), references.end());
	for(const auto &[n, p, expected]:references){
		assert(mod_sqrt(n, p) == expected);
	}
	cout << "passed " << exhaustive << " exhaustive square-table cases, "
		<< reference_cases << " certified Python Tonelli-Shanks cases, reverse-order reuse, zero-discriminant retry and i64 boundaries\n";
	cout << "passed " << probability_cases << " exact modulo-sampling probability checks\n";
}
