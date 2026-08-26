#ifndef BUSFINDER_BACKEND_ROUTE_DATA_H
#define BUSFINDER_BACKEND_ROUTE_DATA_H

#include <string>
#include <vector>

struct route_data {
  struct route_data_node {
    std::string stop_id;
    std::pair<double, double> stop_coords;
    std::string stop_name;
    std::string type;
    std::string route_name;
    std::string time;
  };
  std::string start_stop;
  std::string end_stop;
  std::string start_time;
  std::string end_time;
  std::vector<route_data_node> route_proper;
};

#endif // BUSFINDER_BACKEND_ROUTE_DATA_H
