// 依赖缺省源中的 i64、vi、rep、per；p 须为质数，2 <= p <= INT_MAX。
// 构造预处理 [0,p) 的阶乘与逆阶乘，O(p) 时间、O(p) 空间，须有足够内存。
struct Lucas{
	int p;
	vi fac, ifac;
	Lucas(int p): p(p), fac(p), ifac(p){
		fac[0] = 1;
		rep(i, 1, p - 1){
			fac[i] = int(i64(fac[i - 1]) * i % p);
		}
		ifac[p - 1] = fac[p - 1]; // Wilson：(p-1)! ≡ -1，其逆元也是 -1。
		per(i, p - 1, 1){
			ifac[i - 1] = int(i64(ifac[i]) * i % p);
		}
	}
	// 0 <= m < p；n<0 或 n>m 返回 0，其余返回 C(m,n) mod p；O(1)。
	int C(int m, int n)const {
		if(n < 0 || n > m){
			return 0;
		}
		return int(i64(fac[m]) * ifac[n] % p * ifac[m - n] % p);
	}
	// 0 <= m <= LLONG_MAX；n<0 或 n>m 返回 0，C(0,0)=1。
	// 时间与递归栈空间均为 O(1+log_p(n+1))，最多递归 64 层。
	int solve(i64 m, i64 n)const {
		if(n < 0 || n > m){
			return 0;
		}
		if(n == 0){
			return 1;
		}
		return int(i64(C(int(m % p), int(n % p))) * solve(m / p, n / p) % p);
	}
};
