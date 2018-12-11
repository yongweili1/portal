#include "uAIArithmetic/arithmetic.h"
#include "uAIArithmetic/point2d.h"
#include <cmath>


BEGIN_NAMESPACE


/// \brief        default constructor of the class Point2D
///
/// \return       none
Point2D::Point2D()
:x(0.0),y(1.0)
{
}


/// \brief        constructor of the class Point2D
/// \param[in]    xValue the x part of the 2D point
/// \param[in]    yValue the y part of the 2D point
///
/// \return       none
Point2D::Point2D(double xValue,double yValue)
	:x(xValue), y(yValue)
{
}


/// \brief        copy constructor of the class Point2D
///
/// \param[in]    myPoint the point to be copy
///
/// \return       none
Point2D::Point2D(const Point2D& myPoint)
	:x(myPoint.x), y(myPoint.y)
{
}


/// \brief        destructor of the class Point2D
///
/// \return       none
Point2D::~Point2D()
{
}


/// \brief        overload the plus compound assign operator
///
/// \param[in]    v2 the 2D point to plus
///
/// \return       the product of two points
Point2D& Point2D::operator +=(const Point2D& v2)
{
    * this = *this + v2;
    return *this;
};


/// \brief        overload the plus operator
///
/// \param[in]    v2 the point to plus
///
/// \return       the product of two points
Point2D Point2D::operator +(const Point2D& v2) const
{
    return Point2D(x + v2.x, y + v2.y);
}

/// \brief        overload the plus compound assign operator
///
/// \param[in]    v2 the 2D vector to plus
///
/// \return       the product of one point plus one vector
Point2D& Point2D::operator +=(const Vector2D& v2)
{
    * this = *this + v2;
    return *this;
};

/// \brief        overload the plus operator
///
/// \param[in]    v2 the vector to plus
///
/// \return       the product of one point plus one vector
Point2D Point2D::operator +(const Vector2D& v2) const
{
    return Point2D(x + v2.x, y + v2.y);
}


/// \brief        overload the minus compound assign operator
///
/// \param[in]    v2 the 2D point to minus
///
/// \return       the product of two points
Point2D& Point2D::operator -=(const Vector2D& v2)
{
    this->x = this->x - v2.x;
    this->y = this->y - v2.y;
    return *this;
}


/// \brief        overload the minus operator
///
/// \param[in]    v2 the point to minus
///
/// \return       the product of one point and one vector
Vector2D Point2D::operator -(const Point2D& v2) const
{
    return Vector2D(x - v2.x, y - v2.y);
}


/// \brief Each component multiplies a scalar, overload *= operator 
///
/// \param[in] dScale The scalar to be multiplied with current point
///
/// \return The result point
Point2D& Point2D::operator *=( double dScale )
{
    x *= dScale;
    y *= dScale;
    return *this;
}


/// \brief Each component multiplies a scalar, overload * operator
///
/// \param[in] dScale The scalar to be multiplied with current point
///
/// \return The result point
Point2D Point2D::operator *(double dScale) const
{
    return Point2D( x * dScale,
                             y * dScale);
}


/// \brief        overload the assign operator
///
/// \param[in]    v2 the point to assign from
///
/// \return       the assigned instance
Point2D& Point2D::operator =(const Point2D& v2)
{
    this->x = v2.x;
    this->y = v2.y;
    return *this;
}


/// \brief        overload the inequality operator
///
/// \param[in]    v2 the point to compare with
///
/// \return       if the two points unequal to each other
bool Point2D::operator !=(const Point2D& v2) const
{
    return (std:: fabs( x - v2.x ) > ARITHMETIC_DOUBLE_MICRO || 
            std:: fabs( y - v2.y ) > ARITHMETIC_DOUBLE_MICRO );
}


/// \brief        overload the equality operator
///
/// \param[in]    v2 the point to compare with
///
/// \return       if the two points equal to each other
bool Point2D::operator ==(const Point2D& v2) const
{
    return (std:: fabs( x - v2.x ) < ARITHMETIC_DOUBLE_MICRO && 
            std:: fabs( y - v2.y ) < ARITHMETIC_DOUBLE_MICRO );
}

END_NAMESPACE
