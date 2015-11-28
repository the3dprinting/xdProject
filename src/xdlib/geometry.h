#ifndef GEOMETRY
#define GEOMETRY

namespace xd { namespace Geometry {  //此处又开辟一个命名空间，估计有一些全局函数在里面

bool directions_parallel(double angle1, double angle2, double max_diff = 0);  //返回两个角度之差是否在误差允许的范围内为0度或180度，即平行

}}
#endif // GEOMETRY

