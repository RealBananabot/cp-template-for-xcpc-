// 依赖缺省源中的 vi、rep；按字节比较，长度须能用 int 表示。
// fail[i] 为 s[0..i] 最长真 border 的末下标，无 border 为 -1；长度为 fail[i] + 1。
// 空串返回空数组；O(n) 时间、O(n) 空间。
vi kmp(const string &s){
	int n = (int)s.size();
	vi fail(n, -1);
	rep(i, 1, n - 1){
		int p = fail[i - 1];
		while(p != -1 && s[i] != s[p + 1]){
			p = fail[p];
		}
		if(s[i] == s[p + 1]){
			p++;
		}
		fail[i] = p;
	}
	return fail;
}
