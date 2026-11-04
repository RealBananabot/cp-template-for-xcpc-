// 依赖缺省源中的 vi、pb；str 为小写字母串，位置从 0 开始。
// 1 为偶根（len = 0），2 为奇根（len = -1），fa[1] = fa[2] = 2。
// 两根 dep = 0；非根 dep[u] = dep[fa[u]] + 1，为非空回文后缀个数。
// n 为处理长度上界，须非负且 n + 3 不溢出 int；最多 n + 2 个节点。
// 按 pos = 0, 1, ... 调用 u = ins(u, pos)，初始 u = 1；返回最长回文后缀节点。
// str 可预先赋值或逐字追加，已处理的前缀不可修改；固定字符集下 O(n) 时间、O(Sn) 空间。
struct PAM{
	static const int S = 26;
	string str;
	vector<array<int, S>> s;
	vi fa, len, dep;
	int tot = 2;
	PAM(int n){
		int m = n + 3;
		str.reserve(n);
		s.reserve(m);
		s.resize(3);
		fa.resize(m);
		len.resize(m);
		dep.resize(m);
		fa[1] = fa[2] = 2;
		len[1] = 0, len[2] = -1;
	}
	int getfa(int u, int pos){
		while(pos <= len[u] || str[pos] != str[pos - len[u] - 1]){
			u = fa[u];
		}
		return u;
	}
	int ins(int u, int pos){
		int c = str[pos] - 'a';
		u = getfa(u, pos);
		if(!s[u][c]){
			s[u][c] = ++tot;
			s.pb();
			len[tot] = len[u] + 2;
			fa[tot] = (u == 2 ? 1 : s[getfa(fa[u], pos)][c]);
			dep[tot] = dep[fa[tot]] + 1;
		}
		return s[u][c];
	}
};
