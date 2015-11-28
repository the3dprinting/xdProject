#include "geometry.h"
//#include "ClipperUtils.hpp"
//#include "ExPolygon.hpp"
#include "line.h"
//#include "PolylineCollection.hpp"
#include "clipper.hpp"
#include <algorithm>
#include <cmath>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <assert.h>

namespace xd { namespace Geometry {

static bool
sort_points (Point a, Point b)  //一种排序方案，局部函数用来实现下面的某个函数，a点在b点左边或者垂直下面则为真
{
    return (a.x < b.x) || (a.x == b.x && a.y < b.y);
}

//基于Andrew's monotone chain 2D凸包算法
Polygon
convex_hull(Points points)
{
    assert(points.size() >= 3);
    // 排列输入点
    std::sort(points.begin(), points.end(), sort_points);

    int n = points.size(), k = 0;
    Polygon hull;
    hull.points.resize(2*n);

    // 创建低层包
    for (int i = 0; i < n; i++) {
        while (k >= 2 && points[i].ccw(hull.points[k-2], hull.points[k-1]) <= 0) k--;
        hull.points[k++] = points[i];
    }

    // 创建高层包
    for (int i = n-2, t = k+1; i >= 0; i--) {
        while (k >= t && points[i].ccw(hull.points[k-2], hull.points[k-1]) <= 0) k--;
        hull.points[k++] = points[i];
    }

    hull.points.resize(k);

    assert( hull.points.front().coincides_with(hull.points.back()) );
    hull.points.pop_back();

    return hull;
}

Polygon
convex_hull(const Polygons &polygons)  //找出一堆多边形的总凸包
{
    Points pp;
    for (Polygons::const_iterator p = polygons.begin(); p != polygons.end(); ++p) {
        pp.insert(pp.end(), p->points.begin(), p->points.end());   //在pp中插入区间上的元素
    }
    return convex_hull(pp);
}

bool
directions_parallel(double angle1, double angle2, double max_diff)
{
    double diff = fabs(angle1 - angle2);
    max_diff += EPSILON;
    return diff < max_diff || fabs(diff - PI) < max_diff;
}

}}
