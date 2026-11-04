#include <bits/stdc++.h>
using namespace std;
using vi = vector<int>;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#define per(i, j, k) for(int i = (j); i >= (k); i--)
#include "sa.cpp"

int cases = 0;
void check(const string &s){
	int n = int(size(s));
	auto [rk, sa] = SA(s);
	vi ref(n);
	iota(ref.begin(), ref.end(), 0);
	sort(ref.begin(), ref.end(), [&](int i, int j){
		return lexicographical_compare(s.begin() + i, s.end(), s.begin() + j, s.end(),
			[](char a, char b){return a < b;});
	});
	assert(sa == ref && int(size(rk)) == n);
	vi h = get_height(s, sa, rk), expected(n);
	rep(i, 0, n - 1){
		assert(rk[sa[i]] == i);
		if(i){
			int x = sa[i - 1], y = sa[i];
			while(x + expected[i] < n && y + expected[i] < n && s[x + expected[i]] == s[y + expected[i]]) expected[i]++;
		}
	}
	assert(h == expected);
	cases++;
}

int main(int argc, char **argv){
	if(argc > 1){
	string mode = argv[1];
	const int n = 1000000;
	string s(n, 'a');
	if(mode == "periodic") rep(i, 0, n - 1) s[i] = char('a' + (i & 1));
	if(mode == "random"){
		mt19937 rng(3809);
		for(char &c:s) c = char(rng() % 128);
	}
	auto [rk, sa] = SA(s);
	vi h = get_height(s, sa, rk);
	rep(i, 0, n - 1) assert(rk[sa[i]] == i);
	if(mode == "repeated"){
		rep(i, 0, n - 1) assert(sa[i] == n - i - 1 && h[i] == i);
	} else if(mode == "periodic"){
		rep(i, 0, n / 2 - 1) assert(sa[i] == n - 2 - 2 * i && h[i] == 2 * i);
		rep(j, 0, n / 2 - 1) assert(sa[n / 2 + j] == n - 1 - 2 * j && h[n / 2 + j] == (j ? 2 * j - 1 : 0));
	} else{
		vi ref(n);
		iota(ref.begin(), ref.end(), 0);
		sort(ref.begin(), ref.end(), [&](int i, int j){
			while(i < n && j < n && s[i] == s[j]) i++, j++;
			if(i == n || j == n) return i == n && j != n;
			return s[i] < s[j];
		});
		assert(ref == sa && h[0] == 0);
		rep(i, 1, n - 1){
			int k = 0, x = sa[i - 1], y = sa[i];
			while(x + k < n && y + k < n && s[x + k] == s[y + k]) k++;
			assert(h[i] == k);
		}
	}
	cout << mode << " passed, n=" << n << '\n';
		return 0;
	}
	check("");
	rep(c, 0, 127) check(string(1, char(c)));
	rep(a, 0, 127) rep(b, 0, 127) check(string{char(a), char(b)});
	string all;
	rep(c, 0, 127) all += char(c);
	check(all);
	reverse(all.begin(), all.end());
	check(all);
	rep(n, 0, 9){
		int total = 1;
		rep(i, 1, n) total *= 3;
		rep(mask, 0, total - 1){
			int x = mask;
			string s(n, 'a');
			for(char &c:s){c = char(c + x % 3); x /= 3;}
			check(s);
		}
	}
	mt19937 rng(3809);
	rep(tc, 1, 20000){
		int n = int(rng() % 100);
		int sigma = array{1, 2, 5, 64, 128}[rng() % 5];
		string s(n, 0);
		for(char &c:s) c = char(rng() % sigma);
		check(s);
	}
	cout << "passed " << cases << " brute-force cases\n";
}
