// 同 floor_sum 的闭区间 [0,n] 与参数顺序，答案固定对 998244353 取模。
// n<0 返回 0；0<=a,b,n<=2^62-1，1<=c<=2^62-1；仅依赖 i64。
// O(1+log(c+1)) 时间，O(1) 空间；不要求 c 可逆或为质数。
int floor_sum_mod(i64 n, i64 a, i64 b, i64 c){
	using i128 = __int128;
	const i64 mod = 998244353, inv2 = 499122177;
	if(n < 0){
		return 0;
	}
	n++;
	i64 res = 0;
	for(;;){
		i64 t = n % mod, s = t * ((n - 1) % mod) % mod * inv2 % mod;
		res = (res + s * (a / c % mod) + t * (b / c % mod)) % mod;
		a %= c, b %= c;
		i128 y = i128(a) * n + b;
		if(y < c){
			return int(res);
		}
		n = i64(y / c), b = i64(y % c);
		swap(a, c);
	}
}
