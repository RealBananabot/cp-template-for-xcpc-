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

template<typename T0, typename T1> bool chmin(T0 &x, const T1 &y){
	if(y < x){x = y; return true;} return false;
}
template<typename T0, typename T1> bool chmax(T0 &x, const T1 &y){
	if(x < y){x = y; return true;} return false;
}

template<typename T> void debug(char *s, T x){
	cerr << s <<" = "<< x <<endl;
}
template<typename T, typename ...Ar> void debug(char *s, T x, Ar... y){
	int dep = 0;
	while(!(*s == ',' && dep == 0)){
		if(*s == '(') dep++;
		if(*s == ')') dep--;
		cerr << *s++;
	}
	cerr <<" = "<< x <<",";
	debug(s + 1, y...);
}
#define gdb(...) debug((char*)#__VA_ARGS__, __VA_ARGS__)

namespace _flow{
	// maxcost: 单位流量带来的最大费用变化
	// inf: 总流量和费用的最大绝对值
	constexpr i64 maxcap = 1e18;
	constexpr i64 maxcost = 1e9, inf = 1e18;

	template<typename Cap, typename Cost> struct flow{
		struct edge{
			int v = -1;
			Cap w = 0;
			Cost c = 0;
			int nxt = -1;
		};
		vector<edge> e;
		
		int n, s, t, tm;
		vector<int> bg, vis, fa, fae, cyc;
		Cost cost;
		vector<Cost> pi;
		
		void init(){
			n = 0, s = 0, t = 0, tm = 0, cost = 0;
			e.pb();
			e.pb();
		}
		flow(){
			init();
		}
		flow(int _n){
			init();
			n = _n;
			bg.resize(n, -1);
			vis.resize(n);
			fa.resize(n, -1);
			fae.resize(n);
			pi.resize(n);
		}

		int link(int u, int v, Cap w, Cost c){
			e.pb(v, w, c, bg[u]); bg[u] = int(size(e)) - 1;
			e.pb(u, 0, -c, bg[v]); bg[v] = int(size(e)) - 1;
			return int(size(e)) - 2;
		}
		void dfs(int u){
			vis[u] = 1;
			for(int i = bg[u]; ~i; i = e[i].nxt){
				int v = e[i].v;
				if(!vis[v] && e[i].w){
					fa[v] = u, fae[v] = i;
					dfs(v);
				}
			}
		}
		Cost phi(int u){
			if(u < 0){
				return 0;
			}
			if(vis[u] == tm){
				return pi[u];
			}
			vis[u] = tm;
			return pi[u] = phi(fa[u]) + e[fae[u]].c;
		}
		void pushflow(int id){
			tm ++;
			int p = e[id ^ 1].v, q = e[id].v;
			while(~p){
				vis[p] = tm, p = fa[p];
			}
			while(~q && vis[q] != tm){
				vis[q] = tm, q = fa[q];
			}
			int lca = q;

			int mnv = -1, op = 2;
			Cap mnf = e[id].w;
			for(int i = e[id].v; i != lca; i = fa[i]){
				cyc.pb(fae[i] ^ 1);
				if(e[fae[i] ^ 1].w < mnf){
					mnv = i, mnf = e[fae[i] ^ 1].w, op = 0;
				}
			}
			for(int i = e[id ^ 1].v; i != lca; i = fa[i]){
				cyc.pb(fae[i]);
				if(e[fae[i]].w < mnf){
					mnv = i, mnf = e[fae[i]].w, op = 1;
				}
			}
			cyc.pb(id);

			for(int eid:cyc){
				e[eid].w -= mnf, e[eid ^ 1].w += mnf, cost += mnf * e[eid].c;
			}
			cyc.clear();

			if(op == 2){
				return;
			}
			int lste = id ^ (op ^ 1), lst = e[lste].v, now = e[lste ^ 1].v;
			while(lst != mnv){
				vis[now] = 0, lste ^= 1;
				swap(lste, fae[now]);
				swap(lst, fa[now]);
				swap(lst, now);
			}
		}
		pair<Cap, Cost> solve(){
			dfs(t);
			vis[t] = tm = 2, fa[t] = -1, cost = 0;

			int cnt = int(size(e));
			for(int i = 0, pre = cnt - 1; i != pre; i = (i == cnt - 1)? 0: i+1){
				if(e[i].w && e[i].c + (phi(e[i ^ 1].v) - phi(e[i].v)) < 0){
					pre = i;
					pushflow(i);
				}
			}
			return {e.back().w, cost};
		}
		pair<Cap, Cost> solve(int _s, int _t){
			s = _s, t = _t;
			link(t, s, inf, -maxcost);

			auto [w, c] = solve();
			pair<Cap, Cost> res{w, c + e.back().w * maxcost};
			e.pop_back();
			e.pop_back();
			return res;
		}
	};
}
using namespace _flow;

