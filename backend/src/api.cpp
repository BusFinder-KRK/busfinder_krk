#include <api.h>
#include <crow.h>
#include <route_data.h>

api::api() {
  CROW_ROUTE(app_, "/route/route_stops")
  ([](const crow::request &req) {
    char *stopid1 = req.url_params.get("stopid1");
    char *stopid2 = req.url_params.get("stopid2");
    char *time = req.url_params.get("time");

    if (!stopid1)
      return crow::response(400, "Missing 'stopid1' parameter");
    if (!stopid2)
      return crow::response(400, "Missing 'stopid2' parameter");
    if (!time)
      return crow::response(400, "Missing 'time' parameter");

    const route_data route;
    return crow::response(make_default_response(route));
  });

  CROW_ROUTE(app_, "/route/route_coords")
  ([](const crow::request &req) {
    char *coords1_lat = req.url_params.get("coords1_lat");
    char *coords1_lon = req.url_params.get("coords1_lon");
    char *coords2_lat = req.url_params.get("coords2_lat");
    char *coords2_lon = req.url_params.get("coords2_lon");
    char *time = req.url_params.get("time");

    if (!coords1_lat)
      return crow::response(400, "Missing 'coords1_lat' parameter");
    if (!coords1_lon)
      return crow::response(400, "Missing 'coords1_lon' parameter");
    if (!coords2_lat)
      return crow::response(400, "Missing 'coords2_lat' parameter");
    if (!coords2_lon)
      return crow::response(400, "Missing 'coords2_lon' parameter");
    if (!time)
      return crow::response(400, "Missing 'time' parameter");

    const route_data route;
    return crow::response(make_default_response(route));
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

void api::run() { app_.port(18080).multithreaded().run(); }