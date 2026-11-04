// 依赖缺省源及 sa.cpp；按 ASCII（0..127）比较，不保留输入串引用。
// 查询下标 [0, n)，lcs(x, y) 包含位置 x、y；空串可构造，不可查询。
// 预处理 O(n log n) 时间、空间；lcp/lcs/cmp 均为 O(1)。
// 要求长度能用 int 表示；两份 ST 在 n = 10^6 时约占 145 MiB。
struct sa_lcp{
	struct rmq{
		vector<vi> f;
		rmq(vi a = {}){
			int n = int(size(a));
			if(!n){
				return;
			}
			f.pb(move(a));
			rep(k, 1, __lg(n)){
				int len = 1 << k;
				f.pb(vi(n - len + 1));
				rep(i, 0, n - len){
					f[k][i] = min(f[k - 1][i], f[k - 1][i + len / 2]);
				}
			}
		}
		int get(int l, int r)const {
			int k = __lg(r - l + 1);
			return min(f[k][l], f[k][r - (1 << k) + 1]);
		}
	};
	int n;
	vi rk, rrk;
	rmq pre, suf;
	sa_lcp(const string &s): n(int(size(s))){
		auto [a, b] = SA(s);
		pre = rmq(get_height(s, b, a));
		rk = move(a);
		string t(s.rbegin(), s.rend());
		auto [c, d] = SA(t);
		suf = rmq(get_height(t, d, c));
		rrk = move(c);
	}
	int lcp(int x, int y)const {
		if(x == y){
			return n - x;
		}
		int l = rk[x], r = rk[y];
		if(l > r){
			swap(l, r);
		}
		return pre.get(l + 1, r);
	}
	int lcs(int x, int y)const {
		if(x == y){
			return x + 1;
		}
		int l = rrk[n - 1 - x], r = rrk[n - 1 - y];
		if(l > r){
			swap(l, r);
		}
		return suf.get(l + 1, r);
	}
	bool cmp(int x, int y)const {
		return rk[x] < rk[y];
	}
};
