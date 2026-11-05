// F=sum_{i=0}^n floor((a*i+b)/c)，n<0 返回 0；仅依赖缺省源的 i64。
// 0<=a,b,n<=2^62-1，1<=c<=2^62-1；答案须<=2^63-1。
// O(1+log(c+1)) 时间，O(1) 空间；闭区间参数顺序为 n,a,b,c。
i64 floor_sum(i64 n, i64 a, i64 b, i64 c){
	using i128 = __int128;
	if(n < 0){
		return 0;
	}
	n++;
	i64 res = 0;
	for(;;){
		res += i64(i128(n) * (n - 1) / 2 * (a / c) + i128(n) * (b / c));
		a %= c, b %= c;
		i128 y = i128(a) * n + b;
		if(y < c){
			return res;
		}
		n = i64(y / c), b = i64(y % c);
		swap(a, c);
	}
}
