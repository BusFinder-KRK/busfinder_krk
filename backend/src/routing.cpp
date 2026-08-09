#include <routing.h>
#include <queue>

void Routing::dijkstra_dalekowzrocznosc(const std::string& start, const std::string& target, std::chrono::time_point<std::chrono::system_clock> time) {
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
    std::vector<std::pair<int, Node>>,
    std::greater<std::pair<int, Node>>
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
  ///teraz trzeba bedzie sprawdzic n najblizysz przystankow
  ///maybe i ican use transition table to find the closes stops
  ///


  }


/* TO DO:
 * MAKE A < FOR NODE AND NODETIMEINFO so maps and pq work
 * MAKE SURE YOU DONT NEED TO ADD THE LINE ID TO NODETIMEINFO
 */

}