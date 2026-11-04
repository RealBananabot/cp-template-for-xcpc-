#include <bits/stdc++.h>
using namespace std;
using vi = vector<int>;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#include "manacher.cpp"
int main(int argc, char **){
	if(argc > 1){
		const int n = 11000000;
		string s(n, 'a');
		vi d = manacher(s);
		assert((int)d.size() == 2 * n + 1);
		rep(i, 0, 2 * n) assert(d[i] == min(i, 2 * n - i));
		cout << "passed original length 11000000 boundary\n";
		return 0;
	}
	int cases = 0;
	auto check = [&](const string &s){
		int m = (int)s.size(), n = 2 * m + 1;
		// Oracle uses -1 separators and unsigned byte values 0..255.
		vi t(n, -1), expected(n);
		rep(i, 0, m - 1) t[2 * i + 1] = (unsigned char)s[i];
		rep(i, 0, n - 1){
			while(expected[i] < i && expected[i] < n - i - 1 && t[i - expected[i] - 1] == t[i + expected[i] + 1]) expected[i]++;
		}
		vi d = manacher(s);
		assert(d == expected);
		int longest = 0;
		rep(l, 0, m - 1) rep(r, l, m - 1){
			bool ok = true;
			rep(k, 0, (r - l) / 2) if(s[l + k] != s[r - k]) ok = false;
			if(ok) longest = max(longest, r - l + 1);
		}
		assert(*max_element(d.begin(), d.end()) == longest);
		rep(i, 0, n - 1){
			int l = (i - d[i]) / 2, r = (i + d[i]) / 2;
			assert(r - l == d[i]);
			rep(k, 0, (r - l) / 2 - 1) assert(s[l + k] == s[r - k - 1]);
		}
		cases++;
	};
	rep(n, 0, 9){
		int total = 1;
		rep(i, 1, n) total *= 3;
		rep(mask, 0, total - 1){
			int x = mask;
			string s(n, 'a');
			for(char &c:s){c = string("ab?")[x % 3]; x /= 3;}
			check(s);
		}
	}
	mt19937 rng(3805);
	rep(tc, 1, 5000){
		int n = (int)(rng() % 65), sigma = (int)(rng() % 256) + 1;
		string s(n, 0);
		for(char &c:s) c = char(rng() % sigma);
		check(s);
	}
	assert((manacher("") == vi{0}));
	assert((manacher("a") == vi{0, 1, 0}));
	assert((manacher("aba") == vi{0, 1, 0, 3, 0, 1, 0}));
	cout << "passed " << cases << " radius, original-palindrome, and separator checks\n";
}
