// 杜教筛示例：solve(n) 返回 sum_{i=1}^n phi(i)。
// 0<=n<=INT_MAX；预筛 [1,m]，要求 1<=m<INT_MAX 且 floor(sqrt(n))<=m。
// 一般 h=f*g，有 g(1)F(n)=H(n)-sum_{i=2}^n g(i)F(n/i)，F,H 为前缀和。
// 此处 phi*1=id；平方根拆分双曲线两侧，再补回重复的正方形。
struct Dujiao{
	vi phi;
	vector<i64> sphi;
	unordered_map<int, i64> mem;
	Dujiao(int m = 1000000): phi(m + 1), sphi(m + 1){
		vi pr;
		phi[1] = 1;
		rep(i, 2, m){
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
		}
		rep(i, 1, m){
			sphi[i] = sphi[i - 1] + phi[i];
		}
	}
	i64 solve(int n){
		if(n < int(size(phi))){
			return sphi[n];
		}
		if(auto it = mem.find(n); it != mem.end()){
			return it->se;
		}
		i64 res = i64(n) * (i64(n) + 1) / 2;
		int s = 2;
		for(; i64(s) * s <= n; s++){
			int k = n / s;
			res -= solve(k) + i64(phi[s]) * k;
		}
		s--;
		res += i64(s) * solve(s) - n;
		return mem[n] = res;
	}
};
