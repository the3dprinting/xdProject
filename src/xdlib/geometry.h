#ifndef GEOMETRY
#define GEOMETRY

#include "boundingBox.h"
#include "polygon.h"
#include "polyline.h"
#include "../boost/polygon/voronoi.hpp"
using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;

namespace xd { namespace Geometry {  //此处又开辟一个命名空间，估计有一些全局函数在里面

Polygon convex_hull(Points points);   //生成离散点的凸包
Polygon convex_hull(const Polygons &polygons);   //生成多个多边形的总的凸包
void chained_path(const Points &points, std::vector<Points::size_type> &retval, Point start_near); //从start_near点开始，依次寻找points中距离前一个点最近的点在points角标值，记录在retval中
void chained_path(const Points &points, std::vector<Points::size_type> &retval); //同上，只不过第一个点时points.front()
template<class T> void chained_path_items(Points &points, T &items, T &retval);   //retval返回将points排序后角标对应items的值，不理解？
bool directions_parallel(double angle1, double angle2, double max_diff = 0);  //返回两个角度之差是否在误差允许的范围内为0度或180度，即平行
template<class T> bool contains(const std::vector<T> &vector, const Point &point);  //返回T类型组成的vector是否包含point，T推测为polygon等
double rad2deg(double angle);  //弧度转换为角度返回
double rad2deg_dir(double angle);  //返回和传入弧度垂直的弧度的角度值
double deg2rad(double angle);  //角度转换为弧度返回
void simplify_polygons(const Polygons &polygons, double tolerance, Polygons* retval);

class ArrangeItem {
    public:
    Pointf pos;
    size_t index_x, index_y;
    coordf_t dist;
};
class ArrangeItemIndex {
    public:
    coordf_t index;
    ArrangeItem item;
    ArrangeItemIndex(coordf_t _index, ArrangeItem _item) : index(_index), item(_item) {};
};

double linint(double value, double oldmin, double oldmax, double newmin, double newmax);
Pointfs arrange(size_t total_parts, Pointf part, coordf_t dist, const BoundingBoxf &bb = BoundingBoxf());

class MedialAxis {
    public:
    Points points;
    Lines lines;
    double max_width;
    double min_width;
    MedialAxis(double _max_width, double _min_width) : max_width(_max_width), min_width(_min_width) {};
    void build(Polylines* polylines);

    private:
    typedef voronoi_diagram<double> VD;
    VD vd;
    std::set<const VD::edge_type*> edges;
    Line edge_to_line(const VD::edge_type &edge) const;
    void process_edge_neighbors(const voronoi_diagram<double>::edge_type& edge, Points* points);
    bool is_valid_edge(const voronoi_diagram<double>::edge_type& edge) const;
    const Line& retrieve_segment(const voronoi_diagram<double>::cell_type& cell) const;
};

}}
#endif // GEOMETRY

