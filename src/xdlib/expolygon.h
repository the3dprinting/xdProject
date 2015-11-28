#ifndef EXPOLYGON_H
#define EXPOLYGON_H

#include "polygon.h"
#include "polyline.h"
#include <vector>

namespace xd {

class ExPolygon;
typedef std::vector<ExPolygon> ExPolygons;

class ExPolygon  //���������ϵĶ���Σ���һ�������������ɸ�����ɵ�
{
    public:
    Polygon contour;
    Polygons holes;
    operator Points() const;  //����ת�������������Ͷ������еĵ㣨ע�⸴ϰ����ת��������
    operator Polygons() const;  //����ת������������Polydons��һ����������������һ���Ƕ�
    void scale(double factor);  //�Ŵ�factor��
    void translate(double x, double y);  //ƽ��x y����
    void rotate(double angle, const Point &center);  //��center��ʱ����תangle
    double area() const;  //�����=�������-�����
    bool is_valid() const;  //��Ч�������㣬��������ʱ�룬��˳ʱ�룬ͬʱ��ֻ��3����
    bool contains(const Line &line) const;  //����line�Ƿ��ڶ�����ڲ�
    bool contains(const Polyline &polyline) const;  //����һ�����߶��Ƿ��ڶ�����ڲ�
    bool contains(const Point &point) const;   //���Ƿ���������
    bool contains_b(const Point &point) const;  //���Ƿ��������ڻ��ڱ߽��϶�����true
    bool has_boundary_point(const Point &point) const;  //point�Ƿ��������߽���
    Polygons simplify_p(double tolerance) const;  //�����Ҹ����Ϊtolerance�򻯶���Σ�����Polygons���ɣ���һ��Ϊ�����������Ϊ��
    ExPolygons simplify(double tolerance) const;  //�����Ҹ����Ϊtolerance�򻯶���Σ�����ExPolygons
    void simplify(double tolerance, ExPolygons &expolygons) const;  //������򻯺��Expolygons�����expolygons�ĺ���
    void medial_axis(double max_width, double min_width, Polylines* polylines) const;  //���������ߣ�������polylines����
    void get_trapezoids(Polygons* polygons) const;  //������Ķ���μ��Ϸ�Ϊһ���ı��Σ�����polygons����
    void get_trapezoids(Polygons* polygons, double angle) const;  //��angle�Ƕȷ��򻮷��ı��η���polygons����
    void get_trapezoids2(Polygons* polygons) const;   //����������x������ÿ��������y�����ӳ���ͱ����ཻ��ͼ�λ���Ϊһ�Ѷ���β��洢��polygons��
    void get_trapezoids2(Polygons* polygons, double angle) const;  //��һ���Ƕȷָ���������溯��
    void triangulate(Polygons* polygons) const;  //�Ȱ������溯���ı��λ��������ǻ�
    void triangulate_pp(Polygons* polygons) const;  //https://github.com/ivanfratric/polypartition�����Triangulate_MONO���ǻ�
    void triangulate_p2t(Polygons* polygons) const;   //http://code.google.com/p/poly2tri/������ǻ������������ǻ�
    Lines lines() const;  //��������������е�ת��Ϊlines�������أ���һ����������ε����б�

};

}

#endif // EXPOLYGON_H

