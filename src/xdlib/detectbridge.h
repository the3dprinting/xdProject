#ifndef DETECTBRIDGE_H
#define DETECTBRIDGE_H

#include <string>
#include "constdefine.h"
#include "exPolygon.h"
#include "expolygonGather.h"

namespace xd {

class BridgeDetector {
    public:
    ExPolygon expolygon;
    ExPolygonCollection lower_slices;
    double extrusion_width;  // scaled
    double resolution;
    double angle;

    BridgeDetector(const ExPolygon &_expolygon, const ExPolygonCollection &_lower_slices, coord_t _extrusion_width);//��_expolygon��_lower_slices�󲼶������㣬�洢��_edges(��)��_anchors(��)��
    bool detect_angle();
    void coverage(Polygons* coverage) const;
    void coverage(double angle, Polygons* coverage) const;
    void unsupported_edges(Polylines* unsupported) const;
    void unsupported_edges(double angle, Polylines* unsupported) const;

    private:
    Polylines _edges;   // representing the supporting edges
    ExPolygons _anchors;
};

}


#endif // DETECTBRIDGE_H

