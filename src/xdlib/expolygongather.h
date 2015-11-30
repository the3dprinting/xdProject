#ifndef EXPOLYGONGATHER
#define EXPOLYGONGATHER

#include "constdefine.h"
#include "exPolygon.h"
#include "line.h"
#include "polyline.h"

namespace xd {

class ExPolygonCollection;
typedef std::vector<ExPolygonCollection> ExPolygonCollections;   //ExPolygonCollectin类组成的vector

class ExPolygonCollection
{
    public:
    ExPolygons expolygons;    //其他代码里面有typedef std::vector<ExPolygon> ExPolygons

    ExPolygonCollection() {};
    ExPolygonCollection(const ExPolygon &expolygon);
    ExPolygonCollection(const ExPolygons &expolygons) : expolygons(expolygons) {};
    operator Points() const;  //类型转换函数，返回本身公有成员expolygons里的所有点
    operator Polygons() const;  //类型转换函数，返回本身公有成员expolygons里的所有多边形
    operator ExPolygons&();   //类型转换函数，返回本身公有成员expolygons
    void scale(double factor);   //将本身公有成员expolygons放大factor倍
    void translate(double x, double y);   //将本身公有成员expolygons平移x y距离
    void rotate(double angle, const Point &center);   //将本身公有成员expolygons绕center逆时针旋转angle
    template <class T> bool contains(const T &item) const;  //返回expolygons是否包含item的函数，包含为真，否则假
    bool contains_b(const Point &point) const;   //点是否在轮廓内或在边界上都返回true
    void simplify(double tolerance);   //将本身公有成员expolygons按照弦高误差为tolerance简化
    Polygon convex_hull() const;     //返回本身公有成员expolygons的凸包
    Lines lines() const;   //将expolygons里面的所有的点按照线段返回
    Polygons contours() const;  //将expolygons里面的所有外轮廓组成的向量组返回
};

}

#endif // EXPOLYGONGATHER

