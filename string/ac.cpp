// 依赖缺省源中的 vi、rep、pb；根为 1，字符编号 [0, S)。
// n 为模式串总长上界（非串数），须非负且 n + 2 不溢出 int。
// 每个模式串从 p = 1 插入，p = ins(p, c)；保存终点，重复串终点相同。
// 全部插入后仅调用一次 build，此后不可插入或再次 build。
// 构建后 s 为完整转移表，fa[1] = 0 为哨兵。
// 模式串非空，文本可空；固定字符集时插入 O(总长)，构建 O(S * tot)，空间 O(Sn)。
struct AC{
	static const int S = 26;
	vector<array<int, S>> s;
	vi fa;
	int tot = 1;
	AC(int n){
		int m = n + 2;
		s.reserve(m);
		s.resize(2);
		fa.resize(m);
	}
	int ins(int p, int c){
		if(!s[p][c]){
			s[p][c] = ++tot;
			s.pb();
		}
		return s[p][c];
	}
	void build(){
		s[0].fill(1);
		queue<int> q;
		q.emplace(1);
		while(!q.empty()){
			int u = q.front();
			q.pop();
			rep(c, 0, S - 1){
				int &v = s[u][c];
				if(v){
					fa[v] = s[fa[u]][c];
					q.emplace(v);
				} else{
					v = s[fa[u]][c];
				}
			}
		}
	}
};
