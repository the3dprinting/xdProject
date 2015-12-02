#ifndef MULTIPOINT_H
#define MULTIPOINT_H

#include "constdefine.h"
#include <algorithm>
#include <vector>
#include "line.h"
#include "point.h"

namespace xd {

class BoundingBox;

class MultiPoint  //������ص��࣬����һ��Point����ɵ�����Points
{
    public:
    Points points;

    operator Points() const;
    MultiPoint() {};
    explicit MultiPoint(const Points &_points): points(_points) {};
    void scale(double factor);  //���е������ֵ���Ŵ�factor��
    void translate(double x, double y);  //���е�����궼ƽ�Ƽ���x,y��ֵ
    void translate(const Point &vector);  //ͬ�ϣ�ֻ����������ʽ�ļ�
    void rotate(double angle, const Point &center);  //���е㶼��centerΪ����ѡ��angle��
    void reverse();  //���е����з�����
    Point first_point() const;  //���ش洢�ĵ�һ����
    virtual Point last_point() const = 0;  //������������ʵ�ַ������һ����
    virtual Lines lines() const = 0;  //��������������ʵ�ֵ��Ϊ�߶���ɵ�����
    double length() const;  //Ϊ����ʵ�ַ��������߶ε��ܳ���
    bool is_valid() const;  //���ش洢�ĵ����Ƿ���ڵ���2��
    int find_point(const Point &point) const;  //������point�൱�ĵ�ĽǱ꣬����-1˵��û����ȵĵ�
    bool has_boundary_point(const Point &point) const;  //����point�Ƿ������this��ɵ������ϣ�*thisǿ��ת������Points
    BoundingBox bounding_box() const;  //������points��ɵİ�Χ����
    void remove_duplicate_points();  //ɾ����ȵĵ㣬ʵ�ַ����ܺã������
    void append(const Point &point);  //������һ����
    void append(const Points &points);  //�����Ϻö��
    void append(const Points::const_iterator &begin, const Points::const_iterator &end);  //��ָ�����ʽ���Ϻö��

    static Points _douglas_peucker(const Points &points, const double tolerance);  //�������Ҹ���������»������߶Ρ�
                                                                                   //�ο�https://en.wikipedia.org/wiki/Ramer�CDouglas�CPeucker_algorithm
};

}

#endif // MULTIPOINT_H

