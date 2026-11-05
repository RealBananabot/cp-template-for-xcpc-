// 先粘贴 euclid.cpp；统计每个 R 处的高度及其平方，R 的下标从 0 开始。
struct FloorMoments{
	static const int mod = 998244353;
	int r = 0, u = 0, sum = 0, sq = 0, mix = 0;
	friend FloorMoments operator+(const FloorMoments &A, const FloorMoments &B){
		i64 r = A.r, u = A.u, tri = i64(B.r) * (B.r - 1) / 2 % mod;
		return {(A.r + B.r) % mod, (A.u + B.u) % mod,
			int((A.sum + B.sum + u * B.r) % mod),
			int((A.sq + B.sq + 2 * u * B.sum + B.r * u % mod * u) % mod),
			int((A.mix + B.mix + B.r * u % mod * r + u * tri + r * B.sum) % mod)};
	}
	// 返回 {sum y_i, sum y_i^2, sum i*y_i}，i=0..n，y_i=floor((a*i+b)/c)。
	// n<0 返回全零；0<=a,b,n<=2^62-1，1<=c<=2^62-1；结果均模 mod。
	static array<int, 3> solve(i64 n, i64 a, i64 b, i64 c){
		if(n < 0){
			return {};
		}
		FloorMoments U{0, 1}, R{1};
		auto z = FloorMoments{0, int(b / c % mod)} + R + euclid(a, c, b % c, n, U, R);
		return {z.sum, z.sq, z.mix};
	}
};
