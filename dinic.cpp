namespace _flow{
	template<typename Cap> struct flow{
		static constexpr Cap inf = numeric_limits<Cap>::max();
		struct edge{
			int v = -1;
			Cap w = 0;
			int nxt = 0;
		};
		vector<edge> E;
		
		int n, S, T, tot;
		vector<int> bg, cur, dis, Q;
		int head, tail;
		
		void init(){
			n = 0, S = 0, T = 0, tot = 2, head = 0, tail = 0;
			E.resize(2);
		}
		flow(){
			init();
		}
		flow(int _n){
			init();
			n = _n;
			bg.resize(n);
			cur.resize(n);
			dis.resize(n, -1);
			Q.resize(n + 1);
		}

		int link(int u, int v, Cap w){
			E.pb(edge{v, w, bg[u]}); bg[u] = tot++;
			E.pb(edge{u, 0, bg[v]}); bg[v] = tot++;
			return tot - 2;
		}
		bool bfs(){
			head = 1;
			while(tail){
				int u = Q[tail--];
				dis[u] = -1, cur[u] = bg[u];
			}
			dis[S] = 0;
			Q[++tail] = S;
			while(head <= tail){
				int u = Q[head++];
				for(int id = bg[u]; id; id = E[id].nxt){
					int v = E[id].v;
					Cap w = E[id].w;
					if(w && dis[v] == -1){
						dis[v] = dis[u] + 1;
						Q[++tail] = v;
						if(v == T){
							return 1;
						}
					}
				}
			}
			return 0;
		}
		Cap dfs(int u, Cap rst){
			if(u == T){
				return rst;
			}
			Cap pre = rst;
			for(; cur[u]; cur[u] = E[cur[u]].nxt){
				int v = E[cur[u]].v;
				Cap w = E[cur[u]].w;
				if(w && dis[u] + 1 == dis[v]){
					Cap f = dfs(v, min(rst, w));
					E[cur[u]].w -= f, E[cur[u] ^ 1].w += f, rst -= f;
					if(!rst){
						break;
					}
				}
			}
			return pre - rst;
		}
		Cap solve(){
			rep(i, 0, n - 1){
				dis[i] = -1, cur[i] = bg[i];
			}
			Cap res = 0;
			while(bfs()){
				res += dfs(S, inf);
			}
			return res;
		}
		Cap solve(int _S, int _T){
			S = _S, T = _T;
			return solve();
		}
	};
}
using namespace _flow;
