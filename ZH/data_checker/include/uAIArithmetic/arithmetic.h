#ifndef UIH_ARITHMETIC_H_
#define UIH_ARITHMETIC_H_

#include <cmath>
#include "uAIArithmetic/matrix_4x4.h"
#include "uAIArithmetic/matrix_3x3.h"
#include "uAIArithmetic/point2d.h"
#include "uAIArithmetic/point3d.h"
#include "uAIArithmetic/quaternion.h"
#include "uAIArithmetic/vector2d.h"
#include "uAIArithmetic/vector3d.h"
#include "uAIArithmetic/vector4d.h"
#include "uAIArithmetic/plane.h"
#include "uAIArithmetic/plane2d.h"
#include "uAIArithmetic/camera.h"
#ifndef UIH_ARITHMETIC_PI
#define UIH_ARITHMETIC_PI 3.1415926535897931
#endif
#ifndef ARITHMETIC_DOUBLE_MICRO
#define ARITHMETIC_DOUBLE_MICRO    1.0e-7
#endif
#ifndef UIH_ARITHMETIC_DOUBLE_ZERO
#define UIH_ARITHMETIC_DOUBLE_ZERO     1.0e-15
#endif
#ifndef UIH_ARITHMETIC_REFERENCE_UNDER
#define UIH_ARITHMETIC_REFERENCE_UNDER     -100000
#endif
#ifndef UIH_ARITHMETIC_CT_WWMAX
#define UIH_ARITHMETIC_CT_WWMAX     8192
#endif
#ifndef UIH_ARITHMETIC_CT_WWMIN
#define UIH_ARITHMETIC_CT_WWMIN     1
#endif
#ifndef UIH_ARITHMETIC_MR_WWMAX
#define UIH_ARITHMETIC_MR_WWMAX     2000000
#endif
#ifndef UIH_ARITHMETIC_MR_WWMIN
#define UIH_ARITHMETIC_MR_WWMIN     1
#endif
#ifndef UIH_ARITHMETIC_CT_WLMAX
#define UIH_ARITHMETIC_CT_WLMAX     3072
#endif
#ifndef UIH_ARITHMETIC_CT_WLMIN
#define UIH_ARITHMETIC_CT_WLMIN     -1024
#endif
#ifndef UIH_ARITHMETIC_MR_WLMAX
#define UIH_ARITHMETIC_MR_WLMAX     1000000
#endif
#ifndef UIH_ARITHMETIC_MR_WLMIN
#define UIH_ARITHMETIC_MR_WLMIN     0
#endif

#ifndef RoundToInt32
#define RoundToInt32(x) (int(((x) >= 0.0) ? (x) + 0.5 : (x) - 0.5))
#endif

#endif
