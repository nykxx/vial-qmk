#!/bin/bash
set -euo pipefail

usage() {
    cat <<'EOF'
Usage: ./vial-build.sh [--keep-build] <keyboard:keymap>

Options:
  --keep-build  Copy .build to LOG_DIR/<build-id>/.build/ after success or failure
  -h, --help    Show this help

Environment variables:
  FIRMWARE_DIR  UF2 output directory (default: firmware)
  LOG_DIR       Build log directory (default: logs)
  JOBS          Parallel build jobs (default: nproc)

Relative paths are resolved from the directory containing this script.
UF2 files and log directories use matching timestamped names; previous builds are kept.
Each log directory contains build.log and, with --keep-build, a .build/ copy.
EOF
}

target=
keep_build=false
for arg in "$@"; do
    case "$arg" in
        --keep-build)
            keep_build=true
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            if [[ -n $target || ! $arg =~ ^[[:alnum:]_/-]+:[[:alnum:]_-]+$ ]]; then
                usage >&2
                exit 2
            fi
            target=$arg
            ;;
    esac
done

if [[ -z $target ]]; then
    usage >&2
    exit 2
fi

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

firmware_dir=${FIRMWARE_DIR:-firmware}
log_dir=${LOG_DIR:-logs}
jobs=${JOBS:-$(nproc)}
firmware_name=${target//[\/:]/_}
build_id="${firmware_name}_$(date +%Y%m%d-%H%M%S)-${EPOCHREALTIME#*.}"
firmware_file="$firmware_dir/$build_id.uf2"
run_dir="$log_dir/$build_id"
log_file="$run_dir/build.log"
build_dir="$run_dir/.build"
mkdir -p -- "$firmware_dir" "$run_dir"

# Pipe the whole build through tee so stderr and failure summaries are saved too.
(
    start=$EPOCHREALTIME
    build_started=false
    finish() {
        status=$?
        end=$EPOCHREALTIME
        elapsed=$(( ${end/./} - ${start/./} ))

        # Only preserve this run's files, not an old .build left by a failed clean.
        if [[ $keep_build == true && $build_started == true && -d .build ]]; then
            if cp -a -- .build "$build_dir"; then
                printf '\nBuild artifacts: %s\n' "$build_dir"
            else
                copy_status=$?
                printf '\nError: could not preserve .build in %s\n' "$build_dir" >&2
                if (( status == 0 )); then
                    status=$copy_status
                fi
            fi
        fi

        if (( status == 0 )); then
            printf '\nBuild succeeded.\n'
        else
            printf '\nBuild failed (exit status: %d).\n' "$status"
        fi
        printf 'time: %d.%03d sec\n' "$((elapsed / 1000000))" "$(((elapsed % 1000000) / 1000))"
        printf 'Log: %s\n' "$log_file"
        exit "$status"
    }
    trap finish EXIT

    printf 'Target: %s\nStarted: %s\nJobs: %s\n\n' "$target" "$(date -Iseconds)" "$jobs"
    make clean
    build_started=true
    make -Otarget -j"$jobs" "$target"

    # clean removes .build, so require a fresh artifact before moving the root copy.
    if [[ ! -f ".build/$firmware_name.uf2" || ! -f "$firmware_name.uf2" ]]; then
        printf 'Error: expected UF2 was not generated: %s.uf2\n' "$firmware_name" >&2
        exit 1
    fi
    mv -- "$firmware_name.uf2" "$firmware_file"
    printf '\nFirmware: %s\n' "$firmware_file"
) 2>&1 | tee -- "$log_file"
