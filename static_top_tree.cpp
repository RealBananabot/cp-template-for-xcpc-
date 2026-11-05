// 依赖缺省源中的 vi、pi、rep、per、pb、fi；保留结构与信息合并接口。
// G 为 0 下标无向树，root 为原树根；非空时须 0 <= root < n。
// 原树点 0..n-1 对应叶子，内部节点 n..2n-2，rt 为 Top Tree 根。
// P 为簇父亲，L/R 为左右子簇；根父亲和叶子儿子均为 -1。
// type：叶子 -1，rake 0（保留左簇下界），compress 1（左上、右下）。
// 固定原树拓扑，不支持 link/cut；build 会清空并重建结构。
// 空树 rt = -1；要求 n <= INT_MAX / 2 且内存足够。
// 此实现构建 O(n log n)、空间 O(n)，结构高度 O(log n)。
struct Tree{
	// 按题意填写 T、叶子初始化与两种合并；复杂度按单次合并 O(1) 计。
	struct T{};
	void rake(T &, const T &, const T &){}
	void compress(T &, const T &, const T &){}

	int n = 0, rt = -1;
	vi P, L, R, type;
	vector<T> D;
	Tree(){}
	Tree(vector<vi> G, int root = 0){
		build(move(G), root);
	}
	// 仅对内部节点调用；rake / compress 的左右子簇顺序不可交换。
	void pushup(int u){
		if(type[u] == 0){
			rake(D[u], D[L[u]], D[R[u]]);
		} else{
			compress(D[u], D[L[u]], D[R[u]]);
		}
	}
	int opt(int u, int v, int op){
		int x = int(size(P));
		P.pb(-1);
		L.pb(u);
		R.pb(v);
		type.pb(op);
		D.pb();
		P[u] = P[v] = x;
		return x;
	}
	// a 中的权重为所含原树点数，仅用于平衡，顺序不可改变。
	pi mg(vector<pi> a, int op){
		if(size(a) == 1){
			return a[0];
		}
		int sum = 0;
		for(auto [u, s]:a){
			sum += s;
		}
		vector<pi> le, ri;
		for(auto [u, s]:a){
			(sum > s ? le : ri).pb(u, s);
			sum -= s * 2;
		}
		auto [u, su] = mg(move(le), op);
		auto [v, sv] = mg(move(ri), op);
		return {opt(u, v, op), su + sv};
	}
	void build(vector<vi> G, int root = 0){
		n = int(size(G)), rt = -1;
		P.assign(n, -1);
		L.assign(n, -1);
		R.assign(n, -1);
		type.assign(n, -1);
		D.assign(n, {}); // 在此填写叶子 D[0..n-1] 的初始化。
		if(!n){
			return;
		}
		int m = n * 2 - 1;
		P.reserve(m);
		L.reserve(m);
		R.reserve(m);
		type.reserve(m);
		D.reserve(m);
		vi ord{root}, pa(n, -1), sz(n, 1);
		rep(i, 0, n - 1){
			int u = ord[i];
			if(pa[u] != -1){
				G[u].erase(find(G[u].begin(), G[u].end(), pa[u]));
			}
			for(int v:G[u]){
				pa[v] = u;
				ord.pb(v);
			}
		}
		per(i, n - 1, 0){
			int u = ord[i];
			for(int v:G[u]){
				sz[u] += sz[v];
			}
			if(!G[u].empty()){
				iter_swap(G[u].begin(), max_element(G[u].begin(), G[u].end(), [&](int x, int y){
					return sz[x] < sz[y];
				}));
			}
		}
		auto Rake = [&](auto &r, auto &c, int u)->pi {
			vector<pi> a{{G[u][0], 1}};
			rep(i, 1, int(size(G[u])) - 1){
				a.pb(c(r, c, G[u][i]));
			}
			return mg(move(a), 0);
		};
		auto Compress = [&](auto &r, auto &c, int u)->pi {
			vector<pi> a{{u, 1}};
			while(!G[u].empty()){
				a.pb(r(r, c, u));
				u = G[u][0];
			}
			return mg(move(a), 1);
		};
		rt = Compress(Rake, Compress, root).fi;
		rep(i, n, int(size(P)) - 1){
			pushup(i);
		}
	}
};
