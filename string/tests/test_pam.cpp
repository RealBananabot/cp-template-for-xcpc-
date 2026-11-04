#include <bits/stdc++.h>
using namespace std;
using vi = vector<int>;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#define pb emplace_back
#include "pam.cpp"

bool pal(const string &s){
	return equal(s.begin(), s.end(), s.rbegin());
}
void check(const string &s){
	int n = s.size();
	PAM a(n), b(n);
	a.str = s;
	assert(a.tot == 2 && a.fa[1] == 2 && a.fa[2] == 2);
	assert(a.len[1] == 0 && a.len[2] == -1 && !a.dep[1] && !a.dep[2]);
	vector<string> label(n + 3);
	map<string, int> id;
	set<string> distinct;
	int u = 1, v = 1;
	rep(i, 0, n - 1){
		b.str += s[i];
		u = a.ins(u, i), v = b.ins(v, i);
		int cnt = 0, mx = 0;
		string best;
		rep(j, 0, i){
			string t = s.substr(j, i - j + 1);
			if(pal(t)){
				cnt++;
				distinct.insert(t);
				if((int)t.size() > mx) mx = t.size(), best = t;
			}
		}
		assert(a.len[u] == mx && a.dep[u] == cnt && a.tot - 2 == (int)distinct.size());
		if(id.count(best)) assert(id[best] == u);
		else id[best] = u, label[u] = best;
		assert(u == v && a.tot == b.tot && a.s == b.s);
		assert(a.fa == b.fa && a.len == b.len && a.dep == b.dep);
		assert((int)a.s.size() == a.tot + 1);
	}
	assert(id.size() == distinct.size());
	rep(i, 3, a.tot){
		assert(!label[i].empty() && a.len[i] == (int)label[i].size());
		int p = 1;
		rep(j, 1, (int)label[i].size() - 1){
			string t = label[i].substr(j);
			if(pal(t)){ p = id.at(t); break; }
		}
		assert(a.fa[i] == p && a.len[p] < a.len[i]);
		int cnt = 0;
		for(int j = i; j > 2; j = a.fa[j]) cnt++;
		assert(a.dep[i] == cnt);
	}
	rep(i, 1, a.tot) rep(c, 0, 25){
		int j = a.s[i][c];
		if(!j) continue;
		string t(1, 'a' + c);
		if(i != 2) t += label[i], t += char('a' + c);
		assert(j >= 3 && label[j] == t);
	}
}
int main(int argc, char **){
	if(argc > 1){
		int n = 500000;
		for(int typ = 0; typ < 3; typ++){
			PAM a(n);
			a.str.resize(n);
			rep(i, 0, n - 1) a.str[i] = (typ == 0 ? 'a' : (typ == 1 ? 'a' + i % 2 : 'a' + i % 26));
			int u = 1;
			rep(i, 0, n - 1){
				u = a.ins(u, i);
				if(typ == 0) assert(a.dep[u] == i + 1 && a.len[u] == i + 1 && a.tot == i + 3);
				if(typ == 1) assert(a.dep[u] == (i + 2) / 2 && a.len[u] == 2 * a.dep[u] - 1);
				if(typ == 2) assert(a.dep[u] == 1 && a.len[u] == 1 && a.tot == min(i + 3, 28));
			}
		}
		cout << "max length checks passed\n";
		return 0;
	}
	int cases = 0;
	rep(n, 0, 12) rep(mask, 0, (1 << n) - 1){
		string s(n, 'a');
		rep(i, 0, n - 1) s[i] += mask >> i & 1;
		check(s); cases++;
	}
	mt19937 rng(5496);
	rep(it, 1, 3000){
		int n = rng() % 41, k = (it % 4 == 0 ? 26 : it % 4);
		string s(n, 'a');
		for(char &c:s) c += rng() % k;
		check(s); cases++;
	}
	cout << cases << " brute and structure checks passed\n";
}
