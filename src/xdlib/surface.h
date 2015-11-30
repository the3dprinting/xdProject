#ifndef SURFACE_H
#define SURFACE_H

#include "exPolygon.h"

namespace xd {

enum SurfaceType { stTop, stBottom, stBottomBridge, stInternal, stInternalSolid, stInternalBridge, stInternalVoid };

class Surface
{
    public:
    SurfaceType     surface_type;
    ExPolygon       expolygon;
    double          thickness;          // in mm
    unsigned short  thickness_layers;   // in layers
    double          bridge_angle;       // in radians, ccw, 0 = East, only 0+ (negative means undefined)
    unsigned short  extra_perimeters;

    Surface(SurfaceType _surface_type, const ExPolygon &_expolygon)
        : surface_type(_surface_type), expolygon(_expolygon),
            thickness(-1), thickness_layers(1), bridge_angle(-1), extra_perimeters(0)
        {};
    double area() const;
    bool is_solid() const;   //���ر���������Ƿ���solid����
    bool is_external() const;  //���ر��������Ƿ���external����
    bool is_internal() const;  //���ر��������Ƿ���internal����
    bool is_bottom() const;    //���ر��������Ƿ���bottom����
    bool is_bridge() const;    //���ر��������Ƿ���bridge����

};

typedef std::vector<Surface> Surfaces;
typedef std::vector<Surface*> SurfacesPtr;

}  //����xd�����ռ�


#endif // SURFACE_H

