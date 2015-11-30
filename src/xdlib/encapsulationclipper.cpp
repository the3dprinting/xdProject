#include "encapsulationclipper.h"
#include "geometry.h"

//ѧϰ: vector<int>::value_type AnInt;
//�ȼ��� int AnInt;

namespace xd {

void AddOuterPolyNodeToExPolygons(ClipperLib::PolyNode& polynode, xd::ExPolygons* expolygons)
{
  size_t cnt = expolygons->size();
  expolygons->resize(cnt + 1);  //��һ��resize���ں������һ���������������
  ClipperPath_to_xdMultiPoint(polynode.Contour, &(*expolygons)[cnt].contour);  //������϶���ε�����
  (*expolygons)[cnt].holes.resize(polynode.ChildCount());
  for (int i = 0; i < polynode.ChildCount(); ++i)
  {
    ClipperPath_to_xdMultiPoint(polynode.Childs[i]->Contour, &(*expolygons)[cnt].holes[i]);  //������϶���ο��ܳ��ֵ�n����
    //Add outer polygons contained by (nested within) holes ...
    for (int j = 0; j < polynode.Childs[i]->ChildCount(); ++j)   //�м����ӣ��ͻ��м����������������
      AddOuterPolyNodeToExPolygons(*polynode.Childs[i]->Childs[j], expolygons);  //��һ��ʹ�õݹ�
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
        typename T::value_type p;  //value_typeһ����ģ�庯�����ã���Ϊģ���<>���治֪����ʲô����
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

    // perform union   �����÷�ֵ��ѧϰ����clipper���paths��Ϊpolytree�ķ���
    clipper.AddPaths(input, ClipperLib::ptSubject, true);
    ClipperLib::PolyTree polytree;
    clipper.Execute(ClipperLib::ctUnion, polytree, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);  // offset results work with both EvenOdd and NonZero

    // write to ExPolygons object
    output->clear();
    PolyTreeToExPolygons(polytree, output);  //�ǳ��õ���ȡ����polytree��Ķ������εķ���
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
//ƫ�ö����
void
offset(const xd::Polygons &polygons, ClipperLib::Paths* retval, const float delta,
    double scale, ClipperLib::JoinType joinType, double miterLimit)
{
    // ��ȡ����
    ClipperLib::Paths input;
    xdMultiPoints_to_ClipperPaths(polygons, &input);

    // �Ŵ�����
    scaleClipperPolygons(input, scale);

    // ����ƫ��
    ClipperLib::ClipperOffset co;
    if (joinType == jtRound) {
        co.ArcTolerance = miterLimit;
    } else {
        co.MiterLimit = miterLimit;
    }
    co.AddPaths(input, joinType, ClipperLib::etClosedPolygon);
    co.Execute(*retval, (delta*scale));

    // ��С���
    scaleClipperPolygons(*retval, 1/scale);
}

void
offset(const xd::Polygons &polygons, xd::Polygons* retval, const float delta,
    double scale, ClipperLib::JoinType joinType, double miterLimit)
{
    // ����ƫ��
    ClipperLib::Paths output;
    offset(polygons, &output, delta, scale, joinType, miterLimit);

    // ת����ExPolygons
    ClipperPaths_to_xdMultiPoints(output, retval);
}
//ƫ��Polylines
void
offset(const xd::Polylines &polylines, ClipperLib::Paths* retval, const float delta,
    double scale, ClipperLib::JoinType joinType, double miterLimit)
{
    // ��ȡ����
    ClipperLib::Paths input;
    xdMultiPoints_to_ClipperPaths(polylines, &input);

    // �Ŵ�����
    scaleClipperPolygons(input, scale);

    // ����ƫ��
    ClipperLib::ClipperOffset co;
    if (joinType == jtRound) {
        co.ArcTolerance = miterLimit;
    } else {
        co.MiterLimit = miterLimit;
    }
    co.AddPaths(input, joinType, ClipperLib::etOpenButt);
    co.Execute(*retval, (delta*scale));

    // ��С���
    scaleClipperPolygons(*retval, 1/scale);
}

void
offset(const xd::Polylines &polylines, xd::Polygons* retval, const float delta,
    double scale, ClipperLib::JoinType joinType, double miterLimit)
{
    // ����ƫ��
    ClipperLib::Paths output;
    offset(polylines, &output, delta, scale, joinType, miterLimit);

    // ת����ExPolygons
    ClipperPaths_to_xdMultiPoints(output, retval);
}
//ƫ��surface
void
offset(const xd::Surface &surface, xd::Surfaces* retval, const float delta,
    double scale, ClipperLib::JoinType joinType, double miterLimit)
{
    // ����ƫ��
    xd::ExPolygons expp;
    offset(surface.expolygon, &expp, delta, scale, joinType, miterLimit);

    // Ϊÿһ�����ǵõ���ÿһ��expolygon��¡���Ϊsurface
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
    // ����ƫ��
    ClipperLib::Paths output;
    offset(polygons, &output, delta, scale, joinType, miterLimit);

    // ת����ExPolygons
    ClipperPaths_to_xdExPolygons(output, retval);
}

void
offset2(const xd::Polygons &polygons, ClipperLib::Paths* retval, const float delta1,
    const float delta2, const double scale, const ClipperLib::JoinType joinType, const double miterLimit)
{
    // ��ȡ����
    ClipperLib::Paths input;
   xdMultiPoints_to_ClipperPaths(polygons, &input);

    // �Ŵ�����
    scaleClipperPolygons(input, scale);

    // ׼��ClipperOffset����
    ClipperLib::ClipperOffset co;
    if (joinType == jtRound) {
        co.ArcTolerance = miterLimit;
    } else {
        co.MiterLimit = miterLimit;
    }

    // ���е�һ��ƫ��
    ClipperLib::Paths output1;
    co.AddPaths(input, joinType, ClipperLib::etClosedPolygon);
    co.Execute(output1, (delta1*scale));

    // ���еڶ���ƫ��
    co.Clear();
    co.AddPaths(output1, joinType, ClipperLib::etClosedPolygon);
    co.Execute(*retval, (delta2*scale));

    //��С���
    scaleClipperPolygons(*retval, 1/scale);
}

void
offset2(const xd::Polygons &polygons, xd::Polygons* retval, const float delta1,
    const float delta2, const double scale, const ClipperLib::JoinType joinType, const double miterLimit)
{
    //����ƫ��
    ClipperLib::Paths output;
    offset2(polygons, &output, delta1, delta2, scale, joinType, miterLimit);

    //ת���� ExPolygons
    ClipperPaths_to_xdMultiPoints(output, retval);
}

void
offset2(const xd::Polygons &polygons, xd::ExPolygons* retval, const float delta1,
    const float delta2, const double scale, const ClipperLib::JoinType joinType, const double miterLimit)
{
    //����ƫ��
    ClipperLib::Paths output;
    offset2(polygons, &output, delta1, delta2, scale, joinType, miterLimit);

    //ת����ExPolygons
    ClipperPaths_to_xdExPolygons(output, retval);
}
//�����ǲ���������ĺ�����װ
template <class T>
void _clipper_do(const ClipperLib::ClipType clipType, const xd::Polygons &subject,
    const xd::Polygons &clip, T* retval, const ClipperLib::PolyFillType fillType, const bool safety_offset_)
{
    //��ȡ����
    ClipperLib::Paths input_subject, input_clip;
    xdMultiPoints_to_ClipperPaths(subject, &input_subject);
    xdMultiPoints_to_ClipperPaths(clip,    &input_clip);

    //���а�ȫƫ��
    if (safety_offset_) {
        if (clipType == ClipperLib::ctUnion) {
            safety_offset(&input_subject);
        } else {
            safety_offset(&input_clip);
        }
    }

    //��ʼ��Clipper
    ClipperLib::Clipper clipper;
    clipper.Clear();

    //����polygons
    clipper.AddPaths(input_subject, ClipperLib::ptSubject, true);
    clipper.AddPaths(input_clip, ClipperLib::ptClip, true);

    //���в���
    clipper.Execute(clipType, *retval, fillType, fillType);
}

void _clipper_do(const ClipperLib::ClipType clipType, const xd::Polylines &subject,
    const xd::Polygons &clip, ClipperLib::PolyTree* retval, const ClipperLib::PolyFillType fillType,
    const bool safety_offset_)
{
    //��ȡ����
    ClipperLib::Paths input_subject, input_clip;
    xdMultiPoints_to_ClipperPaths(subject, &input_subject);
    xdMultiPoints_to_ClipperPaths(clip,    &input_clip);

    // perform safety offset
    if (safety_offset_) safety_offset(&input_clip);

    //��ʼ��Clipper
    ClipperLib::Clipper clipper;
    clipper.Clear();

    //����polygons
    clipper.AddPaths(input_subject, ClipperLib::ptSubject, false);
    clipper.AddPaths(input_clip,    ClipperLib::ptClip,    true);

    //���в���
    clipper.Execute(clipType, *retval, fillType, fillType);
}

void _clipper(ClipperLib::ClipType clipType, const xd::Polygons &subject,
    const xd::Polygons &clip, xd::Polygons* retval, bool safety_offset_)
{
    //���в���
    ClipperLib::Paths output;
    _clipper_do<ClipperLib::Paths>(clipType, subject, clip, &output, ClipperLib::pftNonZero, safety_offset_);

    //ת����Polygons
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
    ClipperLib::PolyTreeToPaths(polytree, output);   //�˴��Ķ��һ�ٲ����
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
    const xd::Polygons &clip, xd::Polylines* retval, bool safety_offset_)   //�������ͷ�ļ���û�У���Ϊֻ����һ��������Ҫʹ�ã���ʱû������
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
//�����ǲ�����ĺ�����װ
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
//�����ǲ������ĺ�����װ
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
//�����ǲ���������ķ�װ
void xor_(const xd::Polygons &subject, const xd::Polygons &clip, xd::ExPolygons* retval,
    bool safety_offset_)
{
    _clipper(ClipperLib::ctXor, subject, clip, retval, safety_offset_);
}
//�����ǲ����������ķ�װ
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

void safety_offset(ClipperLib::Paths* paths)    //��ʾ��һ��·��ƫ��10.0�ľ��롣Ϊɶ�ᰲȫ����
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
