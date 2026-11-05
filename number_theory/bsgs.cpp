// 依赖缺省源中的 i64、rep、se；0 <= a,b <= INT_MAX，1 <= p <= INT_MAX。
// 返回 a^x = b (mod p) 的最小非负解，无解返回 -1；约定 0^0=1，p=1 返回 0。
// 不要求互质或质数模数；哈希平均 O(1) 时，时间 O(sqrt(p)+log(p))，空间 O(sqrt(p))。
int bsgs(int a, int p, int b){
	a %= p, b %= p;
	int t = __lg(p), z = 1 % p;
	rep(i, 0, t){
		if(z == b){
			return i;
		}
		if(i < t){
			z = int(i64(z) * a % p);
		}
	}
	int g = gcd(z, p);
	if(b % g){
		return -1;
	}
	z /= g, b /= g, p /= g; // z*a^y = b (mod p)，此时 gcd(a,p)=1。
	int B = int(sqrt(p)) + 1;
	unordered_map<int, int> vis;
	vis.reserve(B);
	for(int j = 0, r = b; j < B; j++){
		vis[r] = j; // 相同余数保留最大 j，使 i*B-j 最小。
		r = int(i64(r) * a % p);
	}
	int step = 1, v = a % p;
	for(int e = B; e; e /= 2){
		if(e % 2){
			step = int(i64(step) * v % p);
		}
		v = int(i64(v) * v % p);
	}
	rep(i, 1, B){
		z = int(i64(z) * step % p);
		auto it = vis.find(z);
		if(it != vis.end()){
			return int(i64(i) * B - it->se + t);
		}
	}
	return -1;
}
