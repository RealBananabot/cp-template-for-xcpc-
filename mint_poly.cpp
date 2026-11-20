#include <bits/stdc++.h>
using namespace std;
using i64 = long long;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#define per(i, j, k) for(int i = (j); i >= (k); i--)
#define pb emplace_back
#define fi first
#define se second
using vi = vector<int>;
using pi = pair<int, int>;

bool chmin(auto &x, const auto &y){
	if(y < x){x = y; return true;} return false;
}
bool chmax(auto &x, const auto &y){
	if(x < y){x = y; return true;} return false;
}

ostream& operator << (ostream &os, const auto &v)
	requires requires { v.begin(); } && (!requires { v.substr(); }) {
	os << "[";
	int f = 0;
	for(const auto &x:v) os << (f++ ? ", " : "") << x;
	return os << "]";
}

template<typename ...Ar> void debug(const char *s, Ar... x){
	auto f = [&](auto x){
		int dep = 0;
		while(*s && !(*s == ',' && dep == 0)){
			if(*s == '(') dep++;
			if(*s == ')') dep--;
			cerr << *s++;
		}
		cerr << " = " << x;
		if(*s == ','){
			cerr << *s++;
		} else{
			cerr << endl;
		}
	};

	(f(x), ...);
}
#define gdb(...) debug(#__VA_ARGS__, __VA_ARGS__)

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

const int LG = 21, S = 1 << LG;
vector<mint> P, fac, ifac, iv, ip2;

auto INIT = [](){
	P.resize(S);
	rep(i, 0, LG - 1){
		mint stp = mint(3).qpow(mod >> (i + 1));
		P[1 << i] = 1;
		rep(j, (1 << i) + 1, (2 << i) - 1){
			P[j] = P[j - 1] * stp;
		}
	}

	fac.resize(S);
	ifac.resize(S);
	iv.resize(S);
	fac[0] = iv[0] = 1;
	rep(i, 1, S - 1){
		fac[i] = fac[i - 1] * i;
	}
	ifac[S - 1] = 1 / fac[S - 1];
	per(i, S - 1, 1){
		ifac[i - 1] = ifac[i] * i;
		iv[i] = ifac[i] * fac[i - 1];
	}

	ip2.resize(LG + 1);
	ip2[0] = 1;
	rep(i, 1, LG){
		ip2[i] = ip2[i - 1] / 2;
	}

	return true;
}();

// Coefficients are stored from lowest to highest degree.
struct poly : vector<mint>{
	using vector<mint>::vector;
	using polyY = vector<poly>;

	// Both transforms require size(*this) == 2^lg and 0 <= lg <= LG.
	// DIF gives bit-reversed frequencies. Scale by ip2[lg] after DIT for the inverse.
	void DIF(int lg){
		for(int i = (1 << lg) / 2; i; i /= 2){
			for(int j = 0; j < (1 << lg); j += 2 * i){
				rep(k, 0, i - 1){
					mint A = (*this)[j + k], B = (*this)[j + k + i];
					(*this)[j + k] = A + B, (*this)[j + k + i] = (A - B) * P[i + k];
				}
			}
		}
	}

	void DIT(int lg){
		for(int i = 1; i < (1 << lg); i *= 2){
			for(int j = 0; j < (1 << lg); j += 2 * i){
				rep(k, 0, i - 1){
					mint A = (*this)[j + k], B = (*this)[j + k + i] * P[i + k];
					(*this)[j + k] = A + B, (*this)[j + k + i] = A - B;
				}
			}
		}
		reverse(begin() + 1, end());
	}
};

using polyY = poly::polyY;

// Requires a nonempty A. A constant has an empty derivative.
poly der(const poly &A){
	poly res = A;
	rep(i, 1, ssize(res) - 1){
		res[i - 1] = res[i] * i;
	}
	res.pop_back();
	return res;
}

// The integration constant is zero. Requires size(A) < S.
poly ints(const poly &A){
	poly res = A;
	res.pb();
	per(i, int(size(res)) - 1, 1){
		res[i] = res[i - 1] * iv[i];
	}
	res[0] = 0;
	return res;
}

poly& operator += (poly &A, const poly &B){
	if(size(B) > size(A)){
		A.resize(size(B));
	}
	rep(i, 0, ssize(B) - 1){
		A[i] += B[i];
	}
	return A;
}

poly operator + (poly A, const poly &B){
	A += B;
	return A;
}

poly& operator -= (poly &A, const poly &B){
	if(size(B) > size(A)){
		A.resize(size(B));
	}
	rep(i, 0, ssize(B) - 1){
		A[i] -= B[i];
	}
	return A;
}

poly operator - (poly A, const poly &B){
	A -= B;
	return A;
}

poly operator * (const mint &k, const poly &A){
	poly res = A;
	for(auto &x:res) x *= k;
	return res;
}

// Full convolution of length size(_A) + size(_B) - 1. At least one input is nonempty.
// When both lengths exceed 64, the output length must be at most S.
poly conv(const poly &_A, const poly &_B){
	if(size(_A) <= 64 || size(_B) <= 64){
		poly res(size(_A) + size(_B) - 1);
		rep(i, 0, ssize(_A) - 1){
			rep(j, 0, ssize(_B) - 1){
				res[i + j] += _A[i] * _B[j];
			}
		}
		return res;
	}

	poly A = _A, B = _B;
	auto len = size(A) + size(B) - 1;
	int lg = int(__lg(len * 2 - 1));
	A.resize(1 << lg), B.resize(1 << lg);
	A.DIF(lg), B.DIF(lg);
	rep(i, 0, (1 << lg) - 1){
		A[i] *= B[i];
	}
	A.DIT(lg);
	A.resize(len);
	for(auto &x:A) x *= ip2[lg];
	return A;
}

// Reciprocal modulo x^size(A). Requires 1 <= size(A) <= S / 2 and A[0] != 0.
poly inv(const poly &A){
	assert(A[0].x != 0);
	poly res{mint(1) / A[0]};
	while(size(res) < size(A)){
		int n = int(size(res)), lg = __lg(n * 4);
		poly buf(1 << lg);
		copy(res.begin(), res.end(), buf.begin());
		buf.DIF(lg);

		poly tmp{A.begin(), A.begin() + min(n * 2, int(size(A)))};
		tmp.resize(n * 4);
		tmp.DIF(lg);
		rep(i, 0, n * 4 - 1) tmp[i] *= buf[i] * buf[i];
		tmp.DIT(lg);

		res.resize(n * 2);
		rep(i, 0, n * 2 - 1){
			res[i] = 2 * res[i] - tmp[i] * ip2[lg];
		}
	}
	res.resize(size(A));
	return res;
}

// Keep size(A) coefficients. The underlying convolutions must fit the NTT bound.
poly& operator *= (poly &A, const poly &B){
	auto sz = size(A);
	A = conv(A, B);
	A.resize(sz);
	return A;
}

poly operator * (poly A, const poly &B){
	A *= B;
	return A;
}

// Series quotient modulo x^size(A). B is nonempty with B[0] != 0.
// The reciprocal and the full convolution must fit their respective bounds.
poly& operator /= (poly &A, const poly &B){
	auto sz = size(A);
	if(size(B) > 1 && size(B) < sz){
		poly b = B;
		b.resize(sz);
		A = conv(A, inv(b));
	} else{
		A = conv(A, inv(B));
	}
	A.resize(sz);
	return A;
}

poly operator / (poly A, const poly &B){
	A /= B;
	return A;
}

// Requires 1 <= size(A) <= S and A[0] == 0. O(n log^2 n) time.
poly exp(const poly &A){
	assert(A[0].x == 0);
	int n = int(size(A));
	int lg = __lg(n * 2 - 1);

	poly a = A, res(size(A));
	rep(i, 0, ssize(a) - 1) a[i] *= i;
	a.resize(1 << lg);

	vector<poly> pre(lg + 1);
	rep(i, 8, lg){
		pre[i].insert(pre[i].end(), a.begin(), a.begin() + (1 << i));
		pre[i].DIF(i);
	}

	res[0] = 1;
	auto slv = [&](auto &self, int l, int r)->void {
		if(r - l <= 128){
			rep(i, l, r - 1){
				rep(j, l, i - 1){
					res[i] += res[j] * a[i - j];
				}
				if(i > 0) res[i] *= iv[i];
			}
			return;
		}
		int mid = (l + r) / 2;
		self(self, l, mid);

		int _lg = __lg((r - l) * 2 - 1);
		poly tmp(res.begin() + l, res.begin() + mid);
		tmp.resize(1 << _lg);
		tmp.DIF(_lg);
		rep(i, 0, (1 << _lg) - 1) tmp[i] *= pre[_lg][i];
		tmp.DIT(_lg);
		rep(i, mid, r - 1) res[i] += tmp[i - l] * ip2[_lg];
		self(self, mid, r);
	};
	slv(slv, 0, n);
	return res;
}

// Requires A[0] == 1 and the series division to fit its bounds.
poly ln(const poly &A){
	assert(A[0].x == 1);
	poly res = der(A) / A;
	return ints(res);
}

// Requires A[0] == 1 and returns the root with constant term 1.
poly sqrt(const poly &A){
	assert(A[0].x == 1);
	return exp(ip2[1] * ln(A));
}

// Requires nonempty A. The integer exponent is k.x in [0, mod).
poly qpow(const poly &A, mint k){
	if(k.x == 0){
		poly res(size(A));
		res[0] = 1;
		return res;
	}

	auto it = A.begin();
	while(it != A.end() && it->x == 0){
		it++;
	}
	i64 len = i64(it - A.begin()) * k.x;
	if(len >= ssize(A)){
		return poly(size(A));
	} else{
		poly a{it, it + (ssize(A) - len)};
		mint t = a[0].qpow(k.x);
		a = t * exp(k * ln(1 / a[0] * a));
		a.insert(a.begin(), len, mint());
		return a;
	}
}

// Returns the polynomial quotient and remainder. Requires size(_P) >= size(_Q) > 0 and _Q.back() != 0.
pair<poly, poly> division(const poly &_P, const poly &_Q){
	const auto lenF = ssize(_P) - ssize(_Q) + 1;
	const auto lenR = ssize(_Q) - 1;
	poly rP(_P.rbegin(), _P.rbegin() + lenF);
	poly rQ(_Q.rbegin(), _Q.rbegin() + min(ssize(_Q), lenF));
	rQ.resize(lenF);
	poly F = rP / rQ;
	reverse(F.begin(), F.end());
	poly R = _P - conv(_Q, F);
	R.resize(lenR);
	return {F, R};
}

// Transposed convolution. Requires 1 <= size(_B) <= size(_A) <= S.
poly convT(const poly &_A, const poly &_B){
	poly A = _A, B = _B;
	const auto k = ssize(B) - 1;
	reverse(B.begin(), B.end());
	int lg = int(__lg(size(A) * 2 - 1));
	A.resize(1 << lg), B.resize(1 << lg);
	A.DIF(lg), B.DIF(lg);
	rep(i, 0, (1 << lg) - 1){
		A[i] *= B[i];
	}
	A.DIT(lg);
	A = poly{A.begin() + k, A.begin() + ssize(_A)};
	for(auto &x:A){
		x *= ip2[lg];
	}
	return A;
}

// f and x are nonempty. Repeated points are allowed. Internal transforms must fit.
poly multipoint_eval(poly f, const poly &x){
	vector<poly> Q(size(x) * 2 - 1);
	poly res;

	auto dfs0 = [&](auto &self, int u, int l, int r)->void {
		if(l + 1 == r){
			Q[u] = {1, -x[l]};
			return;
		}
		int mid = (l + r) / 2, ls = u + 1, rs = u + (mid - l) * 2;
		self(self, ls, l, mid);
		self(self, rs, mid, r);
		Q[u] = conv(Q[ls], Q[rs]);
	};
	dfs0(dfs0, 0, 0, int(size(x)));

	auto dfs1 = [&](auto &self, int u, int l, int r, poly F)->void {
		F.resize(r - l);
		if(l + 1 == r){
			res.pb(F[0]);
			return;
		}
		int mid = (l + r) / 2;
		int ls = u + 1, rs = u + (mid - l) * 2;
		auto FL = convT(F, Q[rs]), FR = convT(F, Q[ls]);
		self(self, ls, l, mid, FL);
		self(self, rs, mid, r, FR);
	};
	Q[0].resize(size(f));
	poly g = inv(Q[0]);
	g.resize(size(f));
	f.resize(size(f) * 2 - 1);
	g = convT(f, g);
	dfs1(dfs1, 0, 0, int(size(x)), g);

	return res;
}

// Nonempty rectangular inputs. The flattened convolution must fit the one-dimensional bound.
polyY conv(const polyY &A, const polyY &B){
	int x = int(size(A) + size(B) - 1);
	int y = int(size(A[0]) + size(B[0]) - 1);
	poly tmpa(size(A) * y), tmpb(size(B) * y);
	rep(i, 0, ssize(A) - 1){
		rep(j, 0, ssize(A[i]) - 1){
			tmpa[i * y + j] = A[i][j];
		}
	}
	rep(i, 0, ssize(B) - 1){
		rep(j, 0, ssize(B[i]) - 1){
			tmpb[i * y + j] = B[i][j];
		}
	}
	poly tmpc = conv(tmpa, tmpb);
	polyY res(x, poly(y));
	rep(i, 0, x - 1){
		rep(j, 0, y - 1){
			res[i][j] = tmpc[i * y + j];
		}
	}
	return res;
}

// Nonempty rectangular inputs. B fits inside A, and A has at most S entries.
polyY convT(const polyY &A, const polyY &B){
	int n = int(size(A)), m = int(size(A[0]));
	const auto k = ssize(B[0]) - 1;
	poly a(n * m), b(size(B) * m);
	rep(i, 0, n - 1){
		rep(j, 0, m - 1){
			a[i * m + j] = A[i][j];
		}
	}
	rep(i, 0, ssize(B) - 1){
		rep(j, 0, ssize(B[i]) - 1){
			b[i * m + j] = B[ssize(B) - 1 - i][k - j];
		}
	}
	int lg = int(__lg(size(a) * 2 - 1));
	a.resize(1 << lg), b.resize(1 << lg);
	a.DIF(lg), b.DIF(lg);
	rep(i, 0, (1 << lg) - 1){
		a[i] *= b[i];
	}
	a.DIT(lg);

	polyY res(size(A) - size(B) + 1, poly(size(A[0]) - size(B[0]) + 1));
	rep(i, 0, ssize(res) - 1){
		rep(j, 0, ssize(res[i]) - 1){
			res[i][j] = a[(i + ssize(B) - 1) * m + j + k] * ip2[lg];
		}
	}
	return res;
}

// [x^n] P(x) / Q(x). Requires n >= 0, nonempty Q with Q[0] != 0, and supported convolution lengths.
// O(M(d) log(n + 2)) time for d = max(size(P), size(Q)).
mint BostanMori(i64 n, poly P, poly Q){
	assert(n >= 0 && !Q.empty() && Q[0].x != 0);
	while(n && !P.empty()){
		if(n < ssize(P) - 1) P.resize(n + 1);
		if(n < ssize(Q) - 1) Q.resize(n + 1);
		poly tQ = Q;
		for(int i = 1; i < ssize(tQ); i += 2) tQ[i] = -tQ[i];
		P = conv(P, tQ), Q = conv(Q, tQ);
		int p = int(n % 2);
		for(int i = p; i < ssize(P); i += 2) P[i / 2] = P[i];
		for(int i = 0; i < ssize(Q); i += 2) Q[i / 2] = Q[i];
		P.resize((size(P) + 1 - p) / 2);
		Q.resize((size(Q) + 1) / 2);
		n /= 2;
	}
	return (P.empty() ? mint(0) : P[0] / Q[0]);
}

// [y^n] P(y,x) / Q(y,x). Requires n >= 0, nonempty rectangular inputs and Q[0][0] != 0.
poly BostanMori(int n, polyY P, polyY Q){
	if(n == 0){
		return P[0] / Q[0];
	}
	polyY tQ = Q;
	for(int i = 1; i < ssize(tQ); i += 2){
		tQ[i] = poly{} - tQ[i];
	}
	polyY nxtP = conv(P, tQ), nxtQ = conv(Q, tQ);
	P.clear(), Q.clear();
	for(int i = n % 2; i <= n && i < ssize(nxtP); i += 2) P.pb(move(nxtP[i]));
	for(int i = 0; i <= n && i < ssize(nxtQ); i += 2) Q.pb(move(nxtQ[i]));
	if(P.empty()) P.pb(size(nxtP[0]));
	return BostanMori(n / 2, move(P), move(Q));
}

// Transposed coefficient extraction. Requires n >= 0 and Q[0][0] != 0.
// Pad Q to n + 1 equally long rows in y.
polyY BostanMoriT(int n, poly P, polyY Q){
	if(n == 0){
		P.resize(size(Q[0]) * 2 - 1);
		return {convT(P, inv(Q[0]))};
	}
	polyY tQ = Q;
	for(int i = 1; i < ssize(tQ); i += 2){
		tQ[i] = poly{} - tQ[i];
	}
	polyY rQ = conv(Q, tQ);
	polyY nxtQ(n / 2 + 1, poly(size(rQ[0])));
	rep(i, 0, ssize(nxtQ) - 1){
		nxtQ[i] = rQ[i * 2];
	}
	polyY nxtP = BostanMoriT(n / 2, P, nxtQ);
	polyY res(size(tQ) * 2 - 1, poly(size(nxtP[0])));
	rep(i, 0, ssize(nxtP) - 1){
		res[i * 2 + n % 2] = nxtP[i];
	}
	return convT(res, tQ);
}

// Compositional inverse modulo x^size(_F). Requires 2 <= size(_F) < S, _F[0] == 0 and _F[1] != 0.
poly composite_inverse(const poly &_F){
	int n = int(size(_F));
	mint v = 1 / _F[1];

	poly F = v * _F;
	polyY P, Q;
	rep(i, 0, n - 1){
		P.pb(poly{i == 0, 0});
		Q.pb(poly{i == 0, -F[i]});
	}
	poly res = BostanMori(n, P, Q);
	poly tmp(n - 1);
	rep(k, 2, n){
		mint val = res[k];
		val = val * n * iv[k];
		tmp[n - k] = val;
	}

	poly ans = qpow(tmp, -mint(1) / n);
	ans.insert(ans.begin(), 0);
	ans.resize(n);

	mint pw = 1;
	rep(i, 1, ssize(ans) - 1){
		pw *= v;
		ans[i] *= pw;
	}
	return ans;
}

// G(F(x)) modulo x^size(G). Requires nonempty G and supported internal transforms.
poly composite(const poly &G, const poly &F){
	poly P = G;
	polyY Q;
	rep(i, 0, ssize(G) - 1){
		Q.pb(poly{(i == 0), (i < ssize(F) ? -F[i] : 0)});
	}

	auto res = BostanMoriT(int(size(G)) - 1, P, Q);
	poly ans(size(res));
	rep(i, 0, ssize(ans) - 1){
		ans[i] = res[ssize(ans) - 1 - i][0];
	}
	return ans;
}

struct dc{
	poly f, g, res;
	vector<poly> pf, pg;
	int n, p;

	dc(int _n) : f(_n), g(_n), res(_n), n(_n), p(0) {}

	// At most min(n, S - 1) pushes.
	void push(mint v0, mint v1){
		f[p] = v0;
		res[p] += f[0] * v1 + g[0] * v0;
		g[p] = v1;
		p ++;

		int t = __builtin_ctz(p), len = 1 << t;
		poly A(f.begin() + p - len, f.begin() + p), B(g.begin() + p - len, g.begin() + p);
		A.resize(len * 2), B.resize(len * 2);
		A.DIF(t + 1), B.DIF(t + 1);
		if(len == p){
			if(len > 1){
				poly C(f.begin(), f.begin() + p), D(g.begin(), g.begin() + p);
				C.DIF(t), D.DIF(t);
				pf.pb(C), pg.pb(D);
			}
			rep(i, 0, len * 2 - 1){
				A[i] *= B[i];
			}
		} else{
			auto &C = pf[t], &D = pg[t];
			rep(i, 0, len * 2 - 1){
				A[i] = (A[i] * D[i] + B[i] * C[i]);
			}
		}
		A.DIT(t + 1);
		rep(i, p, min(p + len, n) - 1){
			res[i] += A[i - (p - len)] * ip2[t + 1];
		}
	}
};

// With 1 <= n <= S, push the input series coefficients in order, starting with 1.
struct dln : dc{
	poly res;
	bool bg;

	dln(int _n): dc(_n), res(_n), bg(0) {}

	void push(mint v){
		if(!bg){
			bg = 1;
		} else{
			res[p + 1] = v - (p == 0 ? 0 : dc::res[p - 1] * iv[p + 1]);
			dc::push((p + 1) * res[p + 1], v);
		}
	}
};

// With 1 <= n <= S, push the input series coefficients in order, starting with 0.
struct dexp : dc{
	poly res;

	dexp(int _n): dc(_n), res(_n) {}

	void push(mint v){
		if(!res[0].x){
			res[0] = 1;
		} else{
			v *= (p + 1);
			dc::push(v, res[p]);
			res[p] = dc::res[p - 1] * iv[p];
		}
	}
};

// With 1 <= n <= S, push the input series coefficients in order, starting with a nonzero term.
struct dinv : dc{
	poly res;
	mint bg;

	dinv(int _n): dc(_n), res(_n), bg(0) {}

	void push(mint v){
		if(!bg.x){
			bg = v, res[0] = 1 / v;
		} else{
			dc::push(v, res[p]);
			res[p] = -res[0] * dc::res[p - 1];
		}
	}
};

// 0 <= m < S. Returns 0 when n is outside [0, m].
mint C(int m, int n){
	return (n < 0 || n > m ? 0 : fac[m] * ifac[n] * ifac[m - n]);
}

// 0 <= n < S, and m - i must fit in int for 0 <= i < n.
mint binom(int m, int n){
	mint res = ifac[n];
	rep(i, 0, n - 1){
		res *= m - i;
	}
	return res;
}

signed main(){
	#ifdef LOCAL
	assert( freopen(".in", "r", stdin) );
	assert( freopen(".out", "w", stdout) );
	#endif
	ios::sync_with_stdio(0);
	cin.tie(0);

	poly a{1, 2, 3};
	cout << a << '\n';
}
