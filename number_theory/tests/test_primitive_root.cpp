// 仅用真实 s.cpp 和试除模板，不引入 Miller–Rabin 或 Pollard–Rho。
#include "../primitive_root.cpp"

int main(){
	static_assert(is_same_v<decltype(primitive_root(i64(2))), i64>);
	int small_cases = 0, reference_cases = 0;
	auto brute = [](int d){
		if(d <= 1){
			return -1;
		}
		int phi = 0;
		rep(g, 1, d - 1){
			phi += (gcd(g, d) == 1);
		}
		rep(g, 1, d - 1){
			if(gcd(g, d) != 1){
				continue;
			}
			int r = 1, order = 0;
			do{
				r = int(i64(r) * g % d);
				order++;
			} while(r != 1);
			if(order == phi){
				return g;
			}
		}
		return -1;
	};
	rep(d, -10, 1000){
		assert(primitive_root(d) == brute(d));
		small_cases++;
	}
	mt19937 rng(6091);
	rep(tc, 1, 200){
		int d = int(rng() % 1500) + 1;
		assert(primitive_root(d) == brute(d));
		small_cases++;
	}
	assert(primitive_root(LLONG_MIN) == -1);
	const i64 bound = (1LL << 62) - 1;
	assert(primitive_root(bound) == -1);
	assert(primitive_root(1000000000000000000LL) == -1);
	// 校验上界附近的循环退出平方，无须实际执行十亿次试除。
	i64 p = (1LL << 31) - 1;
	assert(p * p <= bound);
	p += 2;
	assert(p * p > bound && p * p < LLONG_MAX);
	assert(primitive_root(40487) == 5);
	assert(primitive_root(40487LL * 40487) == 10);
	assert(primitive_root(2 * 40487LL * 40487) == 13);
	int cases;
	assert(bool(cin >> cases));
	// 试除版只使用其声明范围内的参考值，不跑需要十亿次试除的大素数。
	const vector<i64> smooth{4052555153018976267LL, 3909821048582988049LL,
		1490116119384765625LL, 2980232238769531250LL, 4294967311LL, bound};
	rep(i, 1, cases){
		i64 d, expected;
		assert(bool(cin >> d >> expected));
		if(d <= bound && (d <= 2000000 || find(smooth.begin(), smooth.end(), d) != smooth.end())){
			i64 before = d;
			assert(primitive_root(d) == expected);
			assert(d == before);
			reference_cases++;
		}
	}
	assert(reference_cases >= 100);
	cout << "trial division passed " << small_cases << " independent order cases, "
		<< reference_cases << " certified Python cases, i64 smooth powers, boundaries and P6091-scale inputs\n";
}
