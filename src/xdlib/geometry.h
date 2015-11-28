#ifndef GEOMETRY
#define GEOMETRY

#include "boundingBox.h"
#include "polygon.h"
#include "polyline.h"

namespace xd { namespace Geometry {  //�˴��ֿ���һ�������ռ䣬������һЩȫ�ֺ���������

Polygon convex_hull(Points points);   //������ɢ���͹��
Polygon convex_hull(const Polygons &polygons);   //���ɶ������ε��ܵ�͹��
bool directions_parallel(double angle1, double angle2, double max_diff = 0);  //���������Ƕ�֮���Ƿ����������ķ�Χ��Ϊ0�Ȼ�180�ȣ���ƽ��

}}
#endif // GEOMETRY

