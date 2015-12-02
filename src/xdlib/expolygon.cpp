#include "boundingBox.h"
#include "exPolygon.h"
#include "geometry.h"
#include "polygon.h"
#include "line.h"
#include "encapsulationclipper.h"
#include "../polypartition/polypartition.h"
#include "../poly2tri/poly2tri.h"
#include <algorithm>
#include <list>
#include <QDebug>

namespace xd {

ExPolygon::operator Points() const
{
    Points points;
    Polygons pp = *this;  //this强制转换成了Polygons类型，即用了下一个函数！
    for (Polygons::const_iterator poly = pp.begin(); poly != pp.end(); ++poly) {
        for (Points::const_iterator point = poly->points.begin(); point != poly->points.end(); ++point)
            points.push_back(*point);
    }
    return points;
}

ExPolygon::operator Polygons() const
{
    Polygons polygons;
    polygons.reserve(this->holes.size() + 1);
    polygons.push_back(this->contour);
    for (Polygons::const_iterator it = this->holes.begin(); it != this->holes.end(); ++it) {
        polygons.push_back(*it);
    }
    return polygons;
}

void
ExPolygon::scale(double factor)
{
    contour.scale(factor);
    for (Polygons::iterator it = holes.begin(); it != holes.end(); ++it) {
        (*it).scale(factor);
    }
}

void
ExPolygon::translate(double x, double y)
{
    contour.translate(x, y);
    for (Polygons::iterator it = holes.begin(); it != holes.end(); ++it) {
        (*it).translate(x, y);
    }
}

void
ExPolygon::rotate(double angle, const Point &center)
{
    contour.rotate(angle, center);
    for (Polygons::iterator it = holes.begin(); it != holes.end(); ++it) {
        (*it).rotate(angle, center);
    }
}

double
ExPolygon::area() const
{
    double a = this->contour.area();
    for (Polygons::const_iterator it = this->holes.begin(); it != this->holes.end(); ++it) {
        a -= -(*it).area();  // holes have negative area
    }
    return a;
}

bool
ExPolygon::is_valid() const
{
    if (!this->contour.is_valid() || !this->contour.is_counter_clockwise()) return false;
    for (Polygons::const_iterator it = this->holes.begin(); it != this->holes.end(); ++it) {
        if (!(*it).is_valid() || (*it).is_counter_clockwise()) return false;
    }
    return true;
}

bool
ExPolygon::contains(const Line &line) const
{
    return this->contains((Polyline)line);
}

bool
ExPolygon::contains(const Polyline &polyline) const
{
    Polylines pl_out;
    diff((Polylines)polyline, *this, &pl_out);  //这里调用了ClipperUtils里面封装了clipper库的函数diff，表示布尔差
    return pl_out.empty(); //*this剪完了Polyline如果没有东西了，说明polyline全都在*this里面
}

bool
ExPolygon::contains(const Point &point) const
{
    if (!this->contour.contains(point)) return false;   //不在外轮廓内代表不包含
    for (Polygons::const_iterator it = this->holes.begin(); it != this->holes.end(); ++it) {  //在内轮廓内也代表不包含
        if (it->contains(point)) return false;
    }
    return true;
}

// inclusive version of contains() that also checks whether point is on boundaries
bool
ExPolygon::contains_b(const Point &point) const
{
    return this->contains(point) || this->has_boundary_point(point);
}

bool
ExPolygon::has_boundary_point(const Point &point) const
{
    if (this->contour.has_boundary_point(point)) return true;   //contour是调用了父类multiPoint的成员函数
    for (Polygons::const_iterator h = this->holes.begin(); h != this->holes.end(); ++h) {
        if (h->has_boundary_point(point)) return true;
    }
    return false;
}

Polygons
ExPolygon::simplify_p(double tolerance) const
{
    Polygons pp;
    pp.reserve(this->holes.size() + 1);

    // contour
    {
        Polygon p = this->contour;
        p.points.push_back(p.points.front());
        p.points = MultiPoint::_douglas_peucker(p.points, tolerance);
        p.points.pop_back();
        pp.push_back(p);
    }

    // holes
    for (Polygons::const_iterator it = this->holes.begin(); it != this->holes.end(); ++it) {
        Polygon p = *it;
        p.points.push_back(p.points.front());
        p.points = MultiPoint::_douglas_peucker(p.points, tolerance);
        p.points.pop_back();
        pp.push_back(p);
    }
    simplify_polygons(pp, &pp);
    return pp;
}

ExPolygons
ExPolygon::simplify(double tolerance) const
{
    Polygons pp = this->simplify_p(tolerance);
    ExPolygons expp;
    union_(pp, &expp);
    return expp;
}

void
ExPolygon::simplify(double tolerance, ExPolygons &expolygons) const
{
    ExPolygons ep = this->simplify(tolerance);
    expolygons.reserve(expolygons.size() + ep.size());
    expolygons.insert(expolygons.end(), ep.begin(), ep.end());
}

void
ExPolygon::medial_axis(double max_width, double min_width, Polylines* polylines) const
{
    // 初始化几何文件里面的MediaAxis对象
    xd::Geometry::MedialAxis ma(max_width, min_width);

    // 植入Voronoi图线段的链表
    ma.lines = this->contour.lines();
    for (Polygons::const_iterator hole = this->holes.begin(); hole != this->holes.end(); ++hole) {
        Lines lines = hole->lines();
        ma.lines.insert(ma.lines.end(), lines.begin(), lines.end());
    }

    //计算Voronoi图
    ma.build(polylines);  //生成有效voronoi图的边，放在polylines里面。根据输入的min_width舍去了一些边

    // clip segments to our expolygon area
    // (do this before extending endpoints as external segments coule be extended into
    // expolygon, this leaving wrong things inside)  将不在多边形内部的polylines裁剪掉
    intersection(*polylines, *this, polylines);

    // extend initial and final segments of each polyline (they will be clipped)
    // unless they represent closed loops
    for (Polylines::iterator polyline = polylines->begin(); polyline != polylines->end(); ++polyline) {
        if (polyline->points.front().distance_to(polyline->points.back()) < min_width) continue;  //多线段的起点和末尾点太近则不延长了
        // TODO: we should *not* extend endpoints where other polylines start/end
        // (such as T joints, which are returned as three polylines by MedialAxis)
        polyline->extend_start(max_width);
        polyline->extend_end(max_width);
    }

    // clip again after extending endpoints to prevent them from exceeding the expolygon boundaries
    intersection(*polylines, *this, polylines);

    // remove too short polylines
    // (we can't do this check before endpoints extension and clipping because we don't
    // know how long will the endpoints be extended since it depends on polygon thickness
    // which is variable - extension will be <= max_width/2 on each side)
    for (size_t i = 0; i < polylines->size(); ++i) {
        if ((*polylines)[i].length() < max_width) {
            polylines->erase(polylines->begin() + i);
            --i;
        }
    }
}

void
ExPolygon::get_trapezoids(Polygons* polygons) const
{
    ExPolygons expp;
    expp.push_back(*this);
    boost::polygon::get_trapezoids(*polygons, expp); //http://www.boost.org/doc/libs/1_57_0/libs/polygon/doc/gtl_polygon_set_concept.htm
        //分析后得出get_trapezoids函数直接将expp分割后的小四边形加入到polygons后面
}

void
ExPolygon::get_trapezoids(Polygons* polygons, double angle) const
{
    ExPolygon clone = *this;
    clone.rotate(PI/2 - angle, Point(0,0));
    clone.get_trapezoids(polygons);
    for (Polygons::iterator polygon = polygons->begin(); polygon != polygons->end(); ++polygon)
        polygon->rotate(-(PI/2 - angle), Point(0,0));
}

// This algorithm may return more trapezoids than necessary
// (i.e. it may break a single trapezoid in several because
// other parts of the object have x coordinates in the middle)
void
ExPolygon::get_trapezoids2(Polygons* polygons) const
{
    // get all points of this ExPolygon
    Points pp = *this;

    // build our bounding box
    BoundingBox bb(pp);

    // get all x coordinates
    std::vector<coord_t> xx;
    xx.reserve(pp.size());
    for (Points::const_iterator p = pp.begin(); p != pp.end(); ++p)
        xx.push_back(p->x);
    std::sort(xx.begin(), xx.end());

    // find trapezoids by looping from first to next-to-last coordinate
    for (std::vector<coord_t>::const_iterator x = xx.begin(); x != xx.end()-1; ++x) {
        coord_t next_x = *(x + 1);
        if (*x == next_x) continue;

        // build rectangle
        Polygon poly;
        poly.points.resize(4);
        poly[0].x = *x;
        poly[0].y = bb.min.y;
        poly[1].x = next_x;
        poly[1].y = bb.min.y;
        poly[2].x = next_x;
        poly[2].y = bb.max.y;
        poly[3].x = *x;
        poly[3].y = bb.max.y;

        // intersect with this expolygon
        Polygons trapezoids;
        intersection<Polygons,Polygons>(poly, *this, &trapezoids);

        // append results to return value
        polygons->insert(polygons->end(), trapezoids.begin(), trapezoids.end());
    }
}

void
ExPolygon::get_trapezoids2(Polygons* polygons, double angle) const
{
    ExPolygon clone = *this;
    clone.rotate(PI/2 - angle, Point(0,0));
    clone.get_trapezoids2(polygons);
    for (Polygons::iterator polygon = polygons->begin(); polygon != polygons->end(); ++polygon)
        polygon->rotate(-(PI/2 - angle), Point(0,0));
}

// While this triangulates successfully, it's NOT a constrained triangulation
// as it will create more vertices on the boundaries than the ones supplied.
void
ExPolygon::triangulate(Polygons* polygons) const
{
    // first make trapezoids
    Polygons trapezoids;
    this->get_trapezoids2(&trapezoids);

    // then triangulate each trapezoid
    for (Polygons::iterator polygon = trapezoids.begin(); polygon != trapezoids.end(); ++polygon)
        polygon->triangulate_convex(polygons);  //将自身points第一个点和其余依次两点组成的三角形加入到polygons里面（逆时针才会加入）
}

void
ExPolygon::triangulate_pp(Polygons* polygons) const  //使用polypartition.h文件里的算法进行三角形划分
{
    // convert polygons    也就是调用第三方库时需要转换接口类型
    std::list<TPPLPoly> input;

    Polygons pp = *this;
    simplify_polygons(pp, &pp, true);
    ExPolygons expp;
    union_(pp, &expp);

    for (ExPolygons::const_iterator ex = expp.begin(); ex != expp.end(); ++ex) {
        // contour
        {
            TPPLPoly p;
            p.Init(ex->contour.points.size());
            //printf("%zu\n0\n", ex->contour.points.size());
            for (Points::const_iterator point = ex->contour.points.begin(); point != ex->contour.points.end(); ++point) {
                p[ point-ex->contour.points.begin() ].x = point->x;
                p[ point-ex->contour.points.begin() ].y = point->y;
                //printf("%ld %ld\n", point->x, point->y);
            }
            p.SetHole(false);
            input.push_back(p);
        }

        // holes
        for (Polygons::const_iterator hole = ex->holes.begin(); hole != ex->holes.end(); ++hole) {
            TPPLPoly p;
            p.Init(hole->points.size());
            //printf("%zu\n1\n", hole->points.size());
            for (Points::const_iterator point = hole->points.begin(); point != hole->points.end(); ++point) {
                p[ point-hole->points.begin() ].x = point->x;
                p[ point-hole->points.begin() ].y = point->y;
                //printf("%ld %ld\n", point->x, point->y);
            }
            p.SetHole(true);
            input.push_back(p);
        }
    }

    // perform triangulation
    std::list<TPPLPoly> output;
    int res = TPPLPartition().Triangulate_MONO(&input, &output); //https://github.com/ivanfratric/polypartition
    if (res != 1) qDebug("Triangulation failed");

    // 调用完第三方库后要转换回来接口
    for (std::list<TPPLPoly>::iterator poly = output.begin(); poly != output.end(); ++poly) {
        long num_points = poly->GetNumPoints();
        Polygon p;
        p.points.resize(num_points);
        for (long i = 0; i < num_points; ++i) {
            p.points[i].x = (*poly)[i].x;
            p.points[i].y = (*poly)[i].y;
        }
        polygons->push_back(p);
    }
}

void
ExPolygon::triangulate_p2t(Polygons* polygons) const   //使用poly2tri.h里面的函数进行多边形的三角划分
{
    ExPolygons expp;
    simplify_polygons(*this, &expp, true);

    for (ExPolygons::const_iterator ex = expp.begin(); ex != expp.end(); ++ex) {
        p2t::CDT* cdt;

        // TODO: prevent duplicate points

        // contour
        {
            std::vector<p2t::Point*> points;
            for (Points::const_iterator point = ex->contour.points.begin(); point != ex->contour.points.end(); ++point) {
                points.push_back(new p2t::Point(point->x, point->y));
            }
            cdt = new p2t::CDT(points);
        }

        // holes
        for (Polygons::const_iterator hole = ex->holes.begin(); hole != ex->holes.end(); ++hole) {
            std::vector<p2t::Point*> points;
            for (Points::const_iterator point = hole->points.begin(); point != hole->points.end(); ++point) {
                points.push_back(new p2t::Point(point->x, point->y));
            }
            cdt->AddHole(points);
        }

        // perform triangulation
        cdt->Triangulate();
        std::vector<p2t::Triangle*> triangles = cdt->GetTriangles();

        for (std::vector<p2t::Triangle*>::const_iterator triangle = triangles.begin(); triangle != triangles.end(); ++triangle) {
            Polygon p;
            for (int i = 0; i <= 2; ++i) {
                p2t::Point* point = (*triangle)->GetPoint(i);
                p.points.push_back(Point(point->x, point->y));
            }
            polygons->push_back(p);
        }
    }
}

Lines
ExPolygon::lines() const
{
    Lines lines = this->contour.lines();
    for (Polygons::const_iterator h = this->holes.begin(); h != this->holes.end(); ++h) {
        Lines hole_lines = h->lines();
        lines.insert(lines.end(), hole_lines.begin(), hole_lines.end());
    }
    return lines;
}


}
