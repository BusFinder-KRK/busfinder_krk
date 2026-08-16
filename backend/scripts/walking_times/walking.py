import psycopg as p
import dotenv as d
import requests as r

from os import environ
from scipy.spatial import KDTree 
import utm

d.load_dotenv()


conn: p.Connection =  p.connect(
        host = environ["host"],
        port = int(environ["port"]),
        dbname = environ["dbname"],
        user = environ["user"],
        password = environ["password"]
) 
 
query_result_distances: dict[str, tuple[float, float]] = {}
query_result_coordinates: dict[str, tuple[float, float]] = {}
with conn.cursor() as cur:
    cur.execute("SELECT stop_id, stop_lon, stop_lat FROM stops")
    for row in cur:
        x, y, _, _ = utm.from_latlon(row[2], row[1])
        query_result_coordinates[str(row[0])] = (row[1], row[2])
        query_result_distances[str(row[0])] = (x,y)
stop_ids = list(query_result_distances.keys())
coords = list(query_result_distances.values())
tree = KDTree(data=coords)
walking_indices: set[tuple[int, int]] = tree.query_pairs(r=1000)
result: dict[tuple[str, str], float] = {}
sesh = r.Session()
for i,j in walking_indices:
        stop_a = stop_ids[i]
        stop_b = stop_ids[j]
        coords_a = query_result_coordinates[stop_a]
        coords_b = query_result_coordinates[stop_b]
        response = sesh.get(f"http://localhost:5000/route/v1/foot/{coords_a[0]},{coords_a[1]};{coords_b[0]},{coords_b[1]}?overview=false", timeout=5)
        response_json = response.json()
        if response_json.get("code") == "Ok":
                walking_time = response_json["routes"][0]["duration"]/60
                result[(stop_a,stop_b)] = walking_time
        else:
                result[(stop_a,stop_b)] = -1
with open("walking_times.csv", "w") as f:
        for stop_tuple, time in result.items():
                f.write(f"{stop_tuple[0]}\t{stop_tuple[1]}\t{time}\n")


