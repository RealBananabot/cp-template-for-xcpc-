#include "../qdiv.cpp"

int main(){
	using QDiv::u64;
	using QDiv::u128;
	static_assert(sizeof(u64) == 8);
	static_assert(QDiv::B >= 3);
	const u128 M = u128(1) << 64;
	const u64 largest = ULLONG_MAX, common = u64((M - 1) / (QDiv::B - 2));
	QDiv::init();
	assert(QDiv::inv[0] == 0 && QDiv::inv[1] == 0);
	assert(QDiv::qdiv(1, 1) == 0); // 原式的除数 1 错误，不属于契约。
	int cases = 0;
	mt19937_64 rnd(3663);
	rep(y, 2, QDiv::B - 1){
		u64 d = u64(y), reciprocal = QDiv::inv[y];
		assert(u128(reciprocal) == (M + d - 1) / d);
		u64 e = u64(u128(reciprocal) * d - M);
		assert(e < d);
		u64 bound = u64((M - 1) / (d - 1));
		vector<u64> values{0, 1, d - 1, d, d + 1, common - 1, common,
			bound - 1, bound};
		rep(i, 1, 16){
			values.pb(u64(u128(rnd()) % (u128(bound) + 1)));
		}
		for(u64 x:values){
			assert(QDiv::qdiv(x, d) == x / d);
			cases++;
		}
		// 全 u64 范围内：无乘法溢出，结果只能正确或偏大 1。
		for(u64 x:{largest, largest - 1, u64(rnd())}){
			u64 got = QDiv::qdiv(x, d), expected = x / d;
			assert(got == expected || got == expected + 1);
			bool exact = u128(x) * e < u128(d - x % d) * M;
			assert((got == expected) == exact);
			cases++;
		}
		if(e){
			// 在余数为 y-1 的位置检查误差跨越整数边界前后。
			u128 threshold = (M + e - 1) / e;
			u128 x = (threshold + d) / d * d - 1;
			if(x < M){
				assert(QDiv::qdiv(x - 1, d) == (x - 1) / d);
				assert(QDiv::qdiv(x, d) == x / d + 1);
			}
		}
	}
	if(QDiv::B > 3){
		assert(QDiv::qdiv(largest - 1, 3) == 6148914691236517205ULL);
		assert((largest - 1) / 3 == 6148914691236517204ULL);
		// 原接口为 u128，不代表任意 u128 乘积都能保存。
		assert(QDiv::qdiv(M, 3) == 6148914691236517206ULL);
		assert(M / 3 == 6148914691236517205ULL);
		assert(QDiv::qdiv(3 * M - 1, 3) == 1);
		assert((3 * M - 1) / 3 == largest);
	}
	QDiv::init();
	assert(QDiv::qdiv(common, QDiv::B - 1) == common / u64(QDiv::B - 1));
	cout << "passed " << cases << " qdiv comparisons, all " << QDiv::B - 2 << " reciprocal error bounds, "
		<< "boundary counterexamples and repeated initialization\n";
}
