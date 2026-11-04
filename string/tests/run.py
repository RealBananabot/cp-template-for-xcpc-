#!/usr/bin/env python3
"""Compile and check the current string templates, without repository build artifacts."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import json
import os
from pathlib import Path
import re
import shlex
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--sanitize', action='store_true', help='enable ASan and UBSan')
parser.add_argument('--large', action='store_true', help='also run the large boundary cases')
parser.add_argument('--source', type=Path, help='directory containing the template snippets')
args = parser.parse_args()
tests = Path(__file__).resolve().parent
source = (args.source or tests.parent).resolve()
compiler = shlex.split(os.environ.get('CXX', 'g++'))
flags = ['-std=gnu++20', '-O2', '-Wall', '-Wextra', '-Wconversion', '-Werror', '-I', str(source)]
if args.sanitize:
    flags[1] = '-O1'
    flags += ['-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer']
env = os.environ.copy()
if args.sanitize:
    env.setdefault('ASAN_OPTIONS', 'detect_leaks=0')
    env.setdefault('UBSAN_OPTIONS', 'halt_on_error=1')
prelude_path = source / 's.cpp'
if not prelude_path.exists():
    prelude_path = source.parent / 's.cpp'
prelude = prelude_path.read_text()
main = re.search(r'(?m)^(?:signed|int)\s+main\s*\(', prelude)
if main is None:
    raise SystemExit('cannot locate main in ' + str(prelude_path))
prelude = prelude[:main.start()]
common = ['gsam.cpp', 'ac.cpp', 'pam.cpp', 'sa.cpp', 'lyndon.cpp', 'kmp.cpp',
          'z.cpp', 'manacher.cpp', 'min_rotation.cpp', 'lcp_sa.cpp', 'runs.cpp']
body = r'''
int main(){
    assert((kmp("aba") == vi{-1, -1, 0}));
    assert((zfunc("aaa") == vi{3, 2, 1}));
    assert((exkmp("aaa", "aaa") == vi{3, 2, 1}));
    assert((lyndon("ababa") == vi{0, 2, 4, 5}));
    assert((manacher("aba") == vi{0, 1, 0, 3, 0, 1, 0}));
    assert(min_rotation(vi{2, 1, 2, 1}) == 1);
    string s = "aba";
    auto [rk, sa] = SA(s);
    assert((sa == vi{2, 0, 1}));
    assert((get_height(s, sa, rk) == vi{0, 1, 0}));
    sa_lcp t(s);
    assert(t.lcp(0, 2) == 1);
    assert(t.lcs(0, 2) == 1);
    SAM sam(4);
    for(const string &a:vector<string>{"ab", "ba"}){
        int u = 1;
        for(char c:a) u = sam.ins(u, c - 'a');
    }
    assert(sam.solve().first == 4);
    AC ac(2); int end = ac.ins(ac.ins(1, 0), 1); ac.build();
    assert(ac.s[ac.s[1][0]][1] == end && ac.fa[end] == 1);
    PAM pam(3); pam.str = s; int u = 1;
    rep(i, 0, 2) u = pam.ins(u, i);
    assert(pam.dep[u] == 2);
    vector<tuple<int, int, int>> expected{{0, 3, 1}};
    assert(get_runs("aaaa") == expected);
    assert(get_runs<sa_lcp>("aaaa") == expected);
    cout << "current s.cpp integration passed\n";
}
'''
large = {'test_gsam':['large'], 'test_ac':['large'], 'test_pam':['large'],
         'test_manacher':['large'], 'test_min_rotation':['large'],
         'test_lyndon':['repeated', 'single', 'periodic'],
         'test_sa':['repeated', 'periodic', 'random'], 'test_runs':['large']}
with tempfile.TemporaryDirectory(prefix='string-check-') as tmp:
    work = Path(tmp)
    units = sorted(tests.glob('test_*.cpp'))
    for variant in ['hash', 'sa']:
        path = work / ('integration_' + variant + '.cpp')
        includes = common + ['runs_' + variant + '.cpp']
        path.write_text(prelude + '\n' + ''.join('#include ' + json.dumps(str(source / f)) + '\n' for f in includes) + body)
        units.append(path)
    def compile_unit(path):
        binary = work / path.stem
        result = subprocess.run(compiler + flags + [str(path), '-o', str(binary)], text=True, capture_output=True)
        if result.returncode:
            return path.stem, None, path.name + '\n' + result.stdout + result.stderr
        if result.stderr:
            print(result.stderr, flush=True)
        return path.stem, binary, ''
    with ThreadPoolExecutor(max_workers=3) as pool:
        compiled = list(pool.map(compile_unit, units))
    errors = [error for _, _, error in compiled if error]
    if errors:
        raise SystemExit('\n'.join(errors))
    binaries = [(name, binary) for name, binary, _ in compiled]
    print('compiled', len(binaries), 'test and integration units', flush=True)
    for name, binary in binaries:
        print(name + ':', flush=True)
        subprocess.run([str(binary)], check=True, env=env)
        if args.large and name in large:
            for mode in large[name]:
                subprocess.run([str(binary), mode], check=True, env=env)
    print('all requested checks passed', flush=True)
