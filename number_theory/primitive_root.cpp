// 试除版，仅依赖缺省源中的 i64、pb 和标准库；与 Pollard–Rho 版二选一。
// 1<=d<=2^62-1；返回最小原根，不存在（含 d<=1）返回 -1；大素数可能很慢。
i64 primitive_root(i64 d){
	using i128 = __int128;
	if(d == 2 || d == 4){
		return d - 1;
	}
	if(d <= 1 || d % 4 == 0){
		return -1;
	}
	i64 n = (d % 2 ? d : d / 2), p = 3;
	while(p * p <= n && n % p != 0){
		p += 2;
	}
	if(p * p > n){
		p = n;
	}
	i64 t = n;
	while(t % p == 0){
		t /= p;
	}
	if(t != 1){
		return -1;
	}
	i64 phi = n / p * (p - 1), v = p - 1;
	vector<i64> f;
	for(i64 q = 2; q * q <= v; q++){
		if(v % q == 0){
			f.pb(q);
			while(v % q == 0){
				v /= q;
			}
		}
	}
	if(v > 1){
		f.pb(v);
	}
	if(n != p){
		f.pb(p); // phi=p^(k-1)*(p-1)，仅需分解 p-1。
	}
	auto qpow = [](i64 x, i64 y, i64 mod){
		i64 res = 1;
		while(y){
			if(y % 2){
				res = i64(i128(res) * x % mod);
			}
			x = i64(i128(x) * x % mod);
			y /= 2;
		}
		return res;
	};
	for(i64 g = 2; g < d; g++){
		if(gcd(g, d) != 1){
			continue;
		}
		bool ok = true;
		for(i64 q:f){
			if(qpow(g, phi / q, d) == 1){
				ok = false;
				break;
			}
		}
		if(ok){
			return g;
		}
	}
	return -1;
}
