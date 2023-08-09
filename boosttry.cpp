// cpptry.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
// Boost.Range
#include <boost/range.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/adaptors.hpp>
// adaptors
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/adaptor/transformed.hpp>
// containers
#include <vector>
#include <boost/array.hpp>
// just for output
#include <iostream>
#include <boost/assign/std/vector.hpp>




// Model adapt
#include <boost/geometry/geometries/adapted/c_array.hpp>
#include <boost/geometry/geometries/adapted/boost_polygon/point.hpp>
#include <boost/geometry/geometries/adapted/boost_array.hpp>

BOOST_GEOMETRY_REGISTER_C_ARRAY_CS(cs::cartesian);
//BOOST_GEOMETRY_REGISTER_BOOST_ARRAY_CS(cs::cartesian);

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
namespace bga = boost::adaptors;
using namespace boost::assign;

using     XY2D = bg::model::point<float, 2, bg::cs::cartesian>;

template <typename PairFirst, typename PairSecond>
struct pair_maker
{
	using PairResult = std::pair<PairFirst, PairSecond>;

	template<typename T> inline PairResult operator()(T const& v) const
	{
		return PairResult(v.value(), v.index());
	}
};

template <typename point_model, typename VectElementIn>
struct point_maker
{
	template<typename VectElementIn>
	inline point_model operator()(VectElementIn const& v)
	{
		return point_model(v[0], v[1]);
	}
};

template <typename point_model>
struct PrintEachPoint {
	inline void operator()(point_model const& v)
	{
		std::cout << bg::wkt(v) << std::endl;
	}
};

template<class InputIt, class OutputIt, class BinOperation>
OutputIt transform2points(InputIt first1, InputIt last1, OutputIt d_first, BinOperation twobytwo_op)
{
	while (first1 != last1)
		*d_first++ = twobytwo_op(*first1++, *first1++);

	return d_first;
}

std::string boolstr(bool&& v)
{
	return v ? "true" : "false";
}

using  Point_xy = XY2D;
using  Container = std::vector<Point_xy>;
using  RTValue = std::pair<Point_xy, Container::size_type>;
using  RTValueMaker = pair_maker<Point_xy, Container::size_type>;
using  PointMaker = point_maker<Point_xy, std::vector<float>>;
using  Poly_xy = bg::model::polygon <Point_xy, bg::clockwise, bg::closed>;
using  Ring_xy = bg::model::ring <Point_xy, bg::clockwise, bg::closed>;
using  Pointlist_xy = std::vector<Point_xy>;
using  Rtree_t = bgi::rtree< RTValue, bgi::quadratic<16> >;




int main()
{


	double apts[][2]{
		  {0,0},{1,1},{2,2},{3,1},{5,0},
		  {3,-1},{2,-2},{1,-1},{0,0}
	};

	double bpts[][2]{
		  {1,0},{2,1},{3,2},{4,1},{6,0},
		  {4,-1},{3,-2},{2,-1},{1,0}
	};

	std::vector<float> pts{
		3,0,
		4,1,
		4,0
	};

	Poly_xy apoly;
	Poly_xy bpoly;
	Poly_xy ppoly;
	Poly_xy ptspoly;
	Ring_xy ptsring;
	//Pointlist_xy ptslist;
	

	bg::assign_points(apoly, apts);
	bg::assign_points(bpoly, bpts);
	

	Container ptsvec;

	auto ptassign = [](auto& v2, auto& v1)->Point_xy {return Point_xy(v1, v2);};

	transform2points(pts.cbegin(), pts.cend(), std::back_inserter(ptspoly.outer()), ptassign);
	transform2points(pts.cbegin(), pts.cend(), std::back_inserter(ptsring), ptassign);

	auto ptslist = ptsring | bga::indexed(0) | bga::transformed(RTValueMaker());

	for (const auto & element: ptslist) {
		std::cout << "My list el: "
			<< bg::wkt(element.first)
			<< " : "
			<< element.second
			<< std::endl;
	}


		


	//boost::for_each(pts, point_maker<Point_xy,size_t>);

	Point_xy p1(3, 0);
	Point_xy p2(4, 1);
	Point_xy p3(4, 0);

	std::cout << bg::wkt(p1) << std::endl;
	std::cout << bg::wkt(apoly) << std::endl;
	std::cout << boolstr(bg::intersects(p1, apoly)) << std::endl;
	std::cout << bg::wkt(p2) << std::endl;
	std::cout << bg::wkt(bpoly) << std::endl;
	std::cout << boolstr(bg::intersects(p2, bpoly)) << std::endl;

	auto printeachpoint = [](const auto& pt) {std::cout << bg::wkt(pt) << std::endl; };

	boost::for_each(ptsvec, printeachpoint);
	boost::for_each(ptsvec, PrintEachPoint<Point_xy>());




	// create a container of boxes
	Container points;

	points.push_back(p1);
	points.push_back(p2);
	points.push_back(p3);

	// create the rtree passing a Range
	Rtree_t rtree(points | bga::indexed() | bga::transformed(RTValueMaker()));

	//// print the number of values using boxes[0] as indexable
	std::cout << rtree.count(points[0]) << std::endl;

	std::vector<RTValue> result_n;
	rtree.query(bgi::covered_by<Poly_xy>(apoly), std::back_inserter(result_n));

	boost::range::for_each(result_n, [](RTValue& v) {std::cout << v.second << '\n'; });


	return 0;
}