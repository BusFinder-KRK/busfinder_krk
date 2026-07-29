#include <routing.h>
#include <stopfinder.h>
#include <queue>

void Routing::dijkstra_dalekowzrocznosc(const std::string &start, const std::string &target,
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

    while (!pq.empty()) {
        auto [old_cost, old_node] = pq.top();
        pq.pop();

        if (old_node.stop_name == target) {
            Node ending_node = old_node;
            break;
        }
        if (old_cost > record_of_distances[old_node].time_from_start) continue;
        StopFinder sf{adjacent_stops};
        int time_to_arrive;
        for (const auto &[new_stop, distance]: sf.find_stop_ids(old_node.stop_name)) {
            if (new_stop != old_node.stop_name) {
                std::vector<NodeTransport>result = transport_between_stops_(old_node.stop_name, new_stop, record_of_distances[old_node].real_time);
                if (result.empty()) {
                    result = walking_between_stops_(old_node.stop_name, new_stop, record_of_distances[old_node].real_time);
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
                        record_of_distances[node] = nt;
                        pq.push(std::make_pair(old_cost + time_to_arrive, n));
                        prev[n] = old_node;
                    }
                    else {

                    }
                }

            }
        }
    }
    ///teraz trzeba bedzie sprawdzic n najblizysz przystankow
    ///maybe i ican use transition table to find the closes stops
    ///
}


/* TO DO:
 * MAKE A < FOR NODE AND NODETIMEINFO so maps and pq work
 * MAKE SURE YOU DONT NEED TO ADD THE LINE ID TO NODETIMEINFO
 *

}
