#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <string>
#include "line.h"
#include "multiPoint.h"
#include "polyline.h"

namespace xd {

class Polygon;
typedef std::vector<Polygon> Polygons;
//注意：由后面的函数实现推出，Polygon里的Points数据的最后一个点不是第一个点，但使用此类中的函数都实现了多边形封闭的特性，即lastP=firstP
class Polygon : public MultiPoint {
    public:
    operator Polygons() const;  //类型转换函数，转换为Polygons的类型
    operator Polyline() const;  //类型转换函数，转换为Polyline的类型，其中lastP=firstP
    Point& operator[](Points::size_type idx);  //重载操作符[],返回对应角标的点
    const Point& operator[](Points::size_type idx) const;  //同上，不过返回一个const点，不能修改点的值

    Polygon() {};
    explicit Polygon(const Points &points): MultiPoint(points) {};  //必须通过传递points类型来构造Polygon类
    Point last_point() const;  //返回第一个点，同时也即是最后一个点
    Lines lines() const;  //将points转换为lines类来返回，即一个封闭多边形的所有边,points最后一个点不是第一个点
    Polyline split_at_vertex(const Point &point) const;  //由多边形的指定点作为Points的起点返回Polyline类，返回的lastP=firstP，若没有此点，则从开始点分开
    Polyline split_at_index(int index) const;  //同上，但是由角标值确定起始点位置
    Polyline split_at_first_point() const;  //返回Polyline的类型，其中lastP=firstP
    Points equally_spaced_points(double distance) const;  //每隔均匀的距离取一个点，距离指Points线段之和（有第一个点，未必有最后一个点）
    double area() const;  //调用clipper库返回多边形面积
    bool is_counter_clockwise() const;  //返回多边形是否为逆时针
    bool is_clockwise() const;  //返回多边形是否为顺时针
    bool make_counter_clockwise();  //使多边形逆时针
    bool make_clockwise();  //使多边形顺时针
    bool is_valid() const;  //有效多边形指包含的点至少三个点，因为points里面lastP！=firstP
    bool contains(const Point &point) const;  //判断点是否在多边形内———参考：http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
    Polygons simplify(double tolerance) const;  //按弦高小于tolerance简化多边形类里的points，最后的到Polygons
    void simplify(double tolerance, Polygons &polygons) const;  //在polygons后加入本身类中points以tolerance简化后的多边形
    void triangulate_convex(Polygons* polygons) const;  //将自身points第一个点和其余依次两点组成的三角形加入到polygons里面（逆时针才会加入）
    Point centroid() const;  //返回多边形质心，平均密度也就是就是形心——参考：http://en.wikipedia.org/wiki/Centroid#Centroid_of_polygon
    std::string wkt() const;  //以POLYGON((x1 y1,x2 y2,...xn yn))形式的字符串返回
    Points concave_points(double angle = PI) const;  //找出所有凹顶点   实在不懂为啥！
    Points convex_points(double angle = PI) const;  //找出所有凸顶点

};

}

#endif // POLYGON_H

