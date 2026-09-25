# BusFinder KRK

![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?style=flat&logo=cplusplus&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.20+-064F8C?style=flat&logo=cmake&logoColor=white)
![PostgreSQL](https://img.shields.io/badge/PostgreSQL-15+-4169E1?style=flat&logo=postgresql&logoColor=white)

This is a public transport route-planning engine for the city of Krakow, Poland. It uses a custom implementation of Dijkstra's algorithm to efficiently calculate the fastest routes between locations in the city, utilizing GTFS files for reference.

## Architecture overview
- **Frontend**: [**WIP**] React(Vite)
- **Backend**: High performance C++ routing engine, using `libpqxx` for database queries and an embedded API built using `Crow`.
- **Database**: A `Postgres` database consisting of GTFS data.
- **Scripts**: Python & Bash scripts for automated GTFS fetching, walking-time matrix generation, and schedule loading.
## Project Structure
```
busfinder_krk/
└── backend/
    ├── include/ 
    │   ├── config.h 
    │   ├── routing.h
    │   └── transporttable.h
    ├── scripts/
    │   ├── fetch_gtfs/
    │   │   ├── load_gtfs.py
    │   │   └── scheduled_fetch.sh
    │   ├── walking_times/
    │   │   ├── scheduled_walking_times.sh
    │   │   ├── walking.py
    │   │   └── walking_times.csv
    │   ├── requirements.txt
    │   └── RUN_THIS.sh
    ├── src/
    │   ├── api.cpp
    │   ├── main.cpp
    │   ├── routing.cpp
    │   ├── stopfinder.cpp
    │   ├── translator.cpp
    │   └── transporttable.cpp
    ├── .env.example
    └── CMakeLists.txt
```
## Dependencies
- C++23 compliant compiler
- CMake 3.20+
- PostgreSQL 15+
- Python 3 (`requests`, `psycopg`), Bash, `cron`
## Installation 
Since the project is still in active development, it doesn't currently have a streamlined installation process, although we soon plan to containerize the backend using `Docker Compose` and add a React frontend.

## Planned features
- Docker Compose containerization
- React frontend and webpage
- `crontab` management cleanup; currently the `RUN_THIS.sh` script does this
