//
//=======================================================================
// Copyright 2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include <gtest/gtest.h>

#include <pronto/raster/raster_variant.h>
#include <pronto/raster/type_erased_raster.h>
#include <pronto/raster/raster_algebra_operators.h>
#include <pronto/raster/io.h>
#include <pronto/raster/nodata_transform.h>

#include <ranges>

namespace pr = pronto::raster;

class prox : public pr::proxy_reference<prox,int>
{
public:

	prox() : m_v(new int{})	{}
	~prox() = default;
	prox(const prox& p) = default;
	prox(prox&& p) = default;

	const prox& operator=(const prox& v) const	{
		*m_v = *v.m_v;
		return *this;
	}

	operator int() const {
		return *m_v;
	}
	
	const prox& operator=(const int& v) const {
		*m_v = v;
		return *this;
	}
	std::shared_ptr<int> m_v;
};

bool test_type_erased_reference()
{
	auto p = prox{};
	p = 1;
	auto pe = pr::erase_reference_type<int,pr::access::read_write>(p);
	++pe;
	pe+=3;
	return pe == 5 ;
}


bool test_type_erased_raster()
{
	int rows = 2;
	int cols = 3;
	auto a = pr::create_temp<int>(rows, cols);
	int v = 0;
	for (auto&& i : a) {
		v += 1;
	  i = v;
	}

	pr::type_erased_raster_iterator<int> iter(a.begin());
	pr::type_erased_raster_iterator<int> end(a.end());
	
	auto ai = a.begin();
	static_assert(std::input_iterator<decltype(iter)>);
	static_assert(std::forward_iterator<decltype(iter)>);
	int w = 1;
	for(;iter!=end;++iter){
		w *= 10;
		*iter *=  w;
	}
	std::vector<int> vec; 

	pr::type_erased_raster<int> a_erase(a);
	static_assert(std::ranges::range<decltype(a)>);
	static_assert(std::ranges::view<decltype(a)>);
	static_assert(std::ranges::random_access_range<decltype(a)>);
	static_assert(std::ranges::sized_range<decltype(a)>);
	static_assert(!std::ranges::contiguous_range<decltype(a)>);
	
	;

	static_assert(std::ranges::range<decltype(a_erase)>);
	static_assert(std::ranges::view<decltype(a_erase)>);
	static_assert(std::ranges::random_access_range<decltype(a_erase)>);
	static_assert(std::ranges::sized_range<decltype(a_erase)>);
	static_assert(!std::ranges::contiguous_range<decltype(a_erase)>);


	for (auto&& i : a_erase) {
		vec.push_back(i);
	}
	
	return vec == std::vector<int>{ 10, 200, 3000, 40000, 500000,6000000 };

}

bool test_type_erased_plus()
{
	int rows = 2;
	int cols = 3;
	auto a = pr::create_temp<int>(rows, cols);
	auto b = pr::create_temp<int>(rows, cols);
	auto aa = pr::erase_raster_type(a);
	auto bb = pr::erase_raster_type(b);
	
	static_assert(std::ranges::random_access_range<decltype(a)>);
	static_assert(std::ranges::random_access_range<decltype(b)>);
	static_assert(std::ranges::random_access_range<decltype(aa)>);
	static_assert(std::ranges::random_access_range<decltype(bb)>);


	int v = 0;
	for (auto&& i : aa) {
		v += 1;
		i = v;
	}
	v = 0;
	for (auto&& i : bb) {
		v += 100;
		i = v;
	}
	
	auto cc = aa + bb;

	static_assert(std::ranges::random_access_range<decltype(cc)>);

	std::vector<int> vec;
	for (auto&& i :cc) {
		vec.push_back(i);
	}

	return vec ==std::vector<int>{101, 202, 303, 404, 505, 606};

}

bool test_type_erased_plus_nodata()
{
	int rows = 2;
	int cols = 3;
	auto a = pr::create_temp<int>(rows, cols);
	auto b = pr::create_temp<int>(rows, cols);

	auto aa = pr::erase_raster_type(a);
	auto bb = pr::erase_raster_type(b);

	static_assert(std::ranges::random_access_range<decltype(a)>);
	static_assert(std::ranges::random_access_range<decltype(b)>);
	static_assert(std::ranges::random_access_range<decltype(aa)>);
	static_assert(std::ranges::random_access_range<decltype(bb)>);


	int v = 0;
	for (auto&& i : aa) {
		v += 1;
		i = v;
	}

	v = 0;
	for (auto&& i : bb) {
		v += 100;
		i = v;
	}
	
	auto cc = pr::optional_to_nodata(
		pr::erase_raster_type(pr::nodata_to_optional(aa,2)) + 
		pr::erase_raster_type(pr::nodata_to_optional(bb,500)), -999);


	static_assert(std::ranges::random_access_range<decltype(cc)>);

	std::vector<int> vec;
	for (auto&& i : cc) {
		vec.push_back(i);
	}

	return vec == std::vector<int>{101, -999, 303, 404, -999, 606};

}

bool test_plus_constant()
{
	int rows = 2;
	int cols = 3;
	auto a = pr::create_temp<int>(rows, cols);
	int v = 0;
	for (auto&& i : a) {
		v += 1;
		i = v;
	}
	auto b = a + 10;

	std::vector<int> vec;
	for (auto&& i : b) {
		vec.push_back(i);
	}

	return vec == std::vector<int>{11, 12, 13, 14, 15, 16};

}
bool test_variant_type_plus()
{
	int rows = 2;
	int cols = 3;
	auto a = pr::create_temp<int>(rows, cols);
	auto b = pr::create_temp<int>(rows, cols);
	int v = 0;
	for (auto&& i : a) {
		v += 1;
		i = v;
	}

	v = 0;
	for (auto&& i : b) {
		v += 100;
		i = v;
	}
	auto aa = pr::erase_and_hide_raster_type(a);
	auto bb = pr::erase_and_hide_raster_type(b);
	static_assert(pr::RasterVariantConcept<decltype(aa)>);
	auto cc = aa + bb;

	static const int index = 4;
	auto c = std::get<4>(cc);
	std::vector<int> vec;
	for (auto&& i : c) {
		vec.push_back(i);
	}

	return vec == std::vector<int>{101, 202, 303, 404, 505, 606};
	
}

bool test_get_any_blind_raster()
{
	int rows = 3;
	int cols = 5;
	auto a = pr::create_temp<int>(rows, cols);
	int ones = 0;
	for (auto&& i : a) {
		ones += 1;
		i = ones;
	}
	auto aa = pr::erase_and_hide_raster_type(a);

	auto index = aa.index();
	return index == 4;
}


TEST(RasterTest, AnyBlindRaster) {
	EXPECT_TRUE(test_type_erased_reference());
	EXPECT_TRUE(test_type_erased_raster());
	EXPECT_TRUE(test_type_erased_plus());
	EXPECT_TRUE(test_plus_constant());
	EXPECT_TRUE(test_type_erased_plus_nodata());
	EXPECT_TRUE(test_variant_type_plus());
	EXPECT_TRUE(test_get_any_blind_raster());
}

