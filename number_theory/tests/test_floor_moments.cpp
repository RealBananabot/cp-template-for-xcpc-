#include "../euclid.cpp"
#include "../floor_moments.cpp"
#include "../floor_sum_mod.cpp"

int main(){
	using i128 = __int128;
	static_assert(is_same_v<decltype(FloorMoments::solve(0, 0, 0, 1)), array<int, 3>>);
	const int mod = FloorMoments::mod;
	int brute_cases = 0, word_cases = 0;
	rep(n, -1, 15) rep(a, 0, 15) rep(b, 0, 15) rep(c, 1, 16){
		array<int, 3> expected{};
		rep(i, 0, n){
			int y = (a * i + b) / c;
			expected[0] += y, expected[1] += y * y, expected[2] += i * y;
		}
		assert(FloorMoments::solve(n, a, b, c) == expected);
		brute_cases++;
	}
	mt19937_64 rng(5170);
	FloorMoments U{0, 1}, R{1};
	rep(tc, 1, 2000){
		vector<FloorMoments> atoms;
		FloorMoments z;
		int r = 0, u = 0, sum = 0, sq = 0, mix = 0;
		rep(i, 1, 50){
			if(rng() % 2){
				z = z + U, atoms.pb(U), u++;
			} else{
				z = z + R, atoms.pb(R);
				sum += u, sq += u * u, mix += r * u, r++;
			}
		}
		assert(z.r == r && z.u == u && z.sum == sum && z.sq == sq && z.mix == mix);
		FloorMoments A, B, C;
		rep(i, 0, 49){
			if(i < 17){
				A = A + atoms[i];
			} else if(i < 31){
				B = B + atoms[i];
			} else{
				C = C + atoms[i];
			}
		}
		auto left = (A + B) + C, right = A + (B + C);
		assert(left.r == right.r && left.u == right.u && left.sum == right.sum
			&& left.sq == right.sq && left.mix == right.mix && left.mix == z.mix);
		word_cases++;
	}
	assert((FloorMoments::solve(1, 1, 0, 1) == array<int, 3>{1, 1, 1}));
	assert((FloorMoments::solve(LLONG_MIN, 1, 1, 1) == array<int, 3>{}));
	int cases;
	assert(bool(cin >> cases));
	rep(tc, 1, cases){
		i64 n, a, b, c;
		array<int, 3> expected;
		string unused;
		assert(bool(cin >> n >> a >> b >> c >> expected[0] >> expected[1] >> expected[2] >> unused));
		assert(FloorMoments::solve(n, a, b, c) == expected);
	}
	// 题目最大 t，随机全范围数据；第一项另以专用迭代 floor_sum 核对。
	rep(tc, 1, 100000){
		i64 n = i64(rng() % 1000000001), a = i64(rng() % 1000000001);
		i64 b = i64(rng() % 1000000001), c = i64(rng() % 1000000000) + 1;
		auto z = FloorMoments::solve(n, a, b, c);
		assert(z[0] == floor_sum_mod(n, a, b, c));
		assert(0 <= z[1] && z[1] < mod && 0 <= z[2] && z[2] < mod);
	}
	// 原计数超过 mod 后仍可由剩余计数计算三角数。
	i64 n = i64(mod) * mod;
	auto z = FloorMoments::solve(n, 0, 1, 1);
	assert(z[0] == 1 && z[1] == 1);
	assert(z[2] == i64(i128(n) * (n + 1) / 2 % mod));
	cout << "passed " << brute_cases << " naive moment triples, " << word_cases
		<< " literal moment words and associativity checks, " << cases
		<< " Python exact triples and 100000 P5170-scale cross-checks\n";
}
