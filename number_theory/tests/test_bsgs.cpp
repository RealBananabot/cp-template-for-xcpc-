#include "../exgcd.cpp"
#include "../excrt.cpp"
#include "../lucas.cpp"
#include "../exlucas.cpp"
#include "../miller_rabin_int.cpp"
#include "../pollard_rho.cpp"
#include "../bsgs.cpp"

int main(){
	static_assert(is_same_v<decltype(bsgs(2, 7, 4)), int>);
	i64 exhaustive = 0;
	int random_cases = 0, large_cases = 0;
	// 独立枚举完整幂序列，第一次重复后不再有新余数；记录每个余数首次出现的位置。
	auto orbit = [](int a, int p){
		vi first(p, -1);
		for(int r = 1 % p, x = 0; first[r] == -1; x++){
			first[r] = x;
			r = int(i64(r) * a % p);
		}
		return first;
	};
	rep(p, 1, 120) rep(a, 0, p - 1){
		auto first = orbit(a, p);
		rep(b, 0, p - 1){
			assert(bsgs(a, p, b) == first[b]);
			exhaustive++;
		}
	}
	mt19937 rng(4195);
	rep(tc, 1, 3000){
		int p = int(rng() % 1000) + 1;
		int a = int(rng() & INT_MAX), b = int(rng() & INT_MAX);
		auto first = orbit(a, p);
		assert(bsgs(a, p, b) == first[b % p]);
		random_cases++;
	}
	assert(bsgs(0, 1, INT_MAX) == 0);
	assert(bsgs(0, INT_MAX, 1) == 0);
	assert(bsgs(0, INT_MAX, 0) == 1);
	assert(bsgs(0, INT_MAX, 2) == -1);
	assert(bsgs(1, INT_MAX, 2) == -1);
	assert(bsgs(2, 8, 0) == 3);
	assert(bsgs(2, 8, 6) == -1);
	assert(bsgs(2, 22, 10) == 5); // 巨步的初值系数不可省略。
	assert(bsgs(2, 257, 255) == 9); // 重复键若保留最小 j，会错过最小解。
	assert(bsgs(INT_MAX, INT_MAX, INT_MAX) == 1);
	// 已知精确阶的底数：以重复乘法生成目标，指数严格小于阶，故已知最小解。
	auto powers = [&](int a, int p, const vi &exponents){
		int r = 1;
		size_t j = 0;
		rep(x, 0, exponents.back()){
			if(x == exponents[j]){
				assert(bsgs(a, p, r) == x);
				large_cases++;
				if(++j == size(exponents)){
					break;
				}
			}
			r = int(i64(r) * a % p);
		}
	};
	// 3、5 模 2^k 的阶均为 2^(k-2)，k>=3。
	powers(5, 1 << 30, {31, 32768, 200000});
	powers(3, 1 << 30, {32, 32769, 200001});
	powers(5, 998244353, {30, 31600, 200000}); // 下方独立核对这些指数范围内尚未重复。
	{
		unordered_set<int> seen;
		int r = 1;
		rep(x, 0, 200000){
			assert(seen.insert(r).second);
			r = int(i64(r) * 5 % 998244353);
		}
	}
	// 非互质的大模数：2 模 2^30 的尾部和零；a=2^29+1 的阶为 2。
	assert(bsgs(2, 1 << 30, 1 << 29) == 29);
	assert(bsgs(2, 1 << 30, 0) == 30);
	assert(bsgs(2, 1 << 30, 3) == -1);
	assert(bsgs((1 << 29) + 1, 1 << 30, (1 << 29) + 1) == 1);
	// 满额覆盖题目 sum(sqrt(p)) 量级，使用大模数下具有直接证明的短周期。
	rep(i, 1, 158){
		assert(bsgs(1000000000 - 1, 1000000000, 2) == -1);
		large_cases++;
	}
	// int 上界附近的乘法、巨步指数以及可约去大量因子的模数。
	assert(bsgs(INT_MAX - 1, INT_MAX, 2) == -1);
	assert(bsgs(2, INT_MAX, 1 << 30) == 30);
	assert(bsgs(2, 2147483646, 4) == 2);
	assert(bsgs(2, 2000000000, 3) == -1);
	// INT_MAX 为质数，p-1=2*3^2*7*11*31*151*331；逐项检查 7 的阶为 p-1。
	for(int q:vi{2, 3, 7, 11, 31, 151, 331}){
		assert(PR::qpow(i64(7), i64(INT_MAX - 1) / q, i64(INT_MAX)) != 1);
	}
	assert(i64(7) * 1840700269 % INT_MAX == 1);
	assert(bsgs(7, INT_MAX, 1840700269) == INT_MAX - 2);
	// 第一个匹配出现在最后一个巨步，i*B 超过 INT_MAX；必须先用 i64 计算。
	large_cases++;
	cout << "passed " << exhaustive << " exhaustive and " << random_cases
		<< " random orbit cases, " << large_cases << " large cases, boundaries and minimum-solution checks\n";
}
