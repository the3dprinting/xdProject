#include "multiPoint.h"
#include "boundingBox.h"

namespace xd {

MultiPoint::operator Points() const
{
    return this->points;
}

void
MultiPoint::scale(double factor)
{
    for (Points::iterator it = points.begin(); it != points.end(); ++it) {
        (*it).scale(factor);
    }
}

void
MultiPoint::translate(double x, double y)
{
    for (Points::iterator it = points.begin(); it != points.end(); ++it) {
        (*it).translate(x, y);
    }
}

void
MultiPoint::translate(const Point &vector)
{
    this->translate(vector.x, vector.y);
}

void
MultiPoint::rotate(double angle, const Point &center)
{
    for (Points::iterator it = points.begin(); it != points.end(); ++it) {
        (*it).rotate(angle, center);
    }
}

void
MultiPoint::reverse()
{
    std::reverse(this->points.begin(), this->points.end());
}

Point
MultiPoint::first_point() const
{
    return this->points.front();
}

double
MultiPoint::length() const
{
    Lines lines = this->lines();
    double len = 0;
    for (Lines::iterator it = lines.begin(); it != lines.end(); ++it) {
        len += it->length();
    }
    return len;
}

bool
MultiPoint::is_valid() const
{
    return this->points.size() >= 2;
}

int
MultiPoint::find_point(const Point &point) const
{
    for (Points::const_iterator it = this->points.begin(); it != this->points.end(); ++it) {
        if (it->coincides_with(point)) return it - this->points.begin();
    }
    return -1;  // û�ҵ��Ļ�
}

bool
MultiPoint::has_boundary_point(const Point &point) const
{
    double dist = point.distance_to(point.projection_onto(*this));
    return dist < SCALED_EPSILON;
}

BoundingBox
MultiPoint::bounding_box() const
{
    return BoundingBox(this->points);
}

void
MultiPoint::remove_duplicate_points()
{
    for (size_t i = 1; i < this->points.size(); ++i) {
        if (this->points.at(i).coincides_with(this->points.at(i-1))) {
            this->points.erase(this->points.begin() + i);
            --i;
        }
    }
}

void
MultiPoint::append(const Point &point)
{
    this->points.push_back(point);
}

void
MultiPoint::append(const Points &points)
{
    this->append(points.begin(), points.end());
}

void
MultiPoint::append(const Points::const_iterator &begin, const Points::const_iterator &end)
{
    this->points.insert(this->points.end(), begin, end);
}

Points
MultiPoint::_douglas_peucker(const Points &points, const double tolerance)  //���͵ĵݹ鷨������⣬�ʺ�ѧϰ��
{   //�������ʵ��ֻ���˴��ڵĵ㣬�����Ҹ߾������о���������ĵ�
    Points results;
    double dmax = 0;
    size_t index = 0;
    Line full(points.front(), points.back());
    for (Points::const_iterator it = points.begin() + 1; it != points.end(); ++it) {
        // ʹ����̾��룬�Ǵ�ֱ����
        double d = it->distance_to(full);
        if (d > dmax) {
            index = it - points.begin();
            dmax = d;
        }
    }
    if (dmax >= tolerance) {
        Points dp0;
        dp0.reserve(index + 1);
        dp0.insert(dp0.end(), points.begin(), points.begin() + index + 1);
        Points dp1 = MultiPoint::_douglas_peucker(dp0, tolerance);  //�ݹ��÷����ӳ������ĵ�ֿ�
        results.reserve(results.size() + dp1.size() - 1);
        results.insert(results.end(), dp1.begin(), dp1.end() - 1);

        dp0.clear();
        dp0.reserve(points.size() - index + 1);
        dp0.insert(dp0.end(), points.begin() + index, points.end());
        dp1 = MultiPoint::_douglas_peucker(dp0, tolerance);   //�ݹ��÷����ӳ������ĵ�ֿ�
        results.reserve(results.size() + dp1.size());
        results.insert(results.end(), dp1.begin(), dp1.end());
    } else {
        results.push_back(points.front());
        results.push_back(points.back());
    }
    return results;
}

}

