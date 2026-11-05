// P5325：积性 F(p^e)=p^e*(p^e-1)，F(1)=1；先粘贴 pn_sieve.cpp。
// G(n)=n*phi(n)，F=G*H，H(p^e)=(e-1)*p^e*(p-1)。
// (n*phi)*id=id^2，用杜教筛求 G 前缀，再枚举 PN 合并。
// solve(n)：0<=n<=1e10，返回 [1,n] 的 F 和 mod 1e9+7；solve(0)=0。
// 预筛 [1,m]，1<=m<INT_MAX，所有查询须 floor(sqrt(n))<=m。
struct PNDujiao{
	static const int mod = 1000000007;
	vi pr, phi, sg;
	unordered_map<i64, int> mem;
	PNDujiao(int m = 5000000): phi(m + 1), sg(m + 1){
		phi[1] = 1;
		rep(i, 1, m){
			if(!phi[i]){
				pr.pb(i);
				phi[i] = i - 1;
			}
			for(int p:pr){
				if(p > m / i){
					break;
				}
				int j = i * p;
				if(i % p == 0){
					phi[j] = phi[i] * p;
					break;
				}
				phi[j] = phi[i] * (p - 1);
			}
			sg[i] = int((sg[i - 1] + i64(i) * phi[i]) % mod);
		}
	}
	int sum_g(i64 n){
		if(n < i64(size(sg))){
			return sg[int(n)];
		}
		if(auto it = mem.find(n); it != mem.end()){
			return it->se;
		}
		i64 x = n % mod;
		i64 res = x * (x + 1) % mod * (2 * x + 1) % mod * 166666668 % mod
			- x * (x + 1) / 2 % mod;
		int s = 2;
		for(; i64(s) * s <= n; s++){
			i64 k = n / s, t = k % mod;
			res = (res - i64(s) * sum_g(k)
				- i64(s) * phi[s] % mod * (t * (t + 1) / 2 % mod)) % mod;
		}
		s--;
		res = (res + i64(s) * (s + 1) / 2 % mod * sum_g(s)) % mod;
		if(res < 0){
			res += mod;
		}
		return mem[n] = int(res);
	}
	int solve(i64 n){
		auto h = [&](int p, int e, i64 pw){
			return int(i64(e - 1) * (pw % mod) % mod * (p - 1) % mod);
		};
		return pn_sieve(n, pr, mod, h, [&](i64 x){return sum_g(x);});
	}
};
