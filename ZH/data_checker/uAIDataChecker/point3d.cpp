
#include "uAIArithmetic/arithmetic.h"
#include "uAIArithmetic/point3d.h"
#include <cmath>

BEGIN_NAMESPACE

/// \brief        default constructor of the class Point3D
///
/// \return       none
Point3D::Point3D()
: x(0.0), y(1.0), z(0.0)
{
}


/// \brief        constructor of the class Point3D
///
/// \param[in]    xValue the x part of the 3D point
/// \param[in]    yValue the y part of the 3D point
/// \param[in]    zValue the z part of the 3D point
///
/// \return       none
Point3D::Point3D(double xValue,double yValue, double zValue)
                  : x(xValue), y(yValue), z(zValue)
{
}


/// \brief        copy constructor of the class Point3D
///
/// \param[in]    myPoint3D the 3D point to be copy
///
/// \return       none
Point3D::Point3D(const Point3D& myPoint3D)
                : x(myPoint3D.x),
                  y(myPoint3D.y),
                  z(myPoint3D.z)
{
}


/// \brief        destructor of the class Point3D
///
/// \return       none
Point3D::~Point3D()
{
}


/// \brief        overload the plus operator
///
/// \param[in]    v2 the 3D point to plus
///
/// \return       the product of two points
Point3D Point3D::operator +(const Point3D& v2) const
{
    return Point3D(x + v2.x, y + v2.y, z + v2.z);
}


/// \brief        overload the plus compound assign operator
///
/// \param[in]    v2 the 3D point to plus
///
/// \return       the product of two points
Point3D& Point3D::operator +=(const Point3D& v2)
{
    * this = *this + v2;
    return *this;
}


/// \brief        overload the plus operator
///
/// \param[in]    v2 the 3D vector to plus
///
/// \return       the product of one point and one vector
Point3D Point3D::operator +(const Vector3D& v2) const
{
    return Point3D(x + v2.x, y + v2.y, z + v2.z);
}


/// \brief        overload the plus compound assign operator
///
/// \param[in]    v2 the 3D vector to plus
///
/// \return       the product of one point and one vector
Point3D& Point3D::operator +=(const Vector3D& v2)
{
    * this = *this + v2;
    return *this;
}


/// \brief        overload the minus operator
///
/// \param[in]    v2 the 3D point to minus
///
/// \return       the product of two points
Vector3D Point3D::operator -(const Point3D& v2) const
{
    return Vector3D(x - v2.x, y - v2.y, z - v2.z);
}


/// \brief        overload the minus compound assign operator
///
/// \param[in]    v2 the 3D vector to minus
///
/// \return       the product of two points
Point3D& Point3D::operator -=(const Vector3D& v2)
{
    this->x = this->x - v2.x;
    this->y = this->y - v2.y;
    this->z = this->z - v2.z;
    return *this;
}


/// \brief overload operator - for class Vector3D
///
/// \param[in]    v2 the 3D vector to minus
///
/// \return       the new point
Point3D Point3D::operator -(const Vector3D& v2) const
{
    return Point3D(x - v2.x, y - v2.y, z - v2.z);
}

/// \brief Each component multiplies a scalar, overload *= operator 
///
/// \param[in] dScale The scalar to be multiplied with current point
///
/// \return The result point
Point3D& Point3D::operator *=(double dScale)
{
    x *= dScale;
    y *= dScale;
    z *= dScale;
    return *this;
}


/// \brief Each component multiplies a scalar, overload * operator
///
/// \param[in] dScale The scalar to be multiplied with current point
///
/// \return The result point
Point3D Point3D::operator *(double dScale) const
{
    return Point3D( x * dScale,
                             y * dScale,
                             z * dScale);
}


/// \brief        overload the assign operator
///
/// \param[in]    v2 the 3D point to minus
///
/// \return       the assigned instance
Point3D& Point3D::operator =(const Point3D& v2)
{
    this->x = v2.x;
    this->y = v2.y;
    this->z = v2.z;
    return *this;
}


/// \brief        overload the inequality operator
///
/// \param[in]    v2 the 3D point to compare with
///
/// \return       if the two points unequal to each other
bool Point3D::operator !=(const Point3D& v2) const
{
    return (std:: fabs( x - v2.x ) > ARITHMETIC_DOUBLE_MICRO || 
            std:: fabs( y - v2.y ) > ARITHMETIC_DOUBLE_MICRO || 
            std:: fabs( z - v2.z ) > ARITHMETIC_DOUBLE_MICRO );
}


/// \brief        overload the equality operator
///
/// \param[in]    v2 the 3D point to compare with
///
/// \return       if the two points equal to each other
bool Point3D::operator ==(const Point3D& v2) const
{
    return (std:: fabs( x - v2.x ) < ARITHMETIC_DOUBLE_MICRO && 
            std:: fabs( y - v2.y ) < ARITHMETIC_DOUBLE_MICRO && 
            std:: fabs( z - v2.z ) < ARITHMETIC_DOUBLE_MICRO );
}

END_NAMESPACE
