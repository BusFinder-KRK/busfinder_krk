#include <fstream>
#include <routing.h>
#include <queue>
#include <routing.h>

////MAKE SURE THAT DEPARTURE AND ARRIVAL IN REVERESED IS GOOD
//shoudl i keep the continue on walk walk?
//maybe get rid of the safty net punishemtn??

//==================================================
// TO DO FOR OVERREAL PERFOMANCE
// maybe later switch the line ID to be and integer since ints are faster: for
// exaple instead of doing "walk" do a -1 or sth
//==================================================

// for Node struct
bool operator<(const Routing::Node &n1, const Routing::Node &n2) {
    if (n1.stop_name < n2.stop_name)
        return true;
    if (n1.stop_name == n2.stop_name && n1.line_id < n2.line_id)
        return true;
    return false;
}

// for the routing:

Routing::Routing(std::chrono::zoned_time<std::chrono::seconds> time)
    : sf_(adjacent_stops), ttable_(default_query, time.get_sys_time()),translator_() {
    std::cout << "we are in routing constructor" << '\n';
    ttable_.generate_table();
    std::cout << "the table was generated" << '\n';
    load_walking_csv();
    std::cout << "the csv was loaded" << '\n';

}

void Routing::load_walking_csv() {
    std::ifstream file(csv_path_);
    if (!file.is_open()) {
        std::cout << "couldnt open the csv" << '\n';
        return;
    }
    std::string line;
    std::stringstream ss;
    std::string stop1, stop2;
    double time_min;

    while (file >> stop1 >> stop2 >> time_min) {
        walking_times_[std::make_pair(stop1, stop2)] = time_min;
    }
}

std::vector<Routing::NodeTransport> Routing::transport_between_stops(
    const std::string &stop_id1, const std::string &stop_id2,
    std::chrono::zoned_time<std::chrono::seconds> time) {

    auto local_timepoint = time.get_local_time();
    std::vector<Routing::NodeTransport> result;
    std::string houradmin = std::format("{:%H:%M}", local_timepoint);

    auto midnight = std::chrono::floor<std::chrono::days>(local_timepoint);

    auto it = ttable_.table_.find({stop_id1, stop_id2});
    if (it == ttable_.table_.end()) {
        return result;
    }
    const auto &possible_trips = it->second;
    for (const auto &option: possible_trips) {
        if (option.time_stop1 >= houradmin) {
            int h = std::stoi(option.time_stop1.substr(0, 2));
            int m = std::stoi(option.time_stop1.substr(3, 2));
            int minutes_1 = h * 60 + m;
            h = std::stoi(option.time_stop2.substr(0, 2));
            m = std::stoi(option.time_stop2.substr(3, 2));
            int minutes_2 = h * 60 + m;
            auto t1 = midnight + std::chrono::minutes(minutes_1);
            auto t2 = midnight + std::chrono::minutes(minutes_2);
            NodeTransport nt{
                option.trip_id,
                std::chrono::zoned_time{time.get_time_zone(), t1},
                std::chrono::zoned_time{time.get_time_zone(), t2}
            };
            result.push_back(nt);
            if (t1 > local_timepoint + std::chrono::minutes(search_window))
                break;
        }
    }
    return result;
}

std::vector<Routing::NodeTransport> Routing::transport_between_stops_reverse(
    const std::string &stop_id1, const std::string &stop_id2,
    std::chrono::zoned_time<std::chrono::seconds> time) {
    auto local_timepoint = time.get_local_time();
    std::vector<Routing::NodeTransport> result;
    std::string houradmin = std::format("{:%H:%M}", local_timepoint);
    auto midnight = std::chrono::floor<std::chrono::days>(local_timepoint);

    auto it = ttable_.table_.find({stop_id2, stop_id1});
    if (it == ttable_.table_.end()) {
        return result;
    }
    const auto &possible_trips = it->second;

    for (const auto &option: std::views::reverse(possible_trips)) {
        if (option.time_stop2 <= houradmin) {
            int h = std::stoi(option.time_stop1.substr(0, 2));
            int m = std::stoi(option.time_stop1.substr(3, 2));
            int minutes_1 = h * 60 + m;
            h = std::stoi(option.time_stop2.substr(0, 2));
            m = std::stoi(option.time_stop2.substr(3, 2));
            int minutes_2 = h * 60 + m;
            auto t1 = midnight + std::chrono::minutes(minutes_1);
            auto t2 = midnight + std::chrono::minutes(minutes_2);
            NodeTransport nt{
                option.trip_id,
                std::chrono::zoned_time{time.get_time_zone(), t1},
                std::chrono::zoned_time{time.get_time_zone(), t2}
            };
            result.push_back(nt);
            if (t1 < local_timepoint - std::chrono::minutes(search_window))
                break;
        }
    }
    return result;
}

std::vector<Routing::NodeTransport> Routing::walking_between_stops(
    const std::string &stop_id1, const std::string &stop_id2,
    std::chrono::zoned_time<std::chrono::seconds> time) {
    auto local_timepoint = time.get_local_time();
    std::vector<Routing::NodeTransport> vec;

    if (auto it = walking_times_.find(std::make_pair(stop_id1, stop_id2)); it != walking_times_.end()) {
        auto walk_duration = std::chrono::ceil<std::chrono::minutes>(std::chrono::duration<double, std::ratio<60>>(it->second));
        std::chrono::zoned_time<std::chrono::seconds> t2{
            time.get_time_zone(),
            local_timepoint + walk_duration
        };
        NodeTransport nt{"walk", time, t2};
        vec.push_back(nt);
        return vec;
    }

    if (auto it = walking_times_.find(std::make_pair(stop_id2, stop_id1)); it != walking_times_.end()) {
        auto walk_duration = std::chrono::ceil<std::chrono::minutes>(std::chrono::duration<double, std::ratio<60>>(it->second));
        std::chrono::zoned_time<std::chrono::seconds> t2{
            time.get_time_zone(),
            local_timepoint + walk_duration
        };
        NodeTransport nt{"walk", time, t2};
        vec.push_back(nt);
        return vec;
    }
    return vec;
}

std::vector<Routing::NodeTransport> Routing::walking_between_stops_reverse(
    const std::string &stop_id1, const std::string &stop_id2,
    std::chrono::zoned_time<std::chrono::seconds> time) {
    auto local_timepoint = time.get_local_time();
    std::vector<Routing::NodeTransport> vec;

    if (auto it = walking_times_.find(std::make_pair(stop_id1, stop_id2)); it != walking_times_.end()) {
        auto walk_duration = std::chrono::ceil<std::chrono::minutes>(std::chrono::duration<double, std::ratio<60>>(it->second));
        std::chrono::zoned_time<std::chrono::seconds> t1{
            time.get_time_zone(),
            local_timepoint - walk_duration
        };
        NodeTransport nt{"walk", time, t1};
        vec.push_back(nt);
        return vec;
    }

    if (auto it = walking_times_.find(std::make_pair(stop_id2, stop_id1)); it != walking_times_.end()) {
        auto walk_duration = std::chrono::ceil<std::chrono::minutes>(std::chrono::duration<double, std::ratio<60>>(it->second));
        std::chrono::zoned_time<std::chrono::seconds> t1{
            time.get_time_zone(),
            local_timepoint - walk_duration
        };
        NodeTransport nt{"walk", time, t1};
        vec.push_back(nt);
        return vec;
    }
    return vec;
}

std::vector<std::pair<Routing::Node, std::pair<Routing::NodeTimeInfo, Routing::NodeTransport>>> Routing::dijkstra_dalekowzrocznosc( const std::string &start, const std::string &target, std::chrono::zoned_time<std::chrono::seconds> time) {
    std::cout<< "we are inisde dijkstra" << '\n';
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
     *  ROCORD OF DISTACES = MAP: //change should i add nodetraposrt here too?
     *  [NODE] = [NODE_TIME_INFO]
     *
     *  PQ
     *  [COST, NODE]
     *
     *  PREV
     *  [NODE, NODE]
     */
    Node start_node{start, "start"};
    NodeTimeInfo start_node_time{0, time};
    NodeTransport start_node_trans{"start", time, time};
    std::map<Node, std::pair<NodeTimeInfo, NodeTransport>> record_of_distances;
    std::map<Node, Node> prev;
    record_of_distances[start_node] = std::make_pair(start_node_time,start_node_trans);
    std::priority_queue<std::pair<float, Node>,
                std::vector<std::pair<float, Node> >,
                std::greater<std::pair<float, Node> > >
            pq;
    pq.emplace(0, start_node);
    Node ending_node;

    while (!pq.empty()) {
        auto [old_cost, old_node] = pq.top();
        pq.pop();
        if (old_node.stop_name == target) {
            ending_node = old_node;
            break;
        }
        if (old_cost > record_of_distances[old_node].first.cost_from_start)
            continue;
        float time_to_arrive;
        float add_cost;

        for (const auto &[new_stop, distance]:
             sf_.find_stop_ids(old_node.stop_name)) {
            if (new_stop != old_node.stop_name) {
                std::vector<NodeTransport> result =
                        transport_between_stops(old_node.stop_name, new_stop,
                                                record_of_distances[old_node].first.real_time);
                if (result.empty()) {
                    result =
                            walking_between_stops(old_node.stop_name, new_stop,
                                                  record_of_distances[old_node].first.real_time);
                    if (result.empty()) {
                        // safty net - zeby graf sie nie rozspujnil
                        time_to_arrive = std::ceil(distance / this->walking_pace);
                        add_cost =
                                time_to_arrive * 5; // huge punishemnd so the algo is scared of
                        // this option, counted in minutes
                        std::string line_id = "walk - safety net";
                        std::chrono::zoned_time<std::chrono::seconds> arrival_at_B{
                            time.get_time_zone(),
                            record_of_distances[old_node].first.real_time.get_sys_time() +
                            std::chrono::minutes(static_cast<int>(time_to_arrive))
                        };
                        NodeTransport nt{
                            line_id, record_of_distances[old_node].first.real_time,
                            arrival_at_B
                        };
                        result.push_back(nt);
                    }
                }
                for (const auto &nodetrans: result) {
                    if (nodetrans.line_id != "walk - safety net") {
                        auto delta =  nodetrans.arrival.get_local_time() - record_of_distances[old_node].first.real_time.get_local_time();
                        auto total_seconds =
                                std::chrono::duration_cast<std::chrono::seconds>(delta).count();
                        time_to_arrive = total_seconds / 60.0f;
                        add_cost = time_to_arrive;
                    }
                    // this will be usefull if we will want to punish for walking:
                    if (nodetrans.line_id == "walk")
                        add_cost *= walking_multiplier;

                    bool is_node_not_walk = (nodetrans.line_id != "walk" && nodetrans.line_id != "walk - safety net");
                    bool is_old_node_not_walk = (old_node.line_id != "walk" && old_node.line_id != "walk - safety net");

                    if (is_node_not_walk && is_old_node_not_walk && old_node.line_id != nodetrans.line_id) add_cost += this->time_for_change;

                    Node n{new_stop, nodetrans.line_id};
                    //std::cout << "the new node: " << n.line_id << " " << n.stop_name << " " << old_cost + add_cost << " " << node.arrival << '\n';

                    if (!record_of_distances.contains(n)) {
                        NodeTimeInfo nt{old_cost + add_cost, nodetrans.arrival};
                        record_of_distances[n] = std::make_pair(nt, nodetrans);
                        // std::cout << old_cost + time_to_arrive << " "<<n.line_id << " "
                        // << n.stop_name << '\n';
                        pq.emplace(old_cost + add_cost, n);
                        prev[n] = old_node;
                    } else {
                        if (record_of_distances[n].first.cost_from_start > old_cost + add_cost) {
                            NodeTimeInfo nt{old_cost + add_cost, nodetrans.arrival};
                            record_of_distances[n] = std::make_pair(nt, nodetrans);
                            pq.emplace(old_cost + add_cost, n);
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

    std::vector<Node> help;
    help.push_back(ending_node);

    while (!help.empty() && help.back().stop_name != start) {
        if (!prev.contains(help.back()))
            break;
        help.push_back((prev[help.back()]));
    }
    std::reverse(help.begin(), help.end());
    std::cout << "==================" << '\n';
    std::cout << "normal dijkstra" << '\n';
    std::vector<std::pair<Node, std::pair<NodeTimeInfo, NodeTransport>>> ans;
    for (const auto &node: help) {
        std::cout << node.line_id<< " " << translator_.get_human_line(node.line_id) << " " << translator_.get_human_stop_name(node.stop_name) << " " << std::format("{:%T}", record_of_distances[node].first.real_time) << " " <<std::format("{:%T}", record_of_distances[node].second.arrival) <<" " <<std::format("{:%T}", record_of_distances[node].second.departure) << '\n';
        ans.emplace_back(node, record_of_distances[node]);
    }
    return ans;
}

std::vector<std::pair<Routing::Node, Routing::NodeTimeInfo> >
Routing::dijkstra_dalekowzrocznosc_reversed(
    const std::string &start, const std::string &target,
    std::chrono::zoned_time<std::chrono::seconds> time, std::string ending_line) {

    Node start_node{start, ending_line};
    NodeTimeInfo start_node_time{0, time};
    std::map<Node, NodeTimeInfo> record_of_distances;
    std::map<Node, Node> prev;
    record_of_distances[start_node] = start_node_time;
    std::priority_queue<std::pair<float, Node>,
                std::vector<std::pair<float, Node> >,
                std::greater<std::pair<float, Node> > >
            pq;
    pq.emplace(0, start_node);
    Node ending_node;
    std::cout << "size of ttable" << ttable_.table_.size() << '\n';

    while (!pq.empty()) {
        auto [old_cost, old_node] = pq.top();
        pq.pop();
        //std::cout << "hello??? size of the pq: " << pq.size() << '\n';

        //std::cout << old_cost << " " << old_node.stop_name << " " << old_node.line_id << '\n';
        if (old_node.stop_name == target) {
            ending_node = old_node;
            break;
        }
        if (old_cost > record_of_distances[old_node].cost_from_start)
            continue;
        float time_to_arrive;
        float add_cost;

        for (const auto &[new_stop, distance]:
             sf_.find_stop_ids(old_node.stop_name)) {
            if (new_stop != old_node.stop_name) {
                /// change here with transport_between_stops_reverse
                std::vector<NodeTransport> result =
                        transport_between_stops_reverse(old_node.stop_name, new_stop,
                                                record_of_distances[old_node].real_time);
                if (result.empty()) {
                    /// change here with walking_between_stops_reverse
                    result =
                            walking_between_stops_reverse(old_node.stop_name, new_stop,
                                                  record_of_distances[old_node].real_time);
                    if (result.empty()) {
                        time_to_arrive = std::ceil(distance / this->walking_pace);
                        add_cost = time_to_arrive * 5;
                        std::string line_id = "walk - safety net";
                        /// minus over here:
                        std::chrono::zoned_time<std::chrono::seconds> arrival_at_B{
                            time.get_time_zone(),
                            record_of_distances[old_node].real_time.get_sys_time() -
                            std::chrono::minutes(static_cast<int>(time_to_arrive))
                        };
                        NodeTransport nt{
                            line_id, record_of_distances[old_node].real_time,
                            arrival_at_B
                        };
                        result.push_back(nt);
                    }
                }
                for (const auto &node: result) {
                    if (node.line_id != "walk - safety net") {
                        auto delta =record_of_distances[old_node].real_time.get_local_time() - node.arrival.get_local_time();
                        auto total_seconds =
                                std::chrono::duration_cast<std::chrono::seconds>(delta).count();
                        time_to_arrive = total_seconds / 60.0f;
                        add_cost = time_to_arrive;
                    }
                    bool is_node_not_walk = (node.line_id != "walk" && node.line_id != "walk - safety net");
                    bool is_old_node_not_walk = (old_node.line_id != "walk" && old_node.line_id != "walk - safety net");

                    //if (!is_node_not_walk && !is_old_node_not_walk)continue;

                    if (node.line_id == "walk")
                        add_cost *= walking_multiplier;

                    if (is_node_not_walk && is_old_node_not_walk && old_node.line_id != node.line_id) add_cost += this->time_for_change;


                    Node n{new_stop, node.line_id};
                    //std::cout << "the new node: " << n.line_id << " " << n.stop_name << " " << old_cost + add_cost << " " << node.arrival << '\n';

                    if (!record_of_distances.contains(n)) {
                        NodeTimeInfo nt{old_cost + add_cost, node.arrival};
                        record_of_distances[n] = nt;
                        // std::cout << old_cost + time_to_arrive << " "<<n.line_id << " "
                        // << n.stop_name << '\n';
                        pq.emplace(old_cost + add_cost, n);
                        prev[n] = old_node;
                    } else {
                        if (record_of_distances[n].cost_from_start > old_cost + add_cost) {
                            NodeTimeInfo nt{old_cost + add_cost, node.arrival};
                            record_of_distances[n] = nt;
                            pq.emplace(old_cost + add_cost, n);
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


    std::vector<Node> help;
    help.push_back(ending_node);

    while (!help.empty() && help.back().stop_name != start) {
        if (!prev.contains(help.back()))
            break;
        help.push_back((prev[help.back()]));
    }
    std::cout << "==================" << '\n';
    std::cout << "reversed dijkstra" << '\n';
    std::vector<std::pair<Node, NodeTimeInfo> > ans;
    for (const auto &node: help) {
        std::cout << translator_.get_human_line(node.line_id) << " " << translator_.get_human_stop_name(node.stop_name) << " " << std::format("{:%T}", record_of_distances[node].real_time) << '\n';
        ans.emplace_back(node, record_of_distances[node]);
    }
    return ans;
}

route_data Routing::output(const std::string &start, const std::string &target,
                std::chrono::zoned_time<std::chrono::seconds> time) {
    auto vec = dijkstra_dalekowzrocznosc(start, target, time);
    auto arrival_time = vec[vec.size()-1].second.first.real_time;
    route_data endgame;
    std::vector<route_data::route_data_node> endgame_vec;

    int i =1;
    while (i < vec.size()) {
        int start =i-1;
        std::string line = vec[i].first.line_id;
        while (i < vec.size() && vec[i].first.line_id == line) {
            i++;
        }
        int end = i-1;
        std::cout << "from " << start << " to end " << i << '\n';

        endgame_vec.emplace_back(
        vec[start].first.stop_name,
        sf_.get_coords(vec[start].first.stop_name),
        translator_.get_human_stop_name(vec[start].first.stop_name),
        "GET_ON",
        translator_.get_human_line(line),
        std::format("{:%T}", vec[start+1].second.second.departure)
        );
        endgame_vec.emplace_back(
        vec[end].first.stop_name,
        sf_.get_coords(vec[end].first.stop_name),
        translator_.get_human_stop_name(vec[end].first.stop_name),
        "GET_OFF",
        translator_.get_human_line(line),
        std::format("{:%T}", vec[end].second.second.arrival)
        );
    }

    std::cout << " FOR TESTING:" << '\n';

    for (const auto& rd : endgame_vec) {
        std::cout << rd.stop_id << " " << rd.stop_coords.first << " " << rd.stop_coords.second<< " " << rd.stop_name << " " << rd.route_name << " " << rd.time << " " << rd.type << '\n';
    }

    endgame.route_proper = endgame_vec;
    endgame.start_stop = start;
    endgame.end_stop = target;
    endgame.start_time = std::format("{:%T}", time);
    endgame.end_time = std::format("{:%T}", arrival_time);
    return endgame;
}

route_data Routing::output_testing(const std::string &start, const std::string &target,
                std::chrono::zoned_time<std::chrono::seconds> time) {
    auto arrival = dijkstra_dalekowzrocznosc(start, target, time);
    auto arrival_time = arrival[arrival.size()-1].second.first.real_time;
    const auto &line_id = arrival[arrival.size()-1].first.line_id;
    auto vec = dijkstra_dalekowzrocznosc_reversed(target, start, arrival_time, line_id);
    route_data endgame;
    std::vector<route_data::route_data_node> endgame_vec;
    for (const auto& [node, nt] : vec) {
        endgame_vec.emplace_back(
            node.stop_name,
            sf_.get_coords(node.stop_name),
            translator_.get_human_stop_name(node.stop_name),
            "GET_ON",
            translator_.get_human_line(node.stop_name),
            std::format("{:%T}", nt.real_time)
        );
    }
    endgame.route_proper = endgame_vec;
    endgame.start_stop = start;
    endgame.end_stop = target;
    endgame.start_time = std::format("{:%T}", time);
    endgame.end_time = std::format("{:%T}", arrival_time);
    return endgame;
}
