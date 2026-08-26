#ifndef BUSFINDER_BACKEND_API_H
#define BUSFINDER_BACKEND_API_H

#include <crow.h>
#include <route_data.h>

class api {
public:
  api();
  ~api() = default;
  static crow::json::wvalue make_default_response(const route_data&);
  void run();
private:
  crow::SimpleApp app_;
};

#endif // BUSFINDER_BACKEND_API_H
