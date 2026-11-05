#include "../floor_sum.cpp"
#include "../floor_sum_mod.cpp"

int main(){
	using i128 = __int128;
	static_assert(is_same_v<decltype(floor_sum(0, 0, 0, 1)), i64>);
	static_assert(is_same_v<decltype(floor_sum_mod(0, 0, 0, 1)), int>);
	const int mod = 998244353;
	int brute_cases = 0, exact_cases = 0;
	rep(n, -1, 20) rep(a, 0, 20) rep(b, 0, 20) rep(c, 1, 20){
		i128 expected = 0;
		rep(i, 0, n){
			expected += (a * i + b) / c;
		}
		assert(floor_sum(n, a, b, c) == expected);
		assert(floor_sum_mod(n, a, b, c) == expected % mod);
		brute_cases++;
	}
	assert(floor_sum(LLONG_MIN, 1, 1, 1) == 0);
	assert(floor_sum_mod(LLONG_MIN, 1, 1, 1) == 0);
	assert(floor_sum(1, 1, (1LL << 62) - 1, 1) == LLONG_MAX);
	int cases;
	assert(bool(cin >> cases));
	rep(tc, 1, cases){
		i64 n, a, b, c;
		int f, g, h;
		string text;
		assert(bool(cin >> n >> a >> b >> c >> f >> g >> h >> text));
		assert(floor_sum_mod(n, a, b, c) == f);
		if(text != "-1"){
			i128 expected = 0;
			for(char ch:text){
				expected = expected * 10 + (ch - '0');
			}
			assert(floor_sum(n, a, b, c) == expected);
			exact_cases++;
		}
	}
	cout << "passed " << brute_cases << " brute-force floor sums, " << cases
		<< " Python modular references, " << exact_cases << " exact i64 references and empty ranges\n";
}
