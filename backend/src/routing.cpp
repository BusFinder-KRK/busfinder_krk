#include <fstream>
#include <routing.h>
#include <stopfinder.h>
#include <queue>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
//==================================================
//TO DO FOR OVERREAL PERFOMANCE
//maybe later switch the line ID to be and integer since ints are faster: for exaple instead of doing "walk" do a -1 or sth
//==================================================

//for Node struct
bool operator < (const Routing::Node& n1, const Routing::Node& n2) {
    if (n1.stop_name < n2.stop_name)return true;
    if(n1.stop_name == n2.stop_name && n1.line_id < n2.line_id) return true;
    return false;
}

//for the routing:

Routing::Routing() {
    load_walking_json();
}

void Routing::load_walking_json() {
    //i didnt make any checking for debuging but i might add it later
    std::ifstream file(json_path);
    if (file.is_open()) {
        json j;
        file >> j;
        walking_times_ = j.get<std::unordered_map<std::string, int>>();
    }
}


std::vector<std::pair<Routing::Node, Routing::NodeTimeInfo>> Routing::dijkstra_dalekowzrocznosc(const std::string &start, const std::string &target,
                                        std::chrono::time_point<std::chrono::system_clock> time) {
    /* PLAN:
     * - record of distances - the table so far, so its easy to look up the nodes
     * - prev - table with prevous nodes
     * - pq - the prority queue
     *
     * NODE:
     * [ - Bus stop name ("Kapelanka"),
     * - ID name of the line / or "wall" ]
     *
     *NODE_TIME_INFO
     *[time from start of the journy, time the bus takes off]
     *
     *NODE_TRANSPORT_INFO
     *bus_name (std::string)
     *departure
     *arrival
     *
     *  ROCORD OF DISTACES = MAP:
     *  [NODE] = [time from start of the journy, time the bus takes off, line ID]
     *  [NODE] = [NODE_TIME_INFO]
     *
     *  PQ
     *  [COST, NODE]
     *
     *  PREV
     *  [NODE, NODE]
     */
    Node start_node{start, "walk"};
    NodeTimeInfo start_node_time{0, time};
    std::map<Node, NodeTimeInfo> record_of_distances;
    std::map<Node, Node> prev;
    std::priority_queue<
        std::pair<int, Node>,
        std::vector<std::pair<int, Node> >,
        std::greater<std::pair<int, Node> >
    > pq;
    pq.push(std::make_pair(0, start_node));
    Node ending_node;
    StopFinder sf{adjacent_stops};
    //TO DO: =================
    //- check if i can call something after something else exists to save time
    //========================

    while (!pq.empty()) {
        auto [old_cost, old_node] = pq.top();
        pq.pop();

        if (old_node.stop_name == target) {
            ending_node = old_node;
            break;
        }
        if (old_cost > record_of_distances[old_node].time_from_start) continue;
        //StopFinder sf{adjacent_stops}; we moved it upward to be faster
        int time_to_arrive;
        for (const auto &[new_stop, distance]: sf.find_stop_ids(old_node.stop_name)) {
            if (new_stop != old_node.stop_name) {
                std::vector<NodeTransport>result = transport_between_stops(old_node.stop_name, new_stop, record_of_distances[old_node].real_time);
                if (result.empty()) {
                    result = walking_between_stops(old_node.stop_name, new_stop, record_of_distances[old_node].real_time);
                    if (result.empty()) {
                        //safty net - zeby graf sie nie rozspujnil
                        int dist_meters = distance * 1609.344;
                        time_to_arrive = std::ceil(dist_meters / this->walking_pace) * 5 //huge punishemnd so the algo is scared of this option, counted in minutes
                        std::string line_id = "walk - safety net";
                        std::chrono::time_point<std::chrono::system_clock> arrival_at_B = record_of_distances[old_node].real_time + std::chrono::minutes(time_to_arrive_mins);
                        NodeTransport nt{line_id, record_of_distances[old_node].real_time, arrival_at_B};
                        result.push_back(nt);
                    }
                }
                for (const auto& node : result) {
                    if (node.line_id != "walk - safety net") {
                        auto delta = node.arrival - record_of_distances[old_node].real_time;
                        auto total_seconds = std::chrono::duration_cast<std::chrono::seconds>(delta).count();
                        time_to_arrive = static_cast<int>(std::ceil(total_seconds / 60.0f));
                    }
                    //this will be usefull if we will want to punish for walking:
                    if (node.line_id == "walk")time_to_arrive *= walking_multiplier;
                    if (old_node.line_id !="walk" and old_node.line_id != "walk - safety net")time_to_arrive += this->time_for_change;

                    Node n{new_stop, node.line_id};
                    if (!record_of_distances.contains(n)) {
                        NodeTimeInfo nt {old_cost + time_to_arrive, node.arrival};
                        record_of_distances[n] = nt;
                        pq.push(std::make_pair(old_cost + time_to_arrive, n));
                        prev[n] = old_node;
                    }
                    else {
                        if (record_of_distances[n].time_from_start > old_cost + time_to_arrive) {
                            NodeTimeInfo nt {old_cost + time_to_arrive, node.arrival};
                            record_of_distances[n] = nt;
                            pq.push(std::make_pair(old_cost + time_to_arrive, n));
                            prev[n] = old_node;
                        }
                    }
                }

            }
        }
    }
    //teraz otwarzamy droge od tyłu:
    std::vector<Node> help;
    help.push_back(ending_node);
    while (help[help.size()-1].stop_name != start) help.push_back(prev[help[help.size()-1]]);\

    std::vector<std::pair<Node, NodeTimeInfo>> ans;
    for (const auto& node : help) {
        //TO JEST DO ZMIANY W ZALEZNOSCI OD TEGO JAKIE INFO CHCEMY! i chyba pasuje jednak to dego NodeTimeInfo dodac nazwe liniji bo innaczje bedzie pokazywac wczesniejsza i belive
        ans.push_back(std::make_pair(node, record_of_distances[node]));
    }
    return ans;
}

std::vector<Routing::NodeTransport> Routing::transport_between_stops( const std::string& stop_id1, const std::string& stop_id2, std::chrono::time_point<std::chrono::system_clock> time) {
    std::vector<Routing::NodeTransport> result;
    std::string houradmin = std::format("{:%T}", time);
    auto midnight = std::chrono::floor<std::chrono::days>(time);

    auto it = ttable_.table_.find({stop_id1, stop_id2});
    if (it == ttable_.table_.end()) {
        return result;
    }
    const auto& possible_trips = it->second;

    for (const auto& option : possible_trips) {
        if (option.time_stop1 >= houradmin) {
            int h = std::stoi(option.time_stop1.substr(0,2));
            int m = std::stoi(option.time_stop1.substr(3,2));
            int minutes_1 = h*60 + m;
            h = std::stoi(option.time_stop2.substr(0,2));
            m = std::stoi(option.time_stop2.substr(3,2));
            int minutes_2 = h*60 + m;
            auto t1 = midnight + std::chrono::minutes(minutes_1);
            auto t2 = midnight + std::chrono::minutes(minutes_2);
            NodeTransport nt {option.trip_id, t1, t2};
            result.push_back(nt);
            if (t1 > time + std::chrono::minutes(search_window))break;
        }
    }
    return result;
}

std::vector<Routing::NodeTransport> Routing::transport_between_stops_reverse(const std::string& stop_id1, const std::string& stop_id2, std::chrono::time_point<std::chrono::system_clock> time) {
    std::vector<Routing::NodeTransport> result;
    std::string houradmin = std::format("{:%T}", time);
    auto midnight = std::chrono::floor<std::chrono::days>(time);

    auto it = ttable_.table_.find({stop_id1, stop_id2});
    if (it == ttable_.table_.end()) {
        return result;
    }
    const auto& possible_trips = it->second;

    for (const auto& option : std::views::reverse(possible_trips)) {
        if (option.time_stop2 <= houradmin) {
            int h = std::stoi(option.time_stop1.substr(0,2));
            int m = std::stoi(option.time_stop1.substr(3,2));
            int minutes_1 = h*60 + m;
            h = std::stoi(option.time_stop2.substr(0,2));
            m = std::stoi(option.time_stop2.substr(3,2));
            int minutes_2 = h*60 + m;
            auto t1 = midnight + std::chrono::minutes(minutes_1);
            auto t2 = midnight + std::chrono::minutes(minutes_2);
            NodeTransport nt {option.trip_id, t1, t2};
            result.push_back(nt);
            if (t1 < time - std::chrono::minutes(search_window))break;
        }
    }
    return result;
}

//==================================================
//TO DO WITH WALKING (when i will want to maximize for spped:
//do not allocate with vector but just return std::optional<NodeTransport> !
//do not concate the string - just either change the json or just change the approach when loading into RAM? not
//sure about this tho?
//use std pair string string as key in map
//maybe refactor this code into one bit so i dont have to repeat myslef?
//==================================================
std::vector<Routing::NodeTransport> Routing::walking_between_stops( const std::string& stop_id1, const std::string& stop_id2, std::chrono::time_point<std::chrono::system_clock> time) {
    std::vector<Routing::NodeTransport> vec;
    std::string str1 = stop_id1 + "," + stop_id2;

    if (auto it = walking_times_.find(str1); it != walking_times_.end()) {
        std::chrono::time_point<std::chrono::system_clock> t2 = time + std::chrono::minutes(it->second);
        NodeTransport nt{"walk", time, t2};
        vec.push_back(nt);
        return vec;
    }

    std::string str2 = stop_id2 + "," + stop_id1;
    if (auto it = walking_times_.find(str2); it != walking_times_.end()) {
        std::chrono::time_point<std::chrono::system_clock> t2 = time + std::chrono::minutes(it->second);
        NodeTransport nt{"walk", time, t2};
        vec.push_back(nt);
        return vec;
    }
    return vec;
}

std::vector<Routing::NodeTransport> Routing::walking_between_stops_reverse( const std::string& stop_id1, const std::string& stop_id2, std::chrono::time_point<std::chrono::system_clock> time) {
    std::vector<Routing::NodeTransport> vec;
    std::string str1 = stop_id1 + "," + stop_id2;

    if (auto it = walking_times_.find(str1); it != walking_times_.end()) {
        std::chrono::time_point<std::chrono::system_clock> t1 = time - std::chrono::minutes(it->second);
        NodeTransport nt{"walk", t1, time};
        vec.push_back(nt);
        return vec;
    }

    std::string str2 = stop_id2 + "," + stop_id1;
    if (auto it = walking_times_.find(str2); it != walking_times_.end()) {
        std::chrono::time_point<std::chrono::system_clock> t1 = time - std::chrono::minutes(it->second);
        NodeTransport nt{"walk", t1, time};
        vec.push_back(nt);
        return vec;
    }
    return vec;
}
