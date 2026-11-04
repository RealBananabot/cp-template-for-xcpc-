#include <bits/stdc++.h>
using namespace std;
using i64 = long long;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#define per(i, j, k) for(int i = (j); i >= (k); i--)
#define pb emplace_back
#define fi first
#define se second
using vi = vector<int>;
using pi = pair<int, int>;

bool chmin(auto &x, const auto &y){
	if(y < x){x = y; return true;} return false;
}
bool chmax(auto &x, const auto &y){
	if(x < y){x = y; return true;} return false;
}

ostream& operator << (ostream &os, const auto &v) 
	requires requires { v.begin(); } && (!requires { v.substr(); }) {
	os << "[";
	for (int f = 0; const auto& x : v) os << (f++ ? ", " : "") << x;
	return os << "]";
}

template<typename ...Ar> void debug(char *s, Ar... x){
	auto f = [&](auto y){
		int dep = 0;
		while(*s && !(*s == ',' && dep == 0)){
			if(*s == '(') dep++;
			if(*s == ')') dep--;
			cerr << *s++;
		}
		cerr <<" = "<< y;
		if(*s == ','){
			cerr << *s ++;
		} else{
			cerr << endl;
		}
	};

	(f(x), ...);
}
#define gdb(...) debug((char*)#__VA_ARGS__, __VA_ARGS__)

signed main(){
	#ifdef LOCAL
	assert( freopen(".in", "r", stdin) );
	// assert( freopen(".out", "w", stdout) );
	#endif
	ios::sync_with_stdio(0);
	cin.tie(0);

}
