#ifndef POINT_H
#define POINT_H

#include <vector>
#include <math.h>
#include <string>
#include <sstream>
#include "constdefine.h"

namespace xd {

class Line;
class Linef;
class MultiPoint;
class Point;
class Pointf;
class Pointf3;
typedef Point Vector;
typedef Pointf Vectorf;
typedef Pointf3 Vectorf3;
typedef std::vector<Point> Points;   //��Point��ɵ�������
typedef std::vector<Point*> PointPtrs;
typedef std::vector<const Point*> PointConstPtrs;
typedef std::vector<Pointf> Pointfs;
typedef std::vector<Pointf3> Pointf3s;

class Point
{
    public:
    coord_t x;   //long
    coord_t y;
    Point(coord_t _x = 0, coord_t _y = 0): x(_x), y(_y) {};
    Point(int _x, int _y): x(_x), y(_y) {};
    Point(long long _x, long long _y): x(_x), y(_y) {};  // ΪClipper
    Point(double x, double y);
    static Point new_scale(coordf_t x, coordf_t y) {
        return Point(scale_(x), scale_(y));  //Ĭ�ϵ�����£�˫���ȸ�������ֵʵ�ʷŴ���10^6����ȡ�����ͣ��൱�ڱ������ȵ�С��������λ
    };             //ע�⣺ͨ������£�32λ����ϵͳ�г����������2147483647������10^6��ζ���Ժ���Ϊ��λ������ܼ�����������ߴ���21�׶�һ��
    bool operator==(const Point& rhs) const;   //���������==��ʵ�ֹ��̵����������coincides_with��������
    std::string wkt() const;      //������POINT(x y)����ʽ���ɵ��ַ���
    void scale(double factor);    //����Ŵ�factor��
    void translate(double x, double y);   //��������ʸ������(x y)����ƽ��
    void translate(const Vector &vector);    //ʸ���ӣ�ͬ��
    void rotate(double angle, const Point &center);   //��centerΪ������ʱ����תangle�ȣ�angle�ǻ��ȡ����Ƶ����̿��Լ���centerΪ����ԭ�㣩
    bool coincides_with(const Point &point) const;    //�Ƿ�͸��������
    bool coincides_with_epsilon(const Point &point) const;   //��������point��������x<100�ҡ�y<100ʱΪ�棬�൱�ڶ�С��0.0001mm
    int nearest_point_index(const Points &points) const;     //�Ƚ�������ת��Ϊ�㳣ָ���������ٵ�������ĺ�������һϵ�е��о౾������ĵ�ĽǱ�
    int nearest_point_index(const PointConstPtrs &points) const;  //������������ʹ��continue�����������˼�����������ѧϰ��ͬʱע�⣺
    //���Ѱ�������ʱ�������ƽ���Ѿ�С��EPSILON=1e-4������Ѱ�ң���Ϊ�����Ϳ��Ե������ҵ��ĵ��غ���
    int nearest_point_index(const PointPtrs &points) const;  //�Ƚ���ָ������ת��Ϊ�㳣ָ���������ٵ�������ĺ���
    size_t nearest_waypoint_index(const Points &points, const Point &point) const;  //����points�е���������౾���������С�ĵ�ĽǱ�
    //ע�⣺size_t�Ǳ�׼C���ж���ģ�ӦΪunsigned int����64λϵͳ��Ϊ long unsigned int
    bool nearest_point(const Points &points, Point* point) const; //��pointָ��points�о����౾�������ĵ㣬�ɹ�����true
    bool nearest_waypoint(const Points &points, const Point &dest, Point* point) const; //poitnָ��points�е���������౾���������С�ĵ�
    double distance_to(const Point &point) const;  //���ص�point�ľ���
    double distance_to(const Line &line) const;    //�����౾��㵽�����߶�����ľ��룬����߶���һ���㣬�򷵻ص��˵�ľ��루�Ƶ���cpp��
    double perp_distance_to(const Line &line) const;   //���ص������߶εĴ�ֱ���룬����߶���һ���㣬�򷵻ص��˵�ľ���
    double ccw(const Point &p1, const Point &p2) const;  //����p1��p2���p1��this������ֵ>0����ʱ�룬��֮˳ʱ�룬=0����
    double ccw(const Line &line) const;   //���߶ε�����������������ĺ��������ͬ��
    double ccw_angle(const Point &p1, const Point &p2) const;  //���ؽǶ�p1��this��p2�Ļ���ֵ�����������Ǹ�����2pi�õ�����
    Point projection_onto(const MultiPoint &poly) const;  	//this����poly��ɵ�һϵ��line�ϵľ���ͶӰ��С�ĵ㣬���ͶӰ�㲻���ߵ��ϣ��򷵻ؾ���this������߶ζ˵�
    Point projection_onto(const Line &line) const; //����this��line�ϵ�ͶӰ�����ͶӰ�㲻���ߵ��ϣ��򷵻ؾ���this������߶ζ˵�
    Point negative() const;  //���ص�ĸ�ֵ
    Vector vector_to(const Point &point) const;  //����this��point������ֵ��Ҳ��point����
};

Point operator+(const Point& point1, const Point& point2);   //����+����������������������ֱ���ӵĵ�
Point operator*(double scalar, const Point& point2);    //����scalar���Ե������ֵ����ֻ������double�ڳ˺���ߵ����������Ϊ���double���ұ������ִ���

class Point3 : public Point  //�̳�Point�����ά�㣬��������һ��zֵ��������ά������û����չ����ά�ϣ�Ҳû�б�Ҫ
{
    public:
    coord_t z;
    explicit Point3(coord_t _x = 0, coord_t _y = 0, coord_t _z = 0): Point(_x, _y), z(_z) {};
};

std::ostream& operator<<(std::ostream &stm, const Pointf &pointf);  //Ϊ��׼���������Pointf����ʽ��x��y

class Pointf
{
    public:
    coordf_t x;   //double
    coordf_t y;
    explicit Pointf(coordf_t _x = 0, coordf_t _y = 0): x(_x), y(_y) {};
    static Pointf new_unscale(coord_t x, coord_t y) {
        return Pointf(unscale(x), unscale(y));
    };  //��С10^6������Ϊ����ʱ�Ŵ���10^6����������������ʱת��Ϊdouble��ʽ����Ҫ��С
    static Pointf new_unscale(const Point &p) {
        return Pointf(unscale(p.x), unscale(p.y));
    };  //ͬ��
    void scale(double factor);   //����Ŵ�factor��
    void translate(double x, double y);   //����ƽ��x��y����
    void rotate(double angle, const Pointf &center);  //��centerΪ������ʱ����תangle�ȣ�angle�ǻ��ȡ�
    Pointf negative() const;   //���ص�ĸ�ֵ
    Vectorf vector_to(const Pointf &point) const;  //����this��point������ֵ��Ҳ��pointf����
};

class Pointf3 : public Pointf
{
    public:
    coordf_t z;
    explicit Pointf3(coordf_t _x = 0, coordf_t _y = 0, coordf_t _z = 0): Pointf(_x, _y), z(_z) {};
    static Pointf3 new_unscale(coord_t x, coord_t y, coord_t z) {
        return Pointf3(unscale(x), unscale(y), unscale(z));
    };   //��С10^6������Ϊ����ʱ�Ŵ���10^6����������������ʱת��Ϊdouble��ʽ����Ҫ��С
    void scale(double factor);  //����Ŵ�factor��
    void translate(const Vectorf3 &vector);   //����ƽ��x��y��z����
    void translate(double x, double y, double z);  //����ƽ��x��y��z����
    double distance_to(const Pointf3 &point) const;    //��������֮��ľ���
    Pointf3 negative() const;   //���ص�ĸ�ֵ
    Vectorf3 vector_to(const Pointf3 &point) const;  //����this��point������ֵ��Ҳ��pointf����
};

}

#endif // POINT_H

