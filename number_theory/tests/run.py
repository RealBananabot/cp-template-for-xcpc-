#!/usr/bin/env python3
"""Compile current number-theory snippets with the actual s.cpp prelude."""
import argparse
import json
import math
import os
from pathlib import Path
import random
import re
import shlex
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--sanitize', action='store_true', help='enable ASan and UBSan')
args = parser.parse_args()
tests = Path(__file__).resolve().parent
prelude_path = tests.parent.parent / 's.cpp'
prelude = prelude_path.read_text()
main = re.search(r'(?m)^(?:signed|int)\s+main\s*\(', prelude)
if main is None:
    raise SystemExit('cannot locate main in ' + str(prelude_path))
prelude = prelude[:main.start()]
compiler = shlex.split(os.environ.get('CXX', 'g++'))
flags = ['-std=gnu++20', '-O2', '-Wall', '-Wextra', '-Wconversion', '-Werror']
env = os.environ.copy()
if args.sanitize:
    flags[1] = '-O1'
    flags += ['-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer']
    env.setdefault('ASAN_OPTIONS', 'detect_leaks=0')
    env.setdefault('UBSAN_OPTIONS', 'halt_on_error=1')
with tempfile.TemporaryDirectory(prefix='number-theory-check-') as tmp:
    for test in sorted(tests.glob('test_*.cpp')):
        source = Path(tmp) / test.name
        binary = source.with_suffix('')
        source.write_text(prelude + '\n#include ' + json.dumps(str(test)) + '\n')
        subprocess.run(compiler + flags + [str(source), '-o', str(binary)], check=True)
        reference = ''
        if test.stem == 'test_lucas':
            rng = random.Random(3807)
            primes = [p for p in range(2, 101)
                      if all(p % d for d in range(2, math.isqrt(p) + 1))] + [99991]
            largest = (1 << 63) - 1
            rows = []
            for p in primes:
                pairs = [(rng.randrange(1 << 63), rng.randrange(65)) for _ in range(200)]
                pairs += [(largest, n) for n in
                          [-(1 << 63), -1, 0, 1, 64, largest - 64, largest - 1, largest]]
                for m, n in pairs:
                    expected = math.comb(m, n) % p if 0 <= n <= m else 0
                    rows.append(f'{m} {n} {p} {expected}\n')
            reference = str(len(rows)) + '\n' + ''.join(rows)
        elif test.stem == 'test_exlucas':
            rng = random.Random(4720)
            largest = (1 << 63) - 1
            cases = []
            for mod in [1, 2, 4, 8, 16, 27, 81, 125, 343, 1024, 65536,
                        531441, 524288, 999983, 1000000, 720720]:
                for m in [10**18, largest]:
                    cases += [(m, n, mod) for n in
                              [-(1 << 63), -1, 0, 1, 2, 63, m - 63, m - 1, m]]
            for _ in range(500):
                m = rng.randrange(2001)
                cases.append((m, rng.randrange(-1, m + 2), rng.randrange(1, 2001)))
            for _ in range(200):
                cases.append((rng.randrange(1 << 63), rng.randrange(65), rng.randrange(1, 10001)))
            for mod in [1000000, 524288, 531441, 999983, 720720]:
                cases += [(200000, 100000, mod), (100000, 50000, mod)]
            rows = []
            for m, n, mod in cases:
                expected = math.comb(m, n) % mod if 0 <= n <= m else 0
                rows.append(f'{m} {n} {mod} {expected}\n')
            reference = str(len(rows)) + '\n' + ''.join(rows)
        elif test.stem == 'test_miller_rabin':
            from primality_reference import reference_input
            reference = reference_input()
        elif test.stem == 'test_pollard_rho':
            from primality_reference import factorization_input
            reference = factorization_input()
        elif test.stem in ('test_primitive_root', 'test_primitive_root_pollard_rho'):
            from primality_reference import primitive_root_input
            reference = primitive_root_input()
        elif test.stem == 'test_mod_sqrt':
            from primality_reference import mod_sqrt_input
            reference = mod_sqrt_input()
        elif test.stem in ('test_floor_sum', 'test_floor_moments'):
            from floor_reference import reference_input
            reference = reference_input()
        subprocess.run([str(binary)], input=reference, text=True, check=True, env=env)
