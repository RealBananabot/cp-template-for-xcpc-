#include "../exgcd.cpp"

int main(){
	int bezout_cases = 0, inverse_cases = 0, unchecked_cases = 0;
	auto check_exgcd = [&](int a, int b){
		int x = INT_MIN, y = INT_MAX;
		int g = exgcd(a, b, x, y);
		assert(g == gcd(a, b));
		assert(i64(a) * x + i64(b) * y == g);
		if(a == 0 && b == 0){
			assert(x == 1 && y == 0);
		}
		bezout_cases++;
	};
	auto check_inv = [&](int x, int p){
		int res = safe_inv(x, p);
		i64 a = (i64(x) % p + p) % p;
		if(gcd(a, i64(p)) != 1){
			assert(res == -1);
		} else{
			assert(0 <= res && res < p);
			assert(a * res % p == 1 % p);
			if(x >= 0){
				assert(inv(x, p) == res);
				unchecked_cases++;
			}
		}
		inverse_cases++;
	};

	rep(a, 0, 500) rep(b, 0, 500) check_exgcd(a, b);
	rep(p, 1, 128) rep(x, -256, 256){
		int expected = -1;
		rep(y, 0, p - 1){
			if((i64(x) * y % p + p) % p == 1 % p){
				expected = y;
				break;
			}
		}
		assert(safe_inv(x, p) == expected);
		check_inv(x, p);
	}

	vi edge{0, 1, 2, 3, INT_MAX / 2, INT_MAX - 2, INT_MAX - 1, INT_MAX};
	for(int a:edge) for(int b:edge) check_exgcd(a, b);
	vi signed_edge{INT_MIN, INT_MIN + 1, -INT_MAX / 2, -2, -1};
	signed_edge.insert(signed_edge.end(), edge.begin(), edge.end());
	for(int p:edge){
		if(p){
			for(int x:signed_edge) check_inv(x, p);
		}
	}

	i64 a = 0, b = 1;
	while(b <= INT_MAX){
		check_exgcd(int(a), int(b));
		check_exgcd(int(b), int(a));
		check_inv(int(a), int(b));
		check_inv(-int(b), int(b));
		tie(a, b) = pair{b, a + b};
	}

	mt19937 rng(20261104);
	uniform_int_distribution<int> nonnegative(0, INT_MAX),
		positive(1, INT_MAX), any_int(INT_MIN, INT_MAX);
	rep(tc, 1, 100000){
		check_exgcd(nonnegative(rng), nonnegative(rng));
		check_inv(any_int(rng), positive(rng));
	}
	cout << "passed " << bezout_cases << " Bezout cases and "
		<< inverse_cases << " safe_inv cases and " << unchecked_cases << " inv cases\n";
}
