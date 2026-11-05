#include "../min25.cpp"

const int mod = Min25::mod;
int residue(i64 x){
	return int((x % mod + mod) % mod);
}
int factor_value(i64 x){
	i64 res = 1;
	for(i64 p = 2; p * p <= x; p++){
		if(x % p == 0){
			i64 pw = 1;
			while(x % p == 0){
				x /= p;
				pw *= p;
			}
			res = res * (pw % mod * (pw % mod - 1) % mod) % mod;
		}
	}
	if(x > 1){
		res = res * (x % mod * (x % mod - 1) % mod) % mod;
	}
	return residue(res);
}

int main(){
	static_assert(is_same_v<decltype(declval<Min25&>().solve(0)), int>);
	const int N = 1000000;
	vi spf(N + 1), value(N + 1), prefix(N + 1), s1(N + 1), s2(N + 1);
	rep(p, 2, N){
		if(!spf[p]){
			for(int j = p; j <= N; j += p){
				if(!spf[j]){
					spf[j] = p;
				}
			}
		}
	}
	rep(i, 1, N){
		int x = i;
		i64 res = 1;
		while(x > 1){
			int p = spf[x], pw = 1;
			while(x % p == 0){
				x /= p;
				pw *= p;
			}
			res = res * (i64(pw) * (pw - 1) % mod) % mod;
		}
		value[i] = int(res);
		prefix[i] = (prefix[i - 1] + value[i]) % mod;
		s1[i] = s1[i - 1], s2[i] = s2[i - 1];
		if(spf[i] == i){
			s1[i] = residue(s1[i] + i);
			s2[i] = residue(s2[i] + i64(i) * i);
		}
	}
	Min25 d;
	int prefixes = 0, tables = 0, recursive = 0;
	auto check_tables = [&](){
		i64 prev = 0;
		rep(i, 1, d.m){
			i64 x = d.val(i);
			assert(x > prev && x == d.n / (d.n / x));
			assert(d.id(x) == i);
			if(x <= N){
				assert(residue(d.f[i]) == s1[int(x)]);
				assert(residue(d.g[i]) == s2[int(x)]);
				tables++;
			}
			prev = x;
		}
	};
	rep(n, 0, 3000){
		assert(d.solve(n) == prefix[n]);
		prefixes++;
		check_tables();
		if(n <= 128){
			rep(j, 1, d.m) rep(i, 0, int(size(d.pr))){
				int x = int(d.val(j)), expected = 0;
				int last = (i ? d.pr[i - 1] : 1);
				rep(k, 2, x){
					if(spf[k] > last){
						expected = (expected + value[k]) % mod;
					}
				}
				assert(residue(d.calc(x, i)) == expected);
				recursive++;
			}
		}
	}
	mt19937 rnd(5325);
	rep(tc, 1, 200){
		int n = int(rnd() % (N + 1));
		assert(d.solve(n) == prefix[n]);
		prefixes++;
		if(tc <= 10){
			check_tables();
		}
	}
	for(int n:{N, 99991, 99991 - 1, 997 * 997 - 1, 997 * 997, 997 * 997 + 1}){
		assert(d.solve(n) == prefix[n]);
		prefixes++;
		check_tables();
	}
	assert(d.solve(10) == 263);
	assert(d.solve(1000000000) == 710164413); // 题面两组样例。
	check_tables();
	vi snapshots;
	vector<i64> large{1000000007LL, 8589934592LL, 99991LL * 99991, 10000000000LL};
	for(i64 n:large){
		int right = d.solve(n);
		assert(0 <= right && right < mod);
		check_tables();
		int left = d.solve(n - 1);
		assert(residue(i64(right) - left) == factor_value(n));
		snapshots.pb(right);
	}
	assert(d.solve(0) == 0 && d.m == 0 && d.pr.empty());
	assert(d.solve(1) == 1);
	assert(d.solve(large[0]) == snapshots[0]);
	assert(d.solve(2) == 3);
	cout << "passed " << prefixes << " independent prefixes, " << tables
		<< " prime-sum table checks, " << recursive << " restricted recursion checks, "
		<< "both samples, four large adjacent differences and rebuilds; solve(1e10) = "
		<< snapshots.back() << '\n';
}
