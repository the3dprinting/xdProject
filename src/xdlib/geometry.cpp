#include "geometry.h"
#include "encapsulationClipper.h"
#include "exPolygon.h"
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
#include <QDebug>

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

//接受一系列点，根据最近路径原则返回目录列表
void
chained_path(const Points &points, std::vector<Points::size_type> &retval, Point start_near)
{
    PointConstPtrs my_points;
    std::map<const Point*,Points::size_type> indices;
    my_points.reserve(points.size());
    for (Points::const_iterator it = points.begin(); it != points.end(); ++it) {
        my_points.push_back(&*it);   //这里实在看不懂，为啥取指向内容的地址，多此一举么？直接用it不行么？！
        indices[&*it] = it - points.begin();   //map<绝对地址（可以取地址内容）→相对地址（代表角标）>
    }

    retval.reserve(points.size());
    while (!my_points.empty()) {
        Points::size_type idx = start_near.nearest_point_index(my_points);
        start_near = *my_points[idx];
        retval.push_back(indices[ my_points[idx] ]);    //这里取出了map类型indices里对应point地址的角标值
        my_points.erase(my_points.begin() + idx);
    }
}

void
chained_path(const Points &points, std::vector<Points::size_type> &retval)
{
    if (points.empty()) return;  // can't call front() on empty vector
    chained_path(points, retval, points.front());
}

/* retval and items must be different containers */
template<class T>
void
chained_path_items(Points &points, T &items, T &retval)
{
    std::vector<Points::size_type> indices;
    chained_path(points, indices);
    for (std::vector<Points::size_type>::const_iterator it = indices.begin(); it != indices.end(); ++it)
        retval.push_back(items[*it]);
}
template void chained_path_items(Points &points, ClipperLib::PolyNodes &items, ClipperLib::PolyNodes &retval);

bool
directions_parallel(double angle1, double angle2, double max_diff)
{
    double diff = fabs(angle1 - angle2);
    max_diff += EPSILON;
    return diff < max_diff || fabs(diff - PI) < max_diff;
}

template<class T>
bool
contains(const std::vector<T> &vector, const Point &point)
{
    for (typename std::vector<T>::const_iterator it = vector.begin(); it != vector.end(); ++it) {
        if (it->contains(point)) return true;
    }
    return false;
}
template bool contains(const ExPolygons &vector, const Point &point);

double
rad2deg(double angle)
{
    return angle / PI * 180.0;
}

double
rad2deg_dir(double angle)
{
    angle = (angle < PI) ? (-angle + PI/2.0) : (angle + PI/2.0);
    if (angle < 0) angle += PI;
    return rad2deg(angle);
}

double
deg2rad(double angle)
{
    return PI * angle / 180.0;
}

void
simplify_polygons(const Polygons &polygons, double tolerance, Polygons* retval)
{
    Polygons pp;
    for (Polygons::const_iterator it = polygons.begin(); it != polygons.end(); ++it) {
        Polygon p = *it;
        p.points.push_back(p.points.front());
        p.points = MultiPoint::_douglas_peucker(p.points, tolerance);
        p.points.pop_back();
        pp.push_back(p);
    }
    xd::simplify_polygons(pp, retval);  //这个函数调用了封装clipper里面的一个函数
}

double
linint(double value, double oldmin, double oldmax, double newmin, double newmax)
{
    return (value - oldmin) * (newmax - newmin) / (oldmax - oldmin) + newmin;
}

Pointfs
arrange(size_t total_parts, Pointf part, coordf_t dist, const BoundingBoxf &bb)
{
    // use actual part size (the largest) plus separation distance (half on each side) in spacing algorithm
    part.x += dist;
    part.y += dist;

    Pointf area;
    if (bb.defined) {
        area = bb.size();
    } else {
        // bogus area size, large enough not to trigger the error below
        area.x = part.x * total_parts;
        area.y = part.y * total_parts;
    }

    // this is how many cells we have available into which to put parts
    size_t cellw = floor((area.x + dist) / part.x);
    size_t cellh = floor((area.x + dist) / part.x);

    if (total_parts > (cellw * cellh))
        qDebug("%zu parts won't fit in your print area!\n",total_parts);

    // total space used by cells
    Pointf cells(cellw * part.x, cellh * part.y);

    // bounding box of total space used by cells
    BoundingBoxf cells_bb;
    cells_bb.merge(Pointf(0,0)); // min
    cells_bb.merge(cells);  // max

    // center bounding box to area
    cells_bb.translate(
        -(area.x - cells.x) / 2,
        -(area.y - cells.y) / 2
    );

    // list of cells, sorted by distance from center
    std::vector<ArrangeItemIndex> cellsorder;

    // work out distance for all cells, sort into list
    for (size_t i = 0; i <= cellw-1; ++i) {
        for (size_t j = 0; j <= cellh-1; ++j) {
            coordf_t cx = linint(i + 0.5, 0, cellw, cells_bb.min.x, cells_bb.max.x);
            coordf_t cy = linint(j + 0.5, 0, cellh, cells_bb.max.y, cells_bb.min.y);

            coordf_t xd = fabs((area.x / 2) - cx);
            coordf_t yd = fabs((area.y / 2) - cy);

            ArrangeItem c;
            c.pos.x = cx;
            c.pos.y = cy;
            c.index_x = i;
            c.index_y = j;
            c.dist = xd * xd + yd * yd - fabs((cellw / 2) - (i + 0.5));

            // binary insertion sort
            {
                coordf_t index = c.dist;
                size_t low = 0;
                size_t high = cellsorder.size();
                while (low < high) {
                    size_t mid = (low + ((high - low) / 2)) | 0;
                    coordf_t midval = cellsorder[mid].index;

                    if (midval < index) {
                        low = mid + 1;
                    } else if (midval > index) {
                        high = mid;
                    } else {
                        cellsorder.insert(cellsorder.begin() + mid, ArrangeItemIndex(index, c));
                        goto ENDSORT;
                    }
                }
                cellsorder.insert(cellsorder.begin() + low, ArrangeItemIndex(index, c));
            }
            ENDSORT: true;
        }
    }

    // the extents of cells actually used by objects
    coordf_t lx = 0;
    coordf_t ty = 0;
    coordf_t rx = 0;
    coordf_t by = 0;

    // now find cells actually used by objects, map out the extents so we can position correctly
    for (size_t i = 1; i <= total_parts; ++i) {
        ArrangeItemIndex c = cellsorder[i - 1];
        coordf_t cx = c.item.index_x;
        coordf_t cy = c.item.index_y;
        if (i == 1) {
            lx = rx = cx;
            ty = by = cy;
        } else {
            if (cx > rx) rx = cx;
            if (cx < lx) lx = cx;
            if (cy > by) by = cy;
            if (cy < ty) ty = cy;
        }
    }
    // now we actually place objects into cells, positioned such that the left and bottom borders are at 0
    Pointfs positions;
    for (size_t i = 1; i <= total_parts; ++i) {
        ArrangeItemIndex c = cellsorder.front();
        cellsorder.erase(cellsorder.begin());
        coordf_t cx = c.item.index_x - lx;
        coordf_t cy = c.item.index_y - ty;

        positions.push_back(Pointf(cx * part.x, cy * part.y));
    }

    if (bb.defined) {
        for (Pointfs::iterator p = positions.begin(); p != positions.end(); ++p) {
            p->x += bb.min.x;
            p->y += bb.min.y;
        }
    }
    return positions;
}





}}
