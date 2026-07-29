#include <vector>
#include <pair>
struct

class StopFinder {
public:
  int number_of_stops;
  std::vector<std::pair<std::string, float>> find_stop_ids(std::string); //metohod to get you the closest n number of stops based on a stop name
  //returns paif - stop ID and distance between the stops
  std::pair<float, float> get_cordinates(std::string);// get the cords of a bus stop ID;

};