#ifndef SURFACEGATHER_H
#define SURFACEGATHER_H

#include "surface.h"
#include <vector>

namespace xd {

class SurfaceCollection
{
    public:
    Surfaces surfaces;

    operator Polygons() const;  //!< 类型转换函数，将本身的一堆surface转换为Polygons类
    operator ExPolygons() const;   //!< 类型转换函数，将本身的一堆surface转换为ExPolygons类
    void simplify(double tolerance);  //!< 将本身一堆surface里面的expolygon按照弦高误差为tolerance简化
    void group(std::vector<SurfacesPtr> *retval);  //!< 通过共有的属性将surfaces分组
    template <class T> bool any_internal_contains(const T &item) const;  //!< surfaces里面是否有internal类型的surface包含item
    template <class T> bool any_bottom_contains(const T &item) const;   //!< surfaces里面是否有bottom类型的surface包含item
    SurfacesPtr filter_by_type(SurfaceType type);   //!< 将surfaces里面是type类型的挑选出来返回
    void filter_by_type(SurfaceType type, Polygons* polygons);  //!< 将surfaces里面是type类型的挑选出来放到polygonsL里面
};

}

#endif //  SURFACEGATHER_H

