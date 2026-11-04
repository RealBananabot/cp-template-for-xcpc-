#define pb emplace_back
#include <bits/stdc++.h>
using namespace std;
using i64 = long long;
using vi = vector<int>;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#include "gsam.cpp"

int cases = 0;
array<i64, 4> branches{};
void check(const vector<string> &a){
	int n = 0;
	for(const auto &x:a) n += int(size(x));
	SAM t(n);
	for(const auto &x:a){
		int p = 1;
		for(char c:x){
			int v = t.s[p][c - 'a'];
			if(v) branches[t.len[v] == t.len[p] + 1? 0: 1]++;
			else{
				int z = p;
				while(z && !t.s[z][c - 'a']) z = t.fa[z];
				branches[z && t.len[t.s[z][c - 'a']] != t.len[z] + 1? 3: 2]++;
			}
			p = t.ins(p, c - 'a');
		}
	}
	map<string, vector<pair<int, int>>> occ;
	rep(k, 0, int(size(a)) - 1){
		rep(r, 0, int(size(a[k])) - 1){
			rep(l, 0, r) occ[a[k].substr(l, r - l + 1)].emplace_back(k, r);
		}
	}
	map<vector<pair<int, int>>, int> cls;
	vector<vector<pair<int, int>>> endpos(t.tot + 1);
	vi low(t.tot + 1, INT_MAX), high(t.tot + 1);
	for(const auto &[x, pos]:occ){
		int p = 1;
		for(char c:x){
			p = t.s[p][c - 'a'];
			assert(p);
		}
		if(endpos[p].empty()) endpos[p] = pos;
		assert(endpos[p] == pos);
		auto [it, added] = cls.emplace(pos, p);
		assert(added || it->second == p);
		low[p] = min(low[p], int(size(x)));
		high[p] = max(high[p], int(size(x)));
	}
	assert(t.solve().first == i64(size(occ)));
	assert(t.tot == int(size(cls)) + 1);
	assert(t.tot <= 2 * n + 1);
	rep(i, 2, t.tot){
		assert(!endpos[i].empty());
		assert(t.fa[i] >= 1 && t.fa[i] <= t.tot);
		assert(low[i] == t.len[t.fa[i]] + 1 && high[i] == t.len[i]);
		assert(t.len[t.fa[i]] < t.len[i]);
	}
	vi order(t.tot);
	iota(order.begin(), order.end(), 1);
	sort(order.begin(), order.end(), [&](int u, int v){return t.len[u] > t.len[v];});
	vector<i64> paths(t.tot + 1);
	for(int u:order){
		rep(c, 0, 25){
			int v = t.s[u][c];
			if(v){
				assert(t.len[v] > t.len[u]);
				paths[u] += 1 + paths[v];
			}
		}
	}
	assert(paths[1] == i64(size(occ)));
	cases++;
}

int main(int argc, char **){
	if(argc > 1){
		const int n = 1000000;
		for(int typ = 0; typ < 3; typ++){
			SAM t(n);
			if(typ < 2){
				int p = 1;
				for(int i = 0; i < n; i++) p = t.ins(p, (typ ? i % 2 : 0));
				assert(t.tot == n + 1 && t.solve().first == (typ ? 2LL * n - 1 : n));
			} else{
				for(int i = 0; i < 1000; i++){
					int p = 1;
					for(int j = 0; j < 1000; j++) p = t.ins(p, 0);
				}
				assert(t.tot == 1001 && t.solve().first == 1000);
			}
		}
		cout << "passed total length 1000000: repeated, alternating, and duplicate strings\n";
		return 0;
	}
	check({});
	check({""});
	check({"a"});
	check({"a", "a"});
	check({"ab", "b"});
	check({"ab", "cb"});
	check({"abb", "b", "bb", "abb", "ab"});
	check({"abacaba", "bac", "aba", "caba"});
	check({"abcdefghijklmnopqrstuvwxyz"});
	vector<string> pool;
	rep(len, 0, 4){
		rep(mask, 0, (1 << len) - 1){
			string x(len, 'a');
			rep(i, 0, len - 1) x[i] = char('a' + ((mask >> i) & 1));
			pool.emplace_back(x);
		}
	}
	for(const auto &a:pool) for(const auto &b:pool) for(const auto &c:pool) check({a, b, c});
	mt19937 rng(6139);
	rep(tc, 1, 20000){
		vector<string> a;
		int cnt = int(rng() % 8) + 1;
		rep(i, 1, cnt){
			string x;
			if(size(a) && rng() % 3 == 0){
				x = a[rng() % size(a)];
				x.resize(rng() % (size(x) + 1));
			} else{
				int len = int(rng() % 12);
				int sigma = int(rng() % 5) + 1;
				rep(j, 1, len) x += char('a' + rng() % sigma);
			}
			a.emplace_back(x);
		}
		check(a);
		shuffle(a.begin(), a.end(), rng);
		check(a);
	}
	for(i64 x:branches) assert(x);
	cout << "passed " << cases << " cases; branches:";
	for(i64 x:branches) cout << ' ' << x;
	cout << '\n';
}
