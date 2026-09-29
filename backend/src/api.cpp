#include <api.h>
#include <crow.h>
#include <route_data.h>
#include <chrono>
#include <routing.h>

api::api() {
  CROW_ROUTE(app_, "/route/route_stops")
  ([](const crow::request &req) {
    char *stopid1 = req.url_params.get("stopid1");
    char *stopid2 = req.url_params.get("stopid2");
    char *datetime = req.url_params.get("datetime");

    if (!stopid1)
      return crow::response(400, "Missing 'stopid1' parameter");
    if (!stopid2)
      return crow::response(400, "Missing 'stopid2' parameter");
    if (!datetime)
      return crow::response(400, "Missing 'time' parameter");

    auto parsed_time = parse_time(datetime);
    Routing r(parsed_time);
    const RouteData route = r.output(stopid1, stopid2, parsed_time);
    return crow::response(make_default_response(route));
  });

  CROW_ROUTE(app_, "/route/route_coords")
  ([](const crow::request &req) {
    char *coords1_lat = req.url_params.get("coords1_lat");
    char *coords1_lon = req.url_params.get("coords1_lon");
    char *coords2_lat = req.url_params.get("coords2_lat");
    char *coords2_lon = req.url_params.get("coords2_lon");
    char *datetime = req.url_params.get("datetime");

    if (!coords1_lat)
      return crow::response(400, "Missing 'coords1_lat' parameter");
    if (!coords1_lon)
      return crow::response(400, "Missing 'coords1_lon' parameter");
    if (!coords2_lat)
      return crow::response(400, "Missing 'coords2_lat' parameter");
    if (!coords2_lon)
      return crow::response(400, "Missing 'coords2_lon' parameter");
    if (!datetime)
      return crow::response(400, "Missing 'time' parameter");


    // auto parsed_time = parse_time(datetime);
    // Routing r(parsed_time);
    // const route_data route = r.output(stopid1, stopid2, parsed_time);
    return crow::response();
  });

  CROW_ROUTE(app_, "/stops")
  ([](){

  });
}

crow::json::wvalue api::make_default_response(const route_data &data) {
  crow::json::wvalue result;
  result["start_stop"] = data.start_stop;
  result["end_stop"] = data.end_stop;
  result["start_time"] = data.start_time;
  result["end_time"] = data.end_time;
  std::vector<crow::json::wvalue> route_proper;
  route_proper.reserve(data.route_proper.size());

  for (const auto &node : data.route_proper) {
    crow::json::wvalue route_proper_node;
    route_proper_node["stop_id"] = node.stop_id;
    route_proper_node["stop_coords"] =
        std::vector{node.stop_coords.first, node.stop_coords.second};
    route_proper_node["stop_name"] = node.stop_name;
    route_proper_node["type"] = node.type;
    route_proper_node["route_name"] = node.route_name;
    route_proper_node["time"] = node.time;

    route_proper.push_back(std::move(route_proper_node));
  }
  result["route_proper"] = std::move(route_proper);
  return result;
}

std::chrono::zoned_time<std::chrono::seconds>
api::parse_time(const std::string &time_str) {
  std::istringstream ss{time_str};
  std::chrono::local_time<std::chrono::seconds> parsed_local;

  ss >> std::chrono::parse("%Y-%m-%d %H:%M:%S", parsed_local);
  return std::chrono::zoned_time{"Europe/Warsaw", parsed_local};
}

void api::run() { app_.port(18080).multithreaded().run(); }