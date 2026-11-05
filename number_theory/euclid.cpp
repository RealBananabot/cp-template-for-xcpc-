// k>=0；T{} 须为拼接单位元，operator+ 须结合且表示从左到右拼接，不要求交换。
template<class T>
T monoid_pow(T x, i64 k){
	T res{};
	while(k){
		if(k % 2){
			res = res + x;
		}
		k /= 2;
		if(k){
			x = x + x;
		}
	}
	return res;
}

// 万欧；0<=p,l<=2^62-1，1<=q<=2^62-1，0<=r<q；仅依赖 i64。
// 返回 i=1..l 依次拼接 U^(floor((p*i+r)/q)-floor((p*(i-1)+r)/q)) R。
// O(log(p+q+1)+log(l+1)) 次拼接；初始高度的 U^(b/q) 由调用方处理。
template<class T>
T euclid(i64 p, i64 q, i64 r, i64 l, T U, T R){
	using i128 = __int128;
	if(l == 0){
		return {};
	}
	if(p >= q){
		return euclid(p % q, q, r, l, U, monoid_pow(U, p / q) + R);
	}
	i64 m = i64((i128(p) * l + r) / q);
	if(m == 0){
		return monoid_pow(R, l);
	}
	i64 head = (q - r - 1) / p, tail = l - i64((i128(q) * m - r - 1) / p);
	return monoid_pow(R, head) + U
		+ euclid(q, p, (q - r - 1) % p, m - 1, R, U) + monoid_pow(R, tail);
}
