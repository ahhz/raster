#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include <gtest/gtest.h>

#include <pronto/raster/io.h>
#include <pronto/raster/transform_raster_view.h>

#include <vector>

namespace pr = pronto::raster;
namespace fs = pr::filesystem;

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
  int operator()(int a, int b) {
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
TEST(RasterTest, Transform) {
	EXPECT_TRUE(transform_with_overloaded_function_object());
  EXPECT_TRUE(transform_with_uncopyable_function_object());
  EXPECT_TRUE(transform_with_unmovable_function_object());
  EXPECT_TRUE(transform_with_function_object());
	EXPECT_TRUE(transform_with_lambda());
	EXPECT_TRUE(transform_with_user_function());
}