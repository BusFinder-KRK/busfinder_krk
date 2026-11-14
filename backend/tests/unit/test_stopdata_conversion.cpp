#include <gtest/gtest.h>
#include "stopfinder.h"

TEST(StopDataTest, StopDataConstructorConvertsCorrectly) {
  double lat = 50.07747255830289;
  double lon = 19.909417560682908;
  StopData test_data1{"test", lat, lon};
  EXPECT_NEAR(421968, test_data1.x, 0.5);
  EXPECT_NEAR(5547814, test_data1.y, 0.5);

  lat = 50.06146321281982;
  lon = 19.937175093254478;
  StopData test_data2{"test", lat, lon};
  EXPECT_NEAR(423928, test_data2.x, 0.5);
  EXPECT_NEAR(5546006, test_data2.y, 0.5);

  lat = 50.00923594639903;
  lon = 20.05140630857363;
  StopData test_data3{"test", lat, lon};
  EXPECT_NEAR(432031, test_data3.x, 0.5);
  EXPECT_NEAR(5540089, test_data3.y, 0.5);
}
