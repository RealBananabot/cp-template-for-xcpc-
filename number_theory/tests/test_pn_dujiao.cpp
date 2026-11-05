#include "../pn_sieve.cpp"
#include "../pn_dujiao.cpp"
#include "../min25.cpp"

struct PNReference{
	static const int mod = 1000000007;
	vi phi, spf, sg, sf;
	unordered_map<i64, int> mem;
	PNReference(int n): phi(n + 1), spf(n + 1), sg(n + 1), sf(n + 1){
		iota(phi.begin(), phi.end(), 0);
		rep(p, 2, n){
			if(!spf[p]){
				for(int j = p; j <= n; j += p){
					phi[j] -= phi[j] / p;
					if(!spf[j]){
						spf[j] = p;
					}
				}
			}
		}
		rep(i, 1, n){
			i64 value = 1;
			int x = i;
			while(x > 1){
				int p = spf[x], pw = 1;
				while(x % p == 0){
					x /= p, pw *= p;
				}
				value = value * (i64(pw) * (pw - 1) % mod) % mod;
			}
			sf[i] = int((sf[i - 1] + value) % mod);
			sg[i] = int((sg[i - 1] + i64(i) * phi[i]) % mod);
		}
	}
	int sum_g(i64 n){
		using i128 = __int128;
		if(n < i64(size(sg))){
			return sg[int(n)];
		}
		if(auto it = mem.find(n); it != mem.end()){
			return it->se;
		}
		i128 res = i128(n) * (n + 1) * (2 * n + 1) / 6 % mod;
		for(i64 l = 2, r; l <= n; l = r + 1){
			i64 k = n / l;
			r = n / k;
			i128 weight = (i128(l) + r) * (r - l + 1) / 2 % mod;
			res = (res - weight * sum_g(k)) % mod;
		}
		return mem[n] = int((res + mod) % mod);
	}
};

int main(){
	static_assert(is_same_v<decltype(declval<PNDujiao&>().solve(0)), int>);
	PNReference ref(1000000);
	PNDujiao small(64);
	int cases = 0, sums = 0, convolution = 0;
	rep(p, 2, 100){
		if(ref.spf[p] == p){
			vector<i64> h{1, 0};
			vector<i64> powers{1, p};
			for(int e = 2; powers.back() <= 10000000000LL / p; e++){
				i64 pw = powers.back() * p;
				powers.pb(pw);
				using i128 = __int128;
				i128 x = i128(pw) * (pw - 1);
				rep(j, 1, e){
					i128 g = i128(powers[j]) * (powers[j] - powers[j] / p);
					x -= g * h[e - j];
				}
				i64 expected = i64((x % PNDujiao::mod + PNDujiao::mod) % PNDujiao::mod);
				h.pb(expected);
				assert(expected == i64(e - 1) * (pw % PNDujiao::mod) % PNDujiao::mod
					* (p - 1) % PNDujiao::mod);
				convolution++;
			}
		}
	}
	rep(n, 0, 4224){
		assert(small.solve(n) == ref.sf[n]);
		assert(small.sum_g(n) == ref.sg[n]);
		cases++, sums++;
	}
	for(int m:{1, 2, 7}){
		PNDujiao s(m);
		int bound = (m + 1) * (m + 1) - 1;
		rep(n, 0, bound){
			assert(s.solve(n) == ref.sf[n]);
			cases++;
		}
	}
	PNDujiao s;
	mt19937 rnd(5325);
	rep(i, 1, 100){
		int n = int(rnd() % 1000001);
		assert(s.solve(n) == ref.sf[n]);
		cases++;
	}
	Min25 min25;
	vector<i64> large{1000000000, 1000000007, 8589934592LL, 99991LL * 99991, 10000000000LL};
	vi answers;
	for(i64 n:large){
		assert(s.sum_g(n) == ref.sum_g(n));
		sums++;
		int ans = s.solve(n);
		assert(ans == min25.solve(n));
		answers.pb(ans);
	}
	assert(s.solve(10) == 263 && s.solve(1000000000) == 710164413);
	size_t count = size(s.mem);
	rep(i, 0, int(size(large)) - 1){
		assert(s.solve(large[i]) == answers[i]);
	}
	assert(size(s.mem) == count);
	assert(s.solve(0) == 0 && s.solve(1) == 1);
	PNDujiao minimum(100000);
	assert(minimum.solve(10000000000LL) == answers.back());
	cout << "passed " << convolution << " prime-power convolution checks, " << cases
		<< " independent prefixes, " << sums << " weighted Dujiao sums, five large Min25 comparisons, "
		<< "minimum cutoff and cached repeats; solve(1e10) = " << answers.back() << '\n';
}
