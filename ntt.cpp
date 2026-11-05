#include <bits/stdc++.h>
using namespace std;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)

using u32 = uint32_t;
using u64 = uint64_t;
constexpr int mod = 998244353;

struct mint{
	u32 x;

	mint(): x(0){}
	mint(int _x){
		_x %= mod;
		if(_x < 0) _x += mod;
		x = _x;
	}

	mint qpow(int y = mod - 2)const {
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
	mint& operator /= (const mint &B){
		return *this *= B.qpow();
	}
	friend mint operator + (mint A, const mint &B){
		return mint(A) += B;
	}
	friend mint operator - (mint A, const mint &B){
		return mint(A) -= B;
	}
	friend mint operator * (mint A, const mint &B){
		return mint(A) *= B;
	}
	friend mint operator / (mint A, const mint &B){
		return mint(A) /= B;
	}
};

constexpr int LG = 21, S = 1 << LG;
vector<mint> P(S), ip2(LG + 1);

auto INIT = [](){
	rep(i, 0, LG - 1){
		mint stp = mint(3).qpow(mod >> (i + 1));
		P[1 << i] = 1;
		rep(j, (1 << i) + 1, (2 << i) - 1){
			P[j] = P[j - 1] * stp;
		}
	}
	ip2[0] = 1;
	rep(i, 1, LG) ip2[i] = ip2[i - 1] / 2;
	return true;
}();

struct poly : vector<mint>{
	using vector<mint>::vector;

	void DIF(int lg){
		for(int i = 1 << (lg - 1); i; i /= 2){
			for(int j = 0; j < (1 << lg); j += 2 * i){
				rep(k, 0, i - 1){
					mint A = (*this)[j + k], B = (*this)[j + k + i];
					(*this)[j + k] = A + B;
					(*this)[j + k + i] = (A - B) * P[i + k];
				}
			}
		}
	}

	void DIT(int lg){
		for(int i = 1; i < (1 << lg); i *= 2){
			for(int j = 0; j < (1 << lg); j += 2 * i){
				rep(k, 0, i - 1){
					mint A = (*this)[j + k], B = (*this)[j + k + i] * P[i + k];
					(*this)[j + k] = A + B;
					(*this)[j + k + i] = A - B;
				}
			}
		}
		reverse(begin() + 1, end());
	}

	friend poly conv(const poly &_A, const poly &_B){
		if(min(ranges::size(_A), ranges::size(_B)) <= 64){
			poly res(ranges::size(_A) + ranges::size(_B) - 1);
			rep(i, 0, int(ranges::size(_A)) - 1){
				rep(j, 0, int(ranges::size(_B)) - 1){
					res[i + j] += _A[i] * _B[j];
				}
			}
			return res;
		}

		poly A = _A, B = _B;
		int len = int(ranges::size(A) + ranges::size(B) - 1);
		int lg = __lg(len * 2 - 1);
		A.resize(1 << lg), B.resize(1 << lg);
		A.DIF(lg), B.DIF(lg);
		rep(i, 0, (1 << lg) - 1) A[i] *= B[i];
		A.DIT(lg), A.resize(len);
		for(auto &x:A) x *= ip2[lg];
		return A;
	}
};
