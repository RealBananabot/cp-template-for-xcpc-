#include <bits/stdc++.h>
using namespace std;
using i64 = long long;
using vi = vector<int>;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#define per(i, j, k) for(int i = (j); i >= (k); i--)
#define pb emplace_back
#include "sa.cpp"
#include "lcp_sa.cpp"
#include "runs.cpp"
namespace direct_hash{
#include "runs_hash.cpp"
}
namespace direct_sa{
#include "runs_sa.cpp"
}
using runs = vector<tuple<int, int, int>>;

runs brute(const string &s){
	int n = int(size(s));
	runs a;
	rep(l, 0, n - 1) rep(r, l + 1, n - 1){
		rep(p, 1, (r - l + 1) / 2){
			bool ok = true;
			rep(i, l + p, r) if(s[i] != s[i - p]){ok = false; break;}
			if(!ok) continue;
			if((l == 0 || s[l - 1] != s[l - 1 + p]) && (r == n - 1 || s[r + 1] != s[r + 1 - p])) a.pb(l, r, p);
			break;
		}
	}
	return a;
}

int cases = 0, queries = 0;
void check(const string &s){
	int n = int(size(s));
	sa_lcp t(s);
	runs a = direct_hash::get_runs(s), b = get_runs(s, t), c = brute(s);
	assert(direct_sa::get_runs(s) == b);
	if(a != c || b != c){
		cerr << "bad runs, n=" << n << '\n';
		for(char x:s) cerr << int(x) << ' ';
		cerr << '\n';
		abort();
	}
	rep(x, 0, n - 1) rep(y, 0, n - 1){
		int pre = 0, suf = 0;
		while(x + pre < n && y + pre < n && s[x + pre] == s[y + pre]) pre++;
		while(x - suf >= 0 && y - suf >= 0 && s[x - suf] == s[y - suf]) suf++;
		bool less = lexicographical_compare(s.begin() + x, s.end(), s.begin() + y, s.end(), [](char a, char b){return a < b;});
		assert(t.lcp(x, y) == pre);
		assert(t.lcs(x, y) == suf);
		assert(t.cmp(x, y) == less);
		queries++;
	}
	cases++;
}

int main(int argc, char **){
	if(argc > 1){
		const int n = 1000000;
		for(int typ = 0; typ < 2; typ++){
			string s(n, 'a');
			if(typ){
				string a = "a", b = "ab";
				while(int(size(b)) < n){ string c = b + a; a = move(b); b = move(c); }
				s = b.substr(0, n);
			}
			auto exact = direct_sa::get_runs(s);
			if(!typ) assert((exact == runs{{0, n - 1, 1}}));
			assert(get_runs<sa_lcp>(s) == exact);
			assert(direct_hash::get_runs(s) == exact);
		}
		cout << "passed repeated and Fibonacci strings of length 1000000 in all three run variants\n";
		return 0;
	}
	check("");
	rep(c, 0, 127) check(string(1, char(c)));
	rep(x, 0, 127) rep(y, 0, 127) check(string{char(x), char(y)});
	rep(n, 0, 13){
		rep(mask, 0, (1 << n) - 1){
			string s(n, 'a');
			rep(i, 0, n - 1) s[i] = char('a' + ((mask >> i) & 1));
			check(s);
		}
	}
	rep(n, 0, 9){
		int total = 1;
		rep(i, 1, n) total *= 3;
		rep(mask, 0, total - 1){
			string s(n, 'a');
			int x = mask;
			for(char &c:s){c = char(c + x % 3); x /= 3;}
			check(s);
		}
	}
	mt19937 rng(6656);
	rep(tc, 1, 5000){
		int n = int(rng() % 32);
		int sigma = array{1, 2, 3, 26, 128}[rng() % 5];
		string s(n, 0);
		for(char &c:s) c = char(rng() % sigma);
		check(s);
	}
	assert(direct_hash::get_runs("").empty());
	assert(get_runs<sa_lcp>("a").empty());
	string collision = "atdeoqmhatdeoqmhatofnybkaaaaaaaa";
	auto exact = brute(collision);
	assert(get_runs<sa_lcp>(collision) == exact);
	assert(direct_sa::get_runs(collision) == exact);
	assert(direct_hash::get_runs(collision) != exact);
	assert((exact == runs{{0, 17, 8}, {24, 31, 1}}));
	cout << "passed " << cases << " brute-force run checks; " << queries << " query pairs\n";
	cout << "fixed-hash collision regression confirmed; SA matches brute force\n";
}
