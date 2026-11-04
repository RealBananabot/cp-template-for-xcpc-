#include <bits/stdc++.h>
using namespace std;
using vi = vector<int>;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#include "kmp.cpp"
int main(){
	int cases = 0;
	auto check = [&](const string &s){
		vi expected(s.size(), -1);
		rep(i, 0, (int)s.size() - 1){
			rep(len, 1, i){
				if(s.compare(0, len, s, i + 1 - len, len) == 0) expected[i] = len - 1;
			}
		}
		assert(kmp(s) == expected);
		cases++;
	};
	rep(n, 0, 12) rep(mask, 0, (1 << n) - 1){
		string s(n, 'a');
		rep(i, 0, n - 1) s[i] += (mask >> i) & 1;
		check(s);
	}
	mt19937 rng(3375);
	rep(tc, 1, 2000){
		int n = (int)(rng() % 65), sigma = (int)(rng() % 256) + 1;
		string s(n, 0);
		for(char &c:s) c = char(rng() % sigma);
		check(s);
	}
	string s(1000000, 'a');
	vi fail = kmp(s);
	rep(i, 0, (int)s.size() - 1) assert(fail[i] == i - 1);
	cout << "passed " << cases << " brute-force cases and million-character boundary\n";
}
