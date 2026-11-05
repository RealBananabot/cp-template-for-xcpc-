// Min_25 示例：积性 f，f(p^e)=p^e*(p^e-1)，f(1)=1。
// solve(n) 返回 sum_{i=1}^n f(i) mod 1e9+7；0<=n<=1e10，solve(0)=0。
// 先筛质数的一次、二次幂和，再按最小质因子枚举质数幂；普通整数除法。
struct Min25{
	static const int mod = 1000000007;
	i64 n = 0;
	int sq = 0, m = 0;
	vi pr, f, g;
	// 仅用于当前 n 的不同正整除商，编号 [1,m]，值递增。
	int id(i64 x){
		return (x <= sq ? int(x) : m + 1 - int(n / x));
	}
	i64 val(int i){
		return (i <= sq ? i64(i) : n / (m + 1 - i));
	}
	// 排除前 i 个质因子，求 2..x 的 f 之和；x 须为当前 n 的正整除商。
	int calc(i64 x, int i){
		if(x <= 1 || (i && x <= pr[i - 1])){
			return 0;
		}
		int j = id(x);
		i64 res = g[j] - f[j];
		if(i){
			res -= g[pr[i - 1]] - f[pr[i - 1]];
		}
		for(int k = i; k < int(size(pr)) && i64(pr[k]) * pr[k] <= x; k++){
			i64 p = pr[k];
			for(i64 pw = p; pw <= x / p; pw *= p){
				i64 a = pw % mod, b = pw * p % mod;
				res = (res + a * (a - 1) % mod * calc(x / pw, k + 1) + b * (b - 1)) % mod;
			}
		}
		return int(res % mod);
	}
	int solve(i64 _n){
		n = _n;
		sq = int(sqrt(n));
		while(i64(sq + 1) * (sq + 1) <= n){
			sq++;
		}
		while(i64(sq) * sq > n){
			sq--;
		}
		m = sq + int(n / (sq + 1));
		pr.clear();
		vi np(sq + 1);
		rep(i, 2, sq){
			if(!np[i]){
				pr.pb(i);
				for(i64 j = i64(i) * i; j <= sq; j += i){
					np[int(j)] = 1;
				}
			}
		}
		f.assign(m + 1, 0), g.assign(m + 1, 0);
		rep(i, 1, m){
			i64 x = val(i) % mod;
			f[i] = int(x * (x + 1) / 2 % mod - 1);
			g[i] = int(x * (x + 1) % mod * (2 * x + 1) % mod * 166666668 % mod - 1);
		}
		for(int p:pr){
			per(i, m, 1){
				i64 x = val(i);
				if(x < i64(p) * p){
					break;
				}
				int j = id(x / p);
				f[i] = int((f[i] - i64(p) * (f[j] - f[p - 1])) % mod);
				g[i] = int((g[i] - i64(p) * p % mod * (g[j] - g[p - 1])) % mod);
			}
		}
		return (n ? (calc(n, 0) + 1 + mod) % mod : 0);
	}
};
