#include <bits/stdc++.h>
using namespace std;
using vi = vector<int>;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#include "z.cpp"
int main(){
	int cases = 0;
	auto check = [&](const string &s, const string &t){
		int n = int(size(s)), m = int(size(t));
		vi expected(n);
		rep(i, 0, n - 1){
			while(expected[i] < m && expected[i] < n - i && s[i + expected[i]] == t[expected[i]]) expected[i]++;
		}
		assert(exkmp(s, t) == expected);
		cases++;
	};
	vector<string> words;
	rep(n, 0, 6) rep(mask, 0, (1 << n) - 1){
		string s(n, 'a');
		rep(i, 0, n - 1) s[i] = char('a' + ((mask >> i) & 1));
		words.push_back(s);
	}
	for(const auto &s:words) for(const auto &t:words) check(s, t);
	mt19937 rng(5410);
	rep(tc, 1, 5000){
		int n = int(rng() % 65), m = int(rng() % 65), sigma = int(rng() % 128) + 1;
		string s(n, 0), t(m, 0);
		for(char &c:s) c = char(rng() % sigma);
		for(char &c:t) c = char(rng() % sigma);
		check(s, t);
	}
	assert((exkmp("aaaabaa", "aaaaa") == vi{4, 3, 2, 1, 0, 2, 1}));
	string s(20000000, 'a');
	vi p = exkmp(s, "aaa");
	rep(i, 0, int(size(s)) - 1) assert(p[i] == min(3, int(size(s)) - i));
	cout << "passed " << cases << " brute-force cases and 20000000-character text\n";
}
