#!/bin/bash

set -e

SCRIPT_DIR="$( cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

rm -rf ./data/*
wget -q -P ./data/ "https://download.geofabrik.de/europe/poland/malopolskie-latest.osm.pbf"

docker run --rm -v "${PWD}/data:/data" osrm/osrm-backend osrm-extract -p /opt/foot.lua /data/malopolskie-latest.osm.pbf
docker run --rm -v "${PWD}/data:/data" osrm/osrm-backend osrm-partition /data/malopolskie-latest.osrm
docker run --rm -v "${PWD}/data:/data" osrm/osrm-backend osrm-customize /data/malopolskie-latest.osrm
docker run --rm -d --name osrm-server -p 5000:5000 -v "${PWD}:/data" osrm/osrm-backend osrm-routed --algorithm mld /data/malopolskie-latest.osrm

sleep 5

source ../.venv/bin/activate

python walking.py >> cron.log 2>&1

#if docker stop fails, the line below still return true, suppressing set -e
docker stop osrm-server || true
deactivate


