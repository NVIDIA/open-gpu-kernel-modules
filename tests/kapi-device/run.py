#!/usr/bin/env python3
"""Fault-test actual upstream AllocateDevice/FreeDevice bodies without a GPU."""
import argparse
from pathlib import Path
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--baseline', help='Git revision for the expected failing original code')
args = parser.parse_args()
path = 'src/nvidia-modeset/kapi/src/nvkms-kapi.c'
harness = (root/'tests/kapi-device/allocation.c').read_text()
cases = ('device-alloc', 'sema-alloc', 'gpu-open', 'rm-alloc', 'kms-alloc', 'success', 'free-null')

def generate(source):
    start = source.index('static void FreeDevice(')
    end = source.index('\nNvBool nvKmsKapiAllocateSystemMemory', start)
    free_device = source[start:end]
    start = source.index('static struct NvKmsKapiDevice* AllocateDevice')
    end = source.index('\nstatic NvBool GrabOwnership', start)
    allocate_device = source[start:end]
    return harness.replace('/* ACTUAL_FREE_DEVICE */', free_device).replace(
        '/* ACTUAL_ALLOCATE_DEVICE */', allocate_device)

variants = [('working-tree', (root/path).read_text())]
if args.baseline:
    variants.append(('baseline', subprocess.check_output(
        ['git', 'show', args.baseline+':'+path], cwd=root, text=True)))
with tempfile.TemporaryDirectory(prefix='nvkms-allocation-') as tmp:
    tmp = Path(tmp)
    for variant, source in variants:
        program, binary = tmp/(variant+'.c'), tmp/variant
        program.write_text(generate(source))
        subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-O1', '-g',
                        '-fsanitize=address,undefined', '-fno-sanitize-recover=all',
                        '-fno-omit-frame-pointer', str(program), '-o', str(binary)], check=True)
        for case in cases:
            result = subprocess.run([str(binary), case], capture_output=True, text=True)
            expected_bug = variant == 'baseline' and case in ('device-alloc', 'free-null', 'sema-alloc', 'gpu-open')
            if expected_bug:
                diagnostic = ('member access within null pointer' if case in ('device-alloc', 'free-null')
                              else 'CLOSE_WITHOUT_OPEN')
                if result.returncode == 0 or diagnostic not in result.stderr:
                    raise RuntimeError(f'{variant}/{case}: expected {diagnostic}: {result.stderr}')
                line = next(line for line in result.stderr.splitlines() if diagnostic in line)
                print(f'REPRODUCED {variant}/{case}: {line}', flush=True)
            else:
                if result.returncode:
                    raise RuntimeError(f'{variant}/{case}: {result.stderr}')
                print(f'{variant}: {result.stdout.strip()}', flush=True)
