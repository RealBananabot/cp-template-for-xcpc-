import argparse
import math
import statistics
import subprocess
import tempfile
from pathlib import Path

here = Path(__file__).resolve().parent
parser = argparse.ArgumentParser()
parser.add_argument("--source", type=Path, default=here.parents[1] / "mint_poly.cpp")
parser.add_argument("--baseline", type=Path)
mode = parser.add_mutually_exclusive_group()
mode.add_argument("--bench", action="store_true")
mode.add_argument("--stress", action="store_true", help="also check the supported size limits")
parser.add_argument("--rounds", type=int, default=5)
args = parser.parse_args()
if args.rounds < 1:
    parser.error("--rounds must be positive")

def build(source, suite, output):
    template, marker, _ = source.read_text().rpartition("signed main(){")
    if not marker:
        raise ValueError(f"Missing demo main in {source}")
    cpp = output.with_suffix(".cpp")
    cpp.write_text(template + f'\n#line 1 "{suite}"\n' + (here / suite).read_text())
    flags = ["-std=gnu++20", "-O2", "-Wall", "-Wextra", "-Wconversion", "-Werror"]
    if not args.bench:
        flags += ["-D_GLIBCXX_ASSERTIONS", "-fsanitize=undefined", "-fno-sanitize-recover=all"]
    subprocess.run(["g++", *flags, str(cpp), "-o", str(output)], check=True)


def read_bench(output):
    cases = {}
    for line in output.splitlines():
        name, ms, length, checksum = line.split()
        elapsed = float(ms)
        if name in cases or not math.isfinite(elapsed) or elapsed <= 0:
            raise ValueError(f"Invalid benchmark result: {line}")
        cases[name] = (elapsed, (int(length), int(checksum)))
    if not cases:
        raise ValueError("Benchmark produced no results")
    return cases


def benchmark(bins):
    samples = {}
    expected = None
    for r in range(args.rounds):
        for binary in (reversed(bins) if r % 2 else bins):
            output = subprocess.check_output([binary], text=True, timeout=120)
            cases = read_bench(output)
            checksums = {name: check for name, (_, check) in cases.items()}
            if expected is None:
                expected = checksums
            elif checksums != expected:
                changed = sorted(name for name in checksums.keys() | expected.keys()
                                 if checksums.get(name) != expected.get(name))
                raise ValueError(f"Benchmark cases or outputs differ: {', '.join(changed)}")
            for name, (time, _) in cases.items():
                samples.setdefault((name, binary), []).append(time)
    if len(bins) == 2:
        print(f"{'case':<24} {'baseline (ms)':>14} {'current (ms)':>14} {'change':>9}")
    else:
        print(f"{'case':<24} {'median (ms)':>14}")
    for name in expected:
        times = [statistics.median(samples[name, b]) for b in bins]
        if len(times) == 2:
            before, after = times
            print(f"{name:<24} {before:14.2f} {after:14.2f} {(after / before - 1):+9.1%}")
        else:
            print(f"{name:<24} {times[0]:14.2f}")

with tempfile.TemporaryDirectory() as tmp:
    paths = [args.baseline, args.source] if args.baseline else [args.source]
    failures = []
    bins = []
    cases = ["suite", "single-ntt", "short-divisor"]
    if args.stress:
        cases.append("limits")
    for i, source in enumerate(paths):
        binary = Path(tmp) / str(i)
        build(source, "bench.cpp" if args.bench else "check.cpp", binary)
        bins.append(binary)
        if not args.bench:
            for case in cases:
                command = [binary] if case == "suite" else [binary, case]
                try:
                    subprocess.run(command, check=True, timeout=120 if case == "limits" else 60)
                except (subprocess.CalledProcessError, subprocess.TimeoutExpired):
                    failures.append(f"{source}: {case}")
    if failures:
        raise SystemExit("Failed checks:\n" + "\n".join(failures))
    if args.bench:
        benchmark(bins)
    else:
        print(f"Passed {len(cases)} test groups for {len(bins)} source file(s)")
