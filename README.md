# adampelc.com

Code and assets for my personal website

## Project Overview

This repository contains C++ articles and implementations featuring high-performance data structures and algorithms. The codebase includes comprehensive benchmarks and tests for performance-critical components.

### Current Articles

- **SPSC Queue**: Single Producer Single Consumer queue implementations with various approaches:
  - Single-threaded queue implementation
  - Lock-based queue (planned)
  - Comprehensive benchmarks and performance tests

## Build System

This project uses CMake with presets for different build configurations. All builds use C++26 standard with Ninja generator.

### Build Variants

| Variant | Description | CMAKE_BUILD_TYPE | Sanitizers |
|---------|-------------|------------------|------------|
| `release` | Optimized release build | Release | None |
| `debug` | Debug build without optimizations | Debug | None |
| `rel_with_deb_info` | Optimized with debug symbols | RelWithDebInfo | None |
| `asan_ubsan` | Address & UB Sanitizer build | Debug | ASAN + UBSAN |
| `tsan` | Thread Sanitizer build | Debug | TSAN |

## How to Build

### Prerequisites

- CMake 3.31 or higher
- C++26 compatible compiler
- Google Test (for tests)
- Google Benchmark (for benchmarks)

### Build Commands

```bash
# Configure and build a specific variant
cmake --preset <variant>
cmake --build out/build/<variant>

# Examples:
cmake --preset release
cmake --build out/build/release

cmake --preset debug
cmake --build out/build/debug

cmake --preset asan_ubsan
cmake --build out/build/asan_ubsan
```

### Running Tests

```bash
# Run all tests for release build
ctest --preset all

# Run specific test executable
./out/build/<variant>/articles/spsc_queue/spsc_queue.test
```

## Scripts

### Benchmark Runner

**Location**: `scripts/run_benchmark.py`

A standardized benchmark runner that executes benchmarks with consistent parameters and output formatting.

**Features**:
- CPU affinity control using `taskset -c 0,1`
- Standardized benchmark parameters (warmup, timing, repetitions)
- Automatic binary discovery in build directories
- Timestamped CSV output files
- Organized results in `out/measurements/<variant>/` structure

**Usage**:
```bash
python scripts/run_benchmark.py <variant> <binary_name>

# Examples:
python scripts/run_benchmark.py release spsc_queue.benchmark
python scripts/run_benchmark.py debug spsc_queue.test
python scripts/run_benchmark.py asan_ubsan spsc_queue.benchmark
```

**Output**: Results are saved as timestamped CSV files in:
```
out/measurements/<variant>/<path_to_binary>/<binary_name>_YYYY-MM-DD_HH-MM-SS.csv
```

**Benchmark Parameters**:
- Warmup time: 1 second minimum
- Measurement time: 5 seconds minimum per benchmark
- Repetitions: 10 runs per benchmark
- Output format: CSV for easy analysis
