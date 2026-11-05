"""Independent exact references: factor candidates checked by Lucas certificates."""
from functools import cache
import math
import random
import subprocess


@cache
def factors(n):
    line = subprocess.run(['factor', str(n)], check=True, text=True,
                          capture_output=True).stdout.strip()
    left, right = line.split(':', 1)
    result = [int(x) for x in right.split()]
    assert int(left) == n and math.prod(result) == n
    assert all(1 < x <= n for x in result)
    return result


@cache
def prove_prime(n):
    if n < 1000000:
        assert n >= 2 and all(n % d for d in range(2, math.isqrt(n) + 1))
        return
    qs = set(factors(n - 1))
    for q in qs:
        prove_prime(q)
    for a in range(2, 1000):
        if pow(a, n - 1, n) == 1 and all(
                math.gcd(pow(a, (n - 1) // q, n) - 1, n) == 1 for q in qs):
            return
    raise AssertionError(f'no Lucas primality certificate found for {n}')


def reference_input():
    rng = random.Random(249)
    largest = (1 << 63) - 1
    values = [-(1 << 63), -(1 << 31), -1, 0, 1, 2, 3, 5, 7, 61, 73, 193,
              407521, 299210837, (1 << 31) - 1, largest, largest - 24,
              2047, 1373653, 25326001, 3215031751, 2152302898747,
              3474749660383, 341550071728321, 3825123056546413051,
              4759123141, 999999999999999989, 10**18]
    bases = [2, 7, 61, 325, 9375, 28178, 450775, 9780504, 1795265022]
    for a in bases:
        values += factors(a)
        values += [a - 1, a, a + 1]
    values += [largest - i for i in range(1, 40)]
    values += [rng.randrange(2, 1 << 31) for _ in range(100)]
    values += [rng.randrange(2, 1 << 63) for _ in range(200)]
    values += [p * q for p in [1000000007, 1000000009, 2147483647]
               for q in [1000000007, 1000000009, 2147483647]]
    rows = []
    for n in sorted(set(values)):
        prime = False
        if n >= 2:
            fs = factors(n)
            if fs == [n]:
                prove_prime(n)
                prime = True
            else:
                assert len(fs) >= 2 and all(x < n for x in fs)
        rows.append(f'{n} {int(prime)}\n')
    output = str(len(rows)) + '\n' + ''.join(rows)
    powers = []
    for mod in [2, 3, (1 << 31) - 1, 10**18, largest]:
        for _ in range(30):
            x = rng.randrange(mod)
            y = rng.randrange(1 << 63)
            powers.append(f'{x} {y} {mod} {pow(x, y, mod)}\n')
    return output + str(len(powers)) + '\n' + ''.join(powers)


def factorization_input():
    rng = random.Random(458)
    largest = (1 << 63) - 1
    values = [1, 2, 9, 30, 2**62, 3**39, 5**27, 7**22,
              10**18, largest, largest - 24,
              1000000007 * 1000000009, 2147483647**2,
              3037000493**2, 999999999999999989,
              341550071728321, 3825123056546413051]
    values += [rng.randrange(2, 10**18 + 1) for _ in range(100)]
    values += [rng.randrange(2, largest + 1) for _ in range(50)]
    rows = []
    for n in sorted(set(values)):
        fs = sorted(factors(n)) if n > 1 else []
        for p in set(fs):
            prove_prime(p)
        rows.append(f'{n} {len(fs)} ' + ' '.join(map(str, fs)) + '\n')
    return str(len(rows)) + '\n' + ''.join(rows)


@cache
def primitive_root_input():
    rng = random.Random(6091)
    largest = (1 << 63) - 1
    values = {-1, 0, 1, 2, 3, 4, 6, 9, 18, 45, largest, largest - 24,
              40487, 40487**2, 2 * 40487**2, 3**39, 5**27, 7**22,
              11**18, 13**17, 2147483647**2, 2 * 2147483647**2,
              3037000493**2, 1000000007 * 1000000009,
              341550071728321, 3825123056546413051,
              999983, 1000000, 999966, 531441, 781250,
              823543, 524288, 720720, 998244, 999999}
    for p in [2, 3, 5, 7, 11, 13, 17, 19, 23, 31, 37, 101, 1093,
              1000000007, 1000000009, 998244353, 2147483647,
              3037000493, 4294967311, 999999999999999989]:
        prove_prime(p)
        power = p
        while power <= largest:
            values.update([power, power - 1, power + 1, 2 * power])
            power *= p
    values.update(rng.randrange(2, largest + 1) for _ in range(60))
    rows = []
    for d in sorted(v for v in values if v <= largest):
        result = -1
        if d in (2, 4):
            result = d - 1
        elif d > 1:
            fs = factors(d)
            for p in set(fs):
                prove_prime(p)
            odd = {p for p in fs if p != 2}
            if fs.count(2) <= 1 and len(odd) == 1:
                phi = d
                for p in set(fs):
                    phi = phi // p * (p - 1)
                qs = set(factors(phi))
                for q in qs:
                    prove_prime(q)
                # Python 任意精度独立核对每个更小候选；无证书或超出测试上限就失败。
                for g in range(2, min(d, 100000)):
                    if math.gcd(g, d) == 1 and all(
                            pow(g, phi // q, d) != 1 for q in qs):
                        result = g
                        break
                assert result != -1, f'no reference root found for {d}'
        rows.append(f'{d} {result}\n')
    return str(len(rows)) + '\n' + ''.join(rows)


def tonelli_shanks(n, p):
    """Test-only reference; integer arithmetic, independent of Cipolla extensions."""
    n %= p
    if p == 2 or n == 0:
        return n
    if pow(n, (p - 1) // 2, p) != 1:
        return -1
    q, s = p - 1, 0
    while q % 2 == 0:
        q //= 2
        s += 1
    z = 2
    while pow(z, (p - 1) // 2, p) != p - 1:
        z += 1
    c, t = pow(z, q, p), pow(n, q, p)
    x, m = pow(n, (q + 1) // 2, p), s
    while t != 1:
        u = t
        for i in range(1, m):
            u = u * u % p
            if u == 1:
                break
        else:
            raise AssertionError(f'invalid Tonelli-Shanks state for {n}, {p}')
        b = pow(c, 1 << (m - i - 1), p)
        x = x * b % p
        c = b * b % p
        t = t * c % p
        m = i
    return min(x, p - x)


def mod_sqrt_input():
    rng = random.Random(3663)
    largest = (1 << 63) - 1
    primes = [2, 3, 5, 13, 17, 97, 257, 65537, 7340033, 998244353,
              1000000007, 1000000009, 2013265921, 4294967311,
              999999999999999989, 4179340454199820289, largest - 24]
    near = []
    for delta in range(2, 4096, 4):
        p = largest - delta
        # Fermat only filters candidates; prime factors and Lucas certificates prove them.
        if pow(2, p - 1, p) == 1 and factors(p) == [p]:
            prove_prime(p)
            near.append(p)
            if len(near) == 2:
                break
    assert len(near) == 2
    primes += near
    rows = []
    for p in primes:
        prove_prime(p)
        ns = [-(1 << 63), -largest, -p, -1, 0, 1, 2, p // 2,
              p - 1, p, largest]
        ns += [rng.randrange(-(1 << 63), 1 << 63) for _ in range(120)]
        for _ in range(80):
            x = rng.randrange(p)
            n = x * x % p
            assert tonelli_shanks(n, p) == min(x, p - x)
            ns.append(n)
        for n in ns:
            root = tonelli_shanks(n, p)
            if root != -1:
                assert 0 <= root <= p // 2 and root * root % p == n % p
            rows.append(f'{n} {p} {root}\n')
    return str(len(rows)) + '\n' + ''.join(rows)
