// 依赖缺省源中的 i64、vi、rep、per、pb；与 runs_sa.cpp 二选一。
// 返回排序、去重后的 {l, r, p}，0 下标闭区间，p 为最小周期。
// 按无符号字节比较，支持空串；要求 n < INT_MAX。
// O(n log n) 时间、O(n) 空间；固定单模哈希存在碰撞反例，不保证正确。
vector<tuple<int, int, int>> get_runs(const string &s){
	int n = (int)s.size();
	if(n < 2){
		return {};
	}
	const int base = 107897, mod = 998244353;
	vi pw(n + 1), pre(n + 1);
	pw[0] = 1;
	rep(i, 0, n - 1){
		pw[i + 1] = (i64)pw[i] * base % mod;
		pre[i + 1] = ((i64)pre[i] * base + (unsigned char)s[i]) % mod;
	}
	auto val = [&](int l, int r){
		int res = pre[r] - (i64)pre[l] * pw[r - l] % mod;
		return (res < 0 ? res + mod : res);
	};
	auto lcp = [&](int x, int y){
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
	};
	auto lcs = [&](int x, int y){
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
	};
	auto cmp = [&](int x, int y){
		int len = lcp(x, y);
		if(x + len == n || y + len == n){
			return x + len == n && y + len != n;
		}
		return (unsigned char)s[x + len] < (unsigned char)s[y + len];
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
