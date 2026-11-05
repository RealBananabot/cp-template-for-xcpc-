// 依赖缺省源中的 i64、rep；适用于完整有符号 int 范围，x<2 返回 false。
// 固定底数 2,7,61；O(log(x+1)) 时间、O(1) 空间，无随机误判。
namespace PR{
	// 0 <= x < mod，y>=0，mod>=2。
	int qpow(int x, int y, int mod){
		int res = 1;
		while(y){
			if(y % 2){
				res = int(i64(res) * x % mod);
			}
			x = int(i64(x) * x % mod);
			y /= 2;
		}
		return res;
	}
	bool isprime(int x){
		if(x < 2 || x % 2 == 0){
			return x == 2;
		}
		const int T[]{2, 7, 61};
		int t = __builtin_ctz(x - 1);
		auto chk = [&](int a){
			a %= x;
			if(a == 0){
				return true;
			}
			int r = qpow(a, (x - 1) >> t, x);
			if(r == 1 || r == x - 1){
				return true;
			}
			rep(i, 1, t - 1){
				r = int(i64(r) * r % x);
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
}
