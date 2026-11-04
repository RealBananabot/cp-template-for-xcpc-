// 依赖缺省源、sa.cpp、lcp_sa.cpp。
// get_runs<sa_lcp>(s)，也可传入已构造的查询对象。
// 返回排序、去重后的 {l, r, p}，0 下标闭区间，p 为最小周期；空串返回空。
// LCP 对象须对应同一 s，提供精确的 LCP、LCS 和后缀比较。
// 共 O(n) 次查询；单次查询 O(Q) 时枚举含排序 O(nQ + n log n)，辅助空间 O(n)。
// 另计查询结构预处理与空间；使用 sa_lcp 时总时间为 O(n log n)。
vector<tuple<int, int, int>> get_runs(const string &s, const auto &t){
	int n = int(size(s));
	vector<tuple<int, int, int>> res;
	auto chk = [&](int x, int y){
		if(s[x] != s[y]){
			return;
		}
		int ri = t.lcp(x, y), le = t.lcs(x, y);
		if(le + ri - 1 >= y - x){
			auto cur = tuple{x - le + 1, y + ri - 1, y - x};
			if(res.empty() || res.back() != cur){
				res.pb(cur);
			}
		}
	};
	vi stk;
	rep(rev, 0, 1){
		stk.clear();
		per(i, n - 1, 0){
			while(!stk.empty() && (rev ? t.cmp(stk.back(), i) : t.cmp(i, stk.back()))){
				stk.pop_back();
			}
			if(!stk.empty()){
				chk(i, stk.back());
			}
			stk.pb(i);
		}
	}
	sort(res.begin(), res.end());
	res.erase(unique(res.begin(), res.end()), res.end());
	return res;
}

template<typename T> auto get_runs(const string &s){
	return get_runs(s, T(s));
}
