#ifndef ENCAPSULATIONCLIPPER
#define ENCAPSULATIONCLIPPER

#include "constdefine.h"
#include "clipper.hpp"
//#include "ExPolygon.hpp"
#include "polygon.h"
//#include "Surface.hpp"

// import these wherever we're included
using ClipperLib::jtMiter;  //ƫ��ʱ����ĵط���Ϊ������
using ClipperLib::jtRound;  //ƫ��ʱ͹�����Ϊһ��Բ��
using ClipperLib::jtSquare;  //ƫ��ʱ͹�����Ϊ������

namespace xd {

#define CLIPPER_OFFSET_SCALE 100000.0   //�������clipper��ƫ���㷨�ķŴ����


//void AddOuterPolyNodeToExPolygons(ClipperLib::PolyNode& polynode, Slic3r::ExPolygons& expolygons);  //����һ���õĵݹ麯�������ӣ�
//void PolyTreeToExPolygons(ClipperLib::PolyTree& polytree, Slic3r::ExPolygons& expolygons);   //���������ã�ֵ��ѧϰ

//�����������MultiPointʵ���õ���ģ�壬��ΪMultiPoint�ǻ��࣡
void xdMultiPoint_to_ClipperPath(const xd::MultiPoint &input, ClipperLib::Path* output);  //MultiPoint��ת��Ϊclipper��Path��
template <class T>
void xdMultiPoints_to_ClipperPaths(const T &input, ClipperLib::Paths* output);  //MultiPoints��ת��Ϊclipper��Paths��
template <class T>
void ClipperPath_to_xdMultiPoint(const ClipperLib::Path &input, T* output);  //clipper��Path��ת��ΪMultiPoint��
template <class T>
void ClipperPaths_to_xdMultiPoints(const ClipperLib::Paths &input, T* output);  //clipper��Paths��ת��ΪMultiPoints��
//void ClipperPaths_to_xdExPolygons(const ClipperLib::Paths &input, xd::ExPolygons* output);  //clipper��Paths��ת��ΪExPolygons��
                                                                       //�о���������������ˣ���������ΰ���ֿ��ˣ���ʵ������������ĺ�����
void scaleClipperPolygons(ClipperLib::Paths &polygons, const double scale);  //clipper���Paths�����������Ŵ�scale��

// ƫ��polygon
//void offset(const Slic3r::Polygons &polygons, ClipperLib::Paths* retval, const float delta,
//    double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtMiter,
//    double miterLimit = 3);  //��retval�ﷵ��ƫ��delta�Ķ����·����scale����תΪclipperʱ�ķŴ����������Ҫ���������������Ϳɱ仯��clipper��miterLimitĬ��ֵ��2
//void offset(const Slic3r::Polygons &polygons, Slic3r::Polygons* retval, const float delta,
//    double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtMiter,
//    double miterLimit = 3);  //ͬ�ϣ�ֻ������������ΪPolygons��

// ƫ��Polylines
//void offset(const Slic3r::Polylines &polylines, ClipperLib::Paths* retval, const float delta,
//    double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtSquare,
//    double miterLimit = 3);  //ͬ�ϣ�ֻ������ΪPolylines
//void offset(const Slic3r::Polylines &polylines, Slic3r::Polygons* retval, const float delta,
//    double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtSquare,
//    double miterLimit = 3);  //ͬ�ϣ�ֻ������������ΪPolygons��
//void offset(const Slic3r::Surface &surface, Slic3r::Surfaces* retval, const float delta,
//    double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtSquare,
//    double miterLimit = 3);  //����ȿ���surface��˵��������������������������

//void offset(const Slic3r::Polygons &polygons, Slic3r::ExPolygons* retval, const float delta,
//    double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtMiter,
//    double miterLimit = 3);  //ͬ�ϣ�ֻ�������EXPolygons�࣡

//void offset2(const Slic3r::Polygons &polygons, ClipperLib::Paths* retval, const float delta1,
//    const float delta2, double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtMiter,
//    double miterLimit = 3);  //�����������ƫ�������Σ�����ƫ�����κܳ��ã�ֻ�������Paths��
//void offset2(const Slic3r::Polygons &polygons, Slic3r::Polygons* retval, const float delta1,
//    const float delta2, double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtMiter,
//    double miterLimit = 3);  //ͬ�ϣ�ֻ�������Polygons��
//void offset2(const Slic3r::Polygons &polygons, Slic3r::ExPolygons* retval, const float delta1,
//    const float delta2, double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtMiter,
//    double miterLimit = 3);  //ͬ�ϣ�ֻ�������Expolygons��

//template <class T>
//void _clipper_do(ClipperLib::ClipType clipType, const Slic3r::Polygons &subject,
//    const Slic3r::Polygons &clip, T* retval, bool safety_offset_);    //ִ�вü����㣬����Ӧ����paths���ͻ��߶�������͡�
//void _clipper_do(ClipperLib::ClipType clipType, const Slic3r::Polylines &subject,
//    const Slic3r::Polygons &clip, ClipperLib::Paths* retval, bool safety_offset_);  //�������ʵ�ֱ���ü������߶Σ��Ƕ����
//void _clipper(ClipperLib::ClipType clipType, const Slic3r::Polygons &subject,
//    const Slic3r::Polygons &clip, Slic3r::Polygons* retval, bool safety_offset_);    //���������õ�һ��_clipper_do������ʵ��ģ���paths�࣬���ؽ��ΪMultiPoints�����ݶ�̬��ΪPolygons
//void _clipper(ClipperLib::ClipType clipType, const Slic3r::Polygons &subject,
//    const Slic3r::Polygons &clip, Slic3r::ExPolygons* retval, bool safety_offset_);  //���������õ�һ��_clipper_do������ʵ��ģ��Ķ�����࣬���ؽ��ΪExPolygons
//void _clipper(ClipperLib::ClipType clipType, const Slic3r::Polylines &subject,
//    const Slic3r::Polygons &clip, Slic3r::Polylines* retval);     //���������õ�һ��_clipper_do������ʵ��ģ���paths�࣬���ؽ��ΪMultiPoints�����ݶ�̬��ΪPolylines
//void _clipper(ClipperLib::ClipType clipType, const Slic3r::Lines &subject,
//    const Slic3r::Polygons &clip, Slic3r::Lines* retval);  //����ü�Lines�������뵽��ֱ������㷨�ĺð취��

//template <class SubjectType, class ResultType>
//void diff(const SubjectType &subject, const Slic3r::Polygons &clip, ResultType* retval, bool safety_offset_ = false);

//template <class SubjectType, class ResultType>
//void diff(const SubjectType &subject, const Slic3r::ExPolygons &clip, ResultType* retval, bool safety_offset_ = false);

//template <class SubjectType, class ResultType>
//void intersection(const SubjectType &subject, const Slic3r::Polygons &clip, ResultType* retval, bool safety_offset_ = false);

//template <class SubjectType>
//bool intersects(const SubjectType &subject, const Slic3r::Polygons &clip, bool safety_offset_ = false);

//void xor_(const Slic3r::Polygons &subject, const Slic3r::Polygons &clip, Slic3r::ExPolygons* retval,
//    bool safety_offset_ = false);

//template <class T>
//void union_(const Slic3r::Polygons &subject, T* retval, bool safety_offset_ = false);

//void union_(const Slic3r::Polygons &subject1, const Slic3r::Polygons &subject2, Slic3r::Polygons* retval, bool safety_offset = false);

//void union_pt(const Slic3r::Polygons &subject, ClipperLib::PolyTree* retval, bool safety_offset_ = false);
//void union_pt_chained(const Slic3r::Polygons &subject, Slic3r::Polygons* retval, bool safety_offset_ = false);
//static void traverse_pt(ClipperLib::PolyNodes &nodes, Slic3r::Polygons* retval);

void simplify_polygons(const xd::Polygons &subject, xd::Polygons* retval, bool preserve_collinear = false);
//void simplify_polygons(const Slic3r::Polygons &subject, Slic3r::ExPolygons* retval, bool preserve_collinear = false);

//void safety_offset(ClipperLib::Paths* paths);



}

#endif // ENCAPSULATIONCLIPPER

