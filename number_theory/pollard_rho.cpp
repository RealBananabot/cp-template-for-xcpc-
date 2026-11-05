// 内含 i64 Miller–Rabin；仅依赖缺省源中的 i64、pb、rep 和标准库。
namespace PR{
	using i128 = __int128;
	// 0 <= x < mod，y>=0，mod>=2。
	i64 qpow(i64 x, i64 y, i64 mod){
		i64 res = 1;
		while(y){
			if(y % 2){
				res = i64(i128(res) * x % mod);
			}
			x = i64(i128(x) * x % mod);
			y /= 2;
		}
		return res;
	}
	// 完整有符号 i64 范围的确定性判定，x<2 返回 false。
	bool isprime(i64 x){
		if(x < 2 || x % 2 == 0){
			return x == 2;
		}
		const int T[]{2, 325, 9375, 28178, 450775, 9780504, 1795265022};
		// const int T[]{2, 7, 61}; // 仅判定 int 范围时，用此行替换上行。
		int t = __builtin_ctzll(x - 1);
		auto chk = [&](i64 a){
			a %= x;
			if(a == 0){
				return true;
			}
			i64 r = qpow(a, (x - 1) >> t, x);
			if(r == 1 || r == x - 1){
				return true;
			}
			rep(i, 1, t - 1){
				r = i64(i128(r) * r % x);
				if(r == x - 1){
					return true;
				}
			}
			return false;
		};
		for(int a:T){
			if(!chk(a)){
				return false;
			}
		}
		return true;
	}
	mt19937_64 rnd(3663);
	// x 须为合数；返回严格介于 1 和 x 之间的因子，失败重新随机尝试。
	i64 rho(i64 x){
		if(x % 2 == 0){
			return 2;
		}
		for(;;){
			i64 c = i64(rnd() % x), p = 0, q = 0, prd = 1;
			for(int goal = 1; goal <= (1 << 20); goal *= 2){
				rep(i, 1, goal){
					q = i64((i128(q) * q + c) % x);
					prd = i64(i128(prd) * abs(q - p) % x);
					if(i % 127 == 0 && gcd(prd, x) != 1){
						break;
					}
				}
				i64 g = gcd(prd, x);
				if(g != 1){
					if(g != x){
						return g;
					}
					break;
				}
				p = q;
			}
		}
	}
	void work(i64 x, vector<i64> &a){
		if(x == 1){
			return;
		}
		if(isprime(x)){
			a.pb(x);
			return;
		}
		i64 g = rho(x);
		work(g, a);
		work(x / g, a);
	}
	// 1 <= x <= LLONG_MAX；返回不降质因子数组，保留重复项，x=1 返回空数组。
	vector<i64> solve(i64 x){
		vector<i64> a;
		work(x, a);
		sort(a.begin(), a.end());
		return a;
	}
}
