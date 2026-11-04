#include <bits/stdc++.h>
using namespace std;
using vi = vector<int>;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#include "min_rotation.cpp"
int brute(const vi &a){
	int n = int(size(a)), ans = (n ? 0 : -1);
	rep(i, 1, n - 1){
		rep(k, 0, n - 1){
			int x = a[(i + k) % n], y = a[(ans + k) % n];
			if(x == y) continue;
			if(x < y) ans = i;
			break;
		}
	}
	return ans;
}
void check(const vi &a){
	vi b = a;
	assert(min_rotation(a) == brute(a));
	assert(a == b);
}
int main(int argc, char **){
	if(argc > 1){
		int n = 10000000;
		vi a(n, 7);
		assert(min_rotation(a) == 0);
		rep(i, 0, n - 1) a[i] = i % 2;
		assert(min_rotation(a) == 0);
		rep(i, 0, n - 1) a[i] = (i + 1) % 2;
		assert(min_rotation(a) == 1);
		// Here n % 3 == 1: the truncated period changes the best rotation.
		rep(i, 0, n - 1) a[i] = i % 3;
		assert(min_rotation(a) == n - 1);
		rep(i, 0, n - 1) a[i] = (i + 1) % 3;
		assert(min_rotation(a) == n - 2);
		rep(i, 0, n - 1) a[i] = n - i;
		assert(min_rotation(a) == n - 1);
		rep(i, 0, n - 1) a[i] = i;
		assert(min_rotation(a) == 0);
		a.assign(n, INT_MAX); a[n - 1] = INT_MIN;
		assert(min_rotation(a) == n - 1);
		cout << "8 max-length checks passed\n";
		return 0;
	}
	int cases = 0, ways = 1;
	rep(n, 0, 10){
		rep(mask, 0, ways - 1){
			vi a(n); int v = mask;
			for(int &x:a) x = v % 3 - 1, v /= 3;
			check(a); cases++;
		}
		ways *= 3;
	}
	mt19937 rng(1368);
	rep(it, 1, 10000){
		int n = int(rng() % 81);
		vi a(n);
		for(int &x:a){
			if(it % 3 == 0) x = (rng() & 1 ? INT_MIN : INT_MAX);
			else if(it % 3 == 1) x = int(rng() % 30);
			else x = int(rng());
		}
		check(a); cases++;
	}
	cout << cases << " exhaustive and random checks passed\n";
}
