#ifndef BUSFINDER_BACKEND_ROUTING_H
#define BUSFINDER_BACKEND_ROUTING_H

#include <transporttable.h>
#include <vector>

class routing {
public:
  const int search_window{30};
  const double time_for_change{0.5};
  const int walking_multiplier{1};
  const int walking_pace{60};
  const int adjacent_stops{20};

private:
  transporttable ttable_;
  std::vector<trip> transport_between_stops_(
      std::string stop_id1, std::string stop_id2,
      std::chrono::time_point<std::chrono::system_clock> time);
  std::vector<trip> transport_between_stops_reversed_(
      std::string stop_id1, std::string stop_id2,
      std::chrono::time_point<std::chrono::system_clock> time);
  std::vector<trip> walking_between_stops_(
      std::string stop_id1, std::string stop_id2,
      std::chrono::time_point<std::chrono::system_clock> time);
  std::vector<trip> walking_between_stops_reversed_(
      std::string stop_id1, std::string stop_id2,
      std::chrono::time_point<std::chrono::system_clock> time);
};

#endif // BUSFINDER_BACKEND_ROUTER_H
