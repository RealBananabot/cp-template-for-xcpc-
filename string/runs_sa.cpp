// 接在缺省源与 sa.cpp 后使用；与 runs_hash.cpp 二选一。
// 返回排序、去重后的 {l, r, p}，0 下标闭区间，p 为最小周期。
// 按无符号字节比较，支持空串；长度须能用 int 表示。
// O(n log n) 时间、空间；两份 ST 在 n = 10^6 时合计约 145 MiB。
vector<tuple<int, int, int>> get_runs(const string &s){
	int n = (int)s.size();
	if(n < 2){
		return {};
	}
	auto build = [&](const string &t){
		auto [rk, sa] = SA(t);
		vector<vi> f;
		f.pb(get_height(t, sa, rk));
		rep(k, 1, __lg(n)){
			int len = 1 << k;
			f.pb(vi(n - len + 1));
			rep(i, 0, n - len){
				f[k][i] = min(f[k - 1][i], f[k - 1][i + len / 2]);
			}
		}
		return pair{move(rk), move(f)};
	};
	auto [rk, f] = build(s);
	auto [rrk, g] = build(string(s.rbegin(), s.rend()));
	// l、r 为不同后缀的排名，查询 height[min(l,r) + 1 .. max(l,r)]。
	auto query = [&](const vector<vi> &f, int l, int r){
		if(l > r){
			swap(l, r);
		}
		int k = __lg(r - l);
		return min(f[k][l + 1], f[k][r - (1 << k) + 1]);
	};
	auto lcp = [&](int x, int y){
		return (x == y ? n - x : query(f, rk[x], rk[y]));
	};
	auto lcs = [&](int x, int y){
		return (x == y ? x + 1 : query(g, rrk[n - 1 - x], rrk[n - 1 - y]));
	};
	auto cmp = [&](int x, int y){
		return rk[x] < rk[y];
	};
	vector<tuple<int, int, int>> res;
	auto chk = [&](int x, int y){
		if(s[x] != s[y]){
			return;
		}
		int ri = lcp(x, y), le = lcs(x, y);
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
			while(!stk.empty() && (rev ? cmp(stk.back(), i) : cmp(i, stk.back()))){
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
