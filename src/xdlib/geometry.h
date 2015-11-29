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
template<class T> bool contains(const std::vector<T> &vector, const Point &point);  //����T������ɵ�vector�Ƿ����point��T�Ʋ�Ϊpolygon��
double rad2deg(double angle);  //����ת��Ϊ�Ƕȷ���
double rad2deg_dir(double angle);  //���غʹ��뻡�ȴ�ֱ�Ļ��ȵĽǶ�ֵ
double deg2rad(double angle);  //�Ƕ�ת��Ϊ���ȷ���
void simplify_polygons(const Polygons &polygons, double tolerance, Polygons* retval);

class ArrangeItem {
    public:
    Pointf pos;
    size_t index_x, index_y;
    coordf_t dist;
};
class ArrangeItemIndex {
    public:
    coordf_t index;
    ArrangeItem item;
    ArrangeItemIndex(coordf_t _index, ArrangeItem _item) : index(_index), item(_item) {};
};

double linint(double value, double oldmin, double oldmax, double newmin, double newmax);
Pointfs arrange(size_t total_parts, Pointf part, coordf_t dist, const BoundingBoxf &bb = BoundingBoxf());



}}
#endif // GEOMETRY

