// 依赖缺省源中的 i64、vi、rep、pb；字符编号 [0, S)，根为 1，0 表示空转移。
// n 为插入字符串的总长度上界（含重复前缀），须非负且 2n + 2 不溢出 int。
// 预留容量，转移表按需初始化；每个新串从 p = 1 开始，逐字符执行 p = ins(p, c)。
// 固定字符集时构建 O(n)，空间 O(Sn)；solve 返回不同非空子串数、状态数（含根）。
struct SAM{
	static const int S = 26;
	vector<array<int, S>> s;
	vi fa, len;
	int tot = 1;
	SAM(int n){
		int m = n * 2 + 2;
		s.reserve(m);
		s.resize(2);
		fa.resize(m);
		len.resize(m);
	}
	// 内部辅助函数，要求 s[p][c] != 0。
	int split(int p, int c){
		int v = s[p][c];
		if(len[v] == len[p] + 1){
			return v;
		}
		int q = ++tot;
		s.pb(s[v]);
		fa[q] = fa[v], len[q] = len[p] + 1;
		fa[v] = q;
		for(; p && s[p][c] == v; p = fa[p]){
			s[p][c] = q;
		}
		return q;
	}
	int ins(int p, int c){
		if(s[p][c]){
			return split(p, c);
		}
		int u = ++tot;
		s.pb();
		len[u] = len[p] + 1;
		for(; p && !s[p][c]; p = fa[p]){
			s[p][c] = u;
		}
		fa[u] = (p ? split(p, c) : 1);
		return u;
	}
	pair<i64, int> solve()const {
		i64 res = 0;
		rep(i, 2, tot){
			res += len[i] - len[fa[i]];
		}
		return {res, tot};
	}
};
