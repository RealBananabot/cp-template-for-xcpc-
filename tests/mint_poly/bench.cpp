// Appended after the template by run.py.
int main(){
	const int n = S / 2;
	mt19937 rng(12345);
	poly a(n), b(n);
	for(auto &v:a) v = int(rng() % mod);
	for(auto &v:b) v = int(rng() % mod);
	a[0] = 1;
	auto run = [&](string name, auto fn){
		auto start = chrono::steady_clock::now();
		auto result = fn();
		auto end = chrono::steady_clock::now();
		u64 hash = 1;
		size_t len = 0;
		auto add = [&](const poly &row){
			hash = hash * 1000003 + size(row);
			len += size(row);
			for(mint v:row) hash = hash * 1000003 + v.x;
		};
		if constexpr(is_same_v<decltype(result), polyY>){
			hash = hash * 1000003 + size(result);
			for(const auto &row:result) add(row);
		} else{
			add(result);
		}
		cout << name << ' ' << chrono::duration<double, milli>(end - start).count()
		     << ' ' << len << ' ' << hash << '\n';
	};
	run("NTT-roundtrip", [&]{
		poly t = a;
		t.resize(S);
		t.DIF(LG);
		t.DIT(LG);
		return t;
	});
	run("conv", [&]{return conv(a, b);});
	run("inv", [&]{return inv(a);});
	run("division-series", [&]{return b / a;});
	a[0] = 0;
	run("exp", [&]{return exp(a);});

	const int m = 1 << 16, points = 1 << 14, order = 1 << 12;
	poly f(a.begin(), a.begin() + m), g(b.begin(), b.begin() + m);
	f[0] = 1;
	run("series-log", [&]{return ln(f);});
	run("series-sqrt", [&]{return sqrt(f);});
	run("series-power", [&]{return qpow(f, 3);});
	run("convT", [&]{return convT(f, poly(g.begin(), g.begin() + m / 2));});
	poly dividend(a.begin(), a.begin() + 2 * m);
	g.back() = 1;
	run("polynomial-division", [&]{
		auto [q, r] = division(dividend, g);
		return polyY{move(q), move(r)};
	});
	poly x(b.begin(), b.begin() + points);
	poly values(a.begin(), a.begin() + points);
	run("multipoint-eval", [&]{return multipoint_eval(values, x);});

	polyY matrix(64, poly(128)), kernel(35, poly(64));
	for(auto &row:matrix) for(auto &v:row) v = int(rng() % mod);
	for(auto &row:kernel) for(auto &v:row) v = int(rng() % mod);
	run("conv-2d", [&]{return conv(matrix, kernel);});
	run("convT-2d", [&]{return convT(matrix, kernel);});

	poly p(a.begin(), a.begin() + order), q(b.begin(), b.begin() + order);
	q[0] = 1;
	const i64 index = 1000000000000;
	run("bostan-mori", [&]{return poly{BostanMori(index, p, q)};});

	const int h = 1024;
	poly inner(a.begin(), a.begin() + h), outer(b.begin(), b.begin() + h);
	inner[1] = 7;
	polyY numerator(h, poly(2)), denominator(h, poly(2));
	rep(i, 0, h - 1){
		numerator[i][0] = outer[i];
		denominator[i] = poly{i == 0, -inner[i]};
	}
	run("bostan-mori-2d", [&]{return BostanMori(h - 1, numerator, denominator);});
	run("bostan-mori-transpose", [&]{return BostanMoriT(h - 1, outer, denominator);});
	run("composition", [&]{return composite(outer, inner);});
	run("composition-inverse", [&]{return composite_inverse(inner);});

	run("online-conv", [&]{
		dc c(m);
		rep(i, 0, m - 1) c.push(f[i], g[i]);
		return move(c.res);
	});
	run("online-log", [&]{
		dln c(m);
		rep(i, 0, m - 1) c.push(f[i]);
		return move(c.res);
	});
	run("online-exp", [&]{
		dexp c(m);
		rep(i, 0, m - 1) c.push(a[i]);
		return move(c.res);
	});
	run("online-inv", [&]{
		dinv c(m);
		rep(i, 0, m - 1) c.push(f[i]);
		return move(c.res);
	});
	run("berlekamp-massey", [&]{
		bm c;
		rep(i, 0, order - 1){
			c.push(b[i]);
		}
		return move(c.res);
	});
}
