#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <chrono>
#include "transporttable.h"

using namespace std::chrono_literals;

TEST(TransportTableTest, FormatTimeWorksCorrectlyForStartAndEndBeforeMidnight) {
  std::istringstream is{"2026-11-14 18:14:21"};
  std::chrono::time_point<std::chrono::system_clock> parsed_point;
  is >> std::chrono::parse("%Y-%m-%d %H:%M:%S", parsed_point);
  ASSERT_FALSE(is.fail());

  const auto time_zone = std::chrono::locate_zone("Europe/Warsaw");
  const std::string start_target = time_zone->get_info(parsed_point).abbrev == "CEST" ? "18:14:21" : "19:14:21";

  const std::string end_target = time_zone->get_info(parsed_point).abbrev == "CEST" ? "21:14:21" : "22:14:21";

  const TransportTable test{"_", parsed_point};
  const auto [start, end] = test.format_time();

  ASSERT_EQ(start_target, start);
  ASSERT_EQ(end_target, end);
}

TEST(TransportTableTest, FormatTimeWorksCorrectlyStartBeforeMidnightAndEndPastMidnight) {
  std::istringstream is{"2026-11-14 22:14:21"};
  std::chrono::time_point<std::chrono::system_clock> parsed_point;
  is >> std::chrono::parse("%Y-%m-%d %H:%M:%S", parsed_point);
  ASSERT_FALSE(is.fail());

  const auto time_zone = std::chrono::locate_zone("Europe/Warsaw");
  const std::string start_target = time_zone->get_info(parsed_point).abbrev == "CEST" ? "22:14:21" : "23:14:21";

  const std::string end_target = time_zone->get_info(parsed_point).abbrev == "CEST" ? "25:14:21" : "26:14:21";

  const TransportTable test{"_", parsed_point};
  const auto [start, end] = test.format_time();

  ASSERT_EQ(start_target, start);
  ASSERT_EQ(end_target, end);
}

TEST(TransportTableTest, FormatTimeWorksCorrectlyStartPastMidnightAndEndBeforeMidnight) {
  std::istringstream is{"2026-11-14 02:14:21"};
  std::chrono::time_point<std::chrono::system_clock> parsed_point;
  is >> std::chrono::parse("%Y-%m-%d %H:%M:%S", parsed_point);
  ASSERT_FALSE(is.fail());

  const auto time_zone = std::chrono::locate_zone("Europe/Warsaw");
  const std::string start_target = time_zone->get_info(parsed_point).abbrev == "CEST" ? "26:14:21" : "27:14:21";

  const std::string end_target = time_zone->get_info(parsed_point).abbrev == "CEST" ? "05:14:21" : "06:14:21";

  const TransportTable test{"_", parsed_point};

  const auto [start, end] = test.format_time();

  ASSERT_EQ(start_target, start);
  ASSERT_EQ(end_target, end);
}
