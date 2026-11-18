#!/usr/bin/env python3
"""Build and run BAR1 regressions against the production C sources, without a GPU."""

import os
from pathlib import Path
import re
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent


def main():
    # memMap_IMPL needs a live RM/GPU. Compile its actual flag-selection block
    # and mapping validation function in isolation, rather than copying their
    # logic into the test. Fail visibly if their source boundaries change.
    source = (ROOT / "src/nvidia/src/kernel/rmapi/mapping_cpu.c").read_text()
    start = source.index("NvU32 busMapFbFlags = BUS_MAP_FB_FLAGS_MAP_UNICAST;")
    end = source.index("\n                if(DRF_VAL", start)
    policy = source[start:end]
    start = source.index("NV_STATUS\nrmapiValidateKernelMapping\n")
    end = source.index("\nNV_STATUS\nserverMap_Prologue", start)
    validation = source[start:end]
    bus_header = (ROOT / "src/nvidia/generated/g_kern_bus_nvoc.h").read_text()
    bus_flags = "\n".join(
        re.search(r"^#define " + name + r"\s+[^\n]+", bus_header, re.MULTILINE)[0]
        for name in ["BUS_MAP_FB_FLAGS_MAP_UNICAST", "BUS_MAP_FB_FLAGS_ALLOW_DISCONTIG"]
    )

    with tempfile.TemporaryDirectory(prefix="nv-bar1-test-") as tmp:
        tmp = Path(tmp)
        (tmp / "mapping_policy.h").write_text(
            bus_flags + "\n" + validation
            + "\nstatic NvU32 mappingFlags(const TestMapParams *pMapParams)\n{\n"
            + policy
            + "\nreturn busMapFbFlags;\n}\n"
        )
        command = shlex.split(os.environ.get("CC", "cc")) + [
            "-std=gnu11", "-g", "-O1", "-Wall", "-Wextra", "-Werror",
            "-Wno-unused-parameter",
            "-fsanitize=undefined", "-fsanitize-undefined-trap-on-error",
            "-fno-omit-frame-pointer",
            "-DPORT_IS_KERNEL_BUILD=1", "-DPORT_IS_CHECKED_BUILD=0",
            "-DPORT_MODULE_memory=1", "-DPORT_MODULE_atomic=1", "-DNV_PRINTF_ENABLED=0",
            "-DNV_JOURNAL_ASSERT_ENABLE=0", "-DNV_UNIX=1",
            "-include", str(HERE / "test_support.h"),
            "-include", str(ROOT / "src/common/sdk/nvidia/inc/cpuopsys.h"),
        ]
        for include in [
            "src/common/sdk/nvidia/inc", "src/common/inc",
            "src/nvidia/inc/libraries", "src/nvidia/inc/kernel",
            "src/common/shared/inc",
        ]:
            command += ["-I", str(ROOT / include)]
        command += ["-I", str(tmp)]
        command += [str(ROOT / path) for path in [
            "src/nvidia/src/libraries/containers/map.c",
            "src/nvidia/src/libraries/mapping_reuse/mapping_reuse.c",
            "tests/bar1/test_bar1.c",
        ]]
        command += ["-o", str(tmp / "test_bar1")]
        subprocess.run(command, check=True)
        subprocess.run([str(tmp / "test_bar1")], check=True)


if __name__ == "__main__":
    main()
