#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "constdefine.h"
#include "point.h"
#include "polygon.h"

namespace xd {

typedef Point   Size;
typedef Point3  Size3;
typedef Pointf  Sizef;
typedef Pointf3 Sizef3;

template <class PointClass>
class BoundingBoxBase
{
    public:
    PointClass min;
    PointClass max;
    bool defined;

    BoundingBoxBase() : defined(false) {};   //初始化构造函数，没有传入数据，因此标志符defined为假！
    BoundingBoxBase(const std::vector<PointClass> &points);  //传入一系列点，构造完后就给自身的min、max数值添加完毕，因此defined为真！
    void merge(const PointClass &point);  //加上一个点，并更新min和max，如果开始没有点，则把min和max都设置为point
    void merge(const std::vector<PointClass> &points);  //加上points后，更新得到的min和max，实现调用了下面的函数
    void merge(const BoundingBoxBase<PointClass> &bb);  //加上新的bb后，更新min和max
    void scale(double factor);   //将min和max放大一个factor倍
    PointClass size() const;   //返回包围盒总长和宽组成的PointClass类，肯是一个点
    void translate(coordf_t x, coordf_t y);   //将min和max平移(x,y)
    void offset(coordf_t delta);  //min平移（-delta，-delta），max平移（delta，delta）。相当于偏置扩大了delta
    PointClass center() const;   //返回包围盒的中心点的PointClass类
};

template <class PointClass>
class BoundingBox3Base : public BoundingBoxBase<PointClass>
{
    public:
    BoundingBox3Base() : BoundingBoxBase<PointClass>() {};    //同BoundingBoxBase
    BoundingBox3Base(const std::vector<PointClass> &points);  //多了一个z的值
    void merge(const PointClass &point);  //同二维，多了z
    void merge(const std::vector<PointClass> &points);  //同二维，多了z
    void merge(const BoundingBox3Base<PointClass> &bb);  //同二维，多了z
    PointClass size() const;    //返回包围盒总长，宽，高组成的PointClass类，肯是一个点
    void translate(coordf_t x, coordf_t y, coordf_t z);   //同二维，多了z
    void offset(coordf_t delta);  //同二维，多了z
    PointClass center() const;    //同二维，多了z
};

class BoundingBox : public BoundingBoxBase<Point>
{
    public:
    void polygon(Polygon* polygon) const;   //将包围盒的四个点赋值给polygon
    Polygon polygon() const;   //将包围盒的四个点赋值给一个polygon，然后返回

    BoundingBox() : BoundingBoxBase<Point>() {};
    BoundingBox(const Points &points) : BoundingBoxBase<Point>(points) {};
    BoundingBox(const Lines &lines);   //将一系列的line的两个端点作为输入点的构造函数
};

/*
class BoundingBox3  : public BoundingBox3Base<Point3> {};
*/

class BoundingBoxf : public BoundingBoxBase<Pointf> {
    public:
    BoundingBoxf() : BoundingBoxBase<Pointf>() {};
    BoundingBoxf(const std::vector<Pointf> &points) : BoundingBoxBase<Pointf>(points) {};
};

class BoundingBoxf3 : public BoundingBox3Base<Pointf3> {
    public:
    BoundingBoxf3() : BoundingBox3Base<Pointf3>() {};
    BoundingBoxf3(const std::vector<Pointf3> &points) : BoundingBox3Base<Pointf3>(points) {};
};

}

#endif // BOUNDINGBOX_H

