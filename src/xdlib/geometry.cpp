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
sort_points (Point a, Point b)  //һ�����򷽰����ֲ���������ʵ�������ĳ��������a����b����߻��ߴ�ֱ������Ϊ��
{
    return (a.x < b.x) || (a.x == b.x && a.y < b.y);
}

//����Andrew's monotone chain 2D͹���㷨
Polygon
convex_hull(Points points)
{
    assert(points.size() >= 3);
    // ���������
    std::sort(points.begin(), points.end(), sort_points);

    int n = points.size(), k = 0;
    Polygon hull;
    hull.points.resize(2*n);

    // �����Ͳ��
    for (int i = 0; i < n; i++) {
        while (k >= 2 && points[i].ccw(hull.points[k-2], hull.points[k-1]) <= 0) k--;
        hull.points[k++] = points[i];
    }

    // �����߲��
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
convex_hull(const Polygons &polygons)  //�ҳ�һ�Ѷ���ε���͹��
{
    Points pp;
    for (Polygons::const_iterator p = polygons.begin(); p != polygons.end(); ++p) {
        pp.insert(pp.end(), p->points.begin(), p->points.end());   //��pp�в��������ϵ�Ԫ��
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
