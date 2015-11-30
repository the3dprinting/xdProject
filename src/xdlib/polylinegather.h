#ifndef POLYLINEGATHER_H
#define POLYLINEGATHER_H

#include "constdefine.h"
#include "polyline.h"

namespace xd {

class PolylineCollection
{
    public:
    Polylines polylines;
    void chained_path(PolylineCollection* retval, bool no_reverse = false) const;  //同下，只是起始点为第一个poliline的第一个点
    void chained_path_from(Point start_near, PolylineCollection* retval, bool no_reverse = false) const;//从start_near开始找到polylines
    //里的polyline中最近的起始点，然后把这个polyline的最终点当成下一个起始点，最终规划好的polyline存储到retval里面，bool为真表示规划时
    //polyline可以翻转，即一个polyline里面如果最终点是最近点则可以翻转这个polyline
    Point leftmost_point() const;  //返回Polylines里面最左边的点
};

}

#endif // POLYLINEGATHER_H

