#include "../exgcd.cpp"
#include "../excrt.cpp"
#include "../lucas.cpp"

int main(){
	int small_cases = 0, digit_cases = 0, exact_cases = 0, parity_cases = 0;
	vi primes;
	rep(p, 2, 100){
		bool prime = true;
		for(int d = 2; d * d <= p; d++){
			if(p % d == 0){
				prime = false;
			}
		}
		if(prime){
			primes.pb(p);
		}
	}
	primes.pb(99991); // P3807 范围内最大的质数。
	mt19937_64 rng(3807);
	uniform_int_distribution<i64> nonnegative(0, LLONG_MAX);
	for(int p:primes){
		Lucas l(p);
		vector<vi> choose(201, vi(201));
		choose[0][0] = 1;
		rep(m, 1, 200){
			choose[m][0] = choose[m][m] = 1;
			rep(n, 1, m - 1){
				choose[m][n] = int((i64(choose[m - 1][n - 1]) + choose[m - 1][n]) % p);
			}
		}
		rep(m, 0, 200) rep(n, -1, m + 1){
			int expected = 0;
			if(0 <= n && n <= m){
				expected = choose[m][n];
			}
			assert(l.solve(m, n) == expected);
			small_cases++;
			if(m < p){
				assert(l.C(m, n) == expected);
				digit_cases++;
			}
		}
		rep(i, 0, p - 1){
			assert(i64(l.fac[i]) * l.ifac[i] % p == 1);
		}
		assert(l.solve(LLONG_MAX, 0) == 1);
		assert(l.solve(LLONG_MAX, LLONG_MAX) == 1);
		assert(l.solve(LLONG_MAX, 1) == LLONG_MAX % p);
		assert(l.solve(LLONG_MAX, LLONG_MAX - 1) == LLONG_MAX % p);
		assert(l.solve(0, 1) == 0 && l.solve(0, LLONG_MIN) == 0);
		for(i64 power = p;;){
			assert(l.solve(power, 1) == 0);
			assert(l.solve(power, power - 1) == 0);
			assert(l.solve(power, power / 2) == 0);
			if(power > LLONG_MAX / p){
				break;
			}
			power *= p;
		}
	}
	Lucas two(2), three(3);
	rep(tc, 1, 10000){
		i64 m = nonnegative(rng), n = nonnegative(rng);
		int expected = (n <= m && (m & n) == n ? 1 : 0);
		assert(two.solve(m, n) == expected);
		parity_cases++;
	}
	assert(two.solve(5, 2) == 0 && three.solve(5, 2) == 1);
	three = Lucas(7);
	assert(three.solve(10, 3) == 1 && two.solve(5, 2) == 0);

	// 以素因子指数计算精确组合数的模值，独立于 Lucas 和阶乘逆元。
	vi all_primes;
	vector<bool> composite(200001);
	rep(i, 2, 200000){
		if(!composite[i]){
			all_primes.pb(i);
			if(i64(i) * i <= 200000){
				for(int j = i * i; j <= 200000; j += i){
					composite[j] = true;
				}
			}
		}
	}
	auto valuation = [](int n, int q){
		int res = 0;
		while(n){
			n /= q;
			res += n;
		}
		return res;
	};
	rep(tc, 1, 10){
		int n = (tc == 1 ? 100000 : int(rng() % 100000) + 1);
		int m = (tc == 1 ? 100000 : int(rng() % 100000) + 1);
		const int p = 99991;
		Lucas l(p);
		int expected = 1;
		for(int q:all_primes){
			if(q > n + m){
				break;
			}
			int e = valuation(n + m, q) - valuation(n, q) - valuation(m, q);
			rep(i, 1, e){
				expected = int(i64(expected) * q % p);
			}
		}
		assert(l.solve(i64(n) + m, n) == expected);
	}
	assert(inv(3, 11) == 4 && safe_inv(-3, 11) == 7);
	assert((crt_merge({6, 4}, {8, 4}) == pair<i64, i64>{24, 4}));
	// 由 run.py 的 Python math.comb 生成任意精度参考值。
	int reference_count;
	assert(bool(cin >> reference_count));
	optional<Lucas> current;
	rep(tc, 1, reference_count){
		i64 m, n;
		int p, expected;
		assert(bool(cin >> m >> n >> p >> expected));
		if(!current || current->p != p){
			current.emplace(p);
		}
		assert(current->solve(m, n) == expected);
		exact_cases++;
	}
	cout << "passed " << small_cases << " Pascal cases, " << digit_cases
		<< " digit combinations, " << exact_cases << " large exact cases, "
		<< parity_cases << " parity cases, boundaries and 10 P3807-scale cases\n";
}
