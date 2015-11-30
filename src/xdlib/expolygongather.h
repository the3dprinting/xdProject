#ifndef EXPOLYGONGATHER
#define EXPOLYGONGATHER

#include "constdefine.h"
#include "exPolygon.h"
#include "line.h"
#include "polyline.h"

namespace xd {

class ExPolygonCollection;
typedef std::vector<ExPolygonCollection> ExPolygonCollections;   //ExPolygonCollectin����ɵ�vector

class ExPolygonCollection
{
    public:
    ExPolygons expolygons;    //��������������typedef std::vector<ExPolygon> ExPolygons

    ExPolygonCollection() {};
    ExPolygonCollection(const ExPolygon &expolygon);
    ExPolygonCollection(const ExPolygons &expolygons) : expolygons(expolygons) {};
    operator Points() const;  //����ת�����������ر����г�Աexpolygons������е�
    operator Polygons() const;  //����ת�����������ر����г�Աexpolygons������ж����
    operator ExPolygons&();   //����ת�����������ر����г�Աexpolygons
    void scale(double factor);   //�������г�Աexpolygons�Ŵ�factor��
    void translate(double x, double y);   //�������г�Աexpolygonsƽ��x y����
    void rotate(double angle, const Point &center);   //�������г�Աexpolygons��center��ʱ����תangle
    template <class T> bool contains(const T &item) const;  //����expolygons�Ƿ����item�ĺ���������Ϊ�棬�����
    bool contains_b(const Point &point) const;   //���Ƿ��������ڻ��ڱ߽��϶�����true
    void simplify(double tolerance);   //�������г�Աexpolygons�����Ҹ����Ϊtolerance��
    Polygon convex_hull() const;     //���ر����г�Աexpolygons��͹��
    Lines lines() const;   //��expolygons��������еĵ㰴���߶η���
    Polygons contours() const;  //��expolygons�����������������ɵ������鷵��
};

}

#endif // EXPOLYGONGATHER

