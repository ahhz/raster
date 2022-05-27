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

#include <pronto/raster/any_blind_raster.h>
#include <pronto/raster/blind_function.h>
#include <pronto/raster/io.h>

#include <ranges>

namespace pr = pronto::raster;

class prox : public pr::proxy_reference<prox,int>
{
public:

	prox(std::vector<int>& v) : vec{v}, target(3)
	{}
	
	prox(const prox& p) : target{p.target}, vec(p.vec)
	{}

	operator int() const
	{
		return vec[target];
	}
	
	const prox& operator=(const int& v) const {
		vec[target] = v;
		return *this;
	}

	void set_target(int t)
	{
		target = t;
	}
	int target = 3;
	std::vector<int>& vec;
};

bool test_type_erased_reference()
{
	std::vector<int> v{ 1,2,3,4,5 };
	prox p{ v };
	p = 1;
	p.set_target(2);
	pr::type_erased_reference<int>  pe(p);
	++pe;
	pe+=3;

	int x = 100;
	int& xref = x;
	xref += 100;
	pr::type_erased_reference<int> xref_erased(xref);
	xref_erased /= 4;

	return v == std::vector{ 1, 2, 7, 1, 5 } && x == 50;
	
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
	static_assert(std::input_iterator<decltype(iter)>);
	static_assert(std::derived_from < std::_Iter_concept<decltype(iter)>, std::forward_iterator_tag>);
	static_assert(!decltype(iter)::is_single_pass);
	static_assert(std::default_initializable<decltype(iter)>);

	static_assert(std::incrementable<decltype(iter)>);

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
	pr::any_raster<int> any_a(a);
	pr::any_blind_raster blind_a(any_a);
	
	auto index = pr::detail::index_in_packed_list(blind_a, pr::blind_data_types{});
	return index == 4;
}


TEST(RasterTest, AnyBlindRaster) {
	EXPECT_TRUE(test_type_erased_reference());
	EXPECT_TRUE(test_type_erased_raster());
	EXPECT_TRUE(test_get_any_blind_raster());
}

