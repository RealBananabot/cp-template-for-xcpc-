"""Exact Python reference using classical three-moment Euclidean recurrences."""
from functools import cache
import random


def moments(n, a, b, c):
    if n < 0:
        return 0, 0, 0
    qa, ra = divmod(a, c)
    qb, rb = divmod(b, c)
    N, s1, s2 = n + 1, n * (n + 1) // 2, n * (n + 1) * (2 * n + 1) // 6
    if qa or qb:
        f, g, h = moments(n, ra, rb, c)
        return (f + qa * s1 + qb * N,
                g + 2 * qa * h + 2 * qb * f + qa**2 * s2
                + 2 * qa * qb * s1 + qb**2 * N,
                h + qa * s2 + qb * s1)
    if a == 0:
        return 0, 0, 0
    m = (a * n + b) // c
    if m == 0:
        return 0, 0, 0
    f, g, h = moments(m - 1, c, c - b - 1, a)
    assert (g + f) % 2 == 0
    return n * m - f, n * m**2 - 2 * h - f, m * s1 - (g + f) // 2


@cache
def reference_input():
    rng = random.Random(5170)
    for _ in range(500):
        n, a, b, c = rng.randrange(-1, 20), rng.randrange(20), rng.randrange(20), rng.randrange(1, 20)
        ys = [(a * i + b) // c for i in range(n + 1)]
        expected = sum(ys), sum(y * y for y in ys), sum(i * y for i, y in enumerate(ys))
        assert moments(n, a, b, c) == expected
    limit, mod = (1 << 62) - 1, 998244353
    cases = []
    for n in [-1, 0, 1, 2, mod - 1, mod, mod + 1, 10**9, 10**18, limit]:
        for a, b, c in [(0, 0, 1), (0, limit, 1), (1, 0, 1), (16, 0, 1),
                        (17, 0, 1), (limit, limit, 1), (limit, limit, limit),
                        (limit, 0, limit - 1), (limit - 1, limit - 2, limit),
                        (mod, mod - 1, mod), (mod - 1, mod, mod + 1),
                        (10**9, 10**9, 999999937)]:
            cases.append((n, a, b, c))
    cases += [(1, 1, limit, 1), (1, 2, limit, 1)]
    cases += [(rng.randrange(10**9 + 1), rng.randrange(10**9 + 1),
               rng.randrange(10**9 + 1), rng.randrange(1, 10**9 + 1)) for _ in range(600)]
    cases += [(rng.randrange(limit + 1), rng.randrange(limit + 1),
               rng.randrange(limit + 1), rng.randrange(1, limit + 1)) for _ in range(600)]
    cases += [(rng.randrange(100), rng.randrange(limit + 1),
               rng.randrange(limit + 1), rng.randrange(1, limit + 1)) for _ in range(200)]
    fib = [1, 1]
    while fib[-1] + fib[-2] <= limit:
        fib.append(fib[-1] + fib[-2])
    for a, c in zip(fib, fib[1:]):
        for b in [0, c - 1, limit]:
            cases += [(limit, a, b, c), (10**18, c, b, a)]
    rows = []
    for n, a, b, c in cases:
        f, g, h = moments(n, a, b, c)
        exact = f if f < (1 << 63) else -1
        rows.append(f'{n} {a} {b} {c} {f % mod} {g % mod} {h % mod} {exact}\n')
    return str(len(rows)) + '\n' + ''.join(rows)
