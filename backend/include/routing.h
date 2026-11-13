#pragma once

#ifndef BUSFINDER_BACKEND_ROUTING_H
#define BUSFINDER_BACKEND_ROUTING_H

#include "route_data.h"
#include "stopfinder.h"
#include "translator.h"
#include "transporttable.h"
#include <filesystem>
#include <unordered_map>
#include <vector>

class Routing {
public:
  const int search_window;
  const double time_for_change;
  const double walking_multiplier;
  const double walking_pace;
  const int adjacent_stops;
  Translator translator_{};

  struct Node {
    std::string stop_name;
    std::string line_id;

    friend bool operator<(const Node &n1, const Node &n2);
  };

    struct NodeTimeInfo {
    float cost_from_start;
    std::chrono::zoned_time<std::chrono::seconds>
        real_time;

  };

  struct NodeTransport {
    std::string line_id;
    std::chrono::zoned_time<std::chrono::seconds> departure;
    std::chrono::zoned_time<std::chrono::seconds> arrival;
  };

  explicit Routing(std::chrono::zoned_time<std::chrono::seconds> time_arg,
                   int search_window_arg, double time_for_change_arg,
                   double walking_multiplier_arg, double walking_pace_arg,
                   int adjacent_stops_arg);

  RouteData output(const std::string &start, const std::string &target,
                   std::chrono::zoned_time<std::chrono::seconds> time);
  RouteData output_testing(const std::string &start, const std::string &target,
                           std::chrono::zoned_time<std::chrono::seconds> time);

private:
  std::filesystem::path csv_path_ = "scripts/walking_times/walking_times.csv";
  std::unordered_map<std::pair<std::string, std::string>, double, PairHash>
      walking_times_;
  TransportTable ttable_;
  StopFinder sf_;

  void load_walking_csv();

  std::vector<NodeTransport>
  transport_between_stops(const std::string &stop_id1,
                          const std::string &stop_id2,
                          std::chrono::zoned_time<std::chrono::seconds> time);
  std::vector<NodeTransport> transport_between_stops_reverse(
      const std::string &stop_id1, const std::string &stop_id2,
      std::chrono::zoned_time<std::chrono::seconds> time);

  std::optional<NodeTransport>
  walking_between_stops(const std::string &stop_id1,
                        const std::string &stop_id2,
                        std::chrono::zoned_time<std::chrono::seconds> time);

  std::optional<NodeTransport> walking_between_stops_reverse(
      const std::string &stop_id1, const std::string &stop_id2,
      std::chrono::zoned_time<std::chrono::seconds> time);

  std::vector<std::pair<Node, std::pair<NodeTimeInfo, NodeTransport>>>
  dijkstra_dalekowzrocznosc(const std::string &start, const std::string &target,
                            std::chrono::zoned_time<std::chrono::seconds> time);

  std::vector<std::pair<Node, std::pair<NodeTimeInfo, NodeTransport>>>
  dijkstra_new_reversed(const std::string &start, const std::string &target,
                        std::chrono::zoned_time<std::chrono::seconds> time);
};

#endif // BUSFINDER_BACKEND_ROUTING_H
