"""Exercise the Windows benchmark clock with deterministic OS call results."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class BenchmarkClockTests(unittest.TestCase):
    @unittest.skipIf(os.name == "nt", "Windows CI compiles the real benchmark instead")
    def test_counter_conversion_and_failure(self):
        compiler = shutil.which(os.environ.get("CC", "cc"))
        self.assertIsNotNone(compiler, "C compiler required")
        with tempfile.TemporaryDirectory() as temporary:
            source = Path(temporary)
            (source / "windows.h").write_text('''
#ifndef DCC_FAKE_WINDOWS_H
#define DCC_FAKE_WINDOWS_H
typedef struct { long long QuadPart; } LARGE_INTEGER;
static int frequency_ok = 1, counter_ok = 1;
static long long frequency = 10000000, counter = 12500000;
static int QueryPerformanceFrequency(LARGE_INTEGER *v) { v->QuadPart = frequency; return frequency_ok; }
static int QueryPerformanceCounter(LARGE_INTEGER *v) { v->QuadPart = counter; return counter_ok; }
#endif
''')
            (source / "probe.c").write_text('''
#include <assert.h>
#include "windows.h"
#define _WIN32 1
#include "support/benchmark_clock.h"
int main(void) {
    struct timespec value;
    assert(dcc_benchmark_now(&value) == 0);
    assert(value.tv_sec == 1 && value.tv_nsec == 250000000);
    frequency_ok = 0;
    assert(dcc_benchmark_now(&value) != 0);
    frequency_ok = 1; frequency = 0;
    assert(dcc_benchmark_now(&value) != 0);
    frequency = 10000000; counter_ok = 0;
    assert(dcc_benchmark_now(&value) != 0);
    return 0;
}
''')
            compiled = subprocess.run([compiler, "-std=c11", "-I", str(source), "-I", str(ROOT / "tests"),
                                       str(source / "probe.c"), "-o", str(source / "probe")], capture_output=True, text=True)
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            result = subprocess.run([str(source / "probe")], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
