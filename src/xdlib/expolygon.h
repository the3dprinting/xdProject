#ifndef EXPOLYGON_H
#define EXPOLYGON_H

#include "polygon.h"
#include "polyline.h"
#include <vector>

namespace xd {

class ExPolygon;
typedef std::vector<ExPolygon> ExPolygons;

class ExPolygon  //真正意义上的多边形，有一个外轮廓，若干个洞组成的
{
    public:
    Polygon contour;
    Polygons holes;
    operator Points() const;  //类型转换函数，轮廓和洞的所有的点（注意复习类型转换函数）
    operator Polygons() const;  //类型转换函数，返回Polydons第一个是外轮廓，后面一次是洞
    void scale(double factor);  //放大factor倍
    void translate(double x, double y);  //平移x y距离
    void rotate(double angle, const Point &center);  //绕center逆时针旋转angle
    double area() const;  //总面积=轮廓面积-洞面积
    bool is_valid() const;  //有效必须满足，外轮廓逆时针，洞顺时针，同时都只是3个点
    bool contains(const Line &line) const;  //返回line是否在多边形内部
    bool contains(const Polyline &polyline) const;  //返回一个多线段是否在多边形内部
    bool contains(const Point &point) const;   //点是否在轮廓内
    bool contains_b(const Point &point) const;  //点是否在轮廓内或在边界上都返回true
    bool has_boundary_point(const Point &point) const;  //point是否在轮廓边界上
    Polygons simplify_p(double tolerance) const;  //按照弦高误差为tolerance简化多边形，返回Polygons即可，第一个为轮廓，后面的为洞
    ExPolygons simplify(double tolerance) const;  //按照弦高误差为tolerance简化多边形，返回ExPolygons
    void simplify(double tolerance, ExPolygons &expolygons) const;  //将本身简化后的Expolygons传入的expolygons的后面
    void medial_axis(double max_width, double min_width, Polylines* polylines) const;  //生成中轴线，并放在polylines里面
    void get_trapezoids(Polygons* polygons) const;  //将本身的多边形集合分为一堆四边形，放在polygons里面
    void get_trapezoids(Polygons* polygons, double angle) const;  //以angle角度方向划分四边形放在polygons里面
    void get_trapezoids2(Polygons* polygons) const;   //将本身沿着x正方向每两个点在y方向延长后和本身相交的图形划分为一堆多边形并存储到polygons里
    void get_trapezoids2(Polygons* polygons, double angle) const;  //以一定角度分割本身，调用上面函数
    void triangulate(Polygons* polygons) const;  //先按照上面函数四边形化，再三角化
    void triangulate_pp(Polygons* polygons) const;  //https://github.com/ivanfratric/polypartition里面的Triangulate_MONO三角化
    void triangulate_p2t(Polygons* polygons) const;   //http://code.google.com/p/poly2tri/里的三角化方法进行三角化
    Lines lines() const;  //将带洞多边形所有点转换为lines类来返回，即一个带洞多边形的所有边

};

}

#endif // EXPOLYGON_H

