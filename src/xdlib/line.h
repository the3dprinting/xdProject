#ifndef LINE_H
#define LINE_H

#include "constdefine.h"
#include "point.h"

namespace xd {

class Line;
class Linef3;
class Polyline;
typedef std::vector<Line> Lines;

class Line   //���ﶨ��Line�࣬����������Point��
{
    public:
    Point a;
    Point b;
    Line() {};
    explicit Line(Point _a, Point _b): a(_a), b(_b) {};
    std::string wkt() const;   //������LINESTRING(a.x a.y,b.x b.y)����ʽ���ɵ��ַ���
    operator Lines() const;   //����ת�����������಻�践��ֵ�ĺ���֮һ����Line��ת��ΪLines�ࡣ��Lines�������thisָ���һ��Line��
    operator Polyline() const;  //ͬ�ϣ�ֻ��Line��ת��ΪPolyline�ࡣ��Polyline���Points����a��b��
    void scale(double factor);  //Line�������㶼�Ŵ�factor��
    void translate(double x, double y);   //Line�������㶼ƽ�Ƽ�����x��y��ֵ
    void rotate(double angle, const Point &center); //Line�������㶼��center��ʱ����תangle��
    void reverse();  //��a�͵�b����
    double length() const;  //����Line�ĳ���
    Point midpoint() const;  //����Line���е�
    void point_at(double distance, Point* point) const;  //ָ��pointָ����a��b��������distance����λ�ĵ㣬distance=0ʱ��a��
    Point point_at(double distance) const;  //ͬ�ϣ�ֻ�Ƿ��������point��
    bool intersection_infinite(const Line &other, Point* point) const;  //����fault˵�����߶λ���ƽ�У�����pointָ�����߶ν��㡣
    //ע�⣺����ĺ���ʵ�ֵ������߶β��ཻʱ����û���Ƴ���point�Ƿ�ָ���ӳ��߽��㣡����
    bool coincides_with(const Line &line) const;  //�ж������߶��Ƿ���ͬ
    double distance_to(const Point &point) const;  //�����౾���߶ε�����������ľ��룬����߶���һ���㣬�򷵻ص��㵽��ľ���
    bool parallel_to(double angle) const;
    bool parallel_to(const Line &line) const;
    double atan2_() const;  //�����߶κ�ˮƽ�߼нǵĻ���ֵ��ȡֵ��Χ���� -pi �� pi ֮�䣨������ -pi��
    double orientation() const;  //�����߶κ�ˮƽ�߼нǵĻ���ֵ��ȡֵ��Χ���� 0 �� 2pi ֮�䣨������ 0������ʾ�߶ε�ָ��
    double direction() const;    //�����߶κ�ˮƽ�߼нǵĻ���ֵ����������(-pi,0)����piʱ�ͼ�pi���൱��ȡ�෴���򣬼�������[0,pi)�ϡ���ʾ�߶εķ���
    Vector vector() const;  //���ر��������ֵ
    Vector normal() const;  //���ر���ķ����������ͱ���������ֱ������
 };

class Linef    //����double�͵��߶�
{
    public:
    Pointf a;
    Pointf b;
    Linef() {};
    explicit Linef(Pointf _a, Pointf _b): a(_a), b(_b) {};
};

class Linef3   //����double��3ά�߶�
{
    public:
    Pointf3 a;
    Pointf3 b;
    Linef3() {};
    explicit Linef3(Pointf3 _a, Pointf3 _b): a(_a), b(_b) {};
    Pointf3 intersect_plane(double z) const; //�����߶κ�Z��������ֵΪz��ƽ��Ľ���
    void scale(double factor);  //�߶������˵�����궼�Ŵ�factor��

};

}

#endif // LINE_H

