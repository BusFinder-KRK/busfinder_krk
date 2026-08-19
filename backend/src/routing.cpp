#include <fstream>
#include <routing.h>
#include <queue>

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

Routing::Routing(std::chrono::time_point<std::chrono::system_clock> time) : sf_(adjacent_stops), ttable_(default_query, time) {
    std::cout << "inside the constructor" << '\n';
    ttable_.generate_table();
    std::cout << "after generate" << '\n';
    load_walking_csv();
}

void Routing::load_walking_csv() {
    //i didn't make any checking for debuging but i might add it later
    std::ifstream file(csv_path_);
    if (!file.is_open()) {
        std::cout << "couldnt open the csv" << '\n';
        return;
    }
    std::string line;
    std::stringstream ss;
    std::string stop1, stop2;
    float time_min;

    while (std::getline(file, line)) {
        if (!line.empty()) {
            ss.clear();
            ss.str(line);
            if (ss >> stop1 >> stop2 >> time_min) {
                    //LATER CHANGE THIS SO its a pair and not one string
                    std::string key = stop1 + "," + stop2;
                    int time_minutes = static_cast<int>(std::round(time_min));

                    walking_times_[key] = time_minutes;
                }
            }
        }

}

std::vector<Routing::NodeTransport> Routing::transport_between_stops( const std::string& stop_id1, const std::string& stop_id2, std::chrono::time_point<std::chrono::system_clock> time) {
    //std::cout << "we are in transport, size: " << ttable_.table_.size() << '\n';
    //auto test = ttable_.table_.begin();
    //std::cout << "first in ttable: " << test->first.first << " "<< test->first.second << '\n';
    //std::cout << "the stops: " << stop_id1 << " " << stop_id2 << '\n';


    std::vector<Routing::NodeTransport> result;
    auto local_time = std::chrono::zoned_time{std::chrono::current_zone(), time}.get_local_time();
    std::string houradmin = std::format("{:%H:%M}", local_time);
    auto midnight = std::chrono::floor<std::chrono::days>(time);

    auto it = ttable_.table_.find({stop_id1, stop_id2});
    if (it == ttable_.table_.end()) {
        std::cout << "no stop_id1 to stop_id2 in the ttable" << '\n';
        return result;
    }
    const auto& possible_trips = it->second;
    std::cout << "trip found" << '\n';
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
            //std::cout << "node: " << nt.line_id << " " << std::format("{:%T}", std::chrono::floor<std::chrono::seconds>(nt.departure)) << " "
          //<< std::format("{:%T}", std::chrono::floor<std::chrono::seconds>(nt.arrival)) << '\n';
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

std::vector<std::pair<Routing::Node, Routing::NodeTimeInfo>> Routing::dijkstra_dalekowzrocznosc(const std::string &start, const std::string &target,
                                        std::chrono::time_point<std::chrono::system_clock> time)  {
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
    record_of_distances[start_node] = start_node_time;
    std::priority_queue<
        std::pair<float, Node>,
        std::vector<std::pair<float, Node> >,
        std::greater<std::pair<float, Node> >
    > pq;
    pq.emplace(0, start_node);
    Node ending_node;

    //TO DO: =================
    //- check if i can call something after something else exists to save time
    //========================
    std::cout << "before the pq" << '\n';
    while (!pq.empty()) {
        auto [old_cost, old_node] = pq.top();
        pq.pop();
        std::cout << "hello??? size of the pq: " << pq.size() << '\n';
        std::cout << old_cost << " " << old_node.stop_name << " " << old_node.line_id << '\n';
        if (old_node.stop_name == target) {
            ending_node = old_node;
            break;
        }
        if (old_cost > record_of_distances[old_node].cost_from_start) continue;
        float time_to_arrive;
        float add_cost;

        for (const auto &[new_stop, distance]: sf_.find_stop_ids(old_node.stop_name)) {
            std::cout << new_stop << " and " << old_node.stop_name << '\n';
            if (new_stop != old_node.stop_name) {
                std::vector<NodeTransport>result = transport_between_stops(old_node.stop_name, new_stop, record_of_distances[old_node].real_time);
                if (result.empty()) {
                    result = walking_between_stops(old_node.stop_name, new_stop, record_of_distances[old_node].real_time);
                    if (result.empty()) {
                        //safty net - zeby graf sie nie rozspujnil
                        time_to_arrive = std::ceil(distance / this->walking_pace);
                        add_cost = time_to_arrive*5;//huge punishemnd so the algo is scared of this option, counted in minutes
                        std::string line_id = "walk - safety net";
                        std::chrono::time_point<std::chrono::system_clock> arrival_at_B = record_of_distances[old_node].real_time + std::chrono::minutes(static_cast<int>(time_to_arrive));
                        NodeTransport nt{line_id, record_of_distances[old_node].real_time, arrival_at_B};
                        result.push_back(nt);
                    }
                }
                for (const auto& node : result) {
                    if (node.line_id != "walk - safety net") {
                        auto delta = node.arrival - record_of_distances[old_node].real_time;
                        auto total_seconds = std::chrono::duration_cast<std::chrono::seconds>(delta).count();
                        time_to_arrive = total_seconds / 60.0f;
                        add_cost = time_to_arrive;
                    }
                    //this will be usefull if we will want to punish for walking:
                    if (node.line_id == "walk")add_cost *= walking_multiplier;
                    if (old_node.line_id !="walk" and old_node.line_id != "walk - safety net") {
                        if (old_node.line_id != node.line_id)add_cost += this->time_for_change;
                    }

                    Node n{new_stop, node.line_id};
                    std::cout << "the new node: " << n.line_id << " " << n.stop_name << " " << old_cost + add_cost << " " << node.arrival << '\n';

                    if (!record_of_distances.contains(n)) {
                        std::cout << "new node(stop+line)" << '\n';
                        NodeTimeInfo nt {old_cost + add_cost, node.arrival};
                        record_of_distances[n] = nt;
                        //std::cout << old_cost + time_to_arrive << " "<<n.line_id << " " << n.stop_name << '\n';
                        pq.emplace(old_cost + add_cost, n);
                        prev[n] = old_node;
                    }
                    else {
                        if (record_of_distances[n].cost_from_start > old_cost + add_cost) {
                            std::cout << "better cost:]" << '\n';
                            NodeTimeInfo nt {old_cost + add_cost, node.arrival};
                            record_of_distances[n] = nt;
                            pq.emplace(old_cost + add_cost, n);
                            prev[n] = old_node;
                        }
                    }
                    std::cout << "=================" << '\n';
                }

            }
        }
    }
    std::cout << "after pq" << '\n';

    if (ending_node.stop_name.empty()) {
        std::cout << "I fucked up" << '\n';
        return {};

    }

    //teraz otwarzamy droge od tyłu:
    std::vector<Node> help;
    help.push_back(ending_node);
    //while (help[help.size()-1].stop_name != start) help.push_back(prev[help[help.size()-1]]);
    while (!help.empty() && help.back().stop_name != start) {
        if (!prev.contains(help.back()))break;
        help.push_back((prev[help.back()]));
    }
    //LATER GET RID OF THIS:
    std::reverse(help.begin(), help.end());
    std::vector<std::pair<Node, NodeTimeInfo>> ans;
    for (const auto& node : help) {
        std::cout << node.line_id << " " << node.stop_name << " " << std::format("{:%T}", record_of_distances[node].real_time) << " " << record_of_distances[node].cost_from_start << '\n';
        //TO JEST DO ZMIANY W ZALEZNOSCI OD TEGO JAKIE INFO CHCEMY! i chyba pasuje jednak to dego NodeTimeInfo dodac nazwe liniji bo innaczje bedzie pokazywac wczesniejsza i belive
        ans.emplace_back(node, record_of_distances[node]);
    }
    return ans;
}


std::vector<std::pair<Routing::Node, Routing::NodeTimeInfo>> Routing::dijkstra_dalekowzrocznosc_reversed(const std::string &start, const std::string &target,
                                        std::chrono::time_point<std::chrono::system_clock> time) {
    Node start_node{start, "walk"};
    NodeTimeInfo start_node_time{0, time};
    std::map<Node, NodeTimeInfo> record_of_distances;
    std::map<Node, Node> prev;
    record_of_distances[start_node] = start_node_time;
    std::priority_queue<
        std::pair<int, Node>,
        std::vector<std::pair<int, Node> >,
        std::greater<std::pair<int, Node> >
    > pq;
    pq.emplace(0, start_node);
    Node ending_node;

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
        if (old_cost > record_of_distances[old_node].cost_from_start) continue;
        //StopFinder sf{adjacent_stops}; we moved it upward to be faster
        int time_to_arrive;
        for (const auto &[new_stop, distance]: sf_.find_stop_ids(old_node.stop_name)) {
            if (new_stop != old_node.stop_name) {
                ///change here with transport_between_stops_reverse
                std::vector<NodeTransport>result = transport_between_stops_reverse( new_stop, old_node.stop_name, record_of_distances[old_node].real_time);
                if (result.empty()) {
                    ///change here with walking_between_stops_reverse
                    result = walking_between_stops_reverse( new_stop, old_node.stop_name, record_of_distances[old_node].real_time);
                    if (result.empty()) {
                        //safty net - zeby graf sie nie rozspujnil
                        int dist_meters = distance * 1609.344;
                        time_to_arrive = std::ceil(dist_meters / this->walking_pace) * 5; //huge punishemnd so the algo is scared of this option, counted in minutes
                        std::string line_id = "walk - safety net";
                        ///minus over here:
                        std::chrono::time_point<std::chrono::system_clock> arrival_at_B = record_of_distances[old_node].real_time - std::chrono::minutes(time_to_arrive);
                        NodeTransport nt{line_id, record_of_distances[old_node].real_time, arrival_at_B};
                        result.push_back(nt);
                    }
                }
                for (const auto& node : result) {
                    if (node.line_id != "walk - safety net") {
                        auto delta =  record_of_distances[old_node].real_time - node.departure;
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
                        pq.emplace(old_cost + time_to_arrive, n);
                        prev[n] = old_node;
                    }
                    else {
                        if (record_of_distances[n].cost_from_start > old_cost + time_to_arrive) {
                            NodeTimeInfo nt {old_cost + time_to_arrive, node.arrival};
                            record_of_distances[n] = nt;
                            pq.emplace(old_cost + time_to_arrive, n);
                            prev[n] = old_node;
                        }
                    }
                }

            }
        }
    }
    if (ending_node.stop_name.empty()) {
        return {};
    }

    //teraz otwarzamy droge od tyłu:
    std::vector<Node> help;
    help.push_back(ending_node);
    //while (help[help.size()-1].stop_name != start) help.push_back(prev[help[help.size()-1]]);
    while (!help.empty() && help.back().stop_name != start) {
        if (!prev.contains(help.back()))break;
        help.push_back((prev[help.back()]));
    }
    std::reverse(help.begin(), help.end());
    std::vector<std::pair<Node, NodeTimeInfo>> ans;
    for (const auto& node : help) {
        std::cout << node.line_id << " " << node.stop_name << " " << std::format("{:%T}", record_of_distances[node].real_time) << " " << record_of_distances[node].cost_from_start << '\n';
        //TO JEST DO ZMIANY W ZALEZNOSCI OD TEGO JAKIE INFO CHCEMY! i chyba pasuje jednak to dego NodeTimeInfo dodac nazwe liniji bo innaczje bedzie pokazywac wczesniejsza i belive
        ans.emplace_back(node, record_of_distances[node]);
    }
    return ans;
}

//TO DO: ===================
//- how to format this thing?
//==========================
std::vector<std::pair<Routing::Node, Routing::NodeTimeInfo>> Routing::output(const std::string &start, const std::string &target, std::chrono::time_point<std::chrono::system_clock> time) {
    std::vector<std::pair<Routing::Node, Routing::NodeTimeInfo>> arrival = dijkstra_dalekowzrocznosc(start, target, time);
    return arrival;
    /*//now how to we get an arrival time from dijsktra's arrival vector?
    std::chrono::time_point<std::chrono::system_clock> arr_time = arrival[0].second.real_time; //= arrival[cos tam cos tam]?
    std::vector<std::pair<Routing::Node, Routing::NodeTimeInfo>> answer = dijkstra_dalekowzrocznosc(target, start, arr_time);
    //now we should probably format something?
    return answer;*/
}

