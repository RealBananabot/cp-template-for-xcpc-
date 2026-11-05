#include "../dujiao.cpp"

// 独立参考：埃氏筛计算函数值，普通整除分块递推大数前缀。
struct Reference{
	vector<i64> phi;
	unordered_map<int, i64> mem;
	Reference(int n): phi(n + 1){
		vi p(n + 1);
		iota(p.begin(), p.end(), 0);
		rep(i, 2, n){
			if(p[i] == i){
				for(int j = i; j <= n; j += i){
					p[j] -= p[j] / i;
				}
			}
		}
		rep(i, 1, n){
			phi[i] = phi[i - 1] + p[i];
		}
	}
	i64 solve(int n){
		if(n < int(size(phi))){
			return phi[n];
		}
		if(auto it = mem.find(n); it != mem.end()){
			return it->se;
		}
		i64 ans = i64(n) * (i64(n) + 1) / 2;
		for(i64 l = 2, r; l <= n; l = r + 1){
			int k = int(n / l);
			r = n / k;
			ans -= (r - l + 1) * solve(k);
		}
		return mem[n] = ans;
	}
};

int main(){
	static_assert(is_same_v<decltype(declval<Dujiao&>().solve(0)), i64>);
	Reference ref(2000000);
	Dujiao small(64);
	int naive = 0, cases = 0;
	rep(n, 1, 512){
		int phi = 0;
		rep(i, 1, n){
			phi += (gcd(i, n) == 1);
		}
		assert(ref.phi[n] - ref.phi[n - 1] == phi);
		if(n <= 64){
			assert(small.phi[n] == phi);
		}
		naive++;
	}
	rep(n, 0, 4224){
		assert(small.solve(n) == ref.solve(n));
		cases++;
	}
	mt19937 rnd(4213);
	for(int m:{1, 2, 7, 64}){
		Dujiao d(m);
		int bound = (m + 1) * (m + 1) - 1;
		for(int n:{0, 1, m - 1, m, m + 1, bound - 1, bound}){
			assert(d.solve(n) == ref.solve(n));
			cases++;
		}
		rep(i, 1, 100){
			int n = int(rnd() % unsigned(bound + 1));
			assert(d.solve(n) == ref.solve(n));
			cases++;
		}
	}
	Dujiao d;
	vi queries{INT_MAX, INT_MAX - 1, 46340 * 46340 - 1, 46340 * 46340,
		46340 * 46340 + 1, 1000000000, 1000000007, 1000001, 1000000, 0};
	for(int n:queries){
		assert(d.solve(n) == ref.solve(n));
		cases++;
	}
	size_t count = size(d.mem);
	reverse(queries.begin(), queries.end());
	for(int n:queries){
		assert(d.solve(n) == ref.solve(n));
	}
	assert(size(d.mem) == count);
	Dujiao minimum(46340);
	assert(minimum.solve(INT_MAX) == ref.solve(INT_MAX));
	assert(d.solve(0) == 0);
	assert(d.solve(1) == 1);
	assert(d.solve(10) == 32);
	auto z = d.solve(INT_MAX);
	cout << "passed " << naive << " naive function checks, " << cases
		<< " prefix comparisons, INT_MAX, square boundaries, ten mixed queries, "
		<< "minimum cutoff and cached repeats; max result = " << z << '\n';
}
