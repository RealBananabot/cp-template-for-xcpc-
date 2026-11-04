// 依赖缺省源中的 vi、rep、per；按无符号字节排序，支持空串。
// sa[r] 为排名 r 的后缀起点，rk[i] 为后缀 i 的排名，均从 0 开始。
// 返回 {rk, sa}；倍增构建 O(n log n) 时间、O(n) 空间。
// 长度须能用 int 表示；按字节处理，不进行 Unicode 字符解码。
pair<vi, vi> SA(const string &s){
	int n = (int)s.size();
	vi rk(n), sa(n);
	if(!n){
		return {};
	}
	vi cnt(max(n, 256)), tmp(n), nrk(n);
	for(unsigned char c:s){
		cnt[c]++;
	}
	rep(i, 1, 255){
		cnt[i] += cnt[i - 1];
	}
	per(i, n - 1, 0){
		sa[--cnt[(unsigned char)s[i]]] = i;
	}
	int ccnt = 0;
	rep(i, 0, n - 1){
		if(i && s[sa[i]] != s[sa[i - 1]]){
			ccnt++;
		}
		rk[sa[i]] = ccnt;
	}
	ccnt++;
	for(int len = 1; ccnt < n; len *= 2){
		fill(cnt.begin(), cnt.begin() + ccnt, 0);
		rep(i, 0, n - 1){
			cnt[rk[i]]++;
		}
		rep(i, 1, ccnt - 1){
			cnt[i] += cnt[i - 1];
		}
		int cur = 0;
		rep(i, n - len, n - 1){
			tmp[cur++] = i;
		}
		rep(i, 0, n - 1){
			if(sa[i] >= len){
				tmp[cur++] = sa[i] - len;
			}
		}
		per(i, n - 1, 0){
			sa[--cnt[rk[tmp[i]]]] = tmp[i];
		}
		ccnt = 0;
		nrk[sa[0]] = 0;
		rep(i, 1, n - 1){
			int x = sa[i - 1], y = sa[i];
			int a = (x < n - len ? rk[x + len] : -1);
			int b = (y < n - len ? rk[y + len] : -1);
			if(rk[x] != rk[y] || a != b){
				ccnt++;
			}
			nrk[y] = ccnt;
		}
		ccnt++;
		rk.swap(nrk);
		if(ccnt == n){
			break;
		}
	}
	return {move(rk), move(sa)};
}

// sa、rk 须为同一字符串的 SA 结果；O(n) 时间、O(n) 返回空间。
// height[0] = 0，height[r] = lcp(s[sa[r - 1]..], s[sa[r]..])。
vi get_height(const string &s, const vi &sa, const vi &rk){
	int n = (int)s.size();
	vi height(n);
	int k = 0;
	rep(i, 0, n - 1){
		if(!rk[i]){
			k = 0;
			continue;
		}
		if(k){
			k--;
		}
		int j = sa[rk[i] - 1];
		while(k < n - i && k < n - j && s[i + k] == s[j + k]){
			k++;
		}
		height[rk[i]] = k;
	}
	return height;
}
