#include "../include/stopfinder.h"
#include "config.h"

//NOTE:=================
//I'm increamnting the num by 1 bc the first elem of vector will alawyas be the node itslef
//=======================

//TO DO:===============
//- the convertion - I'm currently keeping it in degrees, but i want to change it to meteres - questions:
//- when so it's the fastest? // -are we sure wee need it? (proabbly for theasfty so yeah)
//=====================
StopFinder::StopFinder(const int& num) : number_of_stops(num+1), wrapper_tree_(2, ks) {
    generate_map();
    wrapper_tree_.buildIndex();
}

void StopFinder::generate_map() {
    Config::load();
    const std::string filled_query = std::vformat(def_query, std::make_format_args(number_of_stops));
    connection_.emplace(Config::connection_string);
    pqxx::work transaction{*connection_};
    for (const auto& [stopid, lan, lon] : transaction.query<std::string, float, float>(filled_query)) {
        coordinates_[stopid] = {lan, lon};
        StopData sd(stopid, lan, lon);
        ks.tree_vec_.push_back(sd);
    }
    transaction.commit();
}


//TO DO:===============
//- maybe you can do the vector even faster keep the number_foStops_vector inilisaized in the constructor and then you will just clena it? will that be faster or sis cleaning slow (im guessing its faster but later check it)
//=====================
std::vector<std::pair<std::string, float>> StopFinder::find_stop_ids(std::string stopid) {
    auto it = coordinates_.find(stopid);
    if (it == coordinates_.end())return {};
    auto [x, y] = it->second;
    float points[2] = {x, y};
    std::vector<size_t> out_indices(number_of_stops);
    std::vector<float> out_dist_sq(number_of_stops);

    wrapper_tree_.knnSearch(points, number_of_stops, out_indices.data(), out_dist_sq.data());

    std::vector<std::pair<std::string, float>> closest;
    closest.reserve(number_of_stops);

    for (size_t i=1; i < out_indices.size(); ++i) {
        closest.push_back(std::make_pair(ks.tree_vec_[out_indices[i]].stop_id, sqrt(out_dist_sq[i])));
    }
    return closest;
}

