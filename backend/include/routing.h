#ifndef BUSFINDER_BACKEND_ROUTING_H
#define BUSFINDER_BACKEND_ROUTING_H

#include <transporttable.h>
#include <vector>

class Routing {
public:
    const int search_window{30};
    const double time_for_change{0.5};
    const int walking_multiplier{1};
    const int walking_pace{60};
    const int adjacent_stops{20};

private:
    transporttable ttable_;
    //moje:
    struct Node {
        std::string stop_name; // for example "Kapelanka07"
        std::string line_id; // or "walk"
    };

    struct NodeTimeInfo {
        float time_from_start; //time from start of the journy (change to time type later?)
        std::chrono::time_point<std::chrono::system_clock> real_time; // for example 15:49:23
        //in python there was also the line id here again?
    };

    struct NodeTransport {
        std::string line_id;
        std::chrono::time_point<std::chrono::system_clock> departure;
        std::chrono::time_point<std::chrono::system_clock> arrival;
    };

    std::vector<NodeTransport> transport_between_stops_( std::string stop_id1, std::string stop_id2, std::chrono::time_point<std::chrono::system_clock> time);
    std::vector<NodeTransport> walking_between_stops_( std::string stop_id1, std::string stop_id2, std::chrono::time_point<std::chrono::system_clock> time);

    void dijkstra_dalekowzrocznosc(const std::string &start, const std::string &target,
                                   std::chrono::time_point<std::chrono::system_clock> time);
};

#endif // BUSFINDER_BACKEND_ROUTER_H
