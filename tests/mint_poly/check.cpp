// Appended after mint_poly.cpp by run.py.
namespace checks{
mt19937 rng(20261119);

poly sample(int n){
	poly a(n);
	for(auto &x:a) x = int(rng() % mod);
	return a;
}

void equal(const poly &a, const poly &b, source_location at = source_location::current()){
	if(size(a) != size(b)){
		cerr << at.function_name() << " line " << at.line() << " size " << size(a) << " expected " << size(b) << endl;
		abort();
	}
	rep(i, 0, ssize(a) - 1){
		if(a[i].x != b[i].x){
			cerr << at.function_name() << " line " << at.line() << " coefficient " << i << " got " << a[i] << " expected " << b[i] << endl;
			abort();
		}
	}
}

poly mul(const poly &a, const poly &b, int n){
	poly c(n);
	rep(i, 0, ssize(a) - 1){
		rep(j, 0, min(int(size(b)), n - i) - 1) c[i + j] += a[i] * b[j];
	}
	return c;
}

poly quotient_ref(const poly &p, const poly &q, int n){
	poly a(n);
	mint inv0 = 1 / q[0];
	rep(i, 0, n - 1){
		mint v = (i < ssize(p) ? p[i] : 0);
		rep(j, 1, min(i, int(size(q)) - 1)) v -= q[j] * a[i - j];
		a[i] = v * inv0;
	}
	return a;
}

poly exp_ref(const poly &a){
	poly e(size(a));
	e[0] = 1;
	rep(i, 1, ssize(a) - 1){
		rep(j, 1, i) e[i] += j * a[j] * e[i - j];
		e[i] /= i;
	}
	return e;
}

void ntt(int lg){
	const int n = 1 << lg;
	poly a = sample(n), t = a;
	t.DIF(lg);
	if(lg <= 6){
		mint root = mint(3).qpow((mod - 1) / n);
		rep(i, 0, n - 1){
			int j = 0;
			rep(k, 0, lg - 1) j = (j << 1) | ((i >> k) & 1);
			mint step = root.qpow(i), power = 1, v = 0;
			for(mint x:a){
				v += x * power;
				power *= step;
			}
			assert(t[j].x == v.x);
		}
	}
	t.DIT(lg);
	equal(ip2[lg] * t, a);
}

void scalar(){
	int x = 2;
	assert(chmin(x, 1) && !chmin(x, 1));
	assert(chmax(x, 2) && !chmax(x, 2));
	ostringstream os;
	os << poly{1, -1};
	assert(os.str() == "[1, 998244352]");
	auto old = cerr.rdbuf(os.rdbuf());
	gdb(x);
	cerr.rdbuf(old);
	assert(os.str().ends_with("x = 2\n"));
	assert(mint().x == 0 && mint(-1).x == mod - 1);
	assert(mint(0).qpow(0).x == 1);
	for(int a:{INT_MIN, -mod, 0, mod, INT_MAX}){
		assert(mint(a).x == (i64(a) % mod + mod) % mod);
	}
	rep(i, 0, 999){
		i64 a = rng() % mod, b = 1 + rng() % (mod - 1);
		mint u = int(a), v = int(b);
		assert((u + v).x == (a + b) % mod);
		assert((u - v).x == (a - b + mod) % mod);
		assert((-u).x == (mod - a) % mod);
		assert((u * v).x == a * b % mod);
		assert((u / v).x * b % mod == a);
		i64 power = 1;
		int e = i % 17;
		rep(j, 1, e) power = power * a % mod;
		assert(u.qpow(e).x == power);
	}
	poly row{1};
	rep(n, 0, 50){
		rep(k, 0, n){
			assert(C(n, k).x == row[k].x);
			assert(binom(n, k).x == row[k].x);
		}
		row = mul(row, {1, 1}, n + 2);
	}
	assert(C(5, -1).x == 0 && C(5, 6).x == 0 && C(S - 1, 0).x == 1);
	assert(C(S - 1, S - 1).x == 1 && C(S - 1, S - 2).x == S - 1);
	i64 m = i64(INT_MIN) + 1;
	assert(binom(int(m), 2).x == m * (m - 1) / 2 % mod);
}

void series(int n){
	poly a = sample(n), b = sample(n + 3);
	a[0] = 1;
	equal(conv(a, b), mul(a, b, 2 * n + 2));
	equal(a * b, mul(a, b, n));
	poly sum = b;
	rep(i, 0, n - 1) sum[i] += a[i];
	equal(a + b, sum);
	equal(sum - a, b);
	poly update = a;
	assert(&(update += b) == &update);
	equal(update, sum);
	assert(&(update -= b) == &update);
	poly padded = a;
	padded.resize(size(b));
	equal(update, padded);
	update = a;
	assert(&(update *= b) == &update);
	equal(update, mul(a, b, n));
	update = b;
	assert(&(update /= a) == &update);
	equal(update, quotient_ref(b, a, int(size(b))));
	equal(a / b, quotient_ref(a, b, n));
	poly square = mul(a, a, n), one(n);
	one[0] = 1;
	equal(mul(inv(a), a, n), one);
	poly num(b.begin(), b.begin() + n);
	equal(mul(num / a, a, n), num);
	equal(sqrt(square), a);
	equal(qpow(a, 3), mul(square, a, n));
	equal(qpow(a, 0), one);
	poly d(max(0, n - 1));
	rep(i, 1, n - 1) d[i - 1] = i * a[i];
	equal(der(a), d);
	a[0] = 0;
	equal(ints(d), a);
	poly e = exp_ref(a);
	equal(exp(a), e);
	equal(ln(e), a);
	poly q = sample(n), r = sample(n - 1), divisor = sample(n);
	divisor.back() = 1;
	auto [quot, rem] = division(mul(q, divisor, 2 * n - 1) + r, divisor);
	equal(quot, q);
	equal(rem, r);
	poly corr(4);
	rep(i, 0, 3) rep(j, 0, n - 1) corr[i] += b[i + j] * a[j];
	equal(convT(b, a), corr);
	poly points{0, 1, 1, -1, 7}, values;
	for(mint x:points){
		mint y = 0;
		per(i, n - 1, 0) y = y * x + a[i];
		values.pb(y);
	}
	equal(multipoint_eval(a, points), values);
}

void bostan(){
	assert(BostanMori(10, poly{0, 1}, poly{1, -1, -1}).x == 55);
	assert(BostanMori(17, poly{}, poly{1, -1}).x == 0);
	assert(BostanMori(0, poly{6}, poly{3}).x == 2);
	assert(BostanMori(1, poly{6}, poly{3}).x == 0);
	assert(BostanMori(3, poly{0, 0, 0, 8}, poly{2}).x == 4);
	const i64 index = numeric_limits<i64>::max();
	assert(BostanMori(index, poly{1}, poly{1, -2}).x == mint(2).qpow(int(index % (mod - 1))).x);
	for(int n:{1, 2, 7, 64, 65, 129}){
		poly p = sample(n + 7), q = sample(n);
		q[0] = 7;
		poly a = quotient_ref(p, q, 300);
		for(int i:{0, 1, 2, 63, 64, 65, 127, 128, 129, 299}){
			assert(BostanMori(i, p, q).x == a[i].x);
		}
	}
}

void products_2d(){
	for(int h:{1, 2, 7, 17}){
		for(int w:{1, 4, 9}){
			int p = (h + 1) / 2, q = (w + 1) / 2;
			polyY a(h), b(p), prod(h + p - 1, poly(w + q - 1));
			for(auto &row:a) row = sample(w);
			for(auto &row:b) row = sample(q);
			rep(i, 0, h - 1) rep(j, 0, w - 1) rep(k, 0, p - 1) rep(l, 0, q - 1){
				prod[i + k][j + l] += a[i][j] * b[k][l];
			}
			polyY c = conv(a, b), t = convT(a, b);
			assert(size(c) == size(prod) && ssize(t) == h - p + 1);
			rep(i, 0, ssize(c) - 1) equal(c[i], prod[i]);
			rep(i, 0, h - p){
				poly row(w - q + 1);
				rep(j, 0, w - q) rep(k, 0, p - 1) rep(l, 0, q - 1){
					row[j] += a[i + k][j + l] * b[k][l];
				}
				equal(t[i], row);
			}
		}
	}
}

void rational_2d(){
	// Expand 1/Q in two variables directly and test both coefficient maps.
	rep(n, 0, 12){
		const int w = 1 + n % 5;
		polyY p(n + 1), q(n + 1), invq(n + 1, poly(w));
		for(auto &row:p) row = sample(w);
		for(auto &row:q) row = sample(w);
		q[0][0] = 7;
		mint inv0 = 1 / q[0][0];
		rep(i, 0, n) rep(j, 0, w - 1){
			mint v = (i == 0 && j == 0);
			rep(k, 0, i) rep(l, 0, j){
				if(k || l) v -= q[k][l] * invq[i - k][j - l];
			}
			invq[i][j] = v * inv0;
		}
		poly expected(w), weight = sample(w);
		rep(i, 0, n) rep(j, 0, w - 1) rep(k, 0, j){
			expected[j] += p[i][k] * invq[n - i][j - k];
		}
		poly v = BostanMori(n, p, q);
		assert(ssize(v) >= w);
		v.resize(w);
		equal(v, expected);
		polyY t = BostanMoriT(n, weight, q);
		assert(ssize(t) == n + 1);
		rep(i, 0, n){
			poly row(w);
			rep(j, 0, w - 1) rep(k, j, w - 1) row[j] += weight[k] * invq[n - i][k - j];
			equal(t[i], row);
		}
	}
}

void shapes(){
	for(int n:{1, 2, 9, 65}){
		poly a = sample(n);
		for(int m:{1, 3, 37, 130}){
			poly p = sample(m), f = sample(m), values, g(n);
			p[0] = 0;
			if(m > 1) p[1] = p[0];
			for(mint x:p){
				mint v = 0;
				per(i, n - 1, 0) v = v * x + a[i];
				values.pb(v);
			}
			equal(multipoint_eval(a, p), values);
			per(i, n - 1, 0){
				g = mul(g, f, n);
				g[0] += a[i];
			}
			equal(composite(a, f), g);
		}
	}
	poly a{0, 0, 3, 1, 4, 1, 5}, b = a;
	rep(i, 2, 4){
		b = mul(b, a, int(size(a)));
		equal(qpow(a, i), b);
	}
	equal(ints(poly{}), {0});
	equal(qpow(poly(5), 1), poly(5));
	equal(convT(poly{3}, poly{7}), {21});
	auto [q, r] = division(poly{6, 3, 9}, poly{3});
	equal(q, {2, 1, 3});
	equal(r, {});
}

void higher(){
	poly::polyY a{{1,2,3},{4,5,6},{7,8,9}}, b{{2,3},{1,4}};
	auto c = conv(a,b), t = convT(a,b);
	poly::polyY prod(4, poly(4)), corr(2, poly(2));
	rep(i,0,2) rep(j,0,2) rep(k,0,1) rep(l,0,1)
		prod[i+k][j+l] += a[i][j] * b[k][l];
	rep(i,0,1) rep(j,0,1) rep(k,0,1) rep(l,0,1)
		corr[i][j] += a[i+k][j+l] * b[k][l];
	assert(size(c) == size(prod) && size(t) == size(corr));
	rep(i,0,3) equal(c[i], prod[i]);
	rep(i,0,1) equal(t[i], corr[i]);
	equal(BostanMori(0, poly::polyY{{1,0,0}}, {{1,-1}}), {1,1,1});
	// [y^3] 1/(1-2y), with scalar polynomial coefficients.
	equal(BostanMori(3, poly::polyY{{1},{0},{0},{0}}, {{1},{-2},{0},{0}}), {8});
	equal(BostanMori(3, poly::polyY{{1}}, {{1},{-2}}), {8});
	equal(BostanMori(3, poly::polyY{{1}}, {{1,0},{-2,-1}}), {8,12,6,1});
	equal(BostanMori(1, poly::polyY{{3,4}}, {{1}}), {0,0});
	const int index = 1000000000;
	equal(BostanMori(index, poly::polyY{{1}}, {{1},{-2}}), {mint(2).qpow(index)});
	auto adj = BostanMoriT(0, poly{3,0}, {{1,0}});
	assert(size(adj) == 1);
	equal(adj[0], {3,0});
	for(int n:{2, 5, 16, 33}){
		poly f = sample(n), g = sample(n), expected(n);
		f[0] = 0;
		f[1] = 7;
		per(i, n - 1, 0){
			expected = mul(expected, f, n);
			expected[0] += g[i];
		}
		equal(composite(g, f), expected);
		poly inverse = composite_inverse(f), x(n);
		x[1] = 1;
		equal(composite(f, inverse), x);
		equal(composite(inverse, f), x);
	}
}

void online(int n, bool sparse){
	poly f = sample(n), g = sample(n);
	if(sparse){
		rep(i, 1, n - 1){
			if(i % 3) f[i] = 0;
			if(i % 5) g[i] = 0;
		}
	}
	dc product(n);
	dln logarithm(n);
	dexp exponential(n);
	dinv reciprocal(n);
	f[0] = 1;
	g[0] = 0;
	poly c = mul(f, g, n), l(n), e = exp_ref(g);
	poly v = quotient_ref({1}, 7 * f, n);
	rep(i, 1, n - 1){
		mint val = i * f[i];
		rep(j, 1, i - 1) val -= (i - j) * f[j] * l[i - j];
		l[i] = val / i;
	}
	rep(i, 0, n - 1){
		product.push(f[i], g[i]);
		logarithm.push(f[i]);
		exponential.push(g[i]);
		reciprocal.push(7 * f[i]);
		assert(product.res[i].x == c[i].x && logarithm.res[i].x == l[i].x);
		assert(exponential.res[i].x == e[i].x && reciprocal.res[i].x == v[i].x);
	}
}

bool recurrence_exists(const poly &a, int n, int len){
	polyY A(n - len, poly(len + 1));
	rep(i, len, n - 1){
		rep(j, 0, len - 1){
			A[i - len][j] = a[i - 1 - j];
		}
		A[i - len][len] = a[i];
	}
	int r = 0;
	rep(j, 0, len - 1){
		int k = r;
		while(k < ssize(A) && !A[k][j].x){
			k++;
		}
		if(k == ssize(A)){
			continue;
		}
		swap(A[r], A[k]);
		const mint inv = A[r][j].qpow();
		rep(i, r + 1, ssize(A) - 1){
			const mint t = A[i][j] * inv;
			rep(k, j, len){
				A[i][k] -= t * A[r][k];
			}
		}
		r++;
	}
	rep(i, r, ssize(A) - 1){
		if(A[i][len].x){
			return false;
		}
	}
	return true;
}

void check_recurrence(const poly &a){
	bm c;
	assert(c.res.empty());
	rep(n, 1, ssize(a)){
		c.push(a[n - 1]);
		const int len = int(size(c.res));
		assert(len <= n);
		rep(i, len, n - 1){
			mint v = 0;
			rep(j, 0, len - 1){
				v += c.res[j] * a[i - 1 - j];
			}
			assert(v.x == a[i].x);
		}
		if(len){
			// Feasibility is monotone in the order, by appending zero coefficients.
			assert(!recurrence_exists(a, n, len - 1));
		}
	}
}

void berlekamp_massey(){
	bm c;
	c.push(0);
	assert(c.res.empty());
	c.push(-1);
	equal(c.res, {0, 0});
	c.push(0);
	c.push(0);
	equal(c.res, {0, 0});

	poly a(6);
	rep(i, 0, 728){
		int t = i;
		for(auto &v:a){
			v = t % 3 - 1;
			t /= 3;
		}
		check_recurrence(a);
	}
	rep(i, 0, 249){
		const int n = 1 + int(rng() % 24);
		a = sample(n);
		check_recurrence(a);
		const int len = 1 + int(rng() % n);
		const poly res = sample(len);
		rep(j, len, n - 1){
			a[j] = 0;
			rep(k, 0, len - 1){
				a[j] += res[k] * a[j - 1 - k];
			}
		}
		check_recurrence(a);
	}

	a.assign(64, 0);
	a[1] = 1;
	rep(i, 2, ssize(a) - 1){
		a[i] = a[i - 1] + a[i - 2];
	}
	bm f;
	rep(i, 0, 15){
		f.push(a[i]);
	}
	equal(f.res, {1, 1});
	poly Q{1};
	for(mint v:f.res){
		Q.pb(-v);
	}
	poly P(a.begin(), a.begin() + ssize(f.res));
	P *= Q;
	rep(i, 0, ssize(a) - 1){
		assert(BostanMori(i, P, Q).x == a[i].x);
	}
}

void limits(){
	const int n = S / 2;
	{
		poly a(n, mint(1)), t = conv(a, a);
		assert(ssize(t) == S - 1);
		rep(i, 0, S - 2) assert(t[i].x == u32(min(i + 1, S - 1 - i)));
	}
	{
		poly a(n);
		a[0] = 1;
		a /= poly{1, -1};
		assert(ssize(a) == n);
		for(mint x:a) assert(x.x == 1);
		a.assign(S, mint(6));
		a /= poly{3};
		assert(ssize(a) == S);
		for(mint x:a) assert(x.x == 2);
	}
	{
		poly a(n);
		a[1] = 1;
		poly e = exp(a);
		assert(ssize(e) == n);
		rep(i, 0, n - 1) assert(e[i].x == ifac[i].x);
	}
	{
		dc c(S - 1);
		rep(i, 0, S - 2){
			c.push(1, 1);
			assert(c.res[i].x == u32(i + 1));
		}
	}
	{
		const int k = n - 1;
		poly q(k + 1);
		q[0] = 1, q[k] = -2;
		assert(BostanMori(3 * i64(k) + 1, poly{0, 7}, q).x == 56);
	}
	{
		bm c;
		rep(i, 0, S - 1){
			c.push(0);
		}
		assert(c.res.empty());
		c.push(-1);
		assert(ssize(c.res) == S + 1);
		c.push(1);
		assert(ssize(c.res) == S + 1 && c.res[0].x == mod - 1);
		rep(i, 1, S){
			assert(!c.res[i].x);
		}
	}
	cout << "mint_poly upper limits passed\n";
}
}

int main(int argc, char **argv){
	using namespace checks;
	if(argc == 2 && string(argv[1]) == "short-divisor"){
		equal(poly{1,0,0} / poly{1,-1}, {1,1,1});
		return 0;
	}
	if(argc == 2 && string(argv[1]) == "single-ntt"){
		poly a{7};
		a.DIF(0);
		a.DIT(0);
		equal(a, {7});
		return 0;
	}
	if(argc == 2 && string(argv[1]) == "limits"){
		limits();
		return 0;
	}
	scalar();
	for(int n:{1, 2, 7, 63, 64, 65, 127, 128, 129, 255, 256, 257}){
		series(n);
		online(n, false);
		online(n, true);
	}
	for(int lg:{0, 1, 2, 3, 6, 7, 10}) ntt(lg);
	equal(qpow(poly{0,0,0,1}, mod - 1), poly(4));
	bostan();
	products_2d();
	rational_2d();
	shapes();
	higher();
	berlekamp_massey();
	cout << "mint_poly checks passed\n";
}
