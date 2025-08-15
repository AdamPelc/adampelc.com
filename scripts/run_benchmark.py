#!/usr/bin/env python3
"""
Script to run benchmarks with specific taskset configuration and output formatting.
"""

import argparse
import subprocess
import sys
from datetime import datetime
from pathlib import Path


def find_binary(project_root, variant, binary_name):
    """Find binary in the build directory structure."""
    build_dir = project_root / "out" / "build" / variant
    
    # Search for the binary recursively
    for binary_path in build_dir.rglob(binary_name):
        if binary_path.is_file() and binary_path.stat().st_mode & 0o111:  # Check if executable
            return binary_path
    
    return None


def get_relative_path_from_build(binary_path, variant):
    """Get the relative path from the build/<variant> directory."""
    build_variant_dir = binary_path.parents[0]
    while build_variant_dir.name != variant:
        build_variant_dir = build_variant_dir.parent
        if build_variant_dir == build_variant_dir.parent:  # Reached root
            return None
    
    # Get path relative to build/<variant>
    relative_path = binary_path.parent.relative_to(build_variant_dir)
    return relative_path


def main():
    parser = argparse.ArgumentParser(
        description="Run benchmarks with taskset and standardized output format",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python run_benchmark.py release spsc_queue.benchmark
  python run_benchmark.py debug spsc_queue.test
        """
    )
    
    parser.add_argument(
        "variant",
        choices=["release", "debug", "rel_with_deb_info", "asan_ubsan", "tsan"],
        help="Build variant to run"
    )
    
    parser.add_argument(
        "binary",
        help="Binary name to run"
    )
    
    args = parser.parse_args()
    
    # Find project root (1 level up from this script)
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    
    # Find the binary
    binary_path = find_binary(project_root, args.variant, args.binary)
    
    if not binary_path:
        print(f"Error: Binary '{args.binary}' not found in variant '{args.variant}'")
        print(f"Searched in: {project_root}/out/build/{args.variant}")
        sys.exit(1)
    
    # Get relative path for output directory structure
    relative_path = get_relative_path_from_build(binary_path, args.variant)
    
    # Create output directory and filename
    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    output_filename = f"{args.binary}_{timestamp}.out"
    output_dir = project_root / "out" / "measurements" / args.variant / relative_path
    output_path = output_dir / output_filename
    
    # Create output directory if it doesn't exist
    output_dir.mkdir(parents=True, exist_ok=True)
    
    print(f"Running {args.binary} ({args.variant} variant)...")
    print(f"Executable: {binary_path}")
    print(f"Output: {output_path}")
    print("----------------------------------------")
    
    # Construct the command with taskset and benchmark parameters
    cmd = [
        "taskset", "-c", "0,2",
        str(binary_path),
        "--benchmark_min_warmup_time=1",
        "--benchmark_min_time=5s",
        "--benchmark_repetitions=10",
        "--benchmark_counters_tabular=true"
        "--benchmark_out_format=console",
        f"--benchmark_out={output_path}"
    ]
    
    # Run the benchmark
    try:
        subprocess.run(cmd, cwd=project_root, check=True)
        print(f"\nBenchmark completed successfully!")
        print(f"Results saved to: {output_path}")
    except subprocess.CalledProcessError as e:
        print(f"Benchmark failed with exit code {e.returncode}")
        sys.exit(e.returncode)


if __name__ == "__main__":
    main()