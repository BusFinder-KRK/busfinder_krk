#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <pqxx/pqxx>
#include <nanoflann.hpp>

inline std::string def_query = R"(
                SELECT stop_id, stop_lat, stop_lon FROM stops
                LIMIT {}
                )";



struct StopData {
    std::string stop_id;
    float x;
    float y;
    StopData(std::string s, float p1, float p2) : stop_id(s), x(p1), y(p2) {};
};

class StopFinder {
public:
    struct KdtreeStruct
    {
        std::vector<StopData> tree_vec_;
        size_t kdtree_get_point_count() const {
            return tree_vec_.size();
        }
        float kdtree_get_pt(const size_t idx, int dim) const {
            if (dim == 0)return tree_vec_[idx].x;
            return tree_vec_[idx].y;
        }

        template <class BBOX>
        bool kdtree_get_bbox(BBOX &bb) const
        {
            return false;
        }
    };

    int number_of_stops;
    std::optional<pqxx::connection> connection_;

    std::unordered_map<std::string,std::pair<float, float>> coordinates_; //int GTFS file the lan and lon: 50.072490,20.028500
    KdtreeStruct ks;
    nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<float, KdtreeStruct>, KdtreeStruct, 2, size_t> wrapper_tree_;

    StopFinder(const int& num);
    void put_data(const int& num);
    void generate_map();

    //metohod to get you the closest n number of stops based on a stop name
    //returns paif - stop ID and distance between the stops
    std::vector<std::pair<std::string, float>> find_stop_ids(std::string stopid);

};
