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
//ע�⣺�ɺ���ĺ���ʵ���Ƴ���Polygon���Points���ݵ����һ���㲻�ǵ�һ���㣬��ʹ�ô����еĺ�����ʵ���˶���η�յ����ԣ���lastP=firstP
class Polygon : public MultiPoint {
    public:
    operator Polygons() const;  //����ת��������ת��ΪPolygons������
    operator Polyline() const;  //����ת��������ת��ΪPolyline�����ͣ�����lastP=firstP
    Point& operator[](Points::size_type idx);  //���ز�����[],���ض�Ӧ�Ǳ�ĵ�
    const Point& operator[](Points::size_type idx) const;  //ͬ�ϣ���������һ��const�㣬�����޸ĵ��ֵ

    Polygon() {};
    explicit Polygon(const Points &points): MultiPoint(points) {};  //����ͨ������points����������Polygon��
    Point last_point() const;  //���ص�һ���㣬ͬʱҲ�������һ����
    Lines lines() const;  //��pointsת��Ϊlines�������أ���һ����ն���ε����б�,points���һ���㲻�ǵ�һ����
    Polyline split_at_vertex(const Point &point) const;  //�ɶ���ε�ָ������ΪPoints����㷵��Polyline�࣬���ص�lastP=firstP����û�д˵㣬��ӿ�ʼ��ֿ�
    Polyline split_at_index(int index) const;  //ͬ�ϣ������ɽǱ�ֵȷ����ʼ��λ��
    Polyline split_at_first_point() const;  //����Polyline�����ͣ�����lastP=firstP
    Points equally_spaced_points(double distance) const;  //ÿ�����ȵľ���ȡһ���㣬����ָPoints�߶�֮�ͣ��е�һ���㣬δ�������һ���㣩
    double area() const;  //����clipper�ⷵ�ض�������
    bool is_counter_clockwise() const;  //���ض�����Ƿ�Ϊ��ʱ��
    bool is_clockwise() const;  //���ض�����Ƿ�Ϊ˳ʱ��
    bool make_counter_clockwise();  //ʹ�������ʱ��
    bool make_clockwise();  //ʹ�����˳ʱ��
    bool is_valid() const;  //��Ч�����ָ�����ĵ����������㣬��Ϊpoints����lastP��=firstP
    bool contains(const Point &point) const;  //�жϵ��Ƿ��ڶ�����ڡ������ο���http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
    Polygons simplify(double tolerance) const;  //���Ҹ�С��tolerance�򻯶���������points�����ĵ�Polygons
    void simplify(double tolerance, Polygons &polygons) const;  //��polygons����뱾������points��tolerance�򻯺�Ķ����
    void triangulate_convex(Polygons* polygons) const;  //������points��һ�������������������ɵ������μ��뵽polygons���棨��ʱ��Ż���룩
    Point centroid() const;  //���ض�������ģ�ƽ���ܶ�Ҳ���Ǿ������ġ����ο���http://en.wikipedia.org/wiki/Centroid#Centroid_of_polygon
    std::string wkt() const;  //��POLYGON((x1 y1,x2 y2,...xn yn))��ʽ���ַ�������
    Points concave_points(double angle = PI) const;  //�ҳ����а�����   ʵ�ڲ���Ϊɶ��
    Points convex_points(double angle = PI) const;  //�ҳ�����͹����

};

}

#endif // POLYGON_H

