#ifndef LINE_H
#define LINE_H

#include "constdefine.h"
#include "point.h"

namespace xd {

class Line;
class Linef3;
class Polyline;
typedef std::vector<Line> Lines;

class Line   //这里定义Line类，包含了两个Point类
{
    public:
    Point a;
    Point b;
    Line() {};
    explicit Line(Point _a, Point _b): a(_a), b(_b) {};
    std::string wkt() const;   //返回以LINESTRING(a.x a.y,b.x b.y)的形式生成的字符串
    operator Lines() const;   //类型转换函数，三类不需返回值的函数之一。将Line类转换为Lines类。即Lines类里加入this指向的一个Line类
    operator Polyline() const;  //同上，只是Line类转换为Polyline类。即Polyline类的Points加入a，b点
    void scale(double factor);  //Line的两个点都放大factor倍
    void translate(double x, double y);   //Line的两个点都平移即加上x，y的值
    void rotate(double angle, const Point &center); //Line的两个点都绕center逆时针旋转angle度
    void reverse();  //点a和点b交换
    double length() const;  //返回Line的长度
    Point midpoint() const;  //返回Line的中点
    void point_at(double distance, Point* point) const;  //指针point指向：由a→b方向上走distance个单位的点，distance=0时即a点
    Point point_at(double distance) const;  //同上，只是返回上面的point点
    bool intersection_infinite(const Line &other, Point* point) const;  //返回fault说明两线段基本平行，否则point指向两线段交点。
    //注意：上面的函数实现当两条线段不相交时，还没有推出来point是否指向延长线交点！！！
    bool coincides_with(const Line &line) const;  //判断两个线段是否相同
    double distance_to(const Point &point) const;  //返回类本身线段到给定点最近的距离，如果线段是一个点，则返回到点到点的距离
    bool parallel_to(double angle) const;
    bool parallel_to(const Line &line) const;
    double atan2_() const;  //返回线段和水平线夹角的弧度值，取值范围介于 -pi 到 pi 之间（不包括 -pi）
    double orientation() const;  //返回线段和水平线夹角的弧度值，取值范围介于 0 到 2pi 之间（不包括 0）。表示线段的指向
    double direction() const;    //返回线段和水平线夹角的弧度值，结果如果是(-pi,0)或者pi时就加pi，相当于取相反方向，即得数在[0,pi)上。表示线段的方向
    Vector vector() const;  //返回本身的向量值
    Vector normal() const;  //返回本身的法向量，即和本身向量垂直的向量
 };

class Linef    //建立double型的线段
{
    public:
    Pointf a;
    Pointf b;
    Linef() {};
    explicit Linef(Pointf _a, Pointf _b): a(_a), b(_b) {};
};

class Linef3   //建立double型3维线段
{
    public:
    Pointf3 a;
    Pointf3 b;
    Linef3() {};
    explicit Linef3(Pointf3 _a, Pointf3 _b): a(_a), b(_b) {};
    Pointf3 intersect_plane(double z) const; //返回线段和Z方向坐标值为z的平面的交点
    void scale(double factor);  //线段两个端点的坐标都放大factor倍

};

}

#endif // LINE_H

