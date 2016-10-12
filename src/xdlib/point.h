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
typedef std::vector<Point> Points;   //由Point组成的向量。
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
    Point(long long _x, long long _y): x(_x), y(_y) {};  // 为Clipper
    Point(double x, double y);
    static Point new_scale(coordf_t x, coordf_t y) {
        return Point(scale_(x), scale_(y));  //默认的情况下，双精度浮点坐标值实际放大了10^6倍后取长整型，相当于保留精度到小数点后第六位
    };             //注意：通常情况下，32位操作系统中长整型最大是2147483647，扩大10^6意味着以毫米为单位的零件能计算的最大输入尺寸是21米多一点
    bool operator==(const Point& rhs) const;   //重载运算符==，实现过程调用了下面的coincides_with（）函数
    std::string wkt() const;      //返回以POINT(x y)的形式生成的字符串
    void scale(double factor);    //将点放大factor倍
    void translate(double x, double y);   //将点坐标矢量加上(x y)，即平移
    void translate(const Vector &vector);    //矢量加，同上
    void rotate(double angle, const Point &center);   //以center为中心逆时针旋转angle度，angle是弧度。（推导过程可以假设center为坐标原点）
    bool coincides_with(const Point &point) const;    //是否和给定点相等
    bool coincides_with_epsilon(const Point &point) const;   //到给定点point的坐标差△x<100且△y<100时为真，相当于都小于0.0001mm
    int nearest_point_index(const Points &points) const;     //先将点向量转换为点常指针向量，再调用下面的函数，找一系列点中距本身最近的点的角标
    int nearest_point_index(const PointConstPtrs &points) const;  //这个函数巧妙的使用continue函数，减少了计算量，建议学习，同时注意：
    //如果寻找最近点时，距离的平方已经小于EPSILON=1e-4，则不再寻找，因为这个点就可以当做和找到的点重合了
    int nearest_point_index(const PointPtrs &points) const;  //先将点指针向量转换为点常指针向量，再调用上面的函数
    size_t nearest_waypoint_index(const Points &points, const Point &point) const;  //返回points中到给定点和类本身点距离和最小的点的角标
    //注意：size_t是标准C库中定义的，应为unsigned int，在64位系统中为 long unsigned int
    bool nearest_point(const Points &points, Point* point) const; //让point指向points中距离类本身点最近的点，成功返回true
    bool nearest_waypoint(const Points &points, const Point &dest, Point* point) const; //poitn指向points中到给定点和类本身点距离和最小的点
    double distance_to(const Point &point) const;  //返回到point的距离
    double distance_to(const Line &line) const;    //返回类本身点到给定线段最近的距离，如果线段是一个点，则返回到此点的距离（推导见cpp）
    double perp_distance_to(const Line &line) const;   //返回到给定线段的垂直距离，如果线段是一个点，则返回到此点的距离
    double ccw(const Point &p1, const Point &p2) const;  //返回p1→p2叉乘p1→this，返回值>0是逆时针，反之顺时针，=0则共线
    double ccw(const Line &line) const;   //用线段的两个点代替带入上面的函数，结果同上
    double ccw_angle(const Point &p1, const Point &p2) const;  //返回角度p1→this→p2的弧度值，结果是如果是负数加2pi得到正数
    Point projection_onto(const MultiPoint &poly) const;  	//this到由poly组成的一系列line上的距离投影最小的点，如果投影点不在线的上，则返回距离this最近的线段端点
    Point projection_onto(const Line &line) const; //返回this到line上的投影，如果投影点不在线的上，则返回距离this最近的线段端点
    Point negative() const;  //返回点的负值
    Vector vector_to(const Point &point) const;  //返回this到point的向量值，也是point类型
};

Point operator+(const Point& point1, const Point& point2);   //重载+运算符，返回两个点的坐标分别相加的点
Point operator*(double scalar, const Point& point2);    //返回scalar乘以点的坐标值，但只重载了double在乘号左边的情况，我认为如果double在右边则会出现错误。

class Point3 : public Point  //继承Point类的三维点，仅仅多了一个z值，其他二维函数都没有扩展到三维上，也没有必要
{
    public:
    coord_t z;
    explicit Point3(coord_t _x = 0, coord_t _y = 0, coord_t _z = 0): Point(_x, _y), z(_z) {};
};

std::ostream& operator<<(std::ostream &stm, const Pointf &pointf);  //为标准输出流加入Pointf，形式是x，y

class Pointf
{
    public:
    coordf_t x;   //double
    coordf_t y;
    explicit Pointf(coordf_t _x = 0, coordf_t _y = 0): x(_x), y(_y) {};
    static Pointf new_unscale(coord_t x, coord_t y) {
        return Pointf(unscale(x), unscale(y));
    };  //缩小10^6倍，因为计算时放大了10^6倍，所以最后在输出时转换为double形式的数要缩小
    static Pointf new_unscale(const Point &p) {
        return Pointf(unscale(p.x), unscale(p.y));
    };  //同上
    void scale(double factor);   //坐标放大factor倍
    void translate(double x, double y);   //坐标平移x，y长度
    void rotate(double angle, const Pointf &center);  //以center为中心逆时针旋转angle度，angle是弧度。
    Pointf negative() const;   //返回点的负值
    Vectorf vector_to(const Pointf &point) const;  //返回this到point的向量值，也是pointf类型
};

class Pointf3 : public Pointf
{
    public:
    coordf_t z;
    explicit Pointf3(coordf_t _x = 0, coordf_t _y = 0, coordf_t _z = 0): Pointf(_x, _y), z(_z) {};
    static Pointf3 new_unscale(coord_t x, coord_t y, coord_t z) {
        return Pointf3(unscale(x), unscale(y), unscale(z));
    };   //缩小10^6倍，因为计算时放大了10^6倍，所以最后在输出时转换为double形式的数要缩小
    void scale(double factor);  //坐标放大factor倍
    void translate(const Vectorf3 &vector);   //坐标平移x，y，z长度
    void translate(double x, double y, double z);  //坐标平移x，y，z长度
    double distance_to(const Pointf3 &point) const;    //返回两点之间的距离
    Pointf3 negative() const;   //返回点的负值
    Vectorf3 vector_to(const Pointf3 &point) const;  //返回this到point的向量值，也是pointf类型
};
}   //xd命名空间结束

// 开始Boost，下面是注册点类型，boost使用必须的步骤
#include "../boost/polygon/polygon.hpp"
namespace boost { namespace polygon {
    template <>
    struct geometry_concept<coord_t> { typedef coordinate_concept type; };

    template <>
    struct coordinate_traits<coord_t> {
        typedef coord_t coordinate_type;
        typedef long double area_type;
        typedef long long manhattan_area_type;
        typedef unsigned long long unsigned_area_type;
        typedef long long coordinate_difference;
        typedef long double coordinate_distance;
    };

    template <>
    struct geometry_concept<xd::Point> { typedef point_concept type; };

    template <>
    struct point_traits<xd::Point> {
        typedef coord_t coordinate_type;

        static inline coordinate_type get(const xd::Point& point, orientation_2d orient) {
            return (orient == HORIZONTAL) ? point.x : point.y;
        }
    };

    template <>
    struct point_mutable_traits<xd::Point> {
        typedef coord_t coordinate_type;
        static inline void set(xd::Point& point, orientation_2d orient, coord_t value) {
            if (orient == HORIZONTAL)
                point.x = value;
            else
                point.y = value;
        }
        static inline xd::Point construct(coord_t x_value, coord_t y_value) {
            xd::Point retval;
            retval.x = x_value;
            retval.y = y_value;
            return retval;
        }
    };
} }
// 结束Boost

#endif // POINT_H

