#include "../include/stopfinder.h"

#include <iostream>

#include "config.h"

// NOTE:=================
// I'm increamnting the num by 1 bc the first elem of vector will alawyas be the
// node itslef
//=======================

// TO DO:===============
//- the convertion - I'm currently keeping it in degrees, but i want to change
//it to meteres - questions:
//- when so it's the fastest? // -are we sure wee need it? (proabbly for
//theasfty so yeah)
//=====================
StopData::StopData(std::string s, double p1, double p2) : stop_id(std::move(s)) {
  p1 *= M_PI / 180.0;
  p2 *= M_PI / 180.0;
  constexpr double ref_meridian = 0.366519;
  constexpr double E0 = 500;
  constexpr double k0 = 0.9996;
  constexpr double N0 = 0;

  constexpr double a = 6378.137;
  constexpr double f = 0.00335281066474781;
  constexpr double n = f/(2 - f);

  const double A = a*(1+ pow(n,2)/4 + pow(n,4)/64)/(1+n);

  const double alpha1 = 0.5*n - 2.0/3 * pow(n,2) + 5.0/16 * pow(n,3);
  const double alpha2 = 13.0/48 * pow(n,2) - 3.0/5 * pow(n,3);
  const double alpha3 = 61.0/240 * pow(n,3);

  const double t = sinh(atanh(sin(p1)) - 2*sqrt(n)/(1 + n) * atanh(2*sqrt(n)/(1 + n) * sin(p1)));
  const double xiprime = atan(t/cos(p2 - ref_meridian));
  const double etaprime = atanh(sin(p2 - ref_meridian)/sqrt(1 + t*t));

  x = E0 + k0*A*(etaprime + alpha1*cos(2*xiprime)*sinh(2*etaprime) + alpha2*cos(4*xiprime)*sinh(4*etaprime) +  alpha3*cos(6*xiprime)*sinh(6*etaprime));
  y = N0 + k0*A*(xiprime + alpha1*sin(2*xiprime)*cosh(2*etaprime) + alpha2*sin(4*xiprime)*cosh(4*etaprime) + alpha3*sin(6*xiprime)*cosh(6*etaprime));
}

StopFinder::StopFinder(const int &num)
    : number_of_stops(num + 1), wrapper_tree_(2, ks) {
  generate_map();
  wrapper_tree_.buildIndex();
}

void StopFinder::generate_map() {
  Config::load();
  const std::string filled_query =
      std::vformat(def_query, std::make_format_args(number_of_stops));
  connection_.emplace(Config::connection_string);
  pqxx::work transaction{*connection_};
  for (const auto &[stopid, lan, lon] :
       transaction.query<std::string, float, float>(filled_query)) {
    coordinates_[stopid] = {lan, lon};
    StopData sd(stopid, lan, lon);
    ks.tree_vec_.push_back(sd);
  }
  transaction.commit();
}

// TO DO:===============
//- maybe you can do the vector even faster keep the number_foStops_vector
//inilisaized in the constructor and then you will just clena it? will that be
//faster or sis cleaning slow (im guessing its faster but later check it)
//=====================
std::vector<std::pair<std::string, float>>
StopFinder::find_stop_ids(const std::basic_string<char> &stopid) {
  //std::cout << "we are isnide find stop ids, size: "<< coordinates_.size() << '\n';
  auto test = coordinates_.begin();
  std::cout << test->first << " " << test->second.first << " " << test->second.second << '\n';

  auto it = coordinates_.find(stopid);
  if (it == coordinates_.end())
    return {};
  std::cout << "we found the trip" << '\n';
  auto [x, y] = it->second;
  float points[2] = {x, y};
  std::vector<size_t> out_indices(number_of_stops);
  std::vector<float> out_dist_sq(number_of_stops);

  wrapper_tree_.knnSearch(points, number_of_stops, out_indices.data(),
                          out_dist_sq.data());

  std::vector<std::pair<std::string, float>> closest;
  closest.reserve(number_of_stops);

  for (size_t i = 1; i < out_indices.size(); ++i) {
    closest.emplace_back(ks.tree_vec_[out_indices[i]].stop_id,
                                     sqrt(out_dist_sq[i]));
  }
  return closest;
}
