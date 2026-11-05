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

namespace _flow{
	const i64 inf_cap = 1e18;
	const i64 inf_cost = 1e18;
	template<typename Cap = i64, typename Cost = i64> struct mcmf{
		struct edge{
			int v;
			Cap w;
			Cost c;
			int r;
		};
		int n, m, S, T;
		vi cur;
		vector<Cost> h, dis;
		vector<bool> vis;
		vector< vector<edge> > G;
		
		mcmf(int _n, int _s, int _t){
			n = _n, S = _s, T = _t;
			cur.resize(n);
			h.resize(n);
			dis.resize(n);
			vis.resize(n);
			G.resize(n);
		}
		mcmf(int _n = 0){
			mcmf(_n + 2, _n, _n + 1);
		}
		void link(int u, int v, Cap w, Cost c){
			G[u].pb(edge{v, w, c, int(G[v].size())});
			G[v].pb(edge{u, 0, -c, int(G[u].size()) - 1});
		}
		void spfa(){
			ranges::fill(h, inf_cost);
			h[S]=0, vis[S]=1;
			queue<int> Q;
			Q.emplace(S);
			while(!Q.empty()){
				int u=Q.front();
				Q.pop();
				vis[u] = 0;
				for(auto &[v, w, c, r]:G[u]){
					if(w && chmin(h[v], h[u] + c) && !vis[v]){
						Q.emplace(v);
						vis[v] = 1;
					}
				}
			}
		}
		bool dij(){
			ranges::fill(dis, inf_cost);
			dis[S] = 0;
			priority_queue< pair<Cost, int>, vector< pair<Cost, int> >, greater<> > Q;
			Q.emplace(0, S);
			while(size(Q)){
				auto [d, u] = Q.top();
				Q.pop();
				if(dis[u] != d){
					continue;
				}
				for(auto &[v, w, c0, r]:G[u]){
					Cost c = c0 + (h[u] - h[v]);
					if(w && dis[u] + c <dis[v]){
						dis[v] = dis[u] + c;
						Q.emplace(dis[v], v);
					}
				}
			}
			return dis[T] != inf_cost;
		}
		Cap dfs(int u, Cap rst){
			if(u == S){
				return rst;
			}
			vis[u] = 1;
			Cap pre = rst;
			for(; cur[u] < ssize(G[u]); cur[u] ++){
				auto &[v, w, c0, r] = G[u][cur[u]];
				Cost c = -c0 + (h[v] - h[u]);
				if(dis[v] + c == dis[u] && G[v][r].w && !vis[v]){
					Cap f = dfs(v, min(rst, G[v][r].w));
					G[u][cur[u]].w += f, G[v][r].w -= f, rst -= f;
					if(!rst){
						break;
					}
				}
			}
			vis[u] = 0;
			return pre - rst;
		}
		pair<Cap, Cost> dinic(){
			spfa();
			Cap flow = 0;
			Cost cost = 0;
			while(1){
				rep(i, 0, n - 1){
					h[i] += dis[i];
				}
				dij();
				if(dis[T] == inf_cost){
					break;
				}
				ranges::fill(cur, 0);
				Cap f = dfs(T, inf_cap);
				flow += f;
				cost += f * (h[T] + dis[T]);
			}
			return {flow, cost};
		}
	};
}
using _flow::mcmf;

