#!/usr/bin/env bash
set -euo pipefail

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
if [[ "$(uname -s)" != "Linux" ]]; then
    printf '%s\n' 'Build proyek ini di Linux, misalnya Ubuntu VM.' >&2
    exit 1
fi
if ! command -v make >/dev/null 2>&1; then
    printf '%s\n' 'GNU Make belum tersedia. Di Ubuntu: sudo apt install build-essential' >&2
    exit 1
fi
exec make "$@"
