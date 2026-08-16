#pragma once

#ifndef SRC_TRANSPORTTABLE_H
#define SRC_TRANSPORTTABLE_H

#include <pqxx/pqxx>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <iostream>

inline std::string default_query = R"(
                SELECT DISTINCT s1.stop_id, s2.stop_id, s1.arrival_time, s2.arrival_time, s1.trip_id
                FROM stop_times s1
                JOIN stop_times s2 ON s1.trip_id = s2.trip_id
                JOIN trips t ON s1.trip_id = t.trip_id
                JOIN routes r ON t.route_id = r.route_id
                LEFT JOIN calendar c ON t.service_id = c.service_id
                LEFT JOIN calendar_dates cd ON t.service_id = cd.service_id AND cd.date = $3
                WHERE s1.stop_sequence < s2.stop_sequence
                AND (
                  cd.exception_type = 1
                  OR
                  (
                  $3::DATE BETWEEN c.start_date AND c.end_date AND (cd.exception_type IS NULL OR cd.exception_type != 2)
                  )
                )
                AND s1.trip_id = s2.trip_id
                AND s1.arrival_time > $1
                AND s1.arrival_time < $2
                ORDER BY s1.arrival_time ASC;
                )";

struct PairHash {
  std::size_t operator()(const std::pair<std::string, std::string> &p) const {
    const std::size_t h1 = std::hash<std::string>{}(p.first);
    const std::size_t h2 = std::hash<std::string>{}(p.second);

    // combining the hashes
    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
  }
};

struct Trip {
  std::string time_stop1;
  std::string time_stop2;
  std::string trip_id;
};

class TransportTable {
public:
  std::unordered_map<std::pair<std::string, std::string>, std::vector<Trip>,
                   PairHash>
    table_;

  TransportTable(std::string q = default_query,
                 const std::chrono::time_point<std::chrono::system_clock> t =
                     std::chrono::system_clock::now())
      : query_{std::move(q)}, time_{t} {};
  void generate_table();
private:
  std::optional<pqxx::connection> connection_;
  std::string query_;
  std::chrono::time_point<std::chrono::system_clock> time_;
  /**
   * @brief Formats the time_ parameter to a string usable in the query.
   *
   * @return A (time, time limit) pair formatted for the query.
   */
  std::pair<std::string, std::string> format_time_test() const;
  std::pair<std::string, std::string> format_time() const;
public:
  //helper members;
  void printtable();
  size_t size() const { return table_.size(); }
};

#endif // SRC_TRANSPORTTABLE_H
