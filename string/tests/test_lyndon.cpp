#include <bits/stdc++.h>
using namespace std;
using vi = vector<int>;
#define rep(i, j, k) for(int i = (j); i <= (k); i++)
#define pb emplace_back
#include "lyndon.cpp"

int original(const string &s){
	int n = (int)s.size(), ans = 0;
	int i = 0;
	while(i < n){
		int j = i, k = i + 1;
		while(k < n && s[j] <= s[k]){
			if(s[j] == s[k]) j++, k++;
			else j = i, k++;
		}
		while(i <= j){
			i += k - j;
			ans ^= i;
		}
	}
	return ans;
}

int cmp(const string &s, int l, int r, int x, int y){
	while(l < r && x < y){
		unsigned char a = s[l++], b = s[x++];
		if(a != b) return (a < b ? -1 : 1);
	}
	return (l == r ? (x == y ? 0 : -1) : 1);
}

bool is_lyndon(const string &s, int l, int r){
	int n = r - l;
	rep(shift, 1, n - 1){
		int result = 0;
		rep(i, 0, n - 1){
			unsigned char a = s[l + i], b = s[l + (i + shift) % n];
			if(a != b){
				result = (a < b ? -1 : 1);
				break;
			}
		}
		if(result != -1) return false;
	}
	return true;
}

vi brute(const string &s){
	int n = (int)s.size(), count = 0;
	vector<vi> valid(n, vi(n + 1));
	rep(l, 0, n - 1) rep(r, l + 1, n) valid[l][r] = is_lyndon(s, l, r);
	vi path{0}, answer;
	auto dfs = [&](auto &self, int l)->void {
		if(l == n){
			count++;
			answer = path;
			return;
		}
		rep(r, l + 1, n){
			if(!valid[l][r]) continue;
			if(path.size() >= 2 && cmp(s, path[path.size() - 2], l, l, r) < 0) continue;
			path.pb(r);
			self(self, r);
			path.pop_back();
		}
	};
	dfs(dfs, 0);
	assert(count == 1);
	return answer;
}

int cases = 0, brute_cases = 0, old_cases = 0;
void check(const string &s){
	int n = (int)s.size();
	vi a = lyndon(s);
	assert(!a.empty() && a.front() == 0 && a.back() == n);
	rep(i, 1, (int)a.size() - 1){
		assert(a[i - 1] < a[i]);
		assert(is_lyndon(s, a[i - 1], a[i]));
		if(i > 1) assert(cmp(s, a[i - 2], a[i - 1], a[i - 1], a[i]) >= 0);
	}
	if(n <= 10){
		assert(a == brute(s));
		brute_cases++;
	}
	if(all_of(s.begin(), s.end(), [](unsigned char c){return c < 128;})){
		int ans = 0;
		for(int x:a) ans ^= x;
		assert(ans == original(s));
		old_cases++;
	}
	cases++;
}

int main(int argc, char **argv){
	if(argc > 1){
		string mode = argv[1];
		int n = 5000001;
		string s(n, 'a');
		if(mode == "single") fill(s.begin() + 1, s.end(), 'b');
		if(mode == "periodic") rep(i, 0, n - 1) s[i] = char('a' + (i & 1));
		vi a = lyndon(s);
		if(mode == "single") assert(a == vi({0, n}));
		if(mode == "repeated"){
			assert((int)a.size() == n + 1);
			rep(i, 0, n) assert(a[i] == i);
		}
		if(mode == "periodic"){
			assert((int)a.size() == (n - 1) / 2 + 2 && a.back() == n);
			rep(i, 0, (int)a.size() - 2) assert(a[i] == 2 * i);
		}
		int ans = 0;
		for(int x:a) ans ^= x;
		assert(ans == original(s));
		cout << mode << " passed, n=" << n << ", boundaries=" << a.size() << '\n';
		return 0;
	}
	assert(lyndon("ababa") == vi({0, 2, 4, 5}));
	check("ababa");
	check("");
	rep(c, 0, 255) check(string(1, char(c)));
	rep(a, 0, 255) rep(b, 0, 255) check(string{char(a), char(b)});
	rep(n, 0, 9){
		int total = 1;
		rep(i, 1, n) total *= 3;
		rep(mask, 0, total - 1){
			int x = mask;
			string s(n, 'a');
			for(char &c:s){c += x % 3; x /= 3;}
			check(s);
		}
	}
	mt19937 rng(6114);
	rep(tc, 1, 20000){
		int n = (int)(rng() % 81);
		int sigma = array{1, 2, 3, 26, 256}[rng() % 5];
		string s(n, 0);
		for(char &c:s) c = char(rng() % sigma);
		check(s);
	}
	cout << "passed " << cases << " cases; " << brute_cases << " exhaustive-partition checks; " << old_cases << " original comparisons\n";
}
