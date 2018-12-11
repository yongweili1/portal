#ifndef UIH_ARITHMETIC_POINT3D_H_
#define UIH_ARITHMETIC_POINT3D_H_

#include "uai_app_defs.h"
#include <memory>

BEGIN_NAMESPACE

class  Vector3D;

/// \class Point3D
/// \brief Point3D defines the operations of 3D point.
class Point3D
{
public:
    /// \brief default constructor of the class Point3D
    Point3D();

    /// \brief constructor of the class Point3D
    Point3D(double xValue, double yValue, double zValue);

    /// \brief copy constructor of the class Point3D
    Point3D(const Point3D & myPoint3D);

    /// \brief destructor of the class Point3D
    ~Point3D();

public:
    /// \brief overload operator += for class Point3D
    Point3D & operator += (const Point3D& v2);

    /// \brief overload operator + for class Point3D
    Point3D   operator + (const Point3D& v2) const;

    /// \brief overload operator += for class Point3D
    Point3D & operator += (const Vector3D& v2);

    /// \brief overload operator + for class Point3D
    Point3D   operator + (const Vector3D& v2) const;

    /// \brief overload operator -= for class Point3D
    Point3D & operator -= (const Vector3D& v2);

    /// \brief overload operator - for class Vector3D
    Point3D   operator - (const Vector3D& v2) const;

    /// \brief overload operator - for class Point3D
    Vector3D   operator -(const Point3D& v2) const;

    /// \brief Each component multiplies a scalar, overload *= operator 
    Point3D & operator *= ( double dScale);

    /// \brief Each component multiplies a scalar, overload *= operator
    Point3D operator * ( double dScale) const;

    /// \brief overload operator = for class Point3D
    Point3D & operator = (const Point3D& v2);

    /// \brief overload operator != for class Point3D
    bool operator != (const Point3D & v2) const;

    /// \brief overload operator == for class Point3D
    bool operator == (const Point3D & v2) const;

public:
    double x;
    double y;
    double z;
};

END_NAMESPACE

#endif
