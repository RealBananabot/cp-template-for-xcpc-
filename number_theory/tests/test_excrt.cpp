#include "../exgcd.cpp"
#include "../excrt.cpp"

int main(){
	int brute_cases = 0, large_cases = 0, bezout_cases = 0;
	auto norm = [](i64 x, i64 m){
		return i64((i128(x) % m + m) % m);
	};
	auto check = [&](pair<i64, i64> A, pair<i64, i64> B, bool brute){
		auto before_a = A, before_b = B;
		auto res = crt_merge(A, B);
		assert(A == before_a && B == before_b);
		assert(crt_merge(B, A) == res);
		i64 a = A.fi, b = B.fi, r = norm(A.se, a), s = norm(B.se, b);
		i64 g = gcd(a, b), m = a / g * b;
		if(brute){
			i64 expected = -1;
			for(i64 x = 0; x < m; x++){
				if(x % a == r && x % b == s){
					expected = x;
					break;
				}
			}
			assert((res == (expected == -1 ? pair<i64, i64>{-1, -1} : pair<i64, i64>{m, expected})));
			brute_cases++;
		} else{
			if((s - r) % g){
				assert((res == pair<i64, i64>{-1, -1}));
			} else{
				assert(res.fi == m && 0 <= res.se && res.se < m);
				assert(res.se % a == r && res.se % b == s);
			}
			large_cases++;
		}
	};

	rep(a, 1, 20) rep(b, 1, 20) rep(r, 0, a - 1) rep(s, 0, b - 1){
		check({a, r}, {b, s}, true);
	}
	rep(a, 1, 16) rep(b, 1, 16) rep(r, -4, 4) rep(s, -4, 4){
		check({a, r}, {b, s}, true);
	}
	mt19937_64 rng(4777);
	uniform_int_distribution<i64> any_int(LLONG_MIN, LLONG_MAX);
	rep(tc, 1, 3000){
		i64 a = i64(rng() % 40) + 1, b = i64(rng() % 40) + 1;
		check({a, any_int(rng)}, {b, any_int(rng)}, true);
	}

	const i64 M = 217678233600000000LL; // 2^20 * 3^12 * 5^8
	vector<i64> divisors;
	for(i64 a = 1; a <= (1 << 20); a *= 2){
		for(i64 b = 1; b <= 531441; b *= 3){
			for(i64 c = 1; c <= 390625; c *= 5) divisors.pb(a * b * c);
		}
	}
	rep(tc, 1, 10000){
		i64 a = divisors[rng() % size(divisors)], b = divisors[rng() % size(divisors)];
		check({a, any_int(rng)}, {b, any_int(rng)}, false);
		i64 z = any_int(rng);
		check({a, z}, {b, z}, false);
	}
	vector<i64> edge{0, 1, 2, LLONG_MAX / 2, LLONG_MAX - 1, LLONG_MAX};
	for(i64 a:edge) for(i64 b:edge){
		i64 x, y, g = exgcd(a, b, x, y);
		assert(g == gcd(a, b));
		assert(i128(a) * x + i128(b) * y == g);
		bezout_cases++;
	}
	i64 a = 0, b = 1;
	while(b <= LLONG_MAX - a){
		i64 x, y;
		assert(exgcd(a, b, x, y) == 1);
		assert(i128(a) * x + i128(b) * y == 1);
		assert(exgcd(b, a, x, y) == 1);
		assert(i128(b) * x + i128(a) * y == 1);
		bezout_cases += 2;
		tie(a, b) = pair{b, a + b};
	}
	{
		i64 x, y;
		assert(exgcd(a, b, x, y) == 1);
		assert(i128(a) * x + i128(b) * y == 1);
		assert(exgcd(b, a, x, y) == 1);
		assert(i128(b) * x + i128(a) * y == 1);
		bezout_cases += 2;
	}
	for(i64 x:vector<i64>{LLONG_MIN, -1, 0, 1, LLONG_MAX}){
		check({LLONG_MAX, x}, {LLONG_MAX, x}, false);
		check({LLONG_MAX, x}, {1, LLONG_MIN}, false);
	}
	check({LLONG_MAX, 0}, {LLONG_MAX, 1}, false);
	check({3037000499LL, LLONG_MIN}, {3037000497LL, LLONG_MAX}, false);
	a = LLONG_MAX / 2;
	assert((crt_merge({a, a - 1}, {2, 1}) == pair<i64, i64>{a * 2, a * 2 - 1}));
	check({a, a - 1}, {2, 1}, false);

	pair<i64, i64> none{-1, -1};
	assert(crt_merge(none, {3, 0}) == none);
	assert(crt_merge({3, 0}, none) == none);
	assert(crt_merge(none, none) == none);
	assert(crt_merge(crt_merge({2, 0}, {2, 1}), {3, 0}) == none);
	assert((crt_merge({1, 0}, {1, LLONG_MIN}) == pair<i64, i64>{1, 0}));

	// n=100000，各输入模数 <= 10^12，合并后模数为 M；已知解唯一地确定在 [0,M)。
	pair<i64, i64> res{1, 0};
	const i64 z = 123456789012345678LL;
	vector<i64> small;
	for(i64 d:divisors){
		if(d <= 1000000000000LL){
			small.pb(d);
		}
	}
	vector<i64> first{1 << 20, 531441, 390625};
	rep(i, 0, 99999){
		i64 m = (i < 3 ? first[i] : small[rng() % size(small)]);
		res = crt_merge(res, {m, z % m});
	}
	assert((res == pair<i64, i64>{M, z}));
	cout << "passed " << brute_cases << " brute-force CRT cases, " << large_cases
		<< " large CRT cases, " << bezout_cases << " i64 Bezout cases, sentinel checks"
		<< " and a 100000-equation P4777-scale system\n";
}
