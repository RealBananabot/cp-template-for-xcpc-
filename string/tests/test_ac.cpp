#include <bits/stdc++.h>
using namespace std;
using vi = vector<int>;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#define pb emplace_back
#include "ac.cpp"

int cases = 0;
void check(const vector<string> &a, const string &text){
	int n = 0;
	for(const auto &s:a) n += (int)s.size();
	AC t(n);
	map<string, int> id{{"", 1}};
	vector<string> label(n + 2);
	vi end;
	for(const auto &s:a){
		int p = 1;
		string cur;
		for(char c:s){
			p = t.ins(p, c - 'a');
			cur += c;
			auto [it, added] = id.emplace(cur, p);
			assert(added || it->second == p);
			label[p] = cur;
		}
		end.pb(p);
	}
	assert((int)id.size() == t.tot && (int)t.s.size() == t.tot + 1);
	t.build();
	assert(t.fa[1] == 0);
	auto suffix = [&](const string &s){
		rep(l, 0, (int)s.size()){
			auto it = id.find(s.substr(l));
			if(it != id.end()) return it->second;
		}
		abort();
	};
	rep(u, 1, t.tot){
		if(u != 1){
			assert(t.fa[u] == suffix(label[u].substr(1)));
			assert(label[t.fa[u]].size() < label[u].size());
		}
		rep(c, 0, 25){
			assert(t.s[u][c] == suffix(label[u] + char('a' + c)));
			assert(t.s[0][c] == 1);
		}
	}
	int p = 1;
	string prefix;
	for(char c:text){
		prefix += c;
		p = t.s[p][c - 'a'];
		assert(p == suffix(prefix));
		rep(i, 0, (int)a.size() - 1){
			bool match = false;
			for(int u = p; u; u = t.fa[u]) match |= u == end[i];
			bool expected = prefix.size() >= a[i].size() && prefix.compare(prefix.size() - a[i].size(), a[i].size(), a[i]) == 0;
			assert(match == expected);
		}
	}
	cases++;
}

int main(int argc, char **){
	if(argc > 1){
		const int n = 200000, m = 2000000;
		AC t(n);
		int p = 1;
		rep(i, 1, n) p = t.ins(p, 0);
		assert(p == n + 1 && t.tot == n + 1);
		t.build();
		rep(u, 2, t.tot){
			assert(t.fa[u] == u - 1 && t.s[u][1] == 1);
		}
		assert(t.s[t.tot][0] == t.tot);
		p = 1;
		rep(i, 1, m){
			p = t.s[p][0];
			assert(p == min(i + 1, n + 1));
		}
		assert(t.s[p][25] == 1);
		AC dup(n);
		rep(i, 1, n) assert(dup.ins(1, 0) == 2);
		dup.build();
		assert(dup.tot == 2 && dup.fa[2] == 1);
		cout << "passed total pattern length 200000, text length 2000000, and 200000 repeated patterns\n";
		return 0;
	}
	check({}, "abz");
	check({"a"}, "");
	check({"a", "a", "aa", "aaa"}, "aaaaazaaaaa");
	check({"he", "she", "hers", "his", "he"}, "ahishers");
	check({"ab", "bc", "abc", "b", "c"}, "zabcabcz");
	vector<string> words;
	rep(n, 1, 3) rep(mask, 0, (1 << n) - 1){
		string s(n, 'a');
		rep(i, 0, n - 1) s[i] += (mask >> i) & 1;
		words.pb(s);
	}
	for(const auto &a:words) for(const auto &b:words) for(const auto &c:words){
		check({a, b, c}, "ababaabbabaababbz");
	}
	mt19937 rng(5357);
	rep(tc, 1, 10000){
		vector<string> a;
		int cnt = (int)(rng() % 9), sigma = (int)(rng() % 5) + 1;
		rep(i, 1, cnt){
			string s;
			if(a.size() && rng() % 4 == 0) s = a[rng() % a.size()];
			else{
				int len = (int)(rng() % 9) + 1;
				rep(j, 1, len) s += char('a' + rng() % sigma);
			}
			a.pb(s);
		}
		int len = (int)(rng() % 51);
		string text;
		rep(i, 1, len) text += char((rng() % 8 == 0 ? 'z' : 'a' + rng() % sigma));
		check(a, text);
	}
	cout << "passed " << cases << " trie, failure-link, transition, and match checks\n";
}
