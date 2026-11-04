#include <bits/stdc++.h>
using namespace std;
using vi = vector<int>;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#include "z.cpp"
int main(){
	int cases = 0;
	auto check = [&](const string &s){
		int n = int(size(s));
		vi expected(n);
		rep(i, 0, n - 1){
			while(expected[i] < n - i && s[expected[i]] == s[i + expected[i]]) expected[i]++;
		}
		assert(zfunc(s) == expected);
		cases++;
	};
	rep(n, 0, 12) rep(mask, 0, (1 << n) - 1){
		string s(n, 'a');
		rep(i, 0, n - 1) s[i] = char('a' + ((mask >> i) & 1));
		check(s);
	}
	mt19937 rng(5410);
	rep(tc, 1, 2000){
		int n = int(rng() % 65), sigma = int(rng() % 128) + 1;
		string s(n, 0);
		for(char &c:s) c = char(rng() % sigma);
		check(s);
	}
	string s(20000000, 'a');
	vi z = zfunc(s);
	rep(i, 0, int(size(s)) - 1) assert(z[i] == int(size(s)) - i);
	cout << "passed " << cases << " brute-force cases and n=20000000 boundary\n";
}
