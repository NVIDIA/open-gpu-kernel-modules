#!/usr/bin/env python3
"""Line and branch coverage of the PR 1531 diff, from the host unit tests."""

import json
import os
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
BUILD = os.path.join(ROOT, "tests", "nvpcf", "build")
FILES = [
    "kernel-open/nvidia/nv-acpi.c",
    "kernel-open/nvidia/nv.c",
    "src/nvidia/arch/nvalloc/unix/src/osapi.c",
]
GCOV_DIRS = [
    os.path.join(BUILD, "os"),
    os.path.join(BUILD, "acpi"),
    os.path.join(BUILD, "nv"),
    os.path.join(BUILD, "stub"),
]


def added_lines():
    diff = subprocess.check_output(
        ["git", "diff", "-U0", "origin/main", "--"] + FILES,
        cwd=ROOT,
        text=True,
    )
    current = None
    new_line = None
    added = {}
    for line in diff.splitlines():
        if line.startswith("+++ b/"):
            current = line[6:]
            added.setdefault(current, set())
        elif line.startswith("@@"):
            # @@ -old +new,count @@
            plus = line.split(" ")[2]
            start = plus[1:].split(",")[0]
            new_line = int(start)
        elif line.startswith("+") and not line.startswith("+++") and current:
            added[current].add(new_line)
            new_line += 1
        elif line.startswith("-") and not line.startswith("---"):
            pass
        elif line.startswith("\\"):
            pass
    return added


def load_gcov():
    lines = {}
    branches = {}
    for directory in GCOV_DIRS:
        for name in os.listdir(directory):
            if not name.endswith(".gcda"):
                continue
            subprocess.run(
                ["gcov", "--json-format", "--branch-probabilities", name],
                cwd=directory,
                check=True,
                capture_output=True,
                text=True,
            )
        for name in os.listdir(directory):
            if not name.endswith(".gcov.json.gz"):
                continue
            import gzip

            with gzip.open(os.path.join(directory, name), "rt") as fh:
                data = json.load(fh)
            for src in data["files"]:
                path = os.path.relpath(src["file"], ROOT)
                for line in src["lines"]:
                    key = (path, line["line_number"])
                    lines[key] = lines.get(key, 0) + line["count"]
                    taken = branches.setdefault(path, {})
                    for idx, br in enumerate(line.get("branches", [])):
                        bkey = (line["line_number"], idx)
                        taken[bkey] = taken.get(bkey, 0) + br["count"]
    return lines, branches


def main():
    added = added_lines()
    lines, branches = load_gcov()
    coverable = []
    hit = []
    for path, nums in sorted(added.items()):
        for num in sorted(nums):
            key = (path, num)
            if key not in lines:
                continue
            coverable.append(key)
            if lines[key] > 0:
                hit.append(key)
    missed = [k for k in coverable if k not in hit]
    line_pct = 100.0 * len(hit) / len(coverable) if coverable else 0.0

    branch_total = 0
    branch_hit = 0
    coverable_lines = set(coverable)
    for path, items in branches.items():
        for (num, _idx), count in items.items():
            if (path, num) not in coverable_lines:
                continue
            branch_total += 1
            if count > 0:
                branch_hit += 1
    branch_pct = 100.0 * branch_hit / branch_total if branch_total else 0.0

    print(f"diff line coverage: {len(hit)}/{len(coverable)} ({line_pct:.1f}%)")
    print(f"diff branch coverage: {branch_hit}/{branch_total} ({branch_pct:.1f}%)")
    if missed:
        print("uncovered executable diff lines:")
        for path, num in missed:
            print(f"  {path}:{num}")
    return 0 if not missed else 1


if __name__ == "__main__":
    sys.exit(main())
