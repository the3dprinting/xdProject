#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "constdefine.h"
#include "point.h"
#include "polygon.h"

namespace xd {

typedef Point   Size;
typedef Point3  Size3;
typedef Pointf  Sizef;
typedef Pointf3 Sizef3;

template <class PointClass>
class BoundingBoxBase
{
    public:
    PointClass min;
    PointClass max;
    bool defined;

    BoundingBoxBase() : defined(false) {};   //��ʼ�����캯����û�д������ݣ���˱�־��definedΪ�٣�
    BoundingBoxBase(const std::vector<PointClass> &points);  //����һϵ�е㣬�������͸������min��max��ֵ�����ϣ����definedΪ�棡
    void merge(const PointClass &point);  //����һ���㣬������min��max�������ʼû�е㣬���min��max������Ϊpoint
    void merge(const std::vector<PointClass> &points);  //����points�󣬸��µõ���min��max��ʵ�ֵ���������ĺ���
    void merge(const BoundingBoxBase<PointClass> &bb);  //�����µ�bb�󣬸���min��max
    void scale(double factor);   //��min��max�Ŵ�һ��factor��
    PointClass size() const;   //���ذ�Χ���ܳ��Ϳ���ɵ�PointClass�࣬����һ����
    void translate(coordf_t x, coordf_t y);   //��min��maxƽ��(x,y)
    void offset(coordf_t delta);  //minƽ�ƣ�-delta��-delta����maxƽ�ƣ�delta��delta�����൱��ƫ��������delta
    PointClass center() const;   //���ذ�Χ�е����ĵ��PointClass��
};

template <class PointClass>
class BoundingBox3Base : public BoundingBoxBase<PointClass>
{
    public:
    BoundingBox3Base() : BoundingBoxBase<PointClass>() {};    //ͬBoundingBoxBase
    BoundingBox3Base(const std::vector<PointClass> &points);  //����һ��z��ֵ
    void merge(const PointClass &point);  //ͬ��ά������z
    void merge(const std::vector<PointClass> &points);  //ͬ��ά������z
    void merge(const BoundingBox3Base<PointClass> &bb);  //ͬ��ά������z
    PointClass size() const;    ////���ذ�Χ���ܳ���������ɵ�PointClass�࣬����һ����
    void translate(coordf_t x, coordf_t y, coordf_t z);   //ͬ��ά������z
    void offset(coordf_t delta);  //ͬ��ά������z
    PointClass center() const;    //ͬ��ά������z
};

class BoundingBox : public BoundingBoxBase<Point>
{
    public:
    void polygon(Polygon* polygon) const;   //����Χ�е��ĸ��㸳ֵ��polygon
    Polygon polygon() const;   //����Χ�е��ĸ��㸳ֵ��һ��polygon��Ȼ�󷵻�

    BoundingBox() : BoundingBoxBase<Point>() {};
    BoundingBox(const Points &points) : BoundingBoxBase<Point>(points) {};
    BoundingBox(const Lines &lines);   //��һϵ�е�line�������˵���Ϊ�����Ĺ��캯��
};

/*
class BoundingBox3  : public BoundingBox3Base<Point3> {};
*/

class BoundingBoxf : public BoundingBoxBase<Pointf> {
    public:
    BoundingBoxf() : BoundingBoxBase<Pointf>() {};
    BoundingBoxf(const std::vector<Pointf> &points) : BoundingBoxBase<Pointf>(points) {};
};

class BoundingBoxf3 : public BoundingBox3Base<Pointf3> {
    public:
    BoundingBoxf3() : BoundingBox3Base<Pointf3>() {};
    BoundingBoxf3(const std::vector<Pointf3> &points) : BoundingBox3Base<Pointf3>(points) {};
};

}

#endif // BOUNDINGBOX_H

