#pragma once

#ifndef BUSFINDER_BACKEND_ROUTING_H
#define BUSFINDER_BACKEND_ROUTING_H

#include <vector>
#include  <filesystem>
#include  <unordered_map>
#include "transporttable.h"
#include "stopfinder.h"

class Routing {
public:
    const int search_window{30};
    const double time_for_change{0.5};
    const int walking_multiplier{1};
    const int walking_pace{60};
    const int adjacent_stops{20};

    struct Node {
        std::string stop_name; // for example "Kapelanka07"
        std::string line_id; // or "walk"

        friend bool operator < (const Node& n1, const Node& n2);
    };

    struct NodeTimeInfo {
        int time_from_start; //time from start of the journy (change to time type later?)
        std::chrono::time_point<std::chrono::system_clock> real_time; // for example 15:49:23
        //in python there was also the line id here again?
    };

    struct NodeTransport {
        std::string line_id;
        std::chrono::time_point<std::chrono::system_clock> departure;
        std::chrono::time_point<std::chrono::system_clock> arrival;
    };

    Routing(std::chrono::time_point<std::chrono::system_clock> time);//konstruktor ktory bedzie zmieniac parametry wyzej?
    //moze potem dodaj konstruktor ktory bedzie zmieniac parametry wyzej?
    std::vector<std::pair<Node, NodeTimeInfo>> output(const std::string &start, const std::string &target, std::chrono::time_point<std::chrono::system_clock> time);


private:
    //this is hardcoded - should i make it not hardcoded?
    std::filesystem::path csv_path_ = "scripts/walking_times/walking_times.csv";
    std::unordered_map<std::string, int> walking_times_;
    TransportTable ttable_;
    StopFinder sf_;

    void load_walking_csv();

    std::vector<NodeTransport> transport_between_stops( const std::string& stop_id1, const std::string& stop_id2, std::chrono::time_point<std::chrono::system_clock> time);
    std::vector<NodeTransport> transport_between_stops_reverse( const std::string& stop_id1, const std::string& stop_id2, std::chrono::time_point<std::chrono::system_clock> time);
    std::vector<NodeTransport> walking_between_stops( const std::string& stop_id1, const std::string& stop_id2, std::chrono::time_point<std::chrono::system_clock> time);
    std::vector<NodeTransport> walking_between_stops_reverse( const std::string& stop_id1, const std::string& stop_id2, std::chrono::time_point<std::chrono::system_clock> time);

    std::vector<std::pair<Node, NodeTimeInfo>> dijkstra_dalekowzrocznosc(const std::string &start, const std::string &target, std::chrono::time_point<std::chrono::system_clock> time);

    std::vector<std::pair<Node, NodeTimeInfo>> dijkstra_dalekowzrocznosc_reversed(const std::string &start, const std::string &target, std::chrono::time_point<std::chrono::system_clock> time);

};

#endif // BUSFINDER_BACKEND_ROUTER_H
