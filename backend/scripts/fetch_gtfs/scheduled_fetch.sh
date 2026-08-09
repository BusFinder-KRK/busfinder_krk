#!/bin/bash

set -e

SCRIPT_DIR = "$( cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd "$SCRIPT_DIR"

source ../.venv/bin/activate

python load_gtfs.py >> cron.log 2>&1

deactivate 
