#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include <gtest/gtest.h>

#include <pronto/raster/io.h>
#include <pronto/raster/transform_raster_view.h>

#include <vector>

namespace pr = pronto::raster;
namespace fs = std::filesystem;

bool transform_with_overloaded_function_object()
{
	int rows = 3;
	int cols = 5;
	auto a = pr::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	auto b = pr::create_temp<int>(rows, cols);
	int hundreds = 0;
	for (auto&& i : b) {
		hundreds += 100;
		i = hundreds;
	}

	auto c = pr::transform(std::plus<>{}, a, b);
	std::vector<int> check;
	for (auto&& i : c) {
		check.push_back(i);
	}

	return check == std::vector<int>{101, 202, 303, 404, 505, 606, 707, 808, 909, 1010, 1111, 1212, 1313, 1414, 1515};
}

int my_plus(int a, int b)
{
	return a + b;
}

bool transform_with_user_function()
{
	int rows = 3;
	int cols = 5;
	auto a = pr::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	auto b = pr::create_temp<int>(rows, cols);
	int hundreds = 0;
	for (auto&& i : b) {
		hundreds += 100;
		i = hundreds;
	}

	auto c = pr::transform(my_plus, a, b);
	std::vector<int> check;
	for (auto&& i : c) {
		check.push_back(i);
	}

	return check == std::vector<int>{101, 202, 303, 404, 505, 606, 707, 808, 909, 1010, 1111, 1212, 1313, 1414, 1515};
}

bool transform_with_function_object()
{
	int rows = 3;
	int cols = 5;
	auto a = pr::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	auto b = pr::create_temp<int>(rows, cols);
	int hundreds = 0;
	for (auto&& i : b) {
		hundreds += 100;
		i = hundreds;
	}

	auto c = pr::transform(std::plus<int>{}, a, b);
	std::vector<int> check;
	for (auto&& i : c) {
		check.push_back(i);
	}

	return check == std::vector<int>{101, 202, 303, 404, 505, 606, 707, 808, 909, 1010, 1111, 1212, 1313, 1414, 1515};
}


class plusser_uncopyable
{
public:
  plusser_uncopyable() = default;
  plusser_uncopyable(const plusser_uncopyable&) = delete;
  plusser_uncopyable(plusser_uncopyable&&) = default;
  plusser_uncopyable& operator=(const plusser_uncopyable&) = delete;
  plusser_uncopyable& operator=(plusser_uncopyable&&) = default;
  int operator()(int a, int b) const {
    return a + b;
  }
};

class plusser_unmovable
{
public:
  plusser_unmovable() = default;
  plusser_unmovable(const plusser_unmovable&) = delete;
  plusser_unmovable(plusser_unmovable&&) = delete;
  plusser_unmovable& operator=(const plusser_unmovable&) = delete;
  plusser_unmovable& operator=(plusser_unmovable&&) = delete;
  int operator()(int a, int b) const {
    return a + b;
  }
};

class plusser_non_const
{
public:
  int operator()(int a, int b) {
    return a + b;
  }
};
bool transform_with_uncopyable_function_object()
{
  int rows = 3;
  int cols = 5;
  auto a = pr::create_temp<int>(rows, cols);
  int ones = 0;
  for (auto&& i : a) {
    ones += 1;
    i = ones;
  }
  auto b = pr::create_temp<int>(rows, cols);
  int hundreds = 0;
  for (auto&& i : b) {
    hundreds += 100;
    i = hundreds;
  }
  plusser_uncopyable p{};
  auto c = pr::transform(std::ref(p), a, b).sub_raster(0,0,rows,cols); // sub_raster to force a copy being made
  std::vector<int> check;
  for (auto&& i : c) {
    check.push_back(i);
  }

  return check == std::vector<int>{101, 202, 303, 404, 505, 606, 707, 808, 909, 1010, 1111, 1212, 1313, 1414, 1515};
}

bool transform_with_unmovable_function_object()
{
  int rows = 3;
  int cols = 5;
  auto a = pr::create_temp<int>(rows, cols);
  int ones = 0;
  for (auto&& i : a) {
    ones += 1;
    i = ones;
  }
  auto b = pr::create_temp<int>(rows, cols);
  int hundreds = 0;
  for (auto&& i : b) {
    hundreds += 100;
    i = hundreds;
  }
  plusser_unmovable p{};
  auto c = pr::transform(std::ref(p), a, b);
  std::vector<int> check;
  for (auto&& i : c) {
    check.push_back(i);
  }

  return check == std::vector<int>{101, 202, 303, 404, 505, 606, 707, 808, 909, 1010, 1111, 1212, 1313, 1414, 1515};
}

bool const_transform_over_non_const_function()
{
  int rows = 3;
  int cols = 5;
  auto a = pr::create_temp<int>(rows, cols);
  int ones = 0;
  for (auto&& i : a) {
    ones += 1;
    i = ones;
  }
  auto b = pr::create_temp<int>(rows, cols);
  int hundreds = 0;
  for (auto&& i : b) {
    hundreds += 100;
    i = hundreds;
  }
  auto c = pr::transform(plusser_non_const{}, a, b);
  std::vector<int> check;
  for (auto&& i : c) {
    check.push_back(i);
  }

  return check == std::vector<int>{101, 202, 303, 404, 505, 606, 707, 808, 909, 1010, 1111, 1212, 1313, 1414, 1515};
}


bool transform_with_lambda()
{
	int rows = 3;
	int cols = 5;
	auto a = pr::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	auto b = pr::create_temp<int>(rows, cols);
	int hundreds = 0;
	for (auto&& i : b) {
		hundreds += 100;
		i = hundreds;
	}
	auto lambda = [](int a, int b) {return a + b; };
	auto c = pr::transform(lambda, a, b);
	std::vector<int> check;
	for (auto&& i : c) {
		check.push_back(i);
	}

	return check == std::vector<int>{101, 202, 303, 404, 505, 606, 707, 808, 909, 1010, 1111, 1212, 1313, 1414, 1515};
}

bool transform_empty()
{
  int rows = 3;
  int cols = 5;
  auto a = pr::create_temp<int>(rows, cols);
  int ones = 0;
  for (auto&& i : a) {
    ones += 1;
    i = ones;
  }
  auto b = pr::create_temp<int>(rows, cols);
  int hundreds = 0;
  for (auto&& i : b) {
    hundreds += 100;
    i = hundreds;
  }
  auto lambda = [](int a, int b) {return a + b; };
  auto c = pr::transform(lambda, a, b);
  auto c1 = c.sub_raster(0, 0, 0, 0);
  auto c2 = c.sub_raster(0, 0, 0, 2);
  auto c3 = c.sub_raster(0, 0, 2, 0);

  for (auto&& i : c1)
  {
    return false;
  }
  for (auto&& i : c2)
  {
    return false;
  }
  for (auto&& i : c3)
  {
    return false;
  }
  return c1.size() == 0 && c1.rows() == 0 && c1.cols() == 0 &&
    c2.size() == 0 && c2.rows() == 0 && c2.cols() == 2 &&
    c3.size() == 0 && c3.rows() == 2 && c3.cols() == 0;
}

bool transform_sub_raster()
{
  int rows = 6;
  int cols = 3;
  auto a = pr::create_temp<int>(rows, cols);
  int ones = 0;
  for (auto&& i : a) {
    ones += 1;
    i = ones;
  }
  auto b = pr::create_temp<int>(rows, cols);
  int hundreds = 0;
  for (auto&& i : b) {
    hundreds += 100;
    i = hundreds;
  }
  auto lambda = [](int a, int b) {return a + b; };
  auto c = pr::transform(lambda, a, b).sub_raster(2, 1, 3, 2);
  std::vector<int> check;
  for (auto&& i : c) {
    check.push_back(i);
  }

  return check == std::vector<int>{ 808, 909,  1111, 1212, 1414, 1515};
}

bool transform_sub_raster_random_access()
{
  int rows = 6;
  int cols = 3;
  auto a = pr::create_temp<int>(rows, cols);
  int ones = 0;
  for (auto&& i : a) {
    ones += 1;
    i = ones;
  }
  auto b = pr::create_temp<int>(rows, cols);
  int hundreds = 0;
  for (auto&& i : b) {
    hundreds += 100;
    i = hundreds;
  }
  auto lambda = [](int a, int b) {return a + b; };
  auto c = pr::transform(lambda, a, b).sub_raster(2, 1, 3, 2);
  std::vector<int> check;
  for (auto&& i : c) {
    check.push_back(i);
  }

  auto t1 = *(c.begin() + 2) == *(check.begin() + 2);
  auto t2 = c.begin()[4] == check[4];
  auto t3 = *(--(c.begin() + 3)) == *(--(check.begin() + 3));
  auto t4 = *(++(c.begin() + 3)) == *(++(check.begin() + 3));
  auto t5 = *((c.begin() + 3) - 1) == *((check.begin() + 3) - 1);
  auto i = c.begin() + 2;
  auto i_check = check.begin() + 2;
  auto t6 = *(++i) == *(++i_check);
  auto t7 = *(i++) == *(i_check++);
  auto t8 = *(i) == *(i_check);
  auto t9 = *(--c.end()) == *(--check.end());
  auto t10 = *(c.end() - 2) == *(check.end() - 2);
  return t1 && t2 && t3 && t4 && t5 && t6 && t7 && t8 && t9 && t10;


}

TEST(RasterTest, Transform) {
	EXPECT_TRUE(transform_with_overloaded_function_object());
  EXPECT_TRUE(transform_with_uncopyable_function_object());
  EXPECT_TRUE(transform_with_unmovable_function_object());
  EXPECT_TRUE(transform_with_function_object());
  EXPECT_TRUE(const_transform_over_non_const_function());
  EXPECT_TRUE(transform_with_lambda());
	EXPECT_TRUE(transform_with_user_function());
  EXPECT_TRUE(transform_empty());
  EXPECT_TRUE(transform_sub_raster());
  EXPECT_TRUE(transform_sub_raster_random_access());
}
