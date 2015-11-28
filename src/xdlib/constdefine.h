#ifndef CONSTDEFINE
#define CONSTDEFINE

#define EPSILON 1e-4
#define SCALING_FACTOR 0.000001
#define RESOLUTION 0.0125
#define SCALED_RESOLUTION (RESOLUTION / SCALING_FACTOR)
#define PI 3.141592653589793238
#define LOOP_CLIPPING_LENGTH_OVER_NOZZLE_DIAMETER 0.15
#define SMALL_PERIMETER_LENGTH (6.5 / SCALING_FACTOR) * 2 * PI
#define scale_(val) (val / SCALING_FACTOR)
#define unscale(val) (val * SCALING_FACTOR)
#define SCALED_EPSILON scale_(EPSILON)
typedef long coord_t;
typedef double coordf_t;

namespace xd {

// TODO: make sure X = 0
enum Axis { X, Y, Z };

}

#endif // CONSTDEFINE

