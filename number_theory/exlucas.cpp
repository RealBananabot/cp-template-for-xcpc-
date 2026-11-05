// 依赖 exgcd.cpp 中的 inv，以及缺省源中的 i64、vi、rep。
// C(m,n) mod mod；m>=0，mod>=1；n<0 或 n>m 返回 0，mod=1 返回 0。
// 模数可合数；O(mod + log(mod+1)*log(m+2)) 时间，O(最大质数幂因子) 空间。
int exlucas(i64 m, i64 n, int mod){
	if(n < 0 || n > m || mod == 1){
		return 0;
	}
	i64 res = 0;
	for(int p = 2, t = mod; t > 1;){
		if(t % p){
			p++;
			continue;
		}
		int q = 1;
		while(t % p == 0){
			q *= p;
			t /= p;
		}
		vi f(q, 1);
		rep(i, 1, q - 1){
			f[i] = (i % p ? int(i64(f[i - 1]) * i % q) : f[i - 1]);
		}
		auto fac = [&](i64 x){
			i64 w = 1, c = 0;
			while(x){
				if(x / q % 2){
					w = w * f[q - 1] % q;
				}
				w = w * f[int(x % q)] % q;
				x /= p;
				c += x;
			}
			return pair{w, c}; // 去除全部 p 因子的阶乘、v_p(x!)。
		};
		auto [a, ca] = fac(m);
		auto [b, cb] = fac(n);
		auto [c, cc] = fac(m - n);
		i64 r = a * inv(int(b * c % q), q) % q;
		for(i64 e = ca - cb - cc; e && r; e--){
			r = r * p % q;
		}
		i64 k = mod / q;
		res = (res + k * inv(int(k % q), q) * r) % mod;
	}
	return int(res);
}
