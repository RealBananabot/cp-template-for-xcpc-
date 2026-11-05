#include "../pn_sieve.cpp"

int main(){
	const int N = 3000;
	vi spf(N + 1), pr;
	rep(p, 2, N){
		if(!spf[p]){
			pr.pb(p);
			for(int j = p; j <= N; j += p){
				if(!spf[j]){
					spf[j] = p;
				}
			}
		}
	}
	int cases = 0;
	for(int mod:{1, 2, 6, 97, 1000000007, INT_MAX}){
		vi powerful(N + 1, 1), weight(N + 1, 1 % mod);
		rep(d, 2, N){
			int x = d;
			while(x > 1){
				int p = spf[x], e = 0, pw = 1;
				while(x % p == 0){
					x /= p, pw *= p, e++;
				}
				if(e == 1){
					powerful[d] = weight[d] = 0;
				}
				weight[d] = int(i64(weight[d]) * ((pw + e + p) % mod) % mod);
			}
		}
		auto h = [&](int p, int e, i64 pw){
			assert(e >= 2);
			assert(pw >= i64(p) * p);
			return int((pw + e + p) % mod);
		};
		auto sg = [&](i64 x){return int(x * (x + 1) / 2 % mod);};
		auto check = [&](int n){
			i64 expected = 0;
			int count = 0;
			rep(d, 1, n){
				expected = (expected + i64(weight[d]) * sg(n / d)) % mod;
				count += powerful[d];
			}
			assert(pn_sieve(n, pr, mod, h, sg) == expected);
			assert(pn_sieve(n, pr, mod, [&](int, int, i64){return 1 % mod;},
				[&](i64){return 1 % mod;}) == count % mod);
			cases++;
		};
		rep(n, 0, 256){
			check(n);
		}
		for(int n:{N, 997, 1023, 1024, 1025}){
			check(n);
		}
	}
	int calls = 0;
	assert(pn_sieve(0, pr, 97, [&](int, int, i64){calls++; return 0;},
		[&](i64){calls++; return 0;}) == 0);
	assert(calls == 0);
	vi empty;
	assert(pn_sieve(1, empty, 97, [](int, int, i64){return 0;},
		[](i64){return 1;}) == 1);
	cout << "passed " << cases << " independent weighted PN sums and PN counts, "
		<< "composite moduli, mod=1, empty prefix and singleton\n";
}
