#ifndef POLYLINE
#define POLYLINE

#include "line.h"
#include "multiPoint.h"
#include <string>

namespace xd {
class ExPolygon;
class Polyline;
typedef std::vector<Polyline> Polylines;

class Polyline : public MultiPoint {
    public:
    operator Polylines() const;  //����ת����������Polylineת��ΪPolylines����
    operator Line() const;  //����ת����������Polylineת��ΪLine����
    Point last_point() const;   //ʵ�ָ����麯�������ر���Points�е����һ����
    Point leftmost_point() const;  //����Points��x������С�ĵ㣬������ߵĵ�
    Lines lines() const;  //��Points�еĵ�ÿ����һ��浽Lines�����5�����Polyline��������4���߶�
    void clip_end(double distance);  //�����һ�����ȥdistance������߶Σ����distance�������߶γ��ȣ���PointsΪ��
    void clip_start(double distance);  //ͬ�ϣ�ֻ������ǰ���ȥdistance����
    void extend_end(double distance);  //��β�߶��ӳ�distance�ľ��룬�ӳ�����������һ���߶εķ���
    void extend_start(double distance);  //ͬ�ϣ���ͷ�߶��ӳ�
    Points equally_spaced_points(double distance) const;  //ÿ�����ȵľ���ȡһ���㣬����ָPoints�߶�֮�ͣ��е�һ���㣬δ�������һ���㣡ֻ�е����е�ľ���֮�͵���distance��������ʱ�������һ���� *��ʱ���ٿ�������
    void simplify(double tolerance);  //������߶Σ�ֻҪ����Ҹ�С��tolerance���ɡ�ʹ���˸����_douglas_peucker����ʵ��
    template <class T> void simplify_by_visibility(const T &area);/* This method simplifies all *lines* contained in the supplied area */ //��������area���Ʋ�area���ܲ�ֹһ�飩�е����е�ɾ����
    void split_at(const Point &point, Polyline* p1, Polyline* p2) const;  //��Points��point��������ĵ�ֿ�������Polyline����һ�����һ������point���ڶ�����һ������point
    bool is_straight() const;  //���polyline�������߶�����һ�����ɵ�һ����͵ڶ�������ɵ��߶�ƽ�У��򷵻�true�����򷵻�false
    std::string wkt() const;  //LINESTRING((x1 y1,x2 y2,...,xn,yn))���ַ�����ʽ����

};

}

#endif // POLYLINE

