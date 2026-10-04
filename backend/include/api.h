#ifndef BUSFINDER_BACKEND_API_H
#define BUSFINDER_BACKEND_API_H

#include <crow.h>
#include "route_data.h"

class api {
public:
  api();
  ~api() = default;
  void run();
private:
  crow::SimpleApp app_;
  /// utilities
  static crow::json::wvalue make_default_response(const RouteData&);
  static crow::json::wvalue make_stop_list();
  static std::chrono::zoned_time<std::chrono::seconds> parse_time(const std::string& time_str);
};

#endif // BUSFINDER_BACKEND_API_H
