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
	for (const auto& x : v) os << (f++ ? ", " : "") << x;
	return os << "]";
}

template<typename ...Ar> void debug(char *s, Ar... x){
	auto f = [&](auto x){
		int dep = 0;
		while(*s && !(*s == ',' && dep == 0)){
			if(*s == '(') dep++;
			if(*s == ')') dep--;
			cerr << *s++;
		}
		cerr <<" = "<< x;
		if(*s == ','){
			cerr << *s ++;
		} else{
			cerr << endl;
		}
	};

	(f(x), ...);
}
#define gdb(...) debug((char*)#__VA_ARGS__, __VA_ARGS__)

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
		x = (u64)x * B.x % mod;
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
	mint operator - ()const {
		return mint() - *this;
	}

	friend ostream& operator << (ostream &os, const mint &obj){
		os << obj.x;
		return os;
	}
};

constexpr int LG = 21, S = 1 << LG;
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

struct poly : vector<mint>{
	using vector<mint>::vector;
	using polyY = vector<poly>;

	void DIF(int lg){
		for(int i = 1 << (lg - 1); i ; i /= 2){
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
		reverse(this -> begin() + 1, this -> end());
	}

	friend poly der(const poly &A){
		poly res = A;
		rep(i, 1, (int)res.size() - 1){
			res[i - 1] = res[i] * i;
		}
		res.pop_back();
		return res;
	}

	friend poly ints(const poly &A){
		poly res = A;
		res.pb();
		per(i, (int)res.size() - 1, 0){
			res[i] = res[i - 1] * iv[i];
		}
		res[0] = 0;
		return res;
	}

	poly& operator += (const poly &B){
		if(B.size() > this -> size()){
			this -> resize(B.size());
		}
		rep(i, 0, (int)B.size() - 1){
			(*this)[i] += B[i];
		}
		return *this;
	}

	friend poly operator + (const poly &A, const poly &B){
		return poly(A) += B;
	}

	poly& operator -= (const poly &B){
		if(B.size() > this -> size()){
			this -> resize(B.size());
		}
		rep(i, 0, (int)B.size() - 1){
			(*this)[i] -= B[i];
		}
		return *this;
	}

	friend poly operator - (const poly &A, const poly &B){
		return poly(A) -= B;
	}

	friend poly operator * (const mint &k, const poly &A){
		poly res = A;
		for(auto &x:res) x *= k;
		return res;
	}

	poly& operator *= (const poly &B){
		auto sz = this -> size();
		*this = conv(*this, B);
		this -> resize(sz);
		return *this;
	}

	friend poly operator * (const poly &A, const poly &B){
		return poly(A) *= B;
	}

	poly& operator /= (const poly &B){
		auto sz = this -> size();
		*this = conv(*this, inv(B));
		this -> resize(sz);
		return *this;
	}

	friend poly operator / (const poly &A, const poly &B){
		return poly(A) /= B;
	}

	friend poly conv(const poly &_A, const poly &_B){
		if(_A.size() <= 64 || _B.size() <= 64){
			poly res(_A.size() + _B.size() - 1);
			rep(i, 0, (int)_A.size() - 1){
				rep(j, 0, (int)_B.size() - 1){
					res[i + j] += _A[i] * _B[j];
				}
			}
			return res;
		}

		poly A = _A, B = _B;
		int len = A.size() + B.size() - 1;
		int lg = __lg(len * 2 - 1);
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

	friend poly inv(const poly &A){
		assert(A[0].x != 0);
		poly res{mint(1) / A[0]};
		while(res.size() < A.size()){
			int n = res.size(), lg = __lg(n * 4);
			poly buf(1 << lg);
			copy(res.begin(), res.end(), buf.begin());
			buf.DIF(lg);

			poly tmp{A.begin(), A.begin() + min(n * 2, (int)A.size())};
			tmp.resize(n * 4);
			tmp.DIF(lg);
			rep(i, 0, n * 4 - 1) tmp[i] *= buf[i] * buf[i];
			tmp.DIT(lg);

			res.resize(n * 2);
			rep(i, 0, n * 2 - 1){
				res[i] = 2 * res[i] - tmp[i] * ip2[lg];
			}
		}
		res.resize(A.size());
		return res;
	}


	// O(n \log^2 n)
	friend poly exp(const poly &A){
		assert(A[0].x == 0);
		int n = A.size();
		int lg = __lg(n * 2 - 1);

		poly a = A, res(A.size());
		rep(i, 0, (int)a.size() - 1) a[i] *= i;
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
	
	friend poly ln(const poly &A){
		assert(A[0].x == 1);
		poly res = der(A) / A;
		return ints(res);
	}
	
	friend poly sqrt(const poly &A){
		assert(A[0].x == 1);
		return exp(ip2[1] * ln(A));
	}

	friend poly qpow(const poly &A, mint k){
		if(k.x == 0){
			poly res(A.size());
			res[0] = 1;
			return res;
		}

		auto it = A.begin();
		while(it != A.end() && it -> x == 0){
			it ++;
		}
		int len = (it - A.begin()) * k.x;
		if(len >= (int)A.size()){
			return poly(A.size());
		} else{
			poly a{it, it + (A.size() - len)};
			mint t = a[0].qpow(k.x);
			a = t * exp(k * ln(1 / a[0] * a));
			a.insert(a.begin(), len, mint());
			return a;
		}
	}

	friend pair<poly, poly> division(const poly &_P, const poly &_Q){
		auto lenF = _P.size() - _Q.size() + 1;
		auto lenR = _Q.size() - 1;
		poly rP(_P.rbegin(), _P.rbegin() + lenF);
		poly rQ(_Q.rbegin(), _Q.rbegin() + min(_Q.size(), lenF));
		rQ.resize(lenF);
		poly F = rP / rQ;
		reverse(F.begin(), F.end());
		poly R = _P - conv(_Q, F);
		R.resize(lenR);
		return {F, R};
	}

	friend poly convT(const poly &_A, const poly &_B){
		poly A = _A, B = _B;
		int k = B.size() - 1;
		reverse(B.begin(), B.end());
		int lg = __lg(A.size() * 2 - 1);
		A.resize(1 << lg), B.resize(1 << lg);
		A.DIF(lg), B.DIF(lg);
		rep(i, 0, (1 << lg) - 1){
			A[i] *= B[i];
		}
		A.DIT(lg);
		A = poly{A.begin() + k, A.begin() + _A.size()};
		for(auto &x:A){
			x *= ip2[lg];
		}
		return A;
	}

	friend poly multipoint_eval(poly f, const poly &x){
		vector<poly> Q(x.size() * 2 - 1);
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
		dfs0(dfs0, 0, 0, x.size());

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
		Q[0].resize(f.size());
		poly g = inv(Q[0]);
		g.resize(f.size());
		f.resize(f.size() * 2 - 1);
		g = convT(f, g);
		dfs1(dfs1, 0, 0, x.size(), g);

		return res;
	}

	friend polyY conv(const polyY &A, const polyY &B){
		int x = A.size() + B.size() - 1;
		int y = A[0].size() + B[0].size() - 1;
		poly tmpa(A.size() * y), tmpb(B.size() * y);
		rep(i, 0, int(A.size()) - 1){
			rep(j, 0, int(A[i].size()) - 1){
				tmpa[i * y + j] = A[i][j];
			}
		}
		rep(i, 0, int(B.size()) - 1){
			rep(j, 0, int(B[i].size()) - 1){
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

	friend polyY convT(const polyY &A, const polyY &B){
		int n = A.size(), m = A[0].size(), k = B[0].size() - 1;
		poly a(n * m), b(B.size() * m);
		rep(i, 0, n - 1){
			rep(j, 0, m - 1){
				a[i * m + j] = A[i][j];
			}
		}
		rep(i, 0, int(B.size()) - 1){
			rep(j, 0, int(B[i].size()) - 1){
				b[i * m + j] = B[B.size() - 1 - i][k - j];
			}
		}
		int lg = __lg(a.size() * 2 - 1);
		a.resize(1 << lg), b.resize(1 << lg);
		a.DIF(lg), b.DIF(lg);
		rep(i, 0, (1 << lg) - 1){
			a[i] *= b[i];
		}
		a.DIT(lg);

		polyY res(A.size() - B.size() + 1, poly(A[0].size() - B[0].size() + 1));
		rep(i, 0, int(res.size()) - 1){
			rep(j, 0, int(res[i].size()) - 1){
				res[i][j] = a[(i + B.size() - 1) * m + j + k] * ip2[lg];
			}
		}
		return res;
	}

	friend poly BostanMori(int n, polyY P, polyY Q){
		if(n == 0){
			return P[0] * inv(Q[0]);
		}
		polyY tQ = Q;
		for(int i = 1; i < int(tQ.size()); i += 2){
			tQ[i] = poly{} - tQ[i];
		}
		polyY nxtP = conv(P, tQ), nxtQ = conv(Q, tQ);
		P = polyY(n / 2 + 1, poly(nxtP[0].size())), Q = P;
		rep(i, 0, int(P.size()) - 1){
			P[i] = nxtP[i * 2 + n % 2];
			Q[i] = nxtQ[i * 2];
		}
		return BostanMori(n / 2, P, Q);
	}

	friend polyY BostanMoriT(int n, poly P, polyY Q){
		if(n == 0){
			P.resize(Q[0].size() * 2 - 1);
			return {convT(P, inv(Q[0]))};
		}
		polyY tQ = Q;
		for(int i = 1; i < int(tQ.size()); i += 2){
			tQ[i] = poly{} - tQ[i];
		}
		polyY rQ = conv(Q, tQ);
		polyY nxtQ(n / 2 + 1, poly(rQ[0].size()));
		rep(i, 0, int(nxtQ.size()) - 1){
			nxtQ[i] = rQ[i * 2];
		}
		polyY nxtP = BostanMoriT(n / 2, P, nxtQ);
		polyY res(tQ.size() * 2 - 1, poly(nxtP[0].size()));
		rep(i, 0, int(nxtP.size()) - 1){
			res[i * 2 + n % 2] = nxtP[i];
		}
		return convT(res, tQ);
	}

	friend poly composite_inverse(const poly &_F){
		int n = _F.size();
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
		rep(i, 1, int(ans.size()) - 1){
			pw *= v;
			ans[i] *= pw;
		}
		return ans;
	}

	// G(F(x))
	friend poly composite(const poly &G, const poly &F){
		poly P = G;
		polyY Q;
		rep(i, 0, int(G.size()) - 1){
			Q.pb(poly{(i == 0), (i < int(F.size())? -F[i]: 0)});
		}

		auto res = BostanMoriT(G.size() - 1, P, Q);
		poly ans(res.size());
		rep(i, 0, int(ans.size()) - 1){
			ans[i] = res[ans.size() - 1 - i][0];
		}
		return ans;
	}
};

struct dc{
	poly f, g, res;
	vector<poly> pf, pg;
	int n, p;
	
	dc(int _n) : f(_n), g(_n), res(_n), n(_n), p(0) {}

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

struct dln : dc{
	poly res;
	bool bg;

	dln(int _n): dc(_n), res(_n), bg(0) {}

	void push(mint v){
		if(!bg){
			bg = 1;
		} else{
			res[p + 1] = v - (p == 0? 0: dc::res[p - 1] * iv[p + 1]);
			dc::push((p + 1) * res[p + 1], v);
		}
	}
};

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

mint C(int m, int n){
	return n < 0 || n > m? 0: fac[m] * ifac[n] * ifac[m - n];
}

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
	cout << a <<'\n';
}
