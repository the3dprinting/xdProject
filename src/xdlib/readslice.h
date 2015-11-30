#ifndef READSLICE_H
#define READSLICE_H

#include <../admesh/stl.h>
#include <vector>
#include "boundingBox.h"
#include "line.h"
#include "point.h"
#include "polygon.h"
#include "exPolygon.h"
#include "constdefine.h"

namespace xd {

class TriangleMesh;
class TriangleMeshSlicer;
typedef std::vector<TriangleMesh*> TriangleMeshPtrs;

class TriangleMesh
{
    public:
    TriangleMesh();  //�����������캯����Ĭ��û���޸�����ʼ�������stl
    TriangleMesh(const TriangleMesh &other);   //����һ��TriangleMesh���ͣ���other�����stl���Ƶ�����stl������
    TriangleMesh& operator= (TriangleMesh other);   //��һ��TriangleMesh���stl��ֵ�������stl
    void swap(TriangleMesh &other);  //����һ��TriangleMesh���stl��������Ϊ�������ã���
    ~TriangleMesh();
    void ReadSTLFile(char* input_file);    //��ȡslt�ļ���nice�������˵������⣡
    void write_ascii(char* output_file);   //��ȡslt�ļ���nice�������˵������⣡д��ascii��ʽ���ļ�
    void write_binary(char* output_file);  //��ȡslt�ļ���nice�������˵������⣡д��binary��ʽ���ļ�
    void repair();   //���stlû���޸������޸�һ�£�ͬʱ������stl_check_facets_exact��������������neighbors list���ǽ���this->require_shared_vertices��ǰ�ᣡ��ע�⣺̫����ʹ��stl������������ˣ�ֵ��ѧϰ��
    void WriteOBJFile(char* output_file);   //д��OBJ��ʽ���ļ�
    void scale(float factor);    //���������ų���Ч�����
    void scale(const Pointf3 &versor);   //x y z ����������Ӳ�ͬ�ķ��������ų���Ч�����
    void translate(float x, float y, float z);   //ƽ��(x,y,z)���룬�����ų���Ч�����
    void rotate(float angle, const Axis &axis);  //��ĳ������תһ���Ƕ�
    void rotate_x(float angle);   //��x����תһ���Ƕ�
    void rotate_y(float angle);   //��y����תһ���Ƕ�
    void rotate_z(float angle);   //��z����תһ���Ƕ�
    void flip(const Axis &axis);  //�Ʊ���ĳ������ת
    void flip_x();   //�Ʊ���x����ת
    void flip_y();   //�Ʊ���y����ת
    void flip_z();   //�Ʊ���z����ת
    void align_to_origin();   //�����ƶ���ԭ��
    void rotate(double angle, Point* center);   //��center����תangle�Ƕ�
    TriangleMeshPtrs split() const;   //����һ��meshs�������û�´�Ļ���һ��stl�Ķ�����߽�Ҳ�ֿ��ˣ�
    void merge(const TriangleMesh &mesh);   //��mesh�ϲ��������stl�����ͬʱ���������size
    ExPolygons horizontal_projection() const;   //���ˮƽ���ϵ�ӳ��ƽ�棬ͨ�����������
    Polygon convex_hull();   //�������ʵ�ֹ����Ʋ⹲����ǲ���������Ƭ�Ŀռ�ת�۵㣡��
    BoundingBoxf3 bounding_box() const;    //���ؿռ���ά��Χ��
    void reset_repair_stats();  //�����趨�޸�״̬����Ҫ�޸������ݵ���Ŀ��Ϊ��
    bool needed_repair() const;    //�����Ƿ���Ҫ�޸���true��Ҫ�޸�
    size_t facets_count() const;   //������Ƭ������
    stl_file stl;    //������ݽṹ��stl.h���涨�壬һ��struct�ṹ�������ļ�ָ�룬�ߣ������Ϣ
    bool repaired;  //�Ƿ��޸����ı�־

    private:
    void require_shared_vertices();  //�����˼����stl���������˹��������ݺ�������������
    friend class TriangleMeshSlicer;
};

enum FacetEdgeType { feNone, feTop, feBottom, feHorizontal };

class IntersectionPoint : public Point
{
    public:
    int point_id;
    int edge_id;
    IntersectionPoint() : point_id(-1), edge_id(-1) {};
};

class IntersectionLine : public Line
{
    public:
    int             a_id;
    int             b_id;
    int             edge_a_id;
    int             edge_b_id;
    FacetEdgeType   edge_type;
    bool            skip;
    IntersectionLine() : a_id(-1), b_id(-1), edge_a_id(-1), edge_b_id(-1), edge_type(feNone), skip(false) {};
};
typedef std::vector<IntersectionLine> IntersectionLines;
typedef std::vector<IntersectionLine*> IntersectionLinePtrs;

class TriangleMeshSlicer
{
    public:
    TriangleMesh* mesh;
    TriangleMeshSlicer(TriangleMesh* _mesh);  //���캯������v_scaled_shared��facets_edgesװ��
    ~TriangleMeshSlicer();  //�ͷ�v_scaled_shared
    void slice(const std::vector<float> &z, std::vector<Polygons>* layers);
    void slice(const std::vector<float> &z, std::vector<ExPolygons>* layers);
    void slice_facet(float slice_z, const stl_facet &facet, const int &facet_idx, const float &min_z, const float &max_z, std::vector<IntersectionLine>* lines) const;
    void cut(float z, TriangleMesh* upper, TriangleMesh* lower);   //��stl�ֳ��������ֱ���������TriangleMesh������

    private:
    typedef std::vector< std::vector<int> > t_facets_edges;
    t_facets_edges facets_edges; //��������棨��Ӧstl�����е���id���߶�Ӧ��idֵ��ĳЩidֵ�ظ���Ӧ�ö��ظ�2�Σ�
    stl_vertex* v_scaled_shared; //��¡stl�����еĹ���㣬����㾿����ʲô����
    void make_loops(std::vector<IntersectionLine> &lines, Polygons* loops);   //��lines���ѭ���ķ�ն���Σ��ŵ�loop����
    void make_expolygons(const Polygons &loops, ExPolygons* slices);   //
    void make_expolygons_simple(std::vector<IntersectionLine> &lines, ExPolygons* slices);
    void make_expolygons(std::vector<IntersectionLine> &lines, ExPolygons* slices);
};

}  //����xd�����ռ�

#endif // READSLICE_H

