#include "../exgcd.cpp"
#include "../excrt.cpp"
#include "../lucas.cpp"
#include "../exlucas.cpp"

int main(){
	int small_cases = 0, valuation_cases = 0, exact_cases = 0;
	rep(mod, 1, 128){
		vector<vi> choose(51, vi(51));
		choose[0][0] = 1 % mod;
		rep(m, 1, 50){
			choose[m][0] = choose[m][m] = 1 % mod;
			rep(n, 1, m - 1){
				choose[m][n] = (choose[m - 1][n - 1] + choose[m - 1][n]) % mod;
			}
		}
		rep(m, 0, 50) rep(n, -1, m + 1){
			int expected = (0 <= n && n <= m ? choose[m][n] : 0);
			assert(exlucas(m, n, mod) == expected);
			small_cases++;
		}
	}
	assert(exlucas(8, 4, 8) == 6);
	assert(exlucas(0, 0, 8) == 1 && exlucas(0, 1, 8) == 0);
	assert(exlucas(LLONG_MAX, LLONG_MIN, 8) == 0);
	assert(exlucas(LLONG_MAX, LLONG_MAX, 1000000) == 1);
	assert(exlucas(LLONG_MAX, 1, 1000000) == LLONG_MAX % 1000000);
	assert(exlucas(LLONG_MAX, LLONG_MAX - 1, 1000000) == LLONG_MAX % 1000000);
	mt19937_64 rng(4720);
	uniform_int_distribution<i64> nonnegative(0, LLONG_MAX);
	rep(tc, 1, 300){
		i64 m = nonnegative(rng);
		i64 n = uniform_int_distribution<i64>(0, m)(rng);
		int a = int(rng() % 19) + 1, mod = 1 << a;
		int e = __builtin_popcountll(n) + __builtin_popcountll(m - n) - __builtin_popcountll(m);
		int r = exlucas(m, n, mod);
		assert(r == exlucas(m, m - n, mod));
		if(e >= a){
			assert(r == 0);
		} else{
			assert(r % (1 << e) == 0 && r % (1 << (e + 1)) != 0);
		}
		valuation_cases++;
	}
	Lucas l(97);
	assert(exlucas(200, 100, 97) == l.solve(200, 100));
	assert(inv(3, 8) == 3 && safe_inv(2, 8) == -1);
	assert((crt_merge({8, 6}, {9, 7}) == pair<i64, i64>{72, 70}));

	// Python math.comb 的任意精度参考值，由 run.py 提供。
	int reference_count;
	assert(bool(cin >> reference_count));
	rep(tc, 1, reference_count){
		i64 m, n;
		int mod, expected;
		assert(bool(cin >> m >> n >> mod >> expected));
		assert(exlucas(m, n, mod) == expected);
		exact_cases++;
	}
	cout << "passed " << small_cases << " Pascal cases, " << valuation_cases
		<< " large binary valuation cases, " << exact_cases
		<< " Python exact cases, the C(8,4) mod 8 regression and boundaries\n";
}
