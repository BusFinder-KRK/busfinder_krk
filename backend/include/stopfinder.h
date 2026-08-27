#pragma once

#include <nanoflann.hpp>
#include <pqxx/pqxx>
#include <string>
#include <unordered_map>
#include <vector>

inline std::string test_query = R"(
                SELECT stop_id, stop_lat, stop_lon FROM stops
                )";

struct StopData {
  std::string stop_id;
  double x;
  double y;
  StopData(std::string s, double p1, double p2);
};

class StopFinder {
public:
  struct KdtreeStruct {
    std::vector<StopData> tree_vec_;
    [[nodiscard]] size_t kdtree_get_point_count() const {
      return tree_vec_.size();
    }
    [[nodiscard]] double kdtree_get_pt(const size_t idx, int dim) const {
      if (dim == 0)
        return tree_vec_[idx].x;
      return tree_vec_[idx].y;
    }

    template <class BBOX> static bool kdtree_get_bbox(BBOX &bb) {
      return false;
    }
  };

  int number_of_stops;
  std::optional<pqxx::connection> connection_;

  std::unordered_map<std::string, std::pair<double, double>>
      coordinates_; // int GTFS file the lan and lon: 50.072490,20.028500
  KdtreeStruct ks;
  nanoflann::KDTreeSingleIndexAdaptor<
      nanoflann::L2_Simple_Adaptor<double, KdtreeStruct>, KdtreeStruct, 2,
      size_t>
      wrapper_tree_;

  StopFinder(const int &num);
  void generate_map();
  std::pair<double, double> get_coords(const std::string &busstop) const;

  // metohod to get you the closest n number of stops based on a stop name
  // returns paif - stop ID and distance between the stops
  std::vector<std::pair<std::string, double>>
  find_stop_ids(const std::string &stopid);

  // private: do - it later
};
