// 依赖缺省源中的 vi、rep；变换为 ?s[0]?s[1]?...?，长度为 2 * |s| + 1。
// d[i] 为不含中心的半径，覆盖变换串 [i - d[i], i + d[i]]。
// 对应原串 [(i - d[i]) / 2, (i + d[i]) / 2)，最长回文长度为 max(d)。
// 空串返回 {0}；按字节比较，要求 |s| <= (INT_MAX - 1) / 2；O(n) 时间、空间。
vi manacher(const string &s){
	int m = (int)s.size(), n = m * 2 + 1;
	string t(n, '?');
	rep(i, 0, m - 1){
		t[i * 2 + 1] = s[i];
	}
	vi d(n);
	int l = 0, r = -1;
	rep(i, 0, n - 1){
		if(i <= r){
			d[i] = min(d[l + (r - i)], r - i);
		}
		while(d[i] < i && d[i] < n - i - 1 && t[i - d[i] - 1] == t[i + d[i] + 1]){
			d[i]++;
		}
		if(i + d[i] > r){
			l = i - d[i], r = i + d[i];
		}
	}
	return d;
}
