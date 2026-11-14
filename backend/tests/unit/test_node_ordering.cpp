#include <gtest/gtest.h>
#include <routing.h>

TEST(NodeTest, NodeOrderingWorksCorrectly) {
  Routing::Node n1{"test1", "A"};
  Routing::Node n2{"test2", "A"};
  Routing::Node n3{"test1", "B"};
  Routing::Node n4{"test2", "A"};

  EXPECT_TRUE(n1 < n3);
  EXPECT_TRUE(n3 < n2);

  EXPECT_TRUE(n1 < n2); //transitivity

  //opposite is false
  EXPECT_FALSE(n1 > n3);
  EXPECT_FALSE(n3 > n2);

  EXPECT_FALSE(n1 > n2);

  //equality
  EXPECT_FALSE(n1 == n4);
  EXPECT_TRUE(n2 == n4);
  EXPECT_FALSE(n3 == n4);
  EXPECT_TRUE(n4 == n4);
}
