// 依赖缺省源中的 vi；返回字典序最小循环表示的起始下标（从 0 开始）。
// 多个起点对应相同最小序列时返回最小下标；空序列返回 -1。
// 支持任意 int 元素；长度须不超过 INT_MAX / 2。
// Duval 算法 O(n) 时间、O(1) 辅助空间，不修改输入。
int min_rotation(const vi &a){
	int n = int(size(a)), i = 0, ans = -1;
	while(i < n){
		ans = i;
		int j = i, k = i + 1;
		while(k < n * 2){
			int x = a[(j < n ? j : j - n)];
			int y = a[(k < n ? k : k - n)];
			if(x > y){
				break;
			}
			j = (x == y ? j + 1 : i);
			k++;
		}
		int len = k - j;
		while(i <= j){
			i += len;
		}
	}
	return ans;
}
