#pragma once

#ifndef BUSFINDER_BACKEND_ROUTING_H
#define BUSFINDER_BACKEND_ROUTING_H

#include <vector>
#include  <filesystem>
#include  <unordered_map>
#include "transporttable.h"
#include "stopfinder.h"
#include "translator.h"
#include "route_data.h"

class Routing {
public:
    const int search_window{30};
    const double time_for_change{0.5};
    const double walking_multiplier{1.01};
    const int walking_pace{60};
    const int adjacent_stops{30};

    struct Node {
        std::string stop_name; // for example "Kapelanka07"
        std::string line_id; // or "walk"

        friend bool operator < (const Node& n1, const Node& n2);
    };

    struct NodeTimeInfo {
        float cost_from_start; //time from start of the journy (change to time type later?)
        std::chrono::zoned_time<std::chrono::seconds> real_time; // for example 15:49:23
        //in python there was also the line id here again?
    };

    struct NodeTransport {
        std::string line_id; //can i get rid of this one here?
        std::chrono::zoned_time<std::chrono::seconds> departure; //15:00:00
        std::chrono::zoned_time<std::chrono::seconds> arrival; // 15:03:00
    };

    Routing(std::chrono::zoned_time<std::chrono::seconds> time);//konstruktor ktory bedzie zmieniac parametry wyzej?
    //moze potem dodaj konstruktor ktory bedzie zmieniac parametry wyzej?
    route_data output(const std::string &start, const std::string &target, std::chrono::zoned_time<std::chrono::seconds> time);
    route_data output_testing(const std::string &start, const std::string &target,
                    std::chrono::zoned_time<std::chrono::seconds> time);

private:
    std::filesystem::path csv_path_ = WALKING_CSV_PATH;
    std::unordered_map<std::string, int> walking_times_;
    TransportTable ttable_;
    StopFinder sf_;
    Translator translator_;

    void load_walking_csv();

    std::vector<NodeTransport> transport_between_stops( const std::string& stop_id1, const std::string& stop_id2, std::chrono::zoned_time<std::chrono::seconds> time);
    std::vector<NodeTransport> transport_between_stops_reverse( const std::string& stop_id1, const std::string& stop_id2, std::chrono::zoned_time<std::chrono::seconds> time);
    std::vector<NodeTransport> walking_between_stops( const std::string& stop_id1, const std::string& stop_id2, std::chrono::zoned_time<std::chrono::seconds> time);
    std::vector<NodeTransport> walking_between_stops_reverse( const std::string& stop_id1, const std::string& stop_id2, std::chrono::zoned_time<std::chrono::seconds> time);

    std::vector<std::pair<Node, std::pair<NodeTimeInfo, NodeTransport>>> dijkstra_dalekowzrocznosc(const std::string &start, const std::string &target, std::chrono::zoned_time<std::chrono::seconds> time);

    std::vector<std::pair<Node, NodeTimeInfo>> dijkstra_dalekowzrocznosc_reversed(const std::string &start, const std::string &target, std::chrono::zoned_time<std::chrono::seconds> time, std::string ending_line);

};

#endif // BUSFINDER_BACKEND_ROUTER_H
