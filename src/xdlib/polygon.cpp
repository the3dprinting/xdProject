#include <QDebug>
#include "constdefine.h"
#include "polygon.h"
#include "polyline.h"
#include "encapsulationClipper.h"

namespace xd {

Polygon::operator Polygons() const
{
    Polygons pp;
    pp.push_back(*this);
    return pp;
}

Polygon::operator Polyline() const
{
    return this->split_at_first_point();
}

Point&
Polygon::operator[](Points::size_type idx)
{
    return this->points[idx];
}

const Point&
Polygon::operator[](Points::size_type idx) const
{
    return this->points[idx];
}

Point
Polygon::last_point() const
{
    return this->points.front();  // 对于多边形来说最后一个点等于第一个点
}

Lines
Polygon::lines() const
{
    Lines lines;
    lines.reserve(this->points.size());
    for (Points::const_iterator it = this->points.begin(); it != this->points.end()-1; ++it) {
        lines.push_back(Line(*it, *(it + 1)));
    }
    lines.push_back(Line(this->points.back(), this->points.front()));
    return lines;
}

Polyline
Polygon::split_at_vertex(const Point &point) const
{
    // find index of point
    for (Points::const_iterator it = this->points.begin(); it != this->points.end(); ++it) {
        if (it->coincides_with(point)) {
            return this->split_at_index(it - this->points.begin());
        }
    }
    qDebug("Point not found.\n");
    return Polyline();
}

Polyline
Polygon::split_at_index(int index) const
{
    Polyline polyline;
    polyline.points.reserve(this->points.size() + 1);
    for (Points::const_iterator it = this->points.begin() + index; it != this->points.end(); ++it)
        polyline.points.push_back(*it);
    for (Points::const_iterator it = this->points.begin(); it != this->points.begin() + index + 1; ++it)  //注意，index的点又加上了一个
        polyline.points.push_back(*it);
    return polyline;
}

Polyline
Polygon::split_at_first_point() const
{
    return this->split_at_index(0);
}

Points
Polygon::equally_spaced_points(double distance) const
{
    return this->split_at_first_point().equally_spaced_points(distance);
}

double
Polygon::area() const
{
    ClipperLib::Path p;
    xdMultiPoint_to_ClipperPath(*this, &p);
    return ClipperLib::Area(p);
}

bool
Polygon::is_counter_clockwise() const
{
    ClipperLib::Path p;
    xdMultiPoint_to_ClipperPath(*this, &p);
    return ClipperLib::Orientation(p);
}

bool
Polygon::is_clockwise() const
{
    return !this->is_counter_clockwise();
}

bool
Polygon::make_counter_clockwise()
{
    if (!this->is_counter_clockwise()) {
        this->reverse();
        return true;
    }
    return false;
}

bool
Polygon::make_clockwise()
{
    if (this->is_counter_clockwise()) {
        this->reverse();
        return true;
    }
    return false;
}

bool
Polygon::is_valid() const
{
    return this->points.size() >= 3;
}

bool
Polygon::contains(const Point &point) const
{
    // http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
    bool result = false;
    Points::const_iterator i = this->points.begin();
    Points::const_iterator j = this->points.end() - 1;
    for (; i != this->points.end(); j = i++) {
        if ( ((i->y > point.y) != (j->y > point.y))
            && ((double)point.x < (double)(j->x - i->x) * (double)(point.y - i->y) / (double)(j->y - i->y) + (double)i->x) )
            result = !result;
    }
    return result;
}

// this only works on CCW polygons as CW will be ripped out by Clipper's simplify_polygons()
Polygons
Polygon::simplify(double tolerance) const
{
    // repeat first point at the end in order to apply Douglas-Peucker
    // on the whole polygon
    Points points = this->points;
    points.push_back(points.front());
    Polygon p(MultiPoint::_douglas_peucker(points, tolerance));
    p.points.pop_back();

    Polygons pp;
    pp.push_back(p);
    simplify_polygons(pp, &pp);  //封装clipper里的函数
    return pp;
}

void
Polygon::simplify(double tolerance, Polygons &polygons) const
{
    Polygons pp = this->simplify(tolerance);
    polygons.reserve(polygons.size() + pp.size());
    polygons.insert(polygons.end(), pp.begin(), pp.end());
}

// Only call this on convex polygons or it will return invalid results
void
Polygon::triangulate_convex(Polygons* polygons) const
{
    for (Points::const_iterator it = this->points.begin() + 2; it != this->points.end(); ++it) {
        Polygon p;
        p.points.reserve(3);
        p.points.push_back(this->points.front());
        p.points.push_back(*(it-1));
        p.points.push_back(*it);

        // this should be replaced with a more efficient call to a merge_collinear_segments() method
        if (p.area() > 0) polygons->push_back(p);  //三角形逆时针排列才会加入，所以原多边形应该逆时针
    }
}

// 质心，也是型心
Point
Polygon::centroid() const
{
    double area_temp = this->area();
    double x_temp = 0;
    double y_temp = 0;

    Polyline polyline = this->split_at_first_point();
    for (Points::const_iterator point = polyline.points.begin(); point != polyline.points.end() - 1; ++point) {
        x_temp += (double)( point->x + (point+1)->x ) * ( (double)point->x*(point+1)->y - (double)(point+1)->x*point->y );
        y_temp += (double)( point->y + (point+1)->y ) * ( (double)point->x*(point+1)->y - (double)(point+1)->x*point->y );
    }

    return Point(x_temp/(6*area_temp), y_temp/(6*area_temp));
}

std::string
Polygon::wkt() const
{
    std::ostringstream wkt;
    wkt << "POLYGON((";
    for (Points::const_iterator p = this->points.begin(); p != this->points.end(); ++p) {
        wkt << p->x << " " << p->y;
        if (p != this->points.end()-1) wkt << ",";
    }
    wkt << "))";
    return wkt.str();
}

// find all concave vertices (i.e. having an internal angle greater than the supplied angle)
// (external = right side, thus we consider ccw orientation)
Points
Polygon::concave_points(double angle) const
{
    Points points;
    angle = 2*PI - angle;

    // check whether first point forms a concave angle
    if (this->points.front().ccw_angle(this->points.back(), *(this->points.begin()+1)) <= angle)
        points.push_back(this->points.front());

    // check whether points 1..(n-1) form concave angles
    for (Points::const_iterator p = this->points.begin()+1; p != this->points.end()-1; ++p) {
        if (p->ccw_angle(*(p-1), *(p+1)) <= angle) points.push_back(*p);
    }

    // check whether last point forms a concave angle
    if (this->points.back().ccw_angle(*(this->points.end()-2), this->points.front()) <= angle)
        points.push_back(this->points.back());

    return points;
}

// find all convex vertices (i.e. having an internal angle smaller than the supplied angle)
// (external = right side, thus we consider ccw orientation)
Points
Polygon::convex_points(double angle) const
{
    Points points;
    angle = 2*PI - angle;

    // check whether first point forms a convex angle
    if (this->points.front().ccw_angle(this->points.back(), *(this->points.begin()+1)) >= angle)
        points.push_back(this->points.front());

    // check whether points 1..(n-1) form convex angles
    for (Points::const_iterator p = this->points.begin()+1; p != this->points.end()-1; ++p) {
        if (p->ccw_angle(*(p-1), *(p+1)) >= angle) points.push_back(*p);
    }

    // check whether last point forms a convex angle
    if (this->points.back().ccw_angle(*(this->points.end()-2), this->points.front()) >= angle)
        points.push_back(this->points.back());

    return points;
}


}
