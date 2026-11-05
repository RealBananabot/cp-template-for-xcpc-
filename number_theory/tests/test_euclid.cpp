#include "../euclid.cpp"

struct Matrix{
	static const i64 mod = 998244353;
	i64 a = 1, b = 0, c = 0, d = 1;
	friend Matrix operator+(const Matrix &A, const Matrix &B){
		return {(A.a * B.a + A.b * B.c) % mod, (A.a * B.b + A.b * B.d) % mod,
			(A.c * B.a + A.d * B.c) % mod, (A.c * B.b + A.d * B.d) % mod};
	}
	bool operator==(const Matrix &)const = default;
};

int main(){
	using i128 = __int128;
	int string_cases = 0, matrix_cases = 0;
	rep(p, 0, 12) rep(q, 1, 12) rep(r, 0, q - 1) rep(l, 0, 16){
		string expected;
		int previous = 0;
		rep(i, 1, l){
			int current = (p * i + r) / q;
			expected += string(current - previous, 'U') + 'R';
			previous = current;
		}
		assert(euclid(p, q, r, l, string("U"), string("R")) == expected);
		string_cases++;
	}
	rep(k, 0, 100){
		assert(monoid_pow(string("UR"), k) == [&]{
			string s;
			rep(i, 1, k){
				s += "UR";
			}
			return s;
		}());
	}
	Matrix U{1, 1, 0, 1}, R{1, 0, 1, 1};
	assert(!(U + R == R + U));
	mt19937_64 rng(5170);
	const i64 limit = (1LL << 62) - 1;
	rep(tc, 1, 3000){
		i64 p = i64(rng() % (limit + 1)), q = i64(rng() % limit) + 1;
		i64 r = i64(rng() % q), l = i64(rng() % 51);
		if(tc % 10 == 0){
			q = 1, r = 0;
		}
		i128 previous = 0;
		Matrix expected;
		for(i64 i = 1; i <= l; i++){
			i128 current = (i128(p) * i + r) / q;
			i64 delta = i64(current - previous);
			// U^delta 是已知的幺幂矩阵，不用 monoid_pow 构造参考。
			expected = expected + Matrix{1, delta % Matrix::mod, 0, 1} + R;
			previous = current;
		}
		assert(euclid(p, q, r, l, U, R) == expected);
		matrix_cases++;
	}
	assert(euclid(0, limit, limit - 1, 0, U, R) == Matrix{});
	cout << "passed " << string_cases << " literal noncommutative paths, "
		<< matrix_cases << " i64 matrix paths and monoid powers\n";
}
