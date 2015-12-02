#ifndef MULTIPOINT_H
#define MULTIPOINT_H

#include "constdefine.h"
#include <algorithm>
#include <vector>
#include "line.h"
#include "point.h"

namespace xd {

class BoundingBox;

class MultiPoint  //定义多重点类，包含一个Point类组成的向量Points
{
    public:
    Points points;

    operator Points() const;
    MultiPoint() {};
    explicit MultiPoint(const Points &_points): points(_points) {};
    void scale(double factor);  //所有点的坐标值都放大factor倍
    void translate(double x, double y);  //所有点的坐标都平移加上x,y的值
    void translate(const Point &vector);  //同上，只是有向量形式的加
    void rotate(double angle, const Point &center);  //所有点都以center为中心选择angle度
    void reverse();  //所有点排列方向反向
    Point first_point() const;  //返回存储的第一个点
    virtual Point last_point() const = 0;  //留给子类用来实现返回最后一个点
    virtual Lines lines() const = 0;  //留给其子类用来实现点变为线段组成的向量
    double length() const;  //为子类实现返回所有线段的总长度
    bool is_valid() const;  //返回存储的点数是否大于等于2个
    int find_point(const Point &point) const;  //返回与point相当的点的角标，返回-1说明没有相等的点
    bool has_boundary_point(const Point &point) const;  //返回point是否近似在this组成的轮廓上，*this强制转换成了Points
    BoundingBox bounding_box() const;  //返回由points组成的包围盒类
    void remove_duplicate_points();  //删除相等的点，实现方法很好，佩服！
    void append(const Point &point);  //增加上一个点
    void append(const Points &points);  //增加上好多点
    void append(const Points::const_iterator &begin, const Points::const_iterator &end);  //以指针的形式加上好多点

    static Points _douglas_peucker(const Points &points, const double tolerance);  //在满足弦高误差的情况下化简多点线段。
                                                                                   //参考https://en.wikipedia.org/wiki/Ramer–Douglas–Peucker_algorithm
};

}

#endif // MULTIPOINT_H

