// 依赖缺省源中的 i64、vi、rep；s 须在对象使用期间保持有效且不变。
// 按无符号字节比较，查询下标 [0, n)，lcs(x, y) 包含位置 x、y。
// val(l, r) 为左闭右开区间 [l, r) 的哈希。
// 预处理 O(n)，空间 O(n)，lcp/lcs/cmp 为 O(log n)。
// 固定单模哈希可能碰撞，不保证正确；要求长度 n < INT_MAX。
struct hash_lcp{
	static const int base = 107897, mod = 998244353;
	const string &s;
	int n;
	vi pw, pre;
	hash_lcp(const string &_s): s(_s), n((int)s.size()), pw(n + 1), pre(n + 1){
		pw[0] = 1;
		rep(i, 0, n - 1){
			pw[i + 1] = (i64)pw[i] * base % mod;
			pre[i + 1] = ((i64)pre[i] * base + (unsigned char)s[i]) % mod;
		}
	}
	int val(int l, int r)const {
		int res = pre[r] - (i64)pre[l] * pw[r - l] % mod;
		return (res < 0 ? res + mod : res);
	}
	int lcp(int x, int y)const {
		if(x == y){
			return n - x;
		}
		if(s[x] != s[y]){
			return 0;
		}
		int l = 0, r = n - max(x, y);
		while(l < r){
			int mid = l + (r - l + 1) / 2;
			if(val(x, x + mid) == val(y, y + mid)){
				l = mid;
			} else{
				r = mid - 1;
			}
		}
		return l;
	}
	int lcs(int x, int y)const {
		if(x == y){
			return x + 1;
		}
		if(s[x] != s[y]){
			return 0;
		}
		int l = 0, r = min(x, y) + 1;
		while(l < r){
			int mid = l + (r - l + 1) / 2;
			if(val(x - mid + 1, x + 1) == val(y - mid + 1, y + 1)){
				l = mid;
			} else{
				r = mid - 1;
			}
		}
		return l;
	}
	bool cmp(int x, int y)const {
		int len = lcp(x, y);
		if(x + len == n || y + len == n){
			return x + len == n && y + len != n;
		}
		return (unsigned char)s[x + len] < (unsigned char)s[y + len];
	}
};
