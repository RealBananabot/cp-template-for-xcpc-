// T 为 int 或 i64，a,b 非负；返回 g=gcd(a,b)，使 a*x+b*y=g；x、y 须为不同变量。
// (0,0) 返回 g=0、x=1、y=0；O(log(max(a,b)+2)) 时间与递归栈空间。
template<class T>
T exgcd(T a, T b, T &x, T &y){
	if(b == 0){
		x = 1, y = 0;
		return a;
	}
	T g = exgcd(b, a % b, y, x);
	y -= a / b * x;
	return g;
}

// 0 <= x <= INT_MAX，1 <= p <= INT_MAX，且 gcd(x,p)=1；返回 [0,p) 内的逆元。
// 不检查可逆性；系数已在 (-p,p)，只需修正负数；p=1 返回 0。
// 时间与递归栈空间均为 O(log(p+1))。
int inv(int x, int p){
	int res, _;
	exgcd(x, p, res, _);
	if(res < 0){
		res += p;
	}
	return res;
}

// x 为任意 int，1 <= p <= INT_MAX；返回 [0,p) 内的逆元，无逆元返回 -1。
// p=1 返回 0；模数不必为质数。时间与递归栈空间均为 O(log(p+1))。
int safe_inv(int x, int p){
	x %= p;
	if(x < 0){
		x += p;
	}
	int res, y;
	if(exgcd(x, p, res, y) != 1){
		return -1;
	}
	if(res < 0){
		res += p;
	}
	return res;
}
