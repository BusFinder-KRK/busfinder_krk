#include "transporttable.h"
#include "config.h"
#include <iostream>
#include <pqxx/pqxx>

using string = std::string;

std::pair<string, string> transporttable::format_time() const {
  const auto start_zoned =
      std::chrono::zoned_time{std::chrono::current_zone(), time_};
  const auto end_zoned = std::chrono::zoned_time{std::chrono::current_zone(),
                                                 time_ + std::chrono::hours(3)};
  // calculating start time
  string start_time_result, end_time_result;
  const string start_time_str = std::format("{:%X}", start_zoned);
  const auto start_time_duration =
      start_zoned.get_local_time() -
      std::chrono::floor<std::chrono::days>(start_zoned.get_local_time());
  const std::chrono::hh_mm_ss start_split_time{start_time_duration};
  if (start_time_str >= "00:00:00" && start_time_str < "04:00:00")
    start_time_result = std::to_string(24 + start_split_time.hours().count()) +
                        ":" + start_time_str.substr(2);
  else
    start_time_result = start_time_str;
  // calculating end time;
  string end_time_str = std::format("{:%X}", end_zoned);
  const auto end_time_duration =
      end_zoned.get_local_time() -
      std::chrono::floor<std::chrono::days>(end_zoned.get_local_time());
  const std::chrono::hh_mm_ss end_split_time{end_time_duration};
  if (end_time_str >= "00:00:00" && end_time_str < "04:00:00")
    end_time_result = std::to_string(24 + end_split_time.hours().count()) +
                      ":" + end_time_result.substr(2);
  else
    end_time_result = end_time_str;
  return {start_time_result, end_time_result};
}

void transporttable::generate_table() {
  Config::load();
  auto [start_time, end_time] = format_time();
  string weekday = "c." + std::format("{:%A}", time_);
  string current_date = std::format("{:%Y-%m-%d}", time_);
  std::ranges::transform(weekday, weekday.begin(),
                         [](char c) { return std::tolower(c); });
  string filled_query = std::vformat(query_, std::make_format_args(weekday));
  connection_.emplace(Config::connection_string);
  pqxx::work transaction{*connection_};
  for (const auto& [stopid1, stopid2, arrival_time1, arrival_time2, tripid] :
       transaction.query<string, string, string, string, string>(
           filled_query, {start_time, end_time, current_date})) {
    table_[{stopid1, stopid2}].push_back(
        {arrival_time1, arrival_time2, tripid});
  }
  transaction.commit();
}

void transporttable::printtable() {
  std::cout << table_.size() << std::endl;
  for (const auto& [a, trips] : table_) {
    std::cout << a.first << " " << a.second << std::endl;
    std::cout << "trips: " << std::endl;
    for (const auto &[time_stop1, time_stop2, trip_id] : trips)
      std::cout << time_stop1 << " " << time_stop2 << " " << trip_id
                << std::endl;
    std::cout << std::endl;
  }
}