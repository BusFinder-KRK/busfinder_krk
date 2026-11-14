#include <gtest/gtest.h>

#include "api.h"

using namespace std::chrono_literals;

TEST(APIUtilTest, timeParsedCorrectly) {
  const std::string time_str = "2026-10-11 21:42:11";
  const std::chrono::local_time result = api::parse_time(time_str).get_local_time();
  const std::chrono::local_days result_days = std::chrono::floor<std::chrono::days>(result);
  const std::chrono::hh_mm_ss split_time{result - result_days};
  const std::chrono::year_month_day split_date{result_days};

  ASSERT_EQ(2026y, split_date.year());
  ASSERT_EQ(std::chrono::month(10), split_date.month());
  ASSERT_EQ(11d, split_date.day());

  ASSERT_EQ(21h, split_time.hours());
  ASSERT_EQ(42min, split_time.minutes());
  ASSERT_EQ(11s, split_time.seconds());
}

TEST(APIUtilTest, failForInvalidString) {
  const std::string time_str = "aaa";
  ASSERT_THROW(api::parse_time(time_str), std::invalid_argument);
}
