#include <bits/stdc++.h>
using namespace std;
using i64 = long long;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#define per(i, j, k) for(int i = (j); i >= (k); i--)
#define pb emplace_back
#define fi first
#define se second
using vi = vector<int>;
using pi = pair<int, int>;

bool chmin(auto &x, const auto &y){
	if(y < x){x = y; return true;} return false;
}
bool chmax(auto &x, const auto &y){
	if(x < y){x = y; return true;} return false;
}

ostream& operator << (ostream &os, const auto &v) 
	requires requires { v.begin(); } && (!requires { v.substr(); }) {
	os << "[";
	for (int f = 0; const auto& x : v) os << (f++ ? ", " : "") << x;
	return os << "]";
}

template<typename ...Ar> void debug(char *s, Ar... x){
	auto f = [&](auto y){
		int dep = 0;
		while(*s && !(*s == ',' && dep == 0)){
			if(*s == '(') dep++;
			if(*s == ')') dep--;
			cerr << *s++;
		}
		cerr <<" = "<< y;
		if(*s == ','){
			cerr << *s ++;
		} else{
			cerr << endl;
		}
	};

	(f(x), ...);
}
#define gdb(...) debug((char*)#__VA_ARGS__, __VA_ARGS__)

pair<int, vi> bipartite_matching(int n, int m, const vector<vi> &G){
	vi vis(n), dis(n), match(n, -1), pre(m, -1);
	
	auto bfs = [&]()->bool {
		fill(dis.begin(), dis.end(), -1);

		queue<int> Q;
		rep(i, 0, n - 1){
			if(match[i] == -1){
				dis[i] = 0;
				Q.emplace(i);
			}
		}
		bool res = 0;
		while(Q.size()){
			int u = Q.front();
			Q.pop();
            vis[u] = 0;
			for(int v:G[u]){
				if(pre[v] == -1){
					res = 1;
				} else if(dis[ pre[v] ] == -1){
					dis[ pre[v] ] = dis[u] + 1;
					Q.emplace(pre[v]);
				}
			}
		}
		return res;
	};

	auto dfs = [&](auto &self, int u)->int {
		if(vis[u]){
			return 0;
		}
		vis[u] = 1;
		for(int v:G[u]){
			if(pre[v] == -1 || (dis[ pre[v] ] == dis[u] + 1 && self(self, pre[v]))){
				pre[v] = u, match[u] = v;
				return 1;
			}
		}
		return 0;
	};

	int res = 0;
	while(bfs()){
		rep(i, 0, n - 1){
			if(dis[i] == 0){
				res += dfs(dfs, i);
			}
		}
	}
	return {res, match};
}

