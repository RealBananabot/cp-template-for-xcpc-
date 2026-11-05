// 倒数乘法除法，仅作常数优化参考；不自动替换其他模板中的除法。
// 先 init()，2<=y<B；0<=x<2^48 对当前 B 保证正确，y=1 不支持。
// 一般充分条件：x<2^64 且 x*(y-1)<2^64（按数学整数计算）。
// u128 参数用于扩展乘法，不表示支持任意 128 位被除数。
namespace QDiv{
	using u64 = unsigned long long;
	using u128 = unsigned __int128;
	const int B = 65536 + 114;
	u64 inv[B];
	void init(){
		rep(i, 1, B - 1){
			inv[i] = (-1ULL) / u64(i) + 1;
		}
	}
	u64 qdiv(u128 x, u64 y){
		return u64(x * inv[y] >> 64);
	}
}
