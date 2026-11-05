// Cipolla；仅依赖缺省源中的 i64、fi、se 和标准库。
// p 须为质数，2<=p<=2^63-1，n 为任意 i64；返回最小非负平方根，无解返回 -1。
// 理想随机模型下期望 O(log p) 时间，O(1) 空间；不检查素性。
i64 mod_sqrt(i64 n, i64 p){
	using i128 = __int128;
	n %= p;
	if(n < 0){
		n += p;
	}
	if(p == 2 || n == 0){
		return n;
	}
	auto qpow = [&](i64 x, i64 y){
		i64 res = 1;
		while(y){
			if(y % 2){
				res = i64(i128(res) * x % p);
			}
			x = i64(i128(x) * x % p);
			y /= 2;
		}
		return res;
	};
	if(qpow(n, p / 2) != 1){
		return -1;
	}
	if(p % 4 == 3){
		i64 r = qpow(n, p / 4 + 1);
		return min(r, p - r);
	}
	mt19937_64 rnd(3663);
	i64 a, w;
	do{
		a = i64(rnd() % p);
		w = i64((i128(a) * a - n + p) % p);
	} while(qpow(w, p / 2) != p - 1);
	auto mul = [&](pair<i64, i64> x, pair<i64, i64> y){
		i64 t = i64(i128(x.se) * y.se % p); // 三因子连乘须先取模。
		return pair{i64((i128(x.fi) * y.fi + i128(t) * w) % p),
			i64((i128(x.fi) * y.se + i128(x.se) * y.fi) % p)};
	};
	pair<i64, i64> r{1, 0}, x{a, 1}; // (u,v) 表示 u+v*sqrt(w)。
	for(i64 e = p / 2 + 1; e; e /= 2){
		if(e % 2){
			r = mul(r, x);
		}
		x = mul(x, x);
	}
	return min(r.fi, p - r.fi);
}
