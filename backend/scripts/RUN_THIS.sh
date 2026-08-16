#!/bin/bash

set -e 

chmod +x "$(pwd)/fetch_gtfs/scheduled_fetch.sh"
chmod +x "$(pwd)/walking_times/scheduled_walking_times.sh"

(
	crontab -l 2>/dev/null
	echo "0 0 1 * * $(pwd)/walking_times/scheduled_walking_times.sh >> $(pwd)/cron.log 2>&1"
	echo "0 0 * * 0 $(pwd)/fetch_gtfs/scheduled_fetch.sh >> $(pwd)/cron.log 2>&1"
) | crontab -
