// 依赖缺省源中的 vi、pb；按无符号字节进行 Lyndon 分解。
// 返回边界 {0, ..., n}，各段为 [res[i], res[i + 1])；空串返回 {0}。
// 各段均为 Lyndon 串，且字典序不增；长度须能用 int 表示。
// Duval 算法 O(n) 时间、O(1) 辅助空间，返回数组空间 O(分段数 + 1)。
vi lyndon(const string &s){
	int n = (int)s.size(), i = 0;
	vi res{0};
	while(i < n){
		int j = i, k = i + 1;
		while(k < n && (unsigned char)s[j] <= (unsigned char)s[k]){
			j = (s[j] == s[k] ? j + 1 : i);
			k++;
		}
		int len = k - j;
		while(i <= j){
			i += len;
			res.pb(i);
		}
	}
	return res;
}
