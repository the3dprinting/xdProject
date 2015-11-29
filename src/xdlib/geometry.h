#ifndef GEOMETRY
#define GEOMETRY

#include "boundingBox.h"
#include "polygon.h"
#include "polyline.h"

namespace xd { namespace Geometry {  //�˴��ֿ���һ�������ռ䣬������һЩȫ�ֺ���������

Polygon convex_hull(Points points);   //������ɢ���͹��
Polygon convex_hull(const Polygons &polygons);   //���ɶ������ε��ܵ�͹��
void chained_path(const Points &points, std::vector<Points::size_type> &retval, Point start_near); //��start_near�㿪ʼ������Ѱ��points�о���ǰһ��������ĵ���points�Ǳ�ֵ����¼��retval��
void chained_path(const Points &points, std::vector<Points::size_type> &retval); //ͬ�ϣ�ֻ������һ����ʱpoints.front()
template<class T> void chained_path_items(Points &points, T &items, T &retval);   //retval���ؽ�points�����Ǳ��Ӧitems��ֵ������⣿
bool directions_parallel(double angle1, double angle2, double max_diff = 0);  //���������Ƕ�֮���Ƿ����������ķ�Χ��Ϊ0�Ȼ�180�ȣ���ƽ��


}}
#endif // GEOMETRY

