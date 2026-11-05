// run.py 仅补充真实 s.cpp；此处不得引入其他数论模板，以检查整合文件的依赖。
#include "../pollard_rho.cpp"

int main(){
	static_assert(is_same_v<decltype(PR::qpow(i64(2), i64(10), i64(1009))), i64>);
	static_assert(is_same_v<decltype(PR::isprime(i64(2))), bool>);
	static_assert(is_same_v<decltype(PR::solve(i64(2))), vector<i64>>);
	assert(PR::qpow(2, 10, 1009) == 15);
	assert(!PR::isprime(0) && !PR::isprime(1) && PR::isprime(2));
	assert(PR::isprime(LLONG_MAX - 24));
	assert(!PR::isprime(LLONG_MAX));
	assert((PR::solve(12) == vector<i64>{2, 2, 3}));
	assert((PR::solve(1000000016000000063LL) == vector<i64>{1000000007, 1000000009}));
	assert(empty(PR::solve(1)));
	ostringstream out;
	for(i64 x:vector<i64>{2, 9, 30}){
		auto a = PR::solve(x);
		rep(i, 0, int(size(a)) - 1){
			out << (i ? " " : "") << a[i];
		}
		out << '\n';
	}
	assert(out.str() == "2\n3 3\n2 3 5\n");
	cout << "current s.cpp + pollard_rho.cpp alone passed API, boundaries and sample\n";
}
