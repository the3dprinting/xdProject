#include <cmath>
#include <queue>
#include <deque>
#include <set>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <math.h>
#include <assert.h>
#include <QDebug>
#include "readSlice.h"
#include "encapsulationClipper.h"
#include "geometry.h"


namespace xd {

TriangleMesh::TriangleMesh()
    : repaired(false)
{
    stl_initialize(&this->stl);
}

TriangleMesh::TriangleMesh(const TriangleMesh &other)
    : stl(other.stl), repaired(other.repaired)
{
    this->stl.heads = NULL;
    this->stl.tail  = NULL;
    this->stl.error = other.stl.error;
    if (other.stl.facet_start != NULL) {
        this->stl.facet_start = (stl_facet*)calloc(other.stl.stats.number_of_facets, sizeof(stl_facet));
        std::copy(other.stl.facet_start, other.stl.facet_start + other.stl.stats.number_of_facets, this->stl.facet_start);
    }
    if (other.stl.neighbors_start != NULL) {
        this->stl.neighbors_start = (stl_neighbors*)calloc(other.stl.stats.number_of_facets, sizeof(stl_neighbors));
        std::copy(other.stl.neighbors_start, other.stl.neighbors_start + other.stl.stats.number_of_facets, this->stl.neighbors_start);
    }
    if (other.stl.v_indices != NULL) {
        this->stl.v_indices = (v_indices_struct*)calloc(other.stl.stats.number_of_facets, sizeof(v_indices_struct));
        std::copy(other.stl.v_indices, other.stl.v_indices + other.stl.stats.number_of_facets, this->stl.v_indices);
    }
    if (other.stl.v_shared != NULL) {
        this->stl.v_shared = (stl_vertex*)calloc(other.stl.stats.shared_vertices, sizeof(stl_vertex));
        std::copy(other.stl.v_shared, other.stl.v_shared + other.stl.stats.shared_vertices, this->stl.v_shared);
    }
}

TriangleMesh& TriangleMesh::operator= (TriangleMesh other)
{
    this->swap(other);
    return *this;
}

void
TriangleMesh::swap(TriangleMesh &other)
{
    std::swap(this->stl,                 other.stl);
    std::swap(this->repaired,            other.repaired);
    std::swap(this->stl.facet_start,     other.stl.facet_start);
    std::swap(this->stl.neighbors_start, other.stl.neighbors_start);
    std::swap(this->stl.v_indices,       other.stl.v_indices);
    std::swap(this->stl.v_shared,        other.stl.v_shared);
}

TriangleMesh::~TriangleMesh() {
    stl_close(&this->stl);
}

void
TriangleMesh::ReadSTLFile(char* input_file) {
    stl_open(&stl, input_file);
}

void
TriangleMesh::write_ascii(char* output_file)
{
    stl_write_ascii(&this->stl, output_file, "");
}

void
TriangleMesh::write_binary(char* output_file)
{
    stl_write_binary(&this->stl, output_file, "");
}

void
TriangleMesh::repair() {   //太棒的使用stl三方库的例子了，值得学习！
    if (this->repaired) return;

    // admesh fails when repairing empty meshes
    if (this->stl.stats.number_of_facets == 0) return;

    // checking exact
    stl_check_facets_exact(&stl);   //这里这个函数相当重要，把stl的相邻边序列加上了！
    stl.stats.facets_w_1_bad_edge = (stl.stats.connected_facets_2_edge - stl.stats.connected_facets_3_edge);
    stl.stats.facets_w_2_bad_edge = (stl.stats.connected_facets_1_edge - stl.stats.connected_facets_2_edge);
    stl.stats.facets_w_3_bad_edge = (stl.stats.number_of_facets - stl.stats.connected_facets_1_edge);

    // checking nearby
    int last_edges_fixed = 0;
    float tolerance = stl.stats.shortest_edge;
    float increment = stl.stats.bounding_diameter / 10000.0;
    int iterations = 2;
    if (stl.stats.connected_facets_3_edge < stl.stats.number_of_facets) {
        for (int i = 0; i < iterations; i++) {
            if (stl.stats.connected_facets_3_edge < stl.stats.number_of_facets) {
                //printf("Checking nearby. Tolerance= %f Iteration=%d of %d...", tolerance, i + 1, iterations);
                stl_check_facets_nearby(&stl, tolerance);
                //printf("  Fixed %d edges.\n", stl.stats.edges_fixed - last_edges_fixed);
                last_edges_fixed = stl.stats.edges_fixed;
                tolerance += increment;
            } else {
                break;
            }
        }
    }

    // remove_unconnected
    if (stl.stats.connected_facets_3_edge <  stl.stats.number_of_facets) {
        stl_remove_unconnected_facets(&stl);
    }

    // fill_holes
    if (stl.stats.connected_facets_3_edge < stl.stats.number_of_facets) {
        stl_fill_holes(&stl);
        stl_clear_error(&stl);
    }

    // normal_directions
    stl_fix_normal_directions(&stl);

    // normal_values
    stl_fix_normal_values(&stl);

    // always calculate the volume and reverse all normals if volume is negative
    stl_calculate_volume(&stl);

    // neighbors
    stl_verify_neighbors(&stl);

    this->repaired = true;
}

void
TriangleMesh::reset_repair_stats() {
    this->stl.stats.degenerate_facets   = 0;
    this->stl.stats.edges_fixed         = 0;
    this->stl.stats.facets_removed      = 0;
    this->stl.stats.facets_added        = 0;
    this->stl.stats.facets_reversed     = 0;
    this->stl.stats.backwards_edges     = 0;
    this->stl.stats.normals_fixed       = 0;
}

bool
TriangleMesh::needed_repair() const
{
    return this->stl.stats.degenerate_facets    > 0
        || this->stl.stats.edges_fixed          > 0
        || this->stl.stats.facets_removed       > 0
        || this->stl.stats.facets_added         > 0
        || this->stl.stats.facets_reversed      > 0
        || this->stl.stats.backwards_edges      > 0;
}

size_t
TriangleMesh::facets_count() const
{
    return this->stl.stats.number_of_facets;
}

void
TriangleMesh::WriteOBJFile(char* output_file) {
    stl_generate_shared_vertices(&stl);
    stl_write_obj(&stl, output_file);
}

void TriangleMesh::scale(float factor)
{
    stl_scale(&(this->stl), factor);
    stl_invalidate_shared_vertices(&this->stl);
}

void TriangleMesh::scale(const Pointf3 &versor)
{
    float fversor[3];
    fversor[0] = versor.x;
    fversor[1] = versor.y;
    fversor[2] = versor.z;
    stl_scale_versor(&this->stl, fversor);
    stl_invalidate_shared_vertices(&this->stl);
}

void TriangleMesh::translate(float x, float y, float z)
{
    stl_translate_relative(&(this->stl), x, y, z);
    stl_invalidate_shared_vertices(&this->stl);
}

void TriangleMesh::rotate(float angle, const Axis &axis)
{
    // admesh uses degrees
    angle = xd::Geometry::rad2deg(angle);

    if (axis == X) {
        stl_rotate_x(&(this->stl), angle);
    } else if (axis == Y) {
        stl_rotate_y(&(this->stl), angle);
    } else if (axis == Z) {
        stl_rotate_z(&(this->stl), angle);
    }
    stl_invalidate_shared_vertices(&this->stl);
}

void TriangleMesh::rotate_x(float angle)
{
    this->rotate(angle, X);
}

void TriangleMesh::rotate_y(float angle)
{
    this->rotate(angle, Y);
}

void TriangleMesh::rotate_z(float angle)
{
    this->rotate(angle, Z);
}

void TriangleMesh::flip(const Axis &axis)
{
    if (axis == X) {
        stl_mirror_yz(&this->stl);
    } else if (axis == Y) {
        stl_mirror_xz(&this->stl);
    } else if (axis == Z) {
        stl_mirror_xy(&this->stl);
    }
    stl_invalidate_shared_vertices(&this->stl);
}

void TriangleMesh::flip_x()
{
    this->flip(X);
}

void TriangleMesh::flip_y()
{
    this->flip(Y);
}

void TriangleMesh::flip_z()
{
    this->flip(Z);
}

void TriangleMesh::align_to_origin()
{
    this->translate(
        -(this->stl.stats.min.x),
        -(this->stl.stats.min.y),
        -(this->stl.stats.min.z)
    );
}

void TriangleMesh::rotate(double angle, Point* center)
{
    this->translate(-center->x, -center->y, 0);
    stl_rotate_z(&(this->stl), (float)angle);
    this->translate(+center->x, +center->y, 0);
}

TriangleMeshPtrs
TriangleMesh::split() const
{
    TriangleMeshPtrs meshes;
    std::set<int> seen_facets;

    // we need neighbors
    if (!this->repaired) qDebug("split() requires repair()");

    // loop while we have remaining facets
    while (1) {//这里一直循环到没有一个三角面片为止
        // get the first facet
        std::queue<int> facet_queue;
        std::deque<int> facets;
        for (int facet_idx = 0; facet_idx < this->stl.stats.number_of_facets; facet_idx++) {
            if (seen_facets.find(facet_idx) == seen_facets.end()) {//注意find()方法对集合进行检索，如果找到查找的的键值，则返回该键值的迭代器位置；否则，返回集合最后一个元素后面的一个位置，即end()
                // if facet was not seen put it into queue and start searching
                facet_queue.push(facet_idx);
                break;
            }
        }
        if (facet_queue.empty()) break;   //这里说明seen_facets装满了，即所有面片都遍历过了，所以跳出while循环。

        while (!facet_queue.empty()) {
            int facet_idx = facet_queue.front();
            facet_queue.pop();
            if (seen_facets.find(facet_idx) != seen_facets.end()) continue;
            facets.push_back(facet_idx);
            for (int j = 0; j <= 2; j++) {
                facet_queue.push(this->stl.neighbors_start[facet_idx].neighbor[j]);  //每次加入这次选择边的相邻三条边，必然能遍历完次模型
            }
            seen_facets.insert(facet_idx);  //将选择加入的边加入集合，下次循环不会再使用这个边了
        }//最终跳出循环时facets里面存着挑出来的一个模型的所有序列号，当然洞挑不出来

        TriangleMesh* mesh = new TriangleMesh;
        meshes.push_back(mesh);
        mesh->stl.stats.type = inmemory;
        mesh->stl.stats.number_of_facets = facets.size();
        mesh->stl.stats.original_num_facets = mesh->stl.stats.number_of_facets;
        stl_clear_error(&mesh->stl);
        stl_allocate(&mesh->stl);   //分配存储空间（根据number_of_facets和original_num_facets），第一次分配，初始化这部分内存
        //下面讲的是如何往stl库里面加入一个stl文件的方法！！值得学习！！加入时既要加入，还要更新状态！
        int first = 1;
        for (std::deque<int>::const_iterator facet = facets.begin(); facet != facets.end(); facet++) {
            mesh->stl.facet_start[facet - facets.begin()] = this->stl.facet_start[*facet];
            stl_facet_stats(&mesh->stl, this->stl.facet_start[*facet], first);  //first代表是否是插入的第一条边
            first = 0;
        }
    }

    return meshes;
}

void
TriangleMesh::merge(const TriangleMesh &mesh)
{
    // reset stats and metadata
    int number_of_facets = this->stl.stats.number_of_facets;
    stl_invalidate_shared_vertices(&this->stl);
    this->repaired = false;

    // update facet count and allocate more memory
    this->stl.stats.number_of_facets = number_of_facets + mesh.stl.stats.number_of_facets;
    this->stl.stats.original_num_facets = this->stl.stats.number_of_facets;
    stl_reallocate(&this->stl);  //从新分配更多存储空间，这种分配的内存原来有的部分还在

    // copy facets   将mesh的三角面片加入本身
    for (int i = 0; i < mesh.stl.stats.number_of_facets; i++) {
        this->stl.facet_start[number_of_facets + i] = mesh.stl.facet_start[i];
    }

    // update size
    stl_get_size(&this->stl);  //更新stl里面的size这个结构体，实际上就是一个（x,y,z）
}

/* this will return scaled ExPolygons */
ExPolygons
TriangleMesh::horizontal_projection() const
{
    Polygons pp;
    pp.reserve(this->stl.stats.number_of_facets);
    for (int i = 0; i < this->stl.stats.number_of_facets; i++) {
        stl_facet* facet = &this->stl.facet_start[i];
        Polygon p;
        p.points.resize(3);
        p.points[0] = Point(facet->vertex[0].x / SCALING_FACTOR, facet->vertex[0].y / SCALING_FACTOR);
        p.points[1] = Point(facet->vertex[1].x / SCALING_FACTOR, facet->vertex[1].y / SCALING_FACTOR);
        p.points[2] = Point(facet->vertex[2].x / SCALING_FACTOR, facet->vertex[2].y / SCALING_FACTOR);
        p.make_counter_clockwise();  // do this after scaling, as winding order might change while doing that
        pp.push_back(p);
    }

    // the offset factor was tuned using groovemount.stl
    offset(pp, &pp, 0.01 / SCALING_FACTOR);
    ExPolygons retval;
    union_(pp, &retval, true);
    return retval;
}

Polygon
TriangleMesh::convex_hull()
{
    this->require_shared_vertices();
    Points pp;
    pp.reserve(this->stl.stats.shared_vertices);
    for (int i = 0; i < this->stl.stats.shared_vertices; i++) {
        stl_vertex* v = &this->stl.v_shared[i];
        pp.push_back(Point(v->x / SCALING_FACTOR, v->y / SCALING_FACTOR));
    }
    return xd::Geometry::convex_hull(pp);
}

BoundingBoxf3
TriangleMesh::bounding_box() const
{
    BoundingBoxf3 bb;
    bb.min.x = this->stl.stats.min.x;
    bb.min.y = this->stl.stats.min.y;
    bb.min.z = this->stl.stats.min.z;
    bb.max.x = this->stl.stats.max.x;
    bb.max.y = this->stl.stats.max.y;
    bb.max.z = this->stl.stats.max.z;
    return bb;
}

void
TriangleMesh::require_shared_vertices()
{
    if (!this->repaired) this->repair();   //执行下面函数前必须的！！！
    if (this->stl.v_shared == NULL) stl_generate_shared_vertices(&(this->stl));   //生成共享点，难以理解！
}

void
TriangleMeshSlicer::slice(const std::vector<float> &z, std::vector<Polygons>* layers)
{
    /*
       This method gets called with a list of unscaled Z coordinates and outputs
       a vector pointer having the same number of items as the original list.
       Each item is a vector of polygons created by slicing our mesh at the
       given heights.

       This method should basically combine the behavior of the existing
       Perl methods defined in lib/Slic3r/TriangleMesh.pm:

       - analyze(): this creates the 'facets_edges' and the 'edges_facets'
            tables (we don't need the 'edges' table)

       - slice_facet(): this has to be done for each facet. It generates
            intersection lines with each plane identified by the Z list.
            The get_layer_range() binary search used to identify the Z range
            of the facet is already ported to C++ (see Object.xsp)

       - make_loops(): this has to be done for each layer. It creates polygons
            from the lines generated by the previous step.

        At the end, we free the tables generated by analyze() as we don't
        need them anymore.
        FUTURE: parallelize slice_facet() and make_loops()

        NOTE: this method accepts a vector of floats because the mesh coordinate
        type is float.
    */

    std::vector<IntersectionLines> lines(z.size());

    for (int facet_idx = 0; facet_idx < this->mesh->stl.stats.number_of_facets; facet_idx++) {
        stl_facet* facet = &this->mesh->stl.facet_start[facet_idx];

        // find facet extents   找到面片z方向的最大值和最小值
        float min_z = fminf(facet->vertex[0].z, fminf(facet->vertex[1].z, facet->vertex[2].z));
        float max_z = fmaxf(facet->vertex[0].z, fmaxf(facet->vertex[1].z, facet->vertex[2].z));
//      if(min_z==max_z)
//          int tem=1;
        #ifdef SLIC3R_DEBUG
        printf("\n==> FACET %d (%f,%f,%f - %f,%f,%f - %f,%f,%f):\n", facet_idx,
            facet->vertex[0].x, facet->vertex[0].y, facet->vertex[0].z,
            facet->vertex[1].x, facet->vertex[1].y, facet->vertex[1].z,
            facet->vertex[2].x, facet->vertex[2].y, facet->vertex[2].z);
        printf("z: min = %.2f, max = %.2f\n", min_z, max_z);
        #endif

        // find layer extents   找到z方向切片的面在该三角面片上的范围指针
        std::vector<float>::const_iterator min_layer, max_layer;
        min_layer = std::lower_bound(z.begin(), z.end(), min_z); // first layer whose slice_z is >= min_z   lower_bound超级好的函数
        //注意：下面这个函数如果使用C++调试器会出现错误，g++没事，很奇怪！！
        max_layer = std::upper_bound(z.begin() + (min_layer - z.begin()), z.end(), max_z) - 1; // last layer whose slice_z is <= max_z
        #ifdef SLIC3R_DEBUG
        printf("layers: min = %d, max = %d\n", (int)(min_layer - z.begin()), (int)(max_layer - z.begin()));
        #endif

        //printf("layers: min = %d, max = %d\n", (int)(min_layer - z.begin()), (int)(max_layer - z.begin()));
        for (std::vector<float>::const_iterator it = min_layer; it != max_layer + 1; ++it) {
            std::vector<float>::size_type layer_idx = it - z.begin();  //size_type 相当于 unsigned int类型使用size_type 主要是为了适应不同的平台int类型大小会根据不同平台而不同,所以应该是size_type比int好！
            this->slice_facet(*it / SCALING_FACTOR, *facet, facet_idx, min_z, max_z, &lines[layer_idx]);  //裁剪一个面的函数，结果放在对应z角标编号的lines里
        }
    }

    // v_scaled_shared could be freed here

    // build loops
    layers->resize(z.size());
    for (std::vector<IntersectionLines>::iterator it = lines.begin(); it != lines.end(); ++it) {
        int layer_idx = it - lines.begin();
        #ifdef SLIC3R_DEBUG
        printf("Layer %d:\n", layer_idx);
        #endif

        this->make_loops(*it, &(*layers)[layer_idx]);
    }
}

void
TriangleMeshSlicer::slice(const std::vector<float> &z, std::vector<ExPolygons>* layers)
{
    std::vector<Polygons> layers_p;
    this->slice(z, &layers_p);   //还是调用上一个函数生成Polygons

    layers->resize(z.size());
    for (std::vector<Polygons>::const_iterator loops = layers_p.begin(); loops != layers_p.end(); ++loops) {
        #ifdef SLIC3R_DEBUG
        size_t layer_id = loops - layers_p.begin();
        printf("Layer %zu (slice_z = %.2f):\n", layer_id, z[layer_id]);
        #endif

        this->make_expolygons(*loops, &(*layers)[ loops - layers_p.begin() ]);
    }
}

void
TriangleMeshSlicer::slice_facet(float slice_z, const stl_facet &facet, const int &facet_idx, const float &min_z, const float &max_z, std::vector<IntersectionLine>* lines) const
{
    std::vector<IntersectionPoint> points;
    std::vector< std::vector<IntersectionPoint>::size_type > points_on_layer;
    bool found_horizontal_edge = false;

    /* reorder vertices so that the first one is the one with lowest Z
       this is needed to get all intersection lines in a consistent order
       (external on the right of the line) */
    int i = 0;
    if (facet.vertex[1].z == min_z) {
        // vertex 1 has lowest Z
        i = 1;
    } else if (facet.vertex[2].z == min_z) {
        // vertex 2 has lowest Z
        i = 2;
    }
    for (int j = i; (j-i) < 3; j++) {  // loop through facet edges
        int edge_id = this->facets_edges[facet_idx][j % 3];
        int a_id = this->mesh->stl.v_indices[facet_idx].vertex[j % 3];
        int b_id = this->mesh->stl.v_indices[facet_idx].vertex[(j+1) % 3];
        stl_vertex* a = &this->v_scaled_shared[a_id];  //这句话说明了v_scaled_shared存放着所有点，而且地址放在v_indices里面！
        stl_vertex* b = &this->v_scaled_shared[b_id];  //

        if (a->z == b->z && a->z == slice_z) {
            // edge is horizontal and belongs to the current layer

            stl_vertex &v0 = this->v_scaled_shared[ this->mesh->stl.v_indices[facet_idx].vertex[0] ];
            stl_vertex &v1 = this->v_scaled_shared[ this->mesh->stl.v_indices[facet_idx].vertex[1] ];
            stl_vertex &v2 = this->v_scaled_shared[ this->mesh->stl.v_indices[facet_idx].vertex[2] ];
            IntersectionLine line;
            if (min_z == max_z) {
                line.edge_type = feHorizontal;
                if (this->mesh->stl.facet_start[facet_idx].normal.z < 0) {
                    /*  if normal points downwards this is a bottom horizontal facet so we reverse
                        its point order */
                    std::swap(a, b);
                    std::swap(a_id, b_id);
                }
            } else if (v0.z < slice_z || v1.z < slice_z || v2.z < slice_z) {
                line.edge_type = feTop;
                std::swap(a, b);
                std::swap(a_id, b_id);
            } else {
                line.edge_type = feBottom;
            }
            line.a.x    = a->x;
            line.a.y    = a->y;
            line.b.x    = b->x;
            line.b.y    = b->y;
            line.a_id   = a_id;
            line.b_id   = b_id;
            lines->push_back(line);

            found_horizontal_edge = true;

            // if this is a top or bottom edge, we can stop looping through edges
            // because we won't find anything interesting

            if (line.edge_type != feHorizontal) return;
        } else if (a->z == slice_z) {
            IntersectionPoint point;
            point.x         = a->x;
            point.y         = a->y;
            point.point_id  = a_id;
            points.push_back(point);
            points_on_layer.push_back(points.size()-1);
        } else if (b->z == slice_z) {
            IntersectionPoint point;
            point.x         = b->x;
            point.y         = b->y;
            point.point_id  = b_id;
            points.push_back(point);
            points_on_layer.push_back(points.size()-1);
        } else if ((a->z < slice_z && b->z > slice_z) || (b->z < slice_z && a->z > slice_z)) {
            // edge intersects the current layer; calculate intersection

            IntersectionPoint point;
            point.x         = b->x + (a->x - b->x) * (slice_z - b->z) / (a->z - b->z);
            point.y         = b->y + (a->y - b->y) * (slice_z - b->z) / (a->z - b->z);
            point.edge_id   = edge_id;
            points.push_back(point);
        }
    }
    if (found_horizontal_edge) return;

    if (!points_on_layer.empty()) {
        // we can't have only one point on layer because each vertex gets detected
        // twice (once for each edge), and we can't have three points on layer because
        // we assume this code is not getting called for horizontal facets
        assert(points_on_layer.size() == 2);
        assert( points[ points_on_layer[0] ].point_id == points[ points_on_layer[1] ].point_id );
        if (points.size() < 3) return;  // no intersection point, this is a V-shaped facet tangent to plane
        points.erase( points.begin() + points_on_layer[1] );
    }

    if (!points.empty()) {
        assert(points.size() == 2); // facets must intersect each plane 0 or 2 times
        IntersectionLine line;
        line.a          = (Point)points[1];
        line.b          = (Point)points[0];
        line.a_id       = points[1].point_id;
        line.b_id       = points[0].point_id;
        line.edge_a_id  = points[1].edge_id;
        line.edge_b_id  = points[0].edge_id;
        lines->push_back(line);
        return;
    }
}

void
TriangleMeshSlicer::make_loops(std::vector<IntersectionLine> &lines, Polygons* loops)
{
    /*
    SVG svg("lines.svg");
    svg.draw(lines);
    svg.Close();
    */

    // remove tangent edges
    for (IntersectionLines::iterator line = lines.begin(); line != lines.end(); ++line) {
        if (line->skip || line->edge_type == feNone) continue;

        /* if the line is a facet edge, find another facet edge
           having the same endpoints but in reverse order */
        for (IntersectionLines::iterator line2 = line + 1; line2 != lines.end(); ++line2) {
            if (line2->skip || line2->edge_type == feNone) continue;

            // are these facets adjacent? (sharing a common edge on this layer)
            if (line->a_id == line2->a_id && line->b_id == line2->b_id) {
                line2->skip = true;

                /* if they are both oriented upwards or downwards (like a 'V')
                   then we can remove both edges from this layer since it won't
                   affect the sliced shape */
                /* if one of them is oriented upwards and the other is oriented
                   downwards, let's only keep one of them (it doesn't matter which
                   one since all 'top' lines were reversed at slicing) */
                if (line->edge_type == line2->edge_type) {
                    line->skip = true;
                    break;
                }
            } else if (line->a_id == line2->b_id && line->b_id == line2->a_id) {
                /* if this edge joins two horizontal facets, remove both of them */
                if (line->edge_type == feHorizontal && line2->edge_type == feHorizontal) {
                    line->skip = true;
                    line2->skip = true;
                    break;
                }
            }
        }
    }

    // build a map of lines by edge_a_id and a_id
    std::vector<IntersectionLinePtrs> by_edge_a_id, by_a_id;
    by_edge_a_id.resize(this->mesh->stl.stats.number_of_facets * 3);
    by_a_id.resize(this->mesh->stl.stats.shared_vertices);
    for (IntersectionLines::iterator line = lines.begin(); line != lines.end(); ++line) {
        if (line->skip) continue;
        if (line->edge_a_id != -1) by_edge_a_id[line->edge_a_id].push_back(&(*line));
        if (line->a_id != -1) by_a_id[line->a_id].push_back(&(*line));
    }

    CYCLE: while (1) {   //这里是一个goto语句的开始部分
        // take first spare line and start a new loop
        IntersectionLine* first_line = NULL;
        for (IntersectionLines::iterator line = lines.begin(); line != lines.end(); ++line) {
            if (line->skip) continue;
            first_line = &(*line);
            break;
        }
        if (first_line == NULL) break;
        first_line->skip = true;
        IntersectionLinePtrs loop;
        loop.push_back(first_line);

        /*
        printf("first_line edge_a_id = %d, edge_b_id = %d, a_id = %d, b_id = %d, a = %d,%d, b = %d,%d\n",
            first_line->edge_a_id, first_line->edge_b_id, first_line->a_id, first_line->b_id,
            first_line->a.x, first_line->a.y, first_line->b.x, first_line->b.y);
        */

        while (1) {
            // find a line starting where last one finishes
            IntersectionLine* next_line = NULL;
            if (loop.back()->edge_b_id != -1) {
                IntersectionLinePtrs &candidates = by_edge_a_id[loop.back()->edge_b_id];
                for (IntersectionLinePtrs::iterator lineptr = candidates.begin(); lineptr != candidates.end(); ++lineptr) {
                    if ((*lineptr)->skip) continue;
                    next_line = *lineptr;
                    break;
                }
            }
            if (next_line == NULL && loop.back()->b_id != -1) {
                IntersectionLinePtrs &candidates = by_a_id[loop.back()->b_id];
                for (IntersectionLinePtrs::iterator lineptr = candidates.begin(); lineptr != candidates.end(); ++lineptr) {
                    if ((*lineptr)->skip) continue;
                    next_line = *lineptr;
                    break;
                }
            }

            if (next_line == NULL) {
                // check whether we closed this loop
                if ((loop.front()->edge_a_id != -1 && loop.front()->edge_a_id == loop.back()->edge_b_id)
                    || (loop.front()->a_id != -1 && loop.front()->a_id == loop.back()->b_id)) {
                    // loop is complete
                    Polygon p;
                    p.points.reserve(loop.size());
                    for (IntersectionLinePtrs::iterator lineptr = loop.begin(); lineptr != loop.end(); ++lineptr) {
                        p.points.push_back((*lineptr)->a);
                    }
                    loops->push_back(p);

                    #ifdef SLIC3R_DEBUG
                    printf("  Discovered %s polygon of %d points\n", (p.is_counter_clockwise() ? "ccw" : "cw"), (int)p.points.size());
                    #endif

                    goto CYCLE;
                }

                // we can't close this loop!
                //// push @failed_loops, [@loop];
                //#ifdef SLIC3R_DEBUG
                printf("  Unable to close this loop having %d points\n", (int)loop.size());
                //#endif
                goto CYCLE;
            }
            /*
            printf("next_line edge_a_id = %d, edge_b_id = %d, a_id = %d, b_id = %d, a = %d,%d, b = %d,%d\n",
                next_line->edge_a_id, next_line->edge_b_id, next_line->a_id, next_line->b_id,
                next_line->a.x, next_line->a.y, next_line->b.x, next_line->b.y);
            */
            loop.push_back(next_line);
            next_line->skip = true;
        }
    }
}

class _area_comp {
    public:
    _area_comp(std::vector<double>* _aa) : abs_area(_aa) {};
    bool operator() (const size_t &a, const size_t &b) {
        return (*this->abs_area)[a] > (*this->abs_area)[b];
    }

    private:
    std::vector<double>* abs_area;
};

void
TriangleMeshSlicer::make_expolygons_simple(std::vector<IntersectionLine> &lines, ExPolygons* slices)
{
    Polygons loops;
    this->make_loops(lines, &loops);

    Polygons cw;
    for (Polygons::const_iterator loop = loops.begin(); loop != loops.end(); ++loop) {
        if (loop->area() >= 0) {
            ExPolygon ex;
            ex.contour = *loop;
            slices->push_back(ex);
        } else {
            cw.push_back(*loop);
        }
    }

    // assign holes to contours
    for (Polygons::const_iterator loop = cw.begin(); loop != cw.end(); ++loop) {
        int slice_idx = -1;
        double current_contour_area = -1;
        for (ExPolygons::iterator slice = slices->begin(); slice != slices->end(); ++slice) {
            if (slice->contour.contains(loop->points.front())) {
                double area = slice->contour.area();
                if (area < current_contour_area || current_contour_area == -1) {
                    slice_idx = slice - slices->begin();
                    current_contour_area = area;
                }
            }
        }
        (*slices)[slice_idx].holes.push_back(*loop);
    }
}

void
TriangleMeshSlicer::make_expolygons(const Polygons &loops, ExPolygons* slices)
{
    /*
        Input loops are not suitable for evenodd nor nonzero fill types, as we might get
        two consecutive concentric loops having the same winding order - and we have to
        respect such order. In that case, evenodd would create wrong inversions, and nonzero
        would ignore holes inside two concentric contours.
        So we're ordering loops and collapse consecutive concentric loops having the same
        winding order.
        TODO: find a faster algorithm for this, maybe with some sort of binary search.
        If we computed a "nesting tree" we could also just remove the consecutive loops
        having the same winding order, and remove the extra one(s) so that we could just
        supply everything to offset() instead of performing several union/diff calls.

        we sort by area assuming that the outermost loops have larger area;
        the previous sorting method, based on $b->contains($a->[0]), failed to nest
        loops correctly in some edge cases when original model had overlapping facets
    */

    std::vector<double> area;
    std::vector<double> abs_area;
    std::vector<size_t> sorted_area;  // vector of indices
    for (Polygons::const_iterator loop = loops.begin(); loop != loops.end(); ++loop) {
        double a = loop->area();
        area.push_back(a);
        abs_area.push_back(std::fabs(a));
        sorted_area.push_back(loop - loops.begin());
    }

    std::sort(sorted_area.begin(), sorted_area.end(), _area_comp(&abs_area));  // outer first

    // we don't perform a safety offset now because it might reverse cw loops
    Polygons p_slices;
    for (std::vector<size_t>::const_iterator loop_idx = sorted_area.begin(); loop_idx != sorted_area.end(); ++loop_idx) {
        /* we rely on the already computed area to determine the winding order
           of the loops, since the Orientation() function provided by Clipper
           would do the same, thus repeating the calculation */
        Polygons::const_iterator loop = loops.begin() + *loop_idx;
        if (area[*loop_idx] > +EPSILON) {
            p_slices.push_back(*loop);
        } else if (area[*loop_idx] < -EPSILON) {
            diff(p_slices, *loop, &p_slices);
        }
    }

    // perform a safety offset to merge very close facets (TODO: find test case for this)
    double safety_offset = scale_(0.0499);
    ExPolygons ex_slices;
    offset2(p_slices, &ex_slices, +safety_offset, -safety_offset);

    #ifdef SLIC3R_DEBUG
    size_t holes_count = 0;
    for (ExPolygons::const_iterator e = ex_slices.begin(); e != ex_slices.end(); ++e) {
        holes_count += e->holes.size();
    }
    printf("%zu surface(s) having %zu holes detected from %zu polylines\n",
        ex_slices.size(), holes_count, loops.size());
    #endif

    // append to the supplied collection
    slices->insert(slices->end(), ex_slices.begin(), ex_slices.end());
}

void
TriangleMeshSlicer::make_expolygons(std::vector<IntersectionLine> &lines, ExPolygons* slices)
{
    Polygons pp;
    this->make_loops(lines, &pp);
    this->make_expolygons(pp, slices);
}

void
TriangleMeshSlicer::cut(float z, TriangleMesh* upper, TriangleMesh* lower)
{
    std::vector<IntersectionLine> upper_lines, lower_lines;

    float scaled_z = scale_(z);
    for (int facet_idx = 0; facet_idx < this->mesh->stl.stats.number_of_facets; facet_idx++) {
        stl_facet* facet = &this->mesh->stl.facet_start[facet_idx];

        // find facet extents
        float min_z = fminf(facet->vertex[0].z, fminf(facet->vertex[1].z, facet->vertex[2].z));
        float max_z = fmaxf(facet->vertex[0].z, fmaxf(facet->vertex[1].z, facet->vertex[2].z));

        // intersect facet with cutting plane
        std::vector<IntersectionLine> lines;
        this->slice_facet(scaled_z, *facet, facet_idx, min_z, max_z, &lines);

        // save intersection lines for generating correct triangulations
        for (std::vector<IntersectionLine>::iterator it = lines.begin(); it != lines.end(); ++it) {
            if (it->edge_type == feTop) {
                lower_lines.push_back(*it);
            } else if (it->edge_type == feBottom) {
                upper_lines.push_back(*it);
            } else if (it->edge_type != feHorizontal) {
                lower_lines.push_back(*it);
                upper_lines.push_back(*it);
            }
        }

        if (min_z > z || (min_z == z && max_z > min_z)) {
            // facet is above the cut plane and does not belong to it
            if (upper != NULL) stl_add_facet(&upper->stl, facet);
        } else if (max_z < z || (max_z == z && max_z > min_z)) {
            // facet is below the cut plane and does not belong to it
            if (lower != NULL) stl_add_facet(&lower->stl, facet);
        } else if (min_z < z && max_z > z) {
            // facet is cut by the slicing plane

            // look for the vertex on whose side of the slicing plane there are no other vertices
            int isolated_vertex;
            if ( (facet->vertex[0].z > z) == (facet->vertex[1].z > z) ) {
                isolated_vertex = 2;
            } else if ( (facet->vertex[1].z > z) == (facet->vertex[2].z > z) ) {
                isolated_vertex = 0;
            } else {
                isolated_vertex = 1;
            }

            // get vertices starting from the isolated one
            stl_vertex* v0 = &facet->vertex[isolated_vertex];
            stl_vertex* v1 = &facet->vertex[(isolated_vertex+1) % 3];
            stl_vertex* v2 = &facet->vertex[(isolated_vertex+2) % 3];

            // intersect v0-v1 and v2-v0 with cutting plane and make new vertices
            stl_vertex v0v1, v2v0;
            v0v1.x = v1->x + (v0->x - v1->x) * (z - v1->z) / (v0->z - v1->z);
            v0v1.y = v1->y + (v0->y - v1->y) * (z - v1->z) / (v0->z - v1->z);
            v0v1.z = z;
            v2v0.x = v2->x + (v0->x - v2->x) * (z - v2->z) / (v0->z - v2->z);
            v2v0.y = v2->y + (v0->y - v2->y) * (z - v2->z) / (v0->z - v2->z);
            v2v0.z = z;

            // build the triangular facet
            stl_facet triangle;
            triangle.normal = facet->normal;
            triangle.vertex[0] = *v0;
            triangle.vertex[1] = v0v1;
            triangle.vertex[2] = v2v0;

            // build the facets forming a quadrilateral on the other side
            stl_facet quadrilateral[2];
            quadrilateral[0].normal = facet->normal;
            quadrilateral[0].vertex[0] = *v1;
            quadrilateral[0].vertex[1] = *v2;
            quadrilateral[0].vertex[2] = v0v1;
            quadrilateral[1].normal = facet->normal;
            quadrilateral[1].vertex[0] = *v2;
            quadrilateral[1].vertex[1] = v2v0;
            quadrilateral[1].vertex[2] = v0v1;

            if (v0->z > z) {
                if (upper != NULL) stl_add_facet(&upper->stl, &triangle);
                if (lower != NULL) {
                    stl_add_facet(&lower->stl, &quadrilateral[0]);
                    stl_add_facet(&lower->stl, &quadrilateral[1]);
                }
            } else {
                if (upper != NULL) {
                    stl_add_facet(&upper->stl, &quadrilateral[0]);
                    stl_add_facet(&upper->stl, &quadrilateral[1]);
                }
                if (lower != NULL) stl_add_facet(&lower->stl, &triangle);
            }
        }
    }

    // triangulate holes of upper mesh
    if (upper != NULL) {
        // compute shape of section
        ExPolygons section;
        this->make_expolygons_simple(upper_lines, &section);

        // triangulate section
        Polygons triangles;
        for (ExPolygons::const_iterator expolygon = section.begin(); expolygon != section.end(); ++expolygon)
            expolygon->triangulate_p2t(&triangles);

        // convert triangles to facets and append them to mesh
        for (Polygons::const_iterator polygon = triangles.begin(); polygon != triangles.end(); ++polygon) {
            Polygon p = *polygon;
            p.reverse();
            stl_facet facet;
            facet.normal.x = 0;
            facet.normal.y = 0;
            facet.normal.z = -1;
            for (size_t i = 0; i <= 2; ++i) {
                facet.vertex[i].x = unscale(p.points[i].x);
                facet.vertex[i].y = unscale(p.points[i].y);
                facet.vertex[i].z = z;
            }
            stl_add_facet(&upper->stl, &facet);
        }
    }

    // triangulate holes of lower mesh
    if (lower != NULL) {
        // compute shape of section
        ExPolygons section;
        this->make_expolygons_simple(lower_lines, &section);

        // triangulate section
        Polygons triangles;
        for (ExPolygons::const_iterator expolygon = section.begin(); expolygon != section.end(); ++expolygon)
            expolygon->triangulate_p2t(&triangles);

        // convert triangles to facets and append them to mesh
        for (Polygons::const_iterator polygon = triangles.begin(); polygon != triangles.end(); ++polygon) {
            stl_facet facet;
            facet.normal.x = 0;
            facet.normal.y = 0;
            facet.normal.z = 1;
            for (size_t i = 0; i <= 2; ++i) {
                facet.vertex[i].x = unscale(polygon->points[i].x);
                facet.vertex[i].y = unscale(polygon->points[i].y);
                facet.vertex[i].z = z;
            }
            stl_add_facet(&lower->stl, &facet);
        }
    }


    stl_get_size(&(upper->stl));
    stl_get_size(&(lower->stl));

}

TriangleMeshSlicer::TriangleMeshSlicer(TriangleMesh* _mesh) : mesh(_mesh), v_scaled_shared(NULL)
{
    // build a table to map a facet_idx to its three edge indices
    this->mesh->require_shared_vertices();  //调用此函数就代表修复stl文件了
    typedef std::pair<int,int>              t_edge;
    typedef std::vector<t_edge>             t_edges;  // edge_idx => a_id,b_id       边id到边
    typedef std::map<t_edge,int>            t_edges_map;  // a_id,b_id => edge_idx   边到边id

    this->facets_edges.resize(this->mesh->stl.stats.number_of_facets);

    {
        t_edges edges;   //边id到边
        // reserve() instad of resize() because otherwise we couldn't read .size() below to assign edge_idx
        //容器调用resize()函数后，所有的空间都已经初始化了，所以可以直接访问。
        //而reserve()函数预分配出的空间没有被初始化，所以不可访问。
        edges.reserve(this->mesh->stl.stats.number_of_facets * 3);  // number of edges = number of facets * 3
        t_edges_map edges_map;  //边到边id
        for (int facet_idx = 0; facet_idx < this->mesh->stl.stats.number_of_facets; facet_idx++) {
            this->facets_edges[facet_idx].resize(3);
            for (int i = 0; i <= 2; i++) {
                int a_id = this->mesh->stl.v_indices[facet_idx].vertex[i];
                int b_id = this->mesh->stl.v_indices[facet_idx].vertex[(i+1) % 3];

                int edge_idx;
                t_edges_map::const_iterator my_edge = edges_map.find(std::make_pair(b_id,a_id));
                if (my_edge != edges_map.end()) {
                    edge_idx = my_edge->second;
                } else {
                    /* admesh can assign the same edge ID to more than two facets (which is
                       still topologically correct), so we have to search for a duplicate of
                       this edge too in case it was already seen in this orientation */
                    my_edge = edges_map.find(std::make_pair(a_id,b_id));

                    if (my_edge != edges_map.end()) {
                        edge_idx = my_edge->second;
                    } else {
                        // edge isn't listed in table, so we insert it
                        edge_idx = edges.size();   //因为edges存放不重复的边，因此最后大小即为stl文件的边的数目
                        edges.push_back(std::make_pair(a_id,b_id));
                        edges_map[ edges[edge_idx] ] = edge_idx;
                    }
                }
                this->facets_edges[facet_idx][i] = edge_idx;   //最终组成面（对应stl数据中的面id）边对应的id值（某些id值重复，应该都重复2次）
                                                               //但是最终facets_edges对应查询到相同的边都是一个确定的，相等的值
                #ifdef SLIC3R_DEBUG
                printf("  [facet %d, edge %d] a_id = %d, b_id = %d   --> edge %d\n", facet_idx, i, a_id, b_id, edge_idx);
                #endif
            }
        }
    }

    // clone shared vertices coordinates and scale them
    this->v_scaled_shared = (stl_vertex*)calloc(this->mesh->stl.stats.shared_vertices, sizeof(stl_vertex));
    std::copy(this->mesh->stl.v_shared, this->mesh->stl.v_shared + this->mesh->stl.stats.shared_vertices, this->v_scaled_shared);
    for (int i = 0; i < this->mesh->stl.stats.shared_vertices; i++) {
        this->v_scaled_shared[i].x /= SCALING_FACTOR;
        this->v_scaled_shared[i].y /= SCALING_FACTOR;
        this->v_scaled_shared[i].z /= SCALING_FACTOR;
    }
}

TriangleMeshSlicer::~TriangleMeshSlicer()
{
    if (this->v_scaled_shared != NULL) free(this->v_scaled_shared);
}

}
