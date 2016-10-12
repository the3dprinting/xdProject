#include "encapsulationclipper.h"
#include "geometry.h"

//学习: vector<int>::value_type AnInt;
//等价于 int AnInt;

namespace xd {

void AddOuterPolyNodeToExPolygons(ClipperLib::PolyNode& polynode, xd::ExPolygons* expolygons)
{
  size_t cnt = expolygons->size();
  expolygons->resize(cnt + 1);  //这一步resize后即在后面加上一个（带洞）多边形
  ClipperPath_to_xdMultiPoint(polynode.Contour, &(*expolygons)[cnt].contour);  //这里加上多边形的轮廓
  (*expolygons)[cnt].holes.resize(polynode.ChildCount());
  for (int i = 0; i < polynode.ChildCount(); ++i)
  {
    ClipperPath_to_xdMultiPoint(polynode.Childs[i]->Contour, &(*expolygons)[cnt].holes[i]);  //这里加上多边形可能出现的n个洞
    //Add outer polygons contained by (nested within) holes ...
    for (int j = 0; j < polynode.Childs[i]->ChildCount(); ++j)   //有几个子，就会有几个（带洞）多边形
      AddOuterPolyNodeToExPolygons(*polynode.Childs[i]->Childs[j], expolygons);  //这一步使用递归
  }
}

void PolyTreeToExPolygons(ClipperLib::PolyTree& polytree, xd::ExPolygons* expolygons)
{
  expolygons->clear();
  for (int i = 0; i < polytree.ChildCount(); ++i)
    AddOuterPolyNodeToExPolygons(*polytree.Childs[i], expolygons);
}

void
xdMultiPoint_to_ClipperPath(const xd::MultiPoint &input, ClipperLib::Path* output)
{
    output->clear();
    for (xd::Points::const_iterator pit = input.points.begin(); pit != input.points.end(); ++pit) {
        output->push_back(ClipperLib::IntPoint( (*pit).x, (*pit).y ));
    }
}

template <class T>
void
xdMultiPoints_to_ClipperPaths(const T &input, ClipperLib::Paths* output)
{
    output->clear();
    for (typename T::const_iterator it = input.begin(); it != input.end(); ++it) {
        ClipperLib::Path p;
        xdMultiPoint_to_ClipperPath(*it, &p);
        output->push_back(p);
    }
}

template <class T>
void
ClipperPath_to_xdMultiPoint(const ClipperLib::Path &input, T* output)
{
    output->points.clear();
    for (ClipperLib::Path::const_iterator pit = input.begin(); pit != input.end(); ++pit) {
        output->points.push_back(xd::Point( (*pit).X, (*pit).Y ));
    }
}

template <class T>
void
ClipperPaths_to_xdMultiPoints(const ClipperLib::Paths &input, T* output)
{
    output->clear();
    for (ClipperLib::Paths::const_iterator it = input.begin(); it != input.end(); ++it) {
        typename T::value_type p;  //value_type一般在模板函数里用，因为模板的<>里面不知道是什么类型
        ClipperPath_to_xdMultiPoint(*it, &p);
        output->push_back(p);
    }
}

void
ClipperPaths_to_xdExPolygons(const ClipperLib::Paths &input, xd::ExPolygons* output)
{
    // init Clipper
    ClipperLib::Clipper clipper;
    clipper.Clear();

    // perform union   这里用法值得学习，将clipper里的paths变为polytree的方法
    clipper.AddPaths(input, ClipperLib::ptSubject, true);
    ClipperLib::PolyTree polytree;
    clipper.Execute(ClipperLib::ctUnion, polytree, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);  // offset results work with both EvenOdd and NonZero

    // write to ExPolygons object
    output->clear();
    PolyTreeToExPolygons(polytree, output);  //非常好的提取出了polytree里的多个多边形的方法
}

void
scaleClipperPolygons(ClipperLib::Paths &polygons, const double scale)
{
    for (ClipperLib::Paths::iterator it = polygons.begin(); it != polygons.end(); ++it) {
        for (ClipperLib::Path::iterator pit = (*it).begin(); pit != (*it).end(); ++pit) {
            (*pit).X *= scale;
            (*pit).Y *= scale;
        }
    }
}
//偏置多边形
void
offset(const xd::Polygons &polygons, ClipperLib::Paths* retval, const float delta,
    double scale, ClipperLib::JoinType joinType, double miterLimit)
{
    // 读取输入
    ClipperLib::Paths input;
    xdMultiPoints_to_ClipperPaths(polygons, &input);

    // 放大输入
    scaleClipperPolygons(input, scale);

    // 进行偏置
    ClipperLib::ClipperOffset co;
    if (joinType == jtRound) {
        co.ArcTolerance = miterLimit;
    } else {
        co.MiterLimit = miterLimit;
    }
    co.AddPaths(input, joinType, ClipperLib::etClosedPolygon);
    co.Execute(*retval, (delta*scale));

    // 缩小输出
    scaleClipperPolygons(*retval, 1/scale);
}

void
offset(const xd::Polygons &polygons, xd::Polygons* retval, const float delta,
    double scale, ClipperLib::JoinType joinType, double miterLimit)
{
    // 进行偏置
    ClipperLib::Paths output;
    offset(polygons, &output, delta, scale, joinType, miterLimit);

    // 转换成ExPolygons
    ClipperPaths_to_xdMultiPoints(output, retval);
}
//偏置Polylines
void
offset(const xd::Polylines &polylines, ClipperLib::Paths* retval, const float delta,
    double scale, ClipperLib::JoinType joinType, double miterLimit)
{
    // 读取输入
    ClipperLib::Paths input;
    xdMultiPoints_to_ClipperPaths(polylines, &input);

    // 放大输入
    scaleClipperPolygons(input, scale);

    // 进行偏置
    ClipperLib::ClipperOffset co;
    if (joinType == jtRound) {
        co.ArcTolerance = miterLimit;
    } else {
        co.MiterLimit = miterLimit;
    }
    co.AddPaths(input, joinType, ClipperLib::etOpenButt);
    co.Execute(*retval, (delta*scale));

    // 缩小输出
    scaleClipperPolygons(*retval, 1/scale);
}

void
offset(const xd::Polylines &polylines, xd::Polygons* retval, const float delta,
    double scale, ClipperLib::JoinType joinType, double miterLimit)
{
    // 进行偏置
    ClipperLib::Paths output;
    offset(polylines, &output, delta, scale, joinType, miterLimit);

    // 转换成ExPolygons
    ClipperPaths_to_xdMultiPoints(output, retval);
}
//偏置surface
void
offset(const xd::Surface &surface, xd::Surfaces* retval, const float delta,
    double scale, ClipperLib::JoinType joinType, double miterLimit)
{
    // 进行偏置
    xd::ExPolygons expp;
    offset(surface.expolygon, &expp, delta, scale, joinType, miterLimit);

    // 为每一个我们得到的每一个expolygon克隆输出为surface
    retval->clear();
    retval->reserve(expp.size());
    for (ExPolygons::iterator it = expp.begin(); it != expp.end(); ++it) {
        Surface s = surface;  // clone
        s.expolygon = *it;
        retval->push_back(s);
    }
}

void
offset(const xd::Polygons &polygons, xd::ExPolygons* retval, const float delta,
    double scale, ClipperLib::JoinType joinType, double miterLimit)
{
    // 进行偏置
    ClipperLib::Paths output;
    offset(polygons, &output, delta, scale, joinType, miterLimit);

    // 转换成ExPolygons
    ClipperPaths_to_xdExPolygons(output, retval);
}

void
offset2(const xd::Polygons &polygons, ClipperLib::Paths* retval, const float delta1,
    const float delta2, const double scale, const ClipperLib::JoinType joinType, const double miterLimit)
{
    // 读取输入
    ClipperLib::Paths input;
   xdMultiPoints_to_ClipperPaths(polygons, &input);

    // 放大输入
    scaleClipperPolygons(input, scale);

    // 准备ClipperOffset对象
    ClipperLib::ClipperOffset co;
    if (joinType == jtRound) {
        co.ArcTolerance = miterLimit;
    } else {
        co.MiterLimit = miterLimit;
    }

    // 进行第一次偏置
    ClipperLib::Paths output1;
    co.AddPaths(input, joinType, ClipperLib::etClosedPolygon);
    co.Execute(output1, (delta1*scale));

    // 进行第二次偏置
    co.Clear();
    co.AddPaths(output1, joinType, ClipperLib::etClosedPolygon);
    co.Execute(*retval, (delta2*scale));

    //缩小输出
    scaleClipperPolygons(*retval, 1/scale);
}

void
offset2(const xd::Polygons &polygons, xd::Polygons* retval, const float delta1,
    const float delta2, const double scale, const ClipperLib::JoinType joinType, const double miterLimit)
{
    //进行偏置
    ClipperLib::Paths output;
    offset2(polygons, &output, delta1, delta2, scale, joinType, miterLimit);

    //转换成 ExPolygons
    ClipperPaths_to_xdMultiPoints(output, retval);
}

void
offset2(const xd::Polygons &polygons, xd::ExPolygons* retval, const float delta1,
    const float delta2, const double scale, const ClipperLib::JoinType joinType, const double miterLimit)
{
    //进行偏置
    ClipperLib::Paths output;
    offset2(polygons, &output, delta1, delta2, scale, joinType, miterLimit);

    //转换成ExPolygons
    ClipperPaths_to_xdExPolygons(output, retval);
}
//下面是布尔差运算的函数封装
template <class T>
void _clipper_do(const ClipperLib::ClipType clipType, const xd::Polygons &subject,
    const xd::Polygons &clip, T* retval, const ClipperLib::PolyFillType fillType, const bool safety_offset_)
{
    //读取输入
    ClipperLib::Paths input_subject, input_clip;
    xdMultiPoints_to_ClipperPaths(subject, &input_subject);
    xdMultiPoints_to_ClipperPaths(clip,    &input_clip);

    //进行安全偏置
    if (safety_offset_) {
        if (clipType == ClipperLib::ctUnion) {
            safety_offset(&input_subject);
        } else {
            safety_offset(&input_clip);
        }
    }

    //初始化Clipper
    ClipperLib::Clipper clipper;
    clipper.Clear();

    //增加polygons
    clipper.AddPaths(input_subject, ClipperLib::ptSubject, true);
    clipper.AddPaths(input_clip, ClipperLib::ptClip, true);

    //进行操作
    clipper.Execute(clipType, *retval, fillType, fillType);
}

void _clipper_do(const ClipperLib::ClipType clipType, const xd::Polylines &subject,
    const xd::Polygons &clip, ClipperLib::PolyTree* retval, const ClipperLib::PolyFillType fillType,
    const bool safety_offset_)
{
    //读取输入
    ClipperLib::Paths input_subject, input_clip;
    xdMultiPoints_to_ClipperPaths(subject, &input_subject);
    xdMultiPoints_to_ClipperPaths(clip,    &input_clip);

    // perform safety offset
    if (safety_offset_) safety_offset(&input_clip);

    //初始化Clipper
    ClipperLib::Clipper clipper;
    clipper.Clear();

    //增加polygons
    clipper.AddPaths(input_subject, ClipperLib::ptSubject, false);
    clipper.AddPaths(input_clip,    ClipperLib::ptClip,    true);

    //进行操作
    clipper.Execute(clipType, *retval, fillType, fillType);
}

void _clipper(ClipperLib::ClipType clipType, const xd::Polygons &subject,
    const xd::Polygons &clip, xd::Polygons* retval, bool safety_offset_)
{
    //进行操作
    ClipperLib::Paths output;
    _clipper_do<ClipperLib::Paths>(clipType, subject, clip, &output, ClipperLib::pftNonZero, safety_offset_);

    //转换成Polygons
    ClipperPaths_to_xdMultiPoints(output, retval);
}

void _clipper(ClipperLib::ClipType clipType, const xd::Polygons &subject,
    const xd::Polygons &clip, xd::ExPolygons* retval, bool safety_offset_)
{
    // perform operation
    ClipperLib::PolyTree polytree;
    _clipper_do<ClipperLib::PolyTree>(clipType, subject, clip, &polytree, ClipperLib::pftNonZero, safety_offset_);

    // convert into ExPolygons
    PolyTreeToExPolygons(polytree, retval);
}

void _clipper(ClipperLib::ClipType clipType, const xd::Polylines &subject,
    const xd::Polygons &clip, xd::Polylines* retval, bool safety_offset_)
{
    // perform operation
    ClipperLib::PolyTree polytree;
    _clipper_do(clipType, subject, clip, &polytree, ClipperLib::pftNonZero, safety_offset_);

    // convert into Polylines
    ClipperLib::Paths output;
    ClipperLib::PolyTreeToPaths(polytree, output);   //此处的多此一举不理解
    ClipperPaths_to_xdMultiPoints(output, retval);
}

void _clipper(ClipperLib::ClipType clipType, const xd::Lines &subject,
    const xd::Polygons &clip, xd::Lines* retval, bool safety_offset_)
{
    // convert Lines to Polylines
    xd::Polylines polylines;
    polylines.reserve(subject.size());
    for (xd::Lines::const_iterator line = subject.begin(); line != subject.end(); ++line)
        polylines.push_back(*line);

    // perform operation
    _clipper(clipType, polylines, clip, &polylines, safety_offset_);

    // convert Polylines to Lines
    for (xd::Polylines::const_iterator polyline = polylines.begin(); polyline != polylines.end(); ++polyline)
        retval->push_back(*polyline);
}

void _clipper(ClipperLib::ClipType clipType, const xd::Polygons &subject,
    const xd::Polygons &clip, xd::Polylines* retval, bool safety_offset_)   //这个函数头文件，没有，因为只有上一个函数需要使用，暂时没看懂！
{
    // transform input polygons into polylines
    xd::Polylines polylines;
    polylines.reserve(subject.size());
    for (xd::Polygons::const_iterator polygon = subject.begin(); polygon != subject.end(); ++polygon)
        polylines.push_back(*polygon);  // implicit call to split_at_first_point()

    // perform clipping
    _clipper(clipType, polylines, clip, retval, safety_offset_);

    /* If the split_at_first_point() call above happens to split the polygon inside the clipping area
       we would get two consecutive polylines instead of a single one, so we go through them in order
       to recombine continuous polylines. */
    for (size_t i = 0; i < retval->size(); ++i) {
        for (size_t j = i+1; j < retval->size(); ++j) {
            if ((*retval)[i].points.back().coincides_with((*retval)[j].points.front())) {
                /* If last point of i coincides with first point of j,
                   append points of j to i and delete j */
                (*retval)[i].points.insert((*retval)[i].points.end(), (*retval)[j].points.begin()+1, (*retval)[j].points.end());
                retval->erase(retval->begin() + j);
                --j;
            } else if ((*retval)[i].points.front().coincides_with((*retval)[j].points.back())) {
                /* If first point of i coincides with last point of j,
                   prepend points of j to i and delete j */
                (*retval)[i].points.insert((*retval)[i].points.begin(), (*retval)[j].points.begin(), (*retval)[j].points.end()-1);
                retval->erase(retval->begin() + j);
                --j;
            } else if ((*retval)[i].points.front().coincides_with((*retval)[j].points.front())) {
                /* Since Clipper does not preserve orientation of polylines,
                   also check the case when first point of i coincides with first point of j. */
                (*retval)[j].reverse();
                (*retval)[i].points.insert((*retval)[i].points.begin(), (*retval)[j].points.begin(), (*retval)[j].points.end()-1);
                retval->erase(retval->begin() + j);
                --j;
            } else if ((*retval)[i].points.back().coincides_with((*retval)[j].points.back())) {
                /* Since Clipper does not preserve orientation of polylines,
                   also check the case when last point of i coincides with last point of j. */
                (*retval)[j].reverse();
                (*retval)[i].points.insert((*retval)[i].points.end(), (*retval)[j].points.begin()+1, (*retval)[j].points.end());
                retval->erase(retval->begin() + j);
                --j;
            }
        }
    }
}
//下面是布尔差的函数封装
template <class SubjectType, class ResultType>
void diff(const SubjectType &subject, const xd::Polygons &clip, ResultType* retval, bool safety_offset_)
{
    _clipper(ClipperLib::ctDifference, subject, clip, retval, safety_offset_);
}
template void diff<xd::Polygons, xd::ExPolygons>(const xd::Polygons &subject, const xd::Polygons &clip, xd::ExPolygons* retval, bool safety_offset_);
template void diff<xd::Polygons, xd::Polygons>(const xd::Polygons &subject, const xd::Polygons &clip, xd::Polygons* retval, bool safety_offset_);
template void diff<xd::Polygons, xd::Polylines>(const xd::Polygons &subject, const xd::Polygons &clip, xd::Polylines* retval, bool safety_offset_);
template void diff<xd::Polylines, xd::Polylines>(const xd::Polylines &subject, const xd::Polygons &clip, xd::Polylines* retval, bool safety_offset_);
template void diff<xd::Lines, xd::Lines>(const xd::Lines &subject, const xd::Polygons &clip, xd::Lines* retval, bool safety_offset_);

template <class SubjectType, class ResultType>
void diff(const SubjectType &subject, const xd::ExPolygons &clip, ResultType* retval, bool safety_offset_)
{
    xd::Polygons pp;
    for (xd::ExPolygons::const_iterator ex = clip.begin(); ex != clip.end(); ++ex) {
        xd::Polygons ppp = *ex;
        pp.insert(pp.end(), ppp.begin(), ppp.end());
    }
    diff(subject, pp, retval, safety_offset_);
}
template void diff<xd::Polygons, xd::ExPolygons>(const xd::Polygons &subject, const xd::ExPolygons &clip, xd::ExPolygons* retval, bool safety_offset_);
//下面是布尔交的函数封装
template <class SubjectType, class ResultType>
void intersection(const SubjectType &subject, const xd::Polygons &clip, ResultType* retval, bool safety_offset_)
{
    _clipper(ClipperLib::ctIntersection, subject, clip, retval, safety_offset_);
}
template void intersection<xd::Polygons, xd::ExPolygons>(const xd::Polygons &subject, const xd::Polygons &clip, xd::ExPolygons* retval, bool safety_offset_);
template void intersection<xd::Polygons, xd::Polygons>(const xd::Polygons &subject, const xd::Polygons &clip, xd::Polygons* retval, bool safety_offset_);
template void intersection<xd::Polygons, xd::Polylines>(const xd::Polygons &subject, const xd::Polygons &clip, xd::Polylines* retval, bool safety_offset_);
template void intersection<xd::Polylines, xd::Polylines>(const xd::Polylines &subject, const xd::Polygons &clip, xd::Polylines* retval, bool safety_offset_);
template void intersection<xd::Lines, xd::Lines>(const xd::Lines &subject, const xd::Polygons &clip, xd::Lines* retval, bool safety_offset_);

template <class SubjectType>
bool intersects(const SubjectType &subject, const xd::Polygons &clip, bool safety_offset_)
{
    SubjectType retval;
    intersection(subject, clip, &retval, safety_offset_);
    return !retval.empty();
}
template bool intersects<xd::Polygons>(const xd::Polygons &subject, const xd::Polygons &clip, bool safety_offset_);
template bool intersects<xd::Polylines>(const xd::Polylines &subject, const xd::Polygons &clip, bool safety_offset_);
template bool intersects<xd::Lines>(const xd::Lines &subject, const xd::Polygons &clip, bool safety_offset_);
//下面是布尔异或函数的封装
void xor_(const xd::Polygons &subject, const xd::Polygons &clip, xd::ExPolygons* retval,
    bool safety_offset_)
{
    _clipper(ClipperLib::ctXor, subject, clip, retval, safety_offset_);
}
//下面是布尔并函数的封装
template <class T>
void union_(const xd::Polygons &subject, T* retval, bool safety_offset_)
{
    xd::Polygons p;
    _clipper(ClipperLib::ctUnion, subject, p, retval, safety_offset_);
}
template void union_<xd::ExPolygons>(const xd::Polygons &subject, xd::ExPolygons* retval, bool safety_offset_);
template void union_<xd::Polygons>(const xd::Polygons &subject, xd::Polygons* retval, bool safety_offset_);

void union_(const xd::Polygons &subject1, const xd::Polygons &subject2, xd::Polygons* retval, bool safety_offset)
{
    Polygons pp = subject1;
    pp.insert(pp.end(), subject2.begin(), subject2.end());
    union_(pp, retval, safety_offset);
}

void union_pt(const xd::Polygons &subject, ClipperLib::PolyTree* retval, bool safety_offset_)
{
    xd::Polygons clip;
    _clipper_do<ClipperLib::PolyTree>(ClipperLib::ctUnion, subject, clip, retval, ClipperLib::pftEvenOdd, safety_offset_);
}

void union_pt_chained(const xd::Polygons &subject, xd::Polygons* retval, bool safety_offset_)
{
    ClipperLib::PolyTree pt;
    union_pt(subject, &pt, safety_offset_);
    traverse_pt(pt.Childs, retval);
}

static void traverse_pt(ClipperLib::PolyNodes &nodes, xd::Polygons* retval)
{
    /* use a nearest neighbor search to order these children
       TODO: supply start_near to chained_path() too? */

    // collect ordering points
    Points ordering_points;
    ordering_points.reserve(nodes.size());
    for (ClipperLib::PolyNodes::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
        Point p((*it)->Contour.front().X, (*it)->Contour.front().Y);
        ordering_points.push_back(p);
    }

    // perform the ordering
    ClipperLib::PolyNodes ordered_nodes;
    xd::Geometry::chained_path_items(ordering_points, nodes, ordered_nodes);

    // push results recursively
    for (ClipperLib::PolyNodes::iterator it = ordered_nodes.begin(); it != ordered_nodes.end(); ++it) {
        // traverse the next depth
        traverse_pt((*it)->Childs, retval);

        Polygon p;
        ClipperPath_to_xdMultiPoint((*it)->Contour, &p);
        retval->push_back(p);
        if ((*it)->IsHole()) retval->back().reverse();  // ccw
    }
}

void simplify_polygons(const xd::Polygons &subject, xd::Polygons* retval, bool preserve_collinear)
{
    // convert into Clipper polygons
    ClipperLib::Paths input_subject, output;
    xdMultiPoints_to_ClipperPaths(subject, &input_subject);

    if (preserve_collinear) {
        ClipperLib::Clipper c;
        c.PreserveCollinear(true);
        c.StrictlySimple(true);
        c.AddPaths(input_subject, ClipperLib::ptSubject, true);
        c.Execute(ClipperLib::ctUnion, output, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
    } else {
        ClipperLib::SimplifyPolygons(input_subject, output, ClipperLib::pftNonZero);
    }

    // convert into Slic3r polygons
    ClipperPaths_to_xdMultiPoints(output, retval);
}

void simplify_polygons(const xd::Polygons &subject, xd::ExPolygons* retval, bool preserve_collinear)
{
    if (!preserve_collinear) {
        Polygons polygons;
        simplify_polygons(subject, &polygons, preserve_collinear);
        union_(polygons, retval);
        return;
    }

    // convert into Clipper polygons
    ClipperLib::Paths input_subject;
    xdMultiPoints_to_ClipperPaths(subject, &input_subject);

    ClipperLib::PolyTree polytree;

    ClipperLib::Clipper c;
    c.PreserveCollinear(true);
    c.StrictlySimple(true);
    c.AddPaths(input_subject, ClipperLib::ptSubject, true);
    c.Execute(ClipperLib::ctUnion, polytree, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

    // convert into ExPolygons
    PolyTreeToExPolygons(polytree, retval);
}

void safety_offset(ClipperLib::Paths* paths)    //表示将一个路径偏置10.0的距离。为啥会安全？！
{
    // scale input
    scaleClipperPolygons(*paths, CLIPPER_OFFSET_SCALE);

    // perform offset (delta = scale 1e-05)
    ClipperLib::ClipperOffset co;
    co.MiterLimit = 2;
    co.AddPaths(*paths, ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
    co.Execute(*paths, 10.0 * CLIPPER_OFFSET_SCALE);

    // unscale output
    scaleClipperPolygons(*paths, 1.0/CLIPPER_OFFSET_SCALE);
}

}
