#ifndef ENCAPSULATIONCLIPPER
#define ENCAPSULATIONCLIPPER

#include "constdefine.h"
#include "clipper.hpp"
//#include "ExPolygon.hpp"
#include "polygon.h"
//#include "Surface.hpp"

// import these wherever we're included
using ClipperLib::jtMiter;  //偏置时仅尖的地方变为两个点
using ClipperLib::jtRound;  //偏置时凸顶点变为一段圆弧
using ClipperLib::jtSquare;  //偏置时凸顶点变为两个点

namespace xd {

#define CLIPPER_OFFSET_SCALE 100000.0   //定义调用clipper库偏置算法的放大比例


//void AddOuterPolyNodeToExPolygons(ClipperLib::PolyNode& polynode, Slic3r::ExPolygons& expolygons);  //又是一个好的递归函数的例子！
//void PolyTreeToExPolygons(ClipperLib::PolyTree& polytree, Slic3r::ExPolygons& expolygons);   //这个函数真好，值得学习

//函数名里面的MultiPoint实际用的是模板，因为MultiPoint是基类！
void xdMultiPoint_to_ClipperPath(const xd::MultiPoint &input, ClipperLib::Path* output);  //MultiPoint类转化为clipper的Path类
template <class T>
void xdMultiPoints_to_ClipperPaths(const T &input, ClipperLib::Paths* output);  //MultiPoints类转化为clipper的Paths类
template <class T>
void ClipperPath_to_xdMultiPoint(const ClipperLib::Path &input, T* output);  //clipper的Path类转换为MultiPoint类
template <class T>
void ClipperPaths_to_xdMultiPoints(const ClipperLib::Paths &input, T* output);  //clipper的Paths类转换为MultiPoints类
//void ClipperPaths_to_xdExPolygons(const ClipperLib::Paths &input, xd::ExPolygons* output);  //clipper的Paths类转换为ExPolygons类
                                                                       //感觉这个函数最有用了！！将多边形按块分开了！（实现用了最上面的函数）
void scaleClipperPolygons(ClipperLib::Paths &polygons, const double scale);  //clipper里的Paths类的所有坐标放大scale倍

// 偏置polygon
//void offset(const Slic3r::Polygons &polygons, ClipperLib::Paths* retval, const float delta,
//    double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtMiter,
//    double miterLimit = 3);  //在retval里返回偏置delta的多边形路径，scale代表转为clipper时的放大倍数，结果还要缩回来，加入类型可变化。clipper里miterLimit默认值是2
//void offset(const Slic3r::Polygons &polygons, Slic3r::Polygons* retval, const float delta,
//    double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtMiter,
//    double miterLimit = 3);  //同上，只是输出结果类型为Polygons类

// 偏置Polylines
//void offset(const Slic3r::Polylines &polylines, ClipperLib::Paths* retval, const float delta,
//    double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtSquare,
//    double miterLimit = 3);  //同上，只是输入为Polylines
//void offset(const Slic3r::Polylines &polylines, Slic3r::Polygons* retval, const float delta,
//    double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtSquare,
//    double miterLimit = 3);  //同上，只是输出结果类型为Polygons类
//void offset(const Slic3r::Surface &surface, Slic3r::Surfaces* retval, const float delta,
//    double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtSquare,
//    double miterLimit = 3);  //这个等看完surface再说！！！！！！！！！！！！！

//void offset(const Slic3r::Polygons &polygons, Slic3r::ExPolygons* retval, const float delta,
//    double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtMiter,
//    double miterLimit = 3);  //同上，只是输出是EXPolygons类！

//void offset2(const Slic3r::Polygons &polygons, ClipperLib::Paths* retval, const float delta1,
//    const float delta2, double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtMiter,
//    double miterLimit = 3);  //这个函数可是偏置了两次，看来偏置两次很常用！只是输出是Paths类
//void offset2(const Slic3r::Polygons &polygons, Slic3r::Polygons* retval, const float delta1,
//    const float delta2, double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtMiter,
//    double miterLimit = 3);  //同上，只是输出是Polygons类
//void offset2(const Slic3r::Polygons &polygons, Slic3r::ExPolygons* retval, const float delta1,
//    const float delta2, double scale = 100000, ClipperLib::JoinType joinType = ClipperLib::jtMiter,
//    double miterLimit = 3);  //同上，只是输出是Expolygons类

//template <class T>
//void _clipper_do(ClipperLib::ClipType clipType, const Slic3r::Polygons &subject,
//    const Slic3r::Polygons &clip, T* retval, bool safety_offset_);    //执行裁剪运算，返回应该是paths类型或者多叉树类型。
//void _clipper_do(ClipperLib::ClipType clipType, const Slic3r::Polylines &subject,
//    const Slic3r::Polygons &clip, ClipperLib::Paths* retval, bool safety_offset_);  //这个函数实现表面裁剪的是线段，非多边形
//void _clipper(ClipperLib::ClipType clipType, const Slic3r::Polygons &subject,
//    const Slic3r::Polygons &clip, Slic3r::Polygons* retval, bool safety_offset_);    //哈哈，调用第一个_clipper_do函数，实现模板的paths类，返回结果为MultiPoints，根据多态变为Polygons
//void _clipper(ClipperLib::ClipType clipType, const Slic3r::Polygons &subject,
//    const Slic3r::Polygons &clip, Slic3r::ExPolygons* retval, bool safety_offset_);  //哈哈，调用第一个_clipper_do函数，实现模板的多叉树类，返回结果为ExPolygons
//void _clipper(ClipperLib::ClipType clipType, const Slic3r::Polylines &subject,
//    const Slic3r::Polygons &clip, Slic3r::Polylines* retval);     //哈哈，调用第一个_clipper_do函数，实现模板的paths类，返回结果为MultiPoints，根据多态变为Polylines
//void _clipper(ClipperLib::ClipType clipType, const Slic3r::Lines &subject,
//    const Slic3r::Polygons &clip, Slic3r::Lines* retval);  //这个裁剪Lines，让人想到了直线填充算法的好办法！

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

