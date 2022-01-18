#!/usr/bin/env python3
import sys
from pathlib import Path

from subprocess import run, CalledProcessError

import click

INCLUDE_DIRS = ["include"]
SRC_FILES = ["fibonacci_bench.c", "exception/setjmp.c"]

@click.command()
@click.option('--relaxed', is_flag=True, help="Be more relaxed (disable -Werror)")
@click.option('--optimize', '-O', is_flag=True, help="Set -O2 and disable debug info")
def build(relaxed=False, optimize=False):
    compiler_flags = [
        "-Wall",
        "-Werror",
        "-c", # Output object files
    ]
    if not relaxed:
        compiler_flags.append("-Werror")
    if optimize:
        compiler_flags.append("-O2")
        # TODO: -DNDEBUG ?
    else:
        compiler_flags.append("-g")
    build_dir = Path('build')
    build_dir.mkdir(exist_ok=True)
    object_files = []
    compiler_flags.extend(f"-I{idir}" for idir in INCLUDE_DIRS)
    for file in SRC_FILES:
        p = Path("src", file)
        assert p.exists(), f"Missing {p}"
        obj_file = Path(build_dir, p.parent.relative_to('src'), f"{p.stem}.o")
        if not obj_file.parent.exists():
            obj_file.parent.mkdir(parents=True)
        try:
            run(['gcc', *compiler_flags, str(p), '-o', str(obj_file)], check=True)
        except CalledProcessError as e:
            print(f"Failed to compile {p}", file=sys.stderr)
            sys.exit(1)
        object_files.append(obj_file)
    run(['gcc', *map(str, object_files), '-o', 'build/fibonnaci'], check=True)

if __name__ == "__main__":
    build()
