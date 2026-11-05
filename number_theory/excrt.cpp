// 依赖 exgcd.cpp 中的 exgcd，以及缺省源中的 i64、fi、se。
using i128 = __int128;

// {模数,余数}；模数为正，余数可为任意 i64；lcm 须不超过 LLONG_MAX。
// 返回 {lcm,最小非负解}，无解为 {-1,-1}；无解标记可继续合并。
// O(log(min(A.fi,B.fi)+1)) 时间与递归栈空间；不修改输入。
pair<i64, i64> crt_merge(const pair<i64, i64> &A, const pair<i64, i64> &B){
	if(A.fi == -1 || B.fi == -1){
		return {-1, -1};
	}
	auto [a, r] = A;
	auto [b, s] = B;
	r %= a, s %= b;
	if(r < 0){
		r += a;
	}
	if(s < 0){
		s += b;
	}
	i64 x, y, g = exgcd(a, b, x, y), d = s - r;
	if(d % g){
		return {-1, -1};
	}
	i64 m = a / g * b;
	x = i64((i128(x) * (d / g) * a + r) % m);
	if(x < 0){
		x += m;
	}
	return {m, x};
}
