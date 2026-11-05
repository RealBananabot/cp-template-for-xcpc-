// PN 筛：F=G*H，F、G 积性，F(p)=G(p)，F(1)=G(1)=1。
// 返回 sum_{i=1}^n F(i) mod mod；0<=n<=2^62-1，1<=mod<=INT_MAX。
// pr 递增且不重复，包含所有 <=sqrt(n) 的质数；不修改、不保存输入引用。
// h(p,e,pw)=H(p^e)，e>=2、pw=p^e；sum_g(x)=sum_{i<=x}G(i)。
// 回调均返回 [0,mod)；仅枚举 PN，时间另加回调及质数表预处理开销。
int pn_sieve(i64 n, const vi &pr, int mod, auto h, auto sum_g){
	auto dfs = [&](auto &self, i64 x, int i, i64 v)->int {
		i64 res = v * sum_g(x) % mod;
		for(int j = i; j < int(size(pr)) && i64(pr[j]) * pr[j] <= x; j++){
			i64 p = pr[j], pw = p * p;
			for(int e = 2; pw <= x; e++){
				res = (res + self(self, x / pw, j + 1, v * h(pr[j], e, pw) % mod)) % mod;
				if(pw > x / p){
					break;
				}
				pw *= p;
			}
		}
		return int(res);
	};
	return (n ? dfs(dfs, n, 0, 1 % mod) : 0);
}
