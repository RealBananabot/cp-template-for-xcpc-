#include "../pollard_rho.cpp"
#include "../exgcd.cpp"
#include "../excrt.cpp"
#include "../lucas.cpp"
#include "../exlucas.cpp"

int main(){
	static_assert(is_same_v<vi, vector<int>>);
	static_assert(is_same_v<decltype(PR::solve(i64(2))), vector<i64>>);
	int small_cases = 0, random_cases = 0, reference_cases = 0;
	auto check = [](i64 x, const vector<i64> &expected){
		auto result = PR::solve(x);
		assert(result == expected);
		assert(is_sorted(result.begin(), result.end()));
		i64 remaining = x;
		for(i64 p:result){
			assert(p >= 2 && remaining % p == 0);
			remaining /= p;
		}
		assert(remaining == 1);
	};
	// 独立试除，覆盖全部小整数以及反复调用时结果清空。
	auto trial = [](i64 x){
		vector<i64> result;
		for(i64 p = 2; p <= x / p; p++){
			while(x % p == 0){
				result.pb(p);
				x /= p;
			}
		}
		if(x > 1){
			result.pb(x);
		}
		return result;
	};
	rep(x, 1, 10000){
		check(x, trial(x));
		small_cases++;
	}
	mt19937_64 rng(458);
	rep(tc, 1, 2000){
		i64 x = i64(rng() % 100000000) + 1;
		check(x, trial(x));
		random_cases++;
	}
	// rnd 的首个 c=0 会使旧版 work(9) 原样递归；新版必须重试后结束。
	int retry_seed = 0;
	for(;;retry_seed++){
		mt19937_64 gen(retry_seed);
		if(gen() % 9 == 0){
			break;
		}
	}
	PR::rnd.seed(retry_seed);
	check(9, {3, 3});
	for(int seed:vi{0, 1, 2, 3663, 20261104}){
		PR::rnd.seed(seed);
		check(1, {});
		check(2, {2});
		check(30, {2, 3, 5});
		check(1000000016000000063LL, {1000000007, 1000000009});
		check(4611686014132420609LL, {2147483647, 2147483647});
		check(LLONG_MAX, {7, 7, 73, 127, 337, 92737, 649657});
		check(LLONG_MAX - 24, {LLONG_MAX - 24});
		check(i64(1) << 62, vector<i64>(62, 2));
		i64 power = 1;
		vector<i64> factors;
		while(power <= LLONG_MAX / 3){
			power *= 3;
			factors.pb(3);
		}
		check(power, factors);
	}
	int cases;
	assert(bool(cin >> cases));
	vector<pair<i64, vector<i64>>> large;
	rep(tc, 1, cases){
		i64 x;
		int k;
		assert(bool(cin >> x >> k));
		vector<i64> expected(k);
		for(i64 &p:expected){
			assert(bool(cin >> p));
		}
		check(x, expected);
		if(2 <= x && x <= 1000000000000000000LL){
			large.pb(x, expected);
		}
		reference_cases++;
	}
	// 题目 T=1000 规模；从已独立验证的用例重复组合，不计为独立随机样本。
	assert(!empty(large));
	rep(i, 0, 999){
		const auto &[x, expected] = large[i % size(large)];
		check(x, expected);
	}
	assert(inv(3, 11) == 4);
	assert((crt_merge({6, 4}, {8, 4}) == pair<i64, i64>{24, 4}));
	assert(Lucas(97).solve(200, 100) == exlucas(200, 100, 97));
	cout << "passed " << small_cases << " exhaustive and " << random_cases
		<< " random trial-division cases, " << reference_cases
		<< " certified factorization cases, retry regression, five seeds, boundaries"
		<< " and 1000 QOJ-scale calls\n";
}
