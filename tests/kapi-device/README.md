# KAPI device allocation failure reproducer

Run from this repository's root:

```
python3 tests/kapi-device/run.py --baseline e6a2e01579fbbd1ca2ed43fef2ee2589e9bd8bd9
```

Requires Python 3, Git and a C compiler supporting ASan/UBSan. The baseline
revision must be present locally. No GPU, root access or loaded driver is needed.

The script extracts the actual AllocateDevice and FreeDevice function bodies
from the working tree and, with --baseline, from that Git revision. It compiles
them with ASan/UBSan against a small C boundary double. Only external allocation,
semaphore, GPU-open/close and RM/KMS operations are mocked. Minimal C types model
the fields accessed by those two bodies; this is not a production ABI/layout
or full kernel integration test. No copy of either implementation is embedded
in the harness.

The original baseline must reproduce:

- device-allocation failure: NULL member access in FreeDevice;
- semaphore-allocation failure: GPU close without successful GPU open;
- GPU-open failure: GPU close without successful GPU open;
- FreeDevice(NULL): NULL member access instead of the documented no-op.

The working tree must handle all four cases and preserve successful teardown
and cleanup after RM/KMS allocation failures. Device, semaphore and GPU references
are checked for balance. Each case runs in its own userspace process. The
baseline's expected failures are negative controls, not failures of the fixed
regression suite. The script fails if an expected diagnostic is missing or an
unexpected failure occurs.

The RM/KMS teardown doubles deliberately accept NULL so that the sanitizer's
NULL diagnostic comes from FreeDevice's own device->gpuId access. The actual
KmsFreeDevice can dereference NULL earlier. A passing host test does not prove
behavior of real GPU firmware, proprietary binaries or a running kernel. No
live-driver panic is claimed or needed for this deterministic control-flow test.
