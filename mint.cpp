using u32 = uint32_t;
using u64 = uint64_t;
const int mod = 998244353;
struct mint{
	u32 x;

	mint(): x(0){}
	mint(int _x){
		_x %= mod;
		if(_x < 0) _x += mod;
		x = u32(_x);
	}

	// y >= 0. The default exponent gives the inverse of a nonzero value.
	mint qpow(int y = mod - 2)const{
		assert(y >= 0);
		mint t = *this, res = 1;
		while(y){
			if(y % 2) res *= t;
			t *= t;
			y /= 2;
		}
		return res;
	}

	mint& operator += (const mint &B){
		if((x += B.x) >= mod) x -= mod;
		return *this;
	}
	mint& operator -= (const mint &B){
		if((x -= B.x) >= mod) x += mod;
		return *this;
	}
	mint& operator *= (const mint &B){
		x = u32(u64(x) * B.x % mod);
		return *this;
	}
	// B.x != 0.
	mint& operator /= (const mint &B){
		return *this *= B.qpow();
	}
	friend mint operator + (mint A, const mint &B){
		return A += B;
	}
	friend mint operator - (mint A, const mint &B){
		return A -= B;
	}
	friend mint operator * (mint A, const mint &B){
		return A *= B;
	}
	friend mint operator / (mint A, const mint &B){
		return A /= B;
	}
	mint operator - ()const{
		return mint() - *this;
	}

	friend ostream& operator << (ostream &os, const mint &obj){
		os << obj.x;
		return os;
	}
};

const int N = 1'000'005;
mint fac[N], ifac[N];

auto INIT = [](){
	fac[0] = 1;
	rep(i, 1, N - 1){
		fac[i] = fac[i - 1] * i;
	}
	ifac[N - 1] = 1 / fac[N - 1];
	per(i, N - 1, 1){
		ifac[i - 1] = ifac[i] * i;
	}
	return true;
}();

// 0 <= m < N. Returns 0 when n is outside [0, m].
mint C(int m, int n){
	return (n < 0 || n > m ? 0 : fac[m] * ifac[n] * ifac[m - n]);
}
