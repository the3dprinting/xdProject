#include "geometry.h"
//#include "ClipperUtils.hpp"
//#include "ExPolygon.hpp"
#include "line.h"
//#include "PolylineCollection.hpp"
#include "clipper.hpp"
#include <algorithm>
#include <cmath>
#include <list>
#include <map>
#include <set>
#include <vector>

namespace Slic3r { namespace Geometry {

bool
directions_parallel(double angle1, double angle2, double max_diff)
{
    double diff = fabs(angle1 - angle2);
    max_diff += EPSILON;
    return diff < max_diff || fabs(diff - PI) < max_diff;
}

}}
