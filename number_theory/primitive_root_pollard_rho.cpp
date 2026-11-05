// Pollard–Rho 版，先粘贴 pollard_rho.cpp；与试除版二选一。
// 返回 d 的最小原根，不存在（含 d<=1）返回 -1。
// 支持完整正 i64 范围；仅 2、4、p^k、2p^k 有原根，p 为奇素数、k>=1。
i64 primitive_root(i64 d){
	if(d == 2 || d == 4){
		return d - 1;
	}
	if(d <= 1 || d % 4 == 0){
		return -1;
	}
	i64 n = (d % 2 ? d : d / 2), p = n;
	while(!PR::isprime(p)){
		p = PR::rho(p);
	}
	i64 t = n;
	while(t % p == 0){
		t /= p;
	}
	if(t != 1){
		return -1;
	}
	i64 phi = n / p * (p - 1);
	auto f = PR::solve(p - 1);
	if(n != p){
		f.pb(p); // phi=p^(k-1)*(p-1)，已知的 p 幂不必再分解。
	}
	f.erase(unique(f.begin(), f.end()), f.end());
	for(i64 g = 2; g < d; g++){
		if(gcd(g, d) != 1){
			continue;
		}
		bool ok = true;
		for(i64 q:f){
			if(PR::qpow(g, phi / q, d) == 1){
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
