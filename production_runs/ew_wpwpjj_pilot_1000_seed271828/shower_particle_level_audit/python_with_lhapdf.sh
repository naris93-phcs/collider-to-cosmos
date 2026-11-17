#!/usr/bin/env bash
set -euo pipefail

repo=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../.." && pwd)
python="$HOME/miniforge3/envs/hep/bin/python"
prefix="$repo/MG5_aMC_v3_8_0/HEPTools/lhapdf6_py3"
export PYTHONPATH="$prefix/lib/python3.14/site-packages${PYTHONPATH:+:$PYTHONPATH}"
export LD_LIBRARY_PATH="$prefix/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
exec "$python" -B "$@"
