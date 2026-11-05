#include "../miller_rabin_int.cpp"
#include "../pollard_rho.cpp"
#include "../exgcd.cpp"
#include "../excrt.cpp"
#include "../lucas.cpp"
#include "../exlucas.cpp"

int main(){
	const int N = 1000000;
	vector<bool> composite(N + 1);
	composite[0] = composite[1] = true;
	rep(p, 2, 1000){
		if(!composite[p]){
			for(int x = p * p; x <= N; x += p){
				composite[x] = true;
			}
		}
	}
	rep(x, 0, N){
		assert(PR::isprime(x) == !composite[x]);
		assert(PR::isprime(i64(x)) == !composite[x]);
	}
	const i64 L = i64(INT_MAX) - 99999;
	vector<bool> segment(100000);
	rep(p, 2, 46340){
		if(!composite[p]){
			i64 first = max(i64(p) * p, (L + p - 1) / p * p);
			for(i64 x = first; x <= INT_MAX; x += p){
				segment[int(x - L)] = true;
			}
		}
	}
	rep(i, 0, 99999){
		int x = int(L + i);
		assert(PR::isprime(x) == !segment[i]);
		assert(PR::isprime(i64(x)) == !segment[i]);
	}
	for(int x:vi{INT_MIN, INT_MIN + 1, -100, -1}){
		assert(!PR::isprime(x) && !PR::isprime(i64(x)));
	}
	rep(mod, 2, 40) rep(x, 0, mod - 1){
		int expected = 1;
		rep(y, 0, 30){
			assert(PR::qpow(x, y, mod) == expected);
			assert(PR::qpow(i64(x), i64(y), i64(mod)) == expected);
			expected = expected * x % mod;
		}
	}
	int cases;
	assert(bool(cin >> cases));
	vector<pair<i64, bool>> large;
	rep(tc, 1, cases){
		i64 x;
		bool expected;
		assert(bool(cin >> x >> expected));
		assert(PR::isprime(x) == expected);
		if(INT_MIN <= x && x <= INT_MAX){
			assert(PR::isprime(int(x)) == expected);
		}
		if(100000000000000000LL <= x && x <= 1000000000000000000LL){
			large.pb(x, expected);
		}
	}
	assert(!empty(large));
	rep(i, 0, 99999){
		auto [x, expected] = large[i % size(large)];
		assert(PR::isprime(x) == expected);
	}
	int powers;
	assert(bool(cin >> powers));
	rep(tc, 1, powers){
		i64 x, y, mod, expected;
		assert(bool(cin >> x >> y >> mod >> expected));
		assert(PR::qpow(x, y, mod) == expected);
		if(mod <= INT_MAX && y <= INT_MAX){
			assert(PR::qpow(int(x), int(y), int(mod)) == expected);
		}
	}
	assert(inv(3, 11) == 4);
	assert((crt_merge({6, 4}, {8, 4}) == pair<i64, i64>{24, 4}));
	assert(Lucas(97).solve(200, 100) == exlucas(200, 100, 97));
	cout << "passed " << N + 1 << " sieve values, 100000 INT_MAX-segment values, "
		<< cases << " certified primality cases, " << powers
		<< " Python powers, 100000 QOJ-range large calls, small powers and integration checks\n";
}
