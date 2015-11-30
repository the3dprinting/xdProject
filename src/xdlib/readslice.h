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
    TriangleMesh();  //不传参数构造函数，默认没有修复，初始化本身的stl
    TriangleMesh(const TriangleMesh &other);   //传入一个TriangleMesh类型，把other里面的stl复制到本身stl进来。
    TriangleMesh& operator= (TriangleMesh other);   //另一个TriangleMesh里的stl赋值给本身的stl
    void swap(TriangleMesh &other);  //和另一个TriangleMesh里的stl交换（因为传入引用）。
    ~TriangleMesh();
    void ReadSTLFile(char* input_file);    //读取slt文件，nice，调用了第三方库！
    void write_ascii(char* output_file);   //读取slt文件，nice，调用了第三方库！写出ascii格式的文件
    void write_binary(char* output_file);  //读取slt文件，nice，调用了第三方库！写成binary格式的文件
    void repair();   //如果stl没有修复，则修复一下，同时调用了stl_check_facets_exact函数！它增加了neighbors list，是进行this->require_shared_vertices的前提！→注意：太棒的使用stl三方库的例子了，值得学习！
    void WriteOBJFile(char* output_file);   //写成OBJ格式的文件
    void scale(float factor);    //放缩，并排除无效共享点
    void scale(const Pointf3 &versor);   //x y z 方向比例因子不同的放缩，并排除无效共享点
    void translate(float x, float y, float z);   //平移(x,y,z)距离，，并排除无效共享点
    void rotate(float angle, const Axis &axis);  //绕某个轴旋转一个角度
    void rotate_x(float angle);   //绕x轴旋转一个角度
    void rotate_y(float angle);   //绕y轴旋转一个角度
    void rotate_z(float angle);   //绕z轴旋转一个角度
    void flip(const Axis &axis);  //绕本身某个轴旋转
    void flip_x();   //绕本身x轴旋转
    void flip_y();   //绕本身y轴旋转
    void flip_z();   //绕本身z轴旋转
    void align_to_origin();   //本身移动到原点
    void rotate(double angle, Point* center);   //绕center点旋转angle角度
    TriangleMeshPtrs split() const;   //返回一堆meshs，如果我没猜错的话，一个stl的洞和外边界也分开了！
    void merge(const TriangleMesh &mesh);   //将mesh合并到本身的stl数据里，同时更新里面的size
    ExPolygons horizontal_projection() const;   //求得水平面上的映射平面，通过布尔并求得
    Polygon convex_hull();   //看了这个实现过程推测共享点是不在三角面片的空间转折点！！
    BoundingBoxf3 bounding_box() const;    //返回空间三维包围盒
    void reset_repair_stats();  //从新设定修复状态，需要修复的内容的数目都为零
    bool needed_repair() const;    //返回是否需要修复，true需要修复
    size_t facets_count() const;   //返回面片的数量
    stl_file stl;    //这个数据结构在stl.h里面定义，一个struct结构，包括文件指针，边，面等信息
    bool repaired;  //是否修复过的标志

    private:
    void require_shared_vertices();  //大概意思是在stl里面增加了共享点的内容和索引，不懂！
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
    TriangleMeshSlicer(TriangleMesh* _mesh);  //构造函数，将v_scaled_shared和facets_edges装满
    ~TriangleMeshSlicer();  //释放v_scaled_shared
    void slice(const std::vector<float> &z, std::vector<Polygons>* layers);
    void slice(const std::vector<float> &z, std::vector<ExPolygons>* layers);
    void slice_facet(float slice_z, const stl_facet &facet, const int &facet_idx, const float &min_z, const float &max_z, std::vector<IntersectionLine>* lines) const;
    void cut(float z, TriangleMesh* upper, TriangleMesh* lower);   //将stl分成两个，分别存放在两个TriangleMesh类里面

    private:
    typedef std::vector< std::vector<int> > t_facets_edges;
    t_facets_edges facets_edges; //最终组成面（对应stl数据中的面id）边对应的id值（某些id值重复，应该都重复2次）
    stl_vertex* v_scaled_shared; //克隆stl数据中的共享点，共享点究竟是什么？！
    void make_loops(std::vector<IntersectionLine> &lines, Polygons* loops);   //把lines组成循环的封闭多边形，放到loop里面
    void make_expolygons(const Polygons &loops, ExPolygons* slices);   //
    void make_expolygons_simple(std::vector<IntersectionLine> &lines, ExPolygons* slices);
    void make_expolygons(std::vector<IntersectionLine> &lines, ExPolygons* slices);
};

}  //结束xd命名空间

#endif // READSLICE_H

