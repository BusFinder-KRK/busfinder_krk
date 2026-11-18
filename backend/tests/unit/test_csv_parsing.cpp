#include <gtest/gtest.h>

#include "routing.h"

using namespace std::chrono_literals;

namespace {
  class CSVTest : public testing::Test {
  protected:
    std::unordered_map<std::string, std::pair<double, double> > stops;

    CSVTest() {
      std::filesystem::current_path(PROJECT_ROOT_DIR);
      stops = {{"A", {0, 0}}, {"B", {0, 1}}, {"C", {1, 0}}, {"D", {0.5, 0.5}}, {"E", {0.4, 0.4}}};
    }
  };
}

TEST_F(CSVTest, WalkingCSVFileOpenedCorrectly) {
  auto r1_construct = [this]() {
    TransportTable t1{
      std::unordered_map<std::pair<std::string, std::string>, std::vector<TransportTable::Trip>, PairHash>{},
      std::chrono::time_point<std::chrono::system_clock>{}
    };
    Routing r1{"tests/fixtures/walking_times.csv", std::move(t1), StopFinder{stops, 5}};
  };
  auto r2_construct = [this]() {
    TransportTable t2{
      std::unordered_map<std::pair<std::string, std::string>, std::vector<TransportTable::Trip>, PairHash>{},
      std::chrono::time_point<std::chrono::system_clock>{}
    };
    Routing r2{"tests/fixtures/walking_times.cv", std::move(t2), StopFinder{stops, 5}};
  };
  ASSERT_NO_THROW(r1_construct());
  ASSERT_THROW(r2_construct(), std::ios_base::failure);
}

TEST_F(CSVTest, WalkingCSVValuesParsed) {
  TransportTable t1{
    std::unordered_map<std::pair<std::string, std::string>, std::vector<TransportTable::Trip>, PairHash>{},
    std::chrono::time_point<std::chrono::system_clock>{}
  };
  const auto t = std::chrono::zoned_time{};
  Routing r1{"tests/fixtures/walking_times.csv", std::move(t1), StopFinder{stops, 5}};
  ASSERT_EQ(std::format("{:%T}", t.get_local_time() + 5min), r1.find_route("A", "B", t).end_time);
  ASSERT_EQ(std::format("{:%T}", t.get_local_time() + 7min), r1.find_route("A", "C", t).end_time);
  ASSERT_EQ(std::format("{:%T}", t.get_local_time() + 5min), r1.find_route("A", "D", t).end_time);
  ASSERT_EQ(std::format("{:%T}", t.get_local_time() + 10min), r1.find_route("A", "E", t).end_time);
  ASSERT_EQ(std::format("{:%T}", t.get_local_time() + 10min), r1.find_route("B", "C", t).end_time);
  ASSERT_EQ(std::format("{:%T}", t.get_local_time() + 8min), r1.find_route("B", "D", t).end_time);
  ASSERT_EQ(std::format("{:%T}", t.get_local_time() + 5min), r1.find_route("B", "E", t).end_time);
  ASSERT_EQ(std::format("{:%T}", t.get_local_time() + 2min), r1.find_route("D", "C", t).end_time);
  ASSERT_EQ(std::format("{:%T}", t.get_local_time() + 7min), r1.find_route("D", "E", t).end_time);
}
