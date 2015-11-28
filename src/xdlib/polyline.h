#ifndef POLYLINE
#define POLYLINE

#include "line.h"
#include "multiPoint.h"
#include <string>

namespace xd {
class ExPolygon;
class Polyline;
typedef std::vector<Polyline> Polylines;

class Polyline : public MultiPoint {
    public:
    operator Polylines() const;  //类型转换函数，将Polyline转换为Polylines类型
    operator Line() const;  //类型转换函数，将Polyline转换为Line类型
    Point last_point() const;   //实现父类虚函数，返回本身Points中的最后一个点
    Point leftmost_point() const;  //返回Points中x坐标最小的点，即最左边的点
    Lines lines() const;  //将Points中的点每两个一组存到Lines里，比如5个点的Polyline可以生成4根线段
    void clip_end(double distance);  //从最后一个点减去distance距离的线段，如果distance大于总线段长度，则Points为空
    void clip_start(double distance);  //同上，只不过从前面减去distance距离
    void extend_end(double distance);  //结尾线段延长distance的距离，延长方向就是最后一跟线段的方向。
    void extend_start(double distance);  //同上，开头线段延长
    Points equally_spaced_points(double distance) const;  //每隔均匀的距离取一个点，距离指Points线段之和（有第一个点，未必有最后一个点！只有当所有点的距离之和等于distance的整数倍时才有最后一个点 *有时间再看！！）
    void simplify(double tolerance);  //化简多线段，只要误差弦高小于tolerance即可。使用了父类的_douglas_peucker函数实现
    template <class T> void simplify_by_visibility(const T &area);/* This method simplifies all *lines* contained in the supplied area */ //将存在于area（推测area可能不止一块）中的所有点删除！
    void split_at(const Point &point, Polyline* p1, Polyline* p2) const;  //把Points从point到其最近的点分开成两个Polyline，第一个最后一个点是point；第二个第一个点是point
    bool is_straight() const;  //如果polyline中所有线段中有一个和由第一个点和第二个点组成的线段平行，则返回true，否则返回false
    std::string wkt() const;  //LINESTRING((x1 y1,x2 y2,...,xn,yn))的字符串形式返回

};

}

#endif // POLYLINE

