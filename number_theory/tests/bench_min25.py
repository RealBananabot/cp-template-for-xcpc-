#!/usr/bin/env python3
"""Compare current recursive Min25 with reverse DP; generated variants stay temporary."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import random
import re
import statistics
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'number_theory/min25.cpp'
FLAGS = ['-std=gnu++20', '-O2', '-Wall', '-Wextra', '-Wconversion', '-Werror']

DP = '''\tint calc(i64, int){
\t\tvi h(m + 1);
\t\trep(j, 1, m){
\t\t\th[j] = (g[j] - f[j]) % mod;
\t\t}
\t\tper(k, int(size(pr)) - 1, 0){
\t\t\ti64 p = pr[k], pre = (g[int(p)] - f[int(p)]) % mod;
\t\t\tper(j, m, 1){
\t\t\t\ti64 x = val(j);
\t\t\t\tif(x < p * p){
\t\t\t\t\tbreak;
\t\t\t\t}
\t\t\t\t// DP_STATE
\t\t\t\tfor(i64 pw = p; pw <= x / p; pw *= p){
\t\t\t\t\t// DP_POWER
\t\t\t\t\ti64 a = pw % mod, b = pw * p % mod;
\t\t\t\t\th[j] = int((h[j] + a * (a - 1) % mod * (h[id(x / pw)] - pre)
\t\t\t\t\t\t+ b * (b - 1)) % mod);
\t\t\t\t}
\t\t\t}
\t\t}
\t\treturn h[m];
\t}'''


def replace_calc(source, replacement):
    start = source.index('\tint calc(')
    opening = source.index('{', start)
    depth = 1
    end = opening + 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[:start] + replacement + source[end:]


def renamed(source, name):
    return source.replace('struct Min25{', 'struct ' + name + '{')


def timed(source, name):
    source = renamed(source, name)
    source = source.replace('struct ' + name + '{', 'struct ' + name + '{\n'
                            '\tdouble build_ms = 0, restore_ms = 0;')
    source = source.replace('int solve(i64 _n){', 'int solve(i64 _n){\n'
                            '\t\tauto t0 = chrono::steady_clock::now();')
    old = '\t\treturn (n ? (calc(n, 0) + 1 + mod) % mod : 0);'
    assert source.count(old) == 1
    new = '''\t\tauto t1 = chrono::steady_clock::now();
\t\tint result = (n ? (calc(n, 0) + 1 + mod) % mod : 0);
\t\tauto t2 = chrono::steady_clock::now();
\t\tbuild_ms = chrono::duration<double, milli>(t1 - t0).count();
\t\trestore_ms = chrono::duration<double, milli>(t2 - t1).count();
\t\treturn result;'''
    return source.replace(old, new)


CPP = r'''
#include "variants.hpp"

template<class T> void measure(i64 n, bool dp){
	int answer = 0;
	double build = 0, restore = 0;
	size_t payload = 0;
	auto start = chrono::steady_clock::now();
	{
		T s;
		answer = s.solve(n);
		payload = (s.pr.capacity() + s.f.capacity() + s.g.capacity()
			+ size_t(s.sq + 1) + (dp ? size_t(s.m + 1) : 0)) * sizeof(int);
		if constexpr(requires { s.build_ms; }){
			build = s.build_ms, restore = s.restore_ms;
		}
	}
	auto end = chrono::steady_clock::now();
	// /proc reports this executable's mm high-water mark; getrusage can retain
	// the Python parent's pre-exec RSS high-water mark on Linux.
	ifstream status("/proc/self/status");
	string key, rest;
	size_t peak_rss = 0;
	while(status >> key){
		if(key == "VmHWM:"){
			status >> peak_rss;
			break;
		}
		getline(status, rest);
	}
	assert(peak_rss > 0);
	cout << fixed << setprecision(6) << "{\"answer\":" << answer
		<< ",\"total_ms\":" << chrono::duration<double, milli>(end - start).count()
		<< ",\"build_ms\":" << build << ",\"restore_ms\":" << restore
		<< ",\"rss_kib\":" << peak_rss << ",\"array_bytes\":" << payload << "}\n";
}

void verify(){
	const int N = 100000;
	vi spf(N + 1), largest(N + 1), prefix(N + 1);
	rep(p, 2, N){
		if(!spf[p]){
			for(int j = p; j <= N; j += p){
				if(!spf[j]){
					spf[j] = p;
				}
				largest[j] = p;
			}
		}
	}
	rep(i, 1, N){
		int x = i;
		i64 value = 1;
		while(x > 1){
			int p = spf[x], pw = 1;
			while(x % p == 0){
				x /= p, pw *= p;
			}
			value = value * (i64(pw) * (pw - 1) % Min25::mod) % Min25::mod;
		}
		prefix[i] = int((prefix[i - 1] + value) % Min25::mod);
	}
	Min25 a;
	Min25DP b;
	rep(n, 0, 1000){
		assert(a.solve(n) == prefix[n] && b.solve(n) == prefix[n]);
		CountRec counted;
		assert(counted.solve(n) == prefix[n]);
		unsigned long long expected = (n ? 1 : 0);
		rep(d, 2, n / 2){
			expected += (i64(d) * largest[d] <= n);
		}
		assert(counted.calls == expected);
	}
	mt19937 rnd(5325);
	rep(i, 1, 100){
		int n = int(rnd() % (N + 1));
		assert(a.solve(n) == prefix[n] && b.solve(n) == prefix[n]);
	}
	for(i64 n:{1000000000LL, 8589934592LL, 9998200081LL, 10000000000LL}){
		assert(a.solve(n) == b.solve(n));
	}
	assert(b.solve(1000000000) == 710164413);
	assert(b.solve(0) == 0 && b.solve(10) == 263);
	cout << "verified both restorations against factorization prefixes, large cases and exact DFS counts\n";
}

int main(int argc, char **argv){
	assert(argc >= 2);
	string mode = argv[1];
	if(mode == "verify"){
		verify();
		return 0;
	}
	assert(argc == 3);
	i64 n = stoll(argv[2]);
	if(mode == "rec"){
		measure<Min25>(n, false);
	} else if(mode == "dp"){
		measure<Min25DP>(n, true);
	} else if(mode == "rec_phase"){
		measure<TimeRec>(n, false);
	} else if(mode == "dp_phase"){
		measure<TimeDP>(n, true);
	} else{
		assert(mode == "count");
		CountRec a;
		CountDP b;
		int x = a.solve(n), y = b.solve(n);
		assert(x == y);
		cout << "{\"answer\":" << x << ",\"recursive_calls\":" << a.calls
			<< ",\"prime_updates\":" << a.prime_updates
			<< ",\"dp_states\":" << b.dp_states << ",\"dp_powers\":" << b.dp_powers << "}\n";
	}
}
'''


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repeats', type=int, default=7)
    parser.add_argument('--phase-repeats', type=int, default=3)
    parser.add_argument('--cpu', type=int, default=2)
    parser.add_argument('--inputs', type=int, nargs='+',
                        default=[1000000, 100000000, 1000000000, 8589934592, 9998200081, 10000000000])
    parser.add_argument('--output', type=Path, default=ROOT / 'number_theory/tests/min25_benchmark.json')
    args = parser.parse_args()
    assert args.repeats > 0 and args.phase_repeats > 0
    assert all(0 <= n <= 10000000000 for n in args.inputs)
    os.sched_setaffinity(0, {args.cpu})
    source = SOURCE.read_text()
    digest = hashlib.sha256(SOURCE.read_bytes()).hexdigest()
    prelude = (ROOT / 's.cpp').read_text()
    prelude = prelude[:re.search(r'(?m)^(?:signed|int)\s+main\s*\(', prelude).start()]
    dp = replace_calc(source, DP)
    counted_rec = renamed(source, 'CountRec').replace('struct CountRec{', 'struct CountRec{\n'
        '\tunsigned long long calls = 0, prime_updates = 0;')
    counted_rec = counted_rec.replace('int calc(i64 x, int i){', 'int calc(i64 x, int i){\n\t\tcalls++;')
    counted_rec = counted_rec.replace('int j = id(x / p);', 'prime_updates++;\n\t\t\t\tint j = id(x / p);')
    counted_dp = renamed(dp, 'CountDP').replace('struct CountDP{', 'struct CountDP{\n'
        '\tunsigned long long dp_states = 0, dp_powers = 0;')
    counted_dp = counted_dp.replace('// DP_STATE', 'dp_states++;').replace('// DP_POWER', 'dp_powers++;')
    variants = '\n'.join([source, renamed(dp, 'Min25DP'), timed(source, 'TimeRec'),
                          timed(dp, 'TimeDP'), counted_rec, counted_dp])
    report = {
        'source_sha256': digest, 'compiler': subprocess.check_output(['g++', '--version'], text=True).splitlines()[0],
        'flags': FLAGS, 'cpu': subprocess.check_output(['lscpu'], text=True), 'pinned_cpu': args.cpu,
        'full_repeats': args.repeats, 'phase_repeats': args.phase_repeats,
        'inputs': args.inputs,
        'timing': 'steady_clock; construction, allocation, all preprocessing, restoration and destruction included; input/output excluded',
        'phase_timing': 'instrumented copies; construction of empty vectors negligible, temporary marker destruction follows restore timer',
        'memory': 'Linux /proc/self/status VmHWM for current executable; array_bytes includes vector capacities, temporary sieve markers and DP buffer, excludes object/allocator overhead',
        'cases': []}
    with tempfile.TemporaryDirectory(prefix='min25-benchmark-') as tmp:
        work = Path(tmp)
        (work / 'variants.hpp').write_text(variants)
        src = work / 'bench.cpp'
        src.write_text(prelude + CPP)
        binary = work / 'bench'
        subprocess.run(['g++', *FLAGS, str(src), '-o', str(binary)], check=True)
        subprocess.run([str(binary), 'verify'], check=True)
        san = work / 'check_sanitize'
        subprocess.run(['g++', *[('-O1' if f == '-O2' else f) for f in FLAGS], '-g',
                        '-fsanitize=address,undefined', '-fno-omit-frame-pointer', str(src), '-o', str(san)], check=True)
        env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0', UBSAN_OPTIONS='halt_on_error=1')
        subprocess.run([str(san), 'verify'], env=env, check=True)
        print('strict and sanitizer verification complete; timing runs are sequential', flush=True)

        def run(mode, n):
            return json.loads(subprocess.check_output([str(binary), mode, str(n)], text=True))

        for n in args.inputs:
            row = {'n': n, 'counts': run('count', n)}
            for method in ['rec', 'dp']:
                run(method, n)  # Warm-up, outside reported statistics.
                row[method] = {'full_samples': [], 'phase_samples': []}
            rng = random.Random(n)
            for _ in range(args.repeats):
                order = ['rec', 'dp']
                rng.shuffle(order)
                for method in order:
                    row[method]['full_samples'].append(run(method, n))
            for _ in range(args.phase_repeats):
                order = ['rec', 'dp']
                rng.shuffle(order)
                for method in order:
                    row[method]['phase_samples'].append(run(method + '_phase', n))
            for method in ['rec', 'dp']:
                data = row[method]
                assert all(s['answer'] == row['counts']['answer']
                           for s in data['full_samples'] + data['phase_samples'])
                data['median_total_ms'] = statistics.median(s['total_ms'] for s in data['full_samples'])
                data['median_build_ms'] = statistics.median(s['build_ms'] for s in data['phase_samples'])
                data['median_restore_ms'] = statistics.median(s['restore_ms'] for s in data['phase_samples'])
                data['median_peak_rss_kib'] = statistics.median(s['rss_kib'] for s in data['full_samples'])
                data['array_bytes'] = data['full_samples'][0]['array_bytes']
            report['cases'].append(row)
            print(f"n={n}: recursive {row['rec']['median_total_ms']:.3f} ms, "
                  f"reverse DP {row['dp']['median_total_ms']:.3f} ms; "
                  f"calls={row['counts']['recursive_calls']}", flush=True)
    assert hashlib.sha256(SOURCE.read_bytes()).hexdigest() == digest
    args.output.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n')
    print('saved ' + str(args.output), flush=True)


if __name__ == '__main__':
    main()
