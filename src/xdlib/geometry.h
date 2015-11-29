#ifndef GEOMETRY
#define GEOMETRY

#include "boundingBox.h"
#include "polygon.h"
#include "polyline.h"

namespace xd { namespace Geometry {  //此处又开辟一个命名空间，估计有一些全局函数在里面

Polygon convex_hull(Points points);   //生成离散点的凸包
Polygon convex_hull(const Polygons &polygons);   //生成多个多边形的总的凸包
void chained_path(const Points &points, std::vector<Points::size_type> &retval, Point start_near); //从start_near点开始，依次寻找points中距离前一个点最近的点在points角标值，记录在retval中
void chained_path(const Points &points, std::vector<Points::size_type> &retval); //同上，只不过第一个点时points.front()
template<class T> void chained_path_items(Points &points, T &items, T &retval);   //retval返回将points排序后角标对应items的值，不理解？
bool directions_parallel(double angle1, double angle2, double max_diff = 0);  //返回两个角度之差是否在误差允许的范围内为0度或180度，即平行


}}
#endif // GEOMETRY

