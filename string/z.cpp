// 依赖缺省源中的 vi、rep；按字节比较，长度须能用 int 表示。
// z[i] = lcp(s, s[i..])，z[0] = n；空串返回空数组。
// O(n) 时间、O(n) 空间；用于扩展匹配时，初始区间应为空（r = -1）。
vi zfunc(const string &s){
	int n = (int)s.size();
	vi z(n);
	if(n){
		z[0] = n;
	}
	int l = 0, r = -1;
	rep(i, 1, n - 1){
		if(i <= r){
			z[i] = min(z[i - l], r - i + 1);
		}
		while(z[i] < n - i && s[z[i]] == s[i + z[i]]){
			z[i]++;
		}
		if(i + z[i] - 1 > r){
			l = i, r = i + z[i] - 1;
		}
	}
	return z;
}

// p[i] = lcp(t, s[i..])；s 为文本串，t 为模式串。
// 空文本返回空数组，空模式返回全 0；O(n + m) 时间、空间。
vi exkmp(const string &s, const string &t){
	int n = (int)s.size(), m = (int)t.size();
	vi p(n);
	if(!n || !m){
		return p;
	}
	vi z = zfunc(t);
	int l = 0, r = -1;
	rep(i, 0, n - 1){
		if(i <= r){
			p[i] = min(z[i - l], r - i + 1);
		}
		while(p[i] < m && p[i] < n - i && s[i + p[i]] == t[p[i]]){
			p[i]++;
		}
		if(i + p[i] - 1 > r){
			l = i, r = i + p[i] - 1;
		}
	}
	return p;
}

// 示例：exkmp("aaaabaa", "aaaaa") 返回 {4, 3, 2, 1, 0, 2, 1}。
