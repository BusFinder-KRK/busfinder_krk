import os
import zipfile as z
from datetime import datetime, timedelta, timezone

import dotenv as d
import psycopg as p
import requests as r

url_bus = "https://gtfs.ztp.krakow.pl/GTFS_KRK_A.zip"
url_tram = "https://gtfs.ztp.krakow.pl/GTFS_KRK_T.zip"

bus_filename = "GTFS_BUS.zip"
tram_filename = "GTFS_TRAM.zip"
start_time: datetime = datetime.now(tz=timezone.utc).astimezone()
print(f"[{start_time.strftime("%X %x")}] Starting GTFS import...")
with r.get(url_bus, stream=True) as response:
    response.raise_for_status()
    with open(bus_filename, "wb") as f:
        for chunk in response.iter_content(chunk_size=8192):
            if chunk:
                f.write(chunk)

print("Downloaded BUS data succesfully.")

with r.get(url_tram, stream=True) as response:
    response.raise_for_status()
    with open(tram_filename, "wb") as f:
        for chunk in response.iter_content(chunk_size=8192):
            if chunk:
                f.write(chunk)

print("Downloaded TRAM data succesfully.")
print("Downloaded GTFS files succesfully.")

extract_to_bus = "./GTFS_bus_extracted"
extract_to_tram = "./GTFS_tram_extracted"

with z.ZipFile(bus_filename, 'r') as bus_ref:
    bus_ref.extractall(extract_to_bus)
with z.ZipFile(tram_filename, 'r') as bus_ref:
    bus_ref.extractall(extract_to_tram)

print("Files unzipped succesfully.")

#loading credentials from a .env
d.load_dotenv()

conn = p.connect(
    host = os.environ["host"],
    port = int(os.environ["port"]),
    dbname = os.environ["dbname"],
    user = os.environ["user"],
    password = os.environ["password"]
)

with conn.cursor() as cur:
    cur.execute("TRUNCATE TABLE agency, calendar, calendar_dates, routes, stop_times, stops, trips CASCADE")
    cur.execute("""CREATE TEMP TABLE staging_calendar_dates_bus (service_id TEXT, date TEXT, exception_type INT)""")
    cur.execute("""CREATE TEMP TABLE staging_calendar_dates_tram (service_id TEXT, date TEXT, exception_type INT)""")
    cur.execute("""CREATE TEMP TABLE staging_routes_bus (
                        route_id TEXT,
                        agency_id TEXT,
                        route_short_name TEXT,
                        route_long_name TEXT,
                        route_desc TEXT,
                        route_type INT,
                        route_url TEXT,
                        route_color TEXT,
                        route_text_color TEXT,
                        route_sort_order INT,
                        continuous_pickup INT,
                        continuous_drop_off INT)

                    ON COMMIT DROP""")
    
    cur.execute("""CREATE TEMP TABLE staging_routes_tram (
                        route_id TEXT,
                        agency_id TEXT,
                        route_short_name TEXT,
                        route_long_name TEXT,
                        route_desc TEXT,
                        route_type INT,
                        route_url TEXT,
                        route_color TEXT,
                        route_text_color TEXT)

                    ON COMMIT DROP""")

    cur.execute("""CREATE TEMP TABLE staging_stop_times_bus (
                        trip_id TEXT,
                        arrival_time TEXT,
                        departure_time TEXT,
                        stop_id TEXT,
                        stop_sequence INT,
                        stop_headsign TEXT,
                        pickup_type INT,
                        drop_off_type INT,
                        shape_dist_traveled DOUBLE PRECISION,
                        timepoint BOOL)

                    ON COMMIT DROP""")

    cur.execute("""CREATE TEMP TABLE staging_stop_times_tram (
                        trip_id TEXT,
                        arrival_time TEXT,
                        departure_time TEXT,
                        stop_id TEXT,
                        stop_sequence INT,
                        stop_headsign TEXT,
                        pickup_type INT,
                        drop_off_type INT,
                        shape_dist_traveled DOUBLE PRECISION,
                        timepoint BOOL)

                    ON COMMIT DROP""")

    cur.execute("""CREATE TEMP TABLE staging_stops_bus (
                        stop_id TEXT,
                        stop_code TEXT,
                        stop_name TEXT,
                        stop_desc TEXT,
                        stop_lat DOUBLE PRECISION,
                        stop_lon DOUBLE PRECISION,
                        zone_id TEXT,
                        stop_url TEXT,
                        location_type INT,
                        parent_station TEXT,
                        stop_timezone TEXT,
                        wheelchair_boarding BOOL,
                        platform_code TEXT)

                    ON COMMIT DROP""")

    cur.execute("""CREATE TEMP TABLE staging_stops_tram (
                        stop_id TEXT,
                        stop_code TEXT,
                        stop_name TEXT,
                        stop_desc TEXT,
                        stop_lat DOUBLE PRECISION,
                        stop_lon DOUBLE PRECISION,
                        zone_id TEXT,
                        stop_url TEXT,
                        location_type INT,
                        parent_station TEXT,
                        stop_timezone TEXT,
                        wheelchair_boarding BOOL,
                        platform_code TEXT)

                    ON COMMIT DROP""")

    cur.execute("""CREATE TEMP TABLE staging_trips_bus (
                        route_id TEXT,
                        service_id TEXT,
                        trip_id TEXT,
                        trip_headsign TEXT,
                        trip_short_name TEXT,
                        direction_id INT,
                        block_id TEXT,
                        shape_id TEXT,
                        wheelchair_accessible INT,
                        bikes_allowed INT)
                    
                    ON COMMIT DROP""")

    cur.execute("""CREATE TEMP TABLE staging_trips_tram (
                        trip_id TEXT,
                        route_id TEXT,
                        service_id TEXT,
                        trip_headsign TEXT,
                        trip_short_name TEXT,
                        direction_id INT,
                        block_id TEXT,
                        shape_id TEXT,
                        wheelchair_accessible INT)
                    
                    ON COMMIT DROP""")



    with open("GTFS_bus_extracted/calendar.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY calendar FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())
    with open("GTFS_bus_extracted/calendar_dates.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY staging_calendar_dates_bus FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())
    with open("GTFS_bus_extracted/agency.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY agency FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())
    with open("GTFS_bus_extracted/routes.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY staging_routes_bus FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())
    with open("GTFS_bus_extracted/stops.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY staging_stops_bus FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())
    with open("GTFS_bus_extracted/stop_times.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY staging_stop_times_bus FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())
    with open("GTFS_bus_extracted/trips.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY staging_trips_bus FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())

    print("Copied into bus staging tables succesfully.")

    with open("GTFS_tram_extracted/calendar.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY calendar FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())
    with open("GTFS_tram_extracted/calendar_dates.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY staging_calendar_dates_tram FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())
    with open("GTFS_tram_extracted/agency.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY agency FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())
    with open("GTFS_tram_extracted/routes.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY staging_routes_tram FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())
    with open("GTFS_tram_extracted/stops.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY staging_stops_tram FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())
    with open("GTFS_tram_extracted/stop_times.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY staging_stop_times_tram FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())
    with open("GTFS_tram_extracted/trips.txt", "r") as f:  # noqa: SIM117
        with cur.copy("COPY staging_trips_tram FROM STDIN WITH (FORMAT csv, HEADER true)") as copy:
            copy.write(f.read())

    print("Copied into tram staging tables succesfully.")

    cur.execute("""
            INSERT INTO calendar_dates (
                service_id, date, exception_type
            )
            SELECT
                service_id, date, exception_type
            FROM staging_calendar_dates_bus;
        """)

    cur.execute("""
            INSERT INTO calendar_dates (
                service_id, date, exception_type
            )
            SELECT
                service_id, date, exception_type
            FROM staging_calendar_dates_tram;
        """)

    cur.execute("""
            INSERT INTO routes (
                route_id, agency_id, route_short_name, route_long_name, 
                route_desc, route_type, route_url, route_color, route_text_color
            )
            SELECT 
                route_id, agency_id, route_short_name, route_long_name, 
                route_desc, route_type, route_url, route_color, route_text_color
            FROM staging_routes_bus;
        """)

    cur.execute("""
            INSERT INTO routes (
                route_id, agency_id, route_short_name, route_long_name, 
                route_desc, route_type, route_url, route_color, route_text_color
            )
            SELECT 
                route_id, agency_id, route_short_name, route_long_name, 
                route_desc, route_type, route_url, route_color, route_text_color
            FROM staging_routes_tram;
        """)

    cur.execute("""
            INSERT INTO trips (
                trip_id, route_id, service_id, trip_headsign, trip_short_name, direction_id, block_id, shape_id, wheelchair_accessible
            )
            SELECT
                trip_id, route_id, service_id, trip_headsign, trip_short_name, direction_id, block_id, shape_id, wheelchair_accessible
            FROM staging_trips_bus;
            """)

    cur.execute("""
            INSERT INTO trips (
                trip_id, route_id, service_id, trip_headsign, trip_short_name, direction_id, block_id, shape_id, wheelchair_accessible
            )
            SELECT
                trip_id, route_id, service_id, trip_headsign, trip_short_name, direction_id, block_id, shape_id, wheelchair_accessible
            FROM staging_trips_tram;
            """)
    cur.execute("""
            INSERT INTO stops (
                stop_id, stop_code, stop_name, stop_desc, stop_lat, stop_lon, zone_id, stop_url, location_type, parent_station, stop_timezone, wheelchair_boarding
            )
            SELECT 
                stop_id, stop_code, stop_name, stop_desc, stop_lat, stop_lon, zone_id, stop_url, location_type, parent_station, stop_timezone, wheelchair_boarding
            FROM staging_stops_bus;
        """)

    cur.execute("""
            INSERT INTO stops (
                stop_id, stop_code, stop_name, stop_desc, stop_lat, stop_lon, zone_id, stop_url, location_type, parent_station, stop_timezone, wheelchair_boarding
            )
            SELECT 
                stop_id, stop_code, stop_name, stop_desc, stop_lat, stop_lon, zone_id, stop_url, location_type, parent_station, stop_timezone, wheelchair_boarding
            FROM staging_stops_tram;
        """)

    cur.execute("""
            INSERT INTO stop_times (
                trip_id, arrival_time, departure_time, stop_id, stop_sequence, stop_headsign, pickup_type, drop_off_type, shape_dist_traveled, timepoint
            )
            SELECT
                trip_id, arrival_time, departure_time, stop_id, stop_sequence, stop_headsign, pickup_type, drop_off_type, shape_dist_traveled, timepoint
            FROM staging_stop_times_bus;
    """)

    cur.execute("""
            INSERT INTO stop_times (
                trip_id, arrival_time, departure_time, stop_id, stop_sequence, stop_headsign, pickup_type, drop_off_type, shape_dist_traveled, timepoint
            )
            SELECT
                trip_id, arrival_time, departure_time, stop_id, stop_sequence, stop_headsign, pickup_type, drop_off_type, shape_dist_traveled, timepoint
            FROM staging_stop_times_tram;
    """)
    
    print("Imported into final tables succesfully.")

    from psycopg import sql

    tables = ["agency", "calendar", "calendar_dates", "routes", "stops", "trips", "stop_times"]

    for table in tables:
        query = sql.SQL("SELECT COUNT(*) FROM {};").format(sql.Identifier(table))
        cur.execute(query)
        row = cur.fetchone()
        assert row is not None
        count = row[0]
        print(f"{table}: {count:,} rows")

    end_time: datetime = datetime.now(tz=timezone.utc).astimezone()
    duration: timedelta = end_time - start_time
    print(f"[{end_time.strftime("%X %x")}] GTFS import done. Time elapsed: {duration.seconds} s {duration.microseconds // 1000} ms")

conn.commit()