#include "uAIArithmetic/arithmetic.h"
#include "uAIArithmetic/vector2d.h"
#define DBL_EPSILON     2.2204460492503131e-016

BEGIN_NAMESPACE
/// \brief        default constructor of the class Vector2D
///
/// \return       none
Vector2D::Vector2D()
:x(1.0),y(0.0)
{
}


/// \brief        constructor of the class Vector2D
///
/// \param[in]    xValue the x part of the 2D vector
/// \param[in]    yValue the y part of the 2D vector
///
/// \return       none
Vector2D::Vector2D(double xValue,double yValue)
	:x(xValue), y(yValue)
{
}


/// \brief        copy constructor of the class Vector2D
///
/// \param[in]    myVector2D the vector to be copy
///
/// \return       none
Vector2D::Vector2D(const Vector2D& myVector2D)
                  :x(myVector2D.x), y(myVector2D.y)
{
}


/// \brief        destructor of the class Vector2D
///
/// \return       none
Vector2D::~Vector2D()
{
}


/// \brief        overload the plus operator
///
/// \param[in]    v2 the 2D vector to plus
///
/// \return       the product of two vectors
Vector2D Vector2D::operator +(const Vector2D& v2) const
{
    return Vector2D(x + v2.x, y + v2.y);
}


/// \brief        overload the plus compound assign operator
///
/// \param[in]    v2 the 2D vector to plus
///
/// \return       the product of two vectors
Vector2D& Vector2D::operator +=(const Vector2D& v2)
{
    *this = *this + v2;
    return *this;
}


/// \brief        overload the minus operator
///
/// \param[in]    v2 the vector to minus
///
/// \return       the product of two vectors
Vector2D Vector2D::operator -(const Vector2D& v2) const
{
    return Vector2D(x - v2.x, y - v2.y);
}

/// \brief overload - operator
///
/// \return The result vector
Vector2D Vector2D::operator - () const
{
	return Vector2D(-x, -y);
}



/// \brief        overload the minus compound assign operator
///
/// \param[in]    v2 the 2D vector to minus
///
/// \return       the product of two vectors
Vector2D& Vector2D::operator -=(const Vector2D& v2)
{
    * this = *this - v2 ;
    return *this;
}


/// \brief Each component multiplies a scalar, overload * operator
///
/// \param[in] dScale The scalar to be multiplied with current vector
///
/// \return    The result vector
Vector2D Vector2D::operator *(double dScale) const
{
    return Vector2D( x * dScale,
                              y * dScale);
}


/// \brief Each component multiplies a scalar, overload *= operator 
///
/// \param[in] dScale The scalar to be multiplied with current vector
///
/// \return   The result vector
Vector2D& Vector2D::operator *=(double dScale)
{
    x *= dScale;
    y *= dScale;
    return *this;
}


/// \brief        overload the assign operator
///
/// \param[in]    v2 the 2D vector to assign from
///
/// \return       the assigned instance
Vector2D& Vector2D::operator =(const Vector2D& v2) 
{
    this->x = v2.x;
    this->y = v2.y;
    return *this;
}


/// \brief        overload the inequality operator
///
/// \param[in]    v2 the 2D vector to compare with
///
/// \return       if the two vectors unequal to each other
bool Vector2D::operator !=(const Vector2D& v2) const
{
    return (std:: fabs( x - v2.x ) > ARITHMETIC_DOUBLE_MICRO || 
            std:: fabs( y - v2.y ) > ARITHMETIC_DOUBLE_MICRO );
}


/// \brief        overload the equality operator
///
/// \param[in]    v2 the 2D vector to compare with
///
/// \return       if the two vectors equal to each other
bool Vector2D::operator ==(const Vector2D& v2) const
{
    return (std:: fabs( x - v2.x ) < ARITHMETIC_DOUBLE_MICRO && 
            std:: fabs( y - v2.y ) < ARITHMETIC_DOUBLE_MICRO );
}


/// \brief Calculates the angle between 2 vectors.
///        Vectors do not have to be unit-length.
///
/// \param[in] myVector The other vector
///
/// \return the angle between 2 vectors
double Vector2D::AngleBetween(const Vector2D& myOther) const
{
    double dlen = Norm() * myOther.Norm();

    dlen = (dlen > ARITHMETIC_DOUBLE_MICRO)? dlen : ARITHMETIC_DOUBLE_MICRO;

    double dproduct = DotProduct(myOther) / dlen;
    dproduct = std::min(dproduct, 1.0);
    dproduct = std::max(dproduct, -1.0);
    return std::acos(dproduct);
}


/// \brief Calculates the Dot Product between 2 vectors.
///         If both are unit vectors, the dot product is the cosine of the angle; 
///         otherwise the dot product must be divided by the product of the lengths 
///         of both vectors to get the cosine of the angle.
///
/// \param[in] myVector Another vector
///
/// \return the dot product of the two vectors
double Vector2D::DotProduct(const Vector2D& myVector) const
{
    return x * myVector.x + y * myVector.y;
}


/// \brief        calculate the magnitude of the vector
///
/// \return        the magnitude result
double Vector2D::Norm() const
{
    double squreVlue = x * x + y * y;
    double normValue = squreVlue > 0 ? sqrt(squreVlue) : 0;
    return normValue;
}


/// \brief        normalize the vector, make the magnitude equal to 1
///
/// \return       if the vector has 0 magnitude
bool Vector2D::Normalize()
{
    double normValue = this->Norm();
    bool bNormalizable = normValue > DBL_EPSILON ? true : false;
    if (bNormalizable)
    {
        x /= normValue;
        y /= normValue;
    }
    return bNormalizable;
}


/// \brief        calculate the angle produced by rotating first vector to second vector
///
/// \param[in]    v1 the 2D vector to rotate from
/// \param[in]    v2 the 2D vector to rotate towards
///
/// \return       the angle product of two vectors
double AngleBetween(const Vector2D& v1, 
                           const Vector2D& v2)
{
    //cos<a,b>=a▪b/|a|.|b|
    double dlen = v1.Norm() * v2.Norm();

    dlen = dlen > ARITHMETIC_DOUBLE_MICRO
		? dlen : ARITHMETIC_DOUBLE_MICRO;
    double dproduct = DotProduct(v1, v2)/dlen;

    dproduct = std::min(dproduct, 1.0);
    dproduct = std::max(dproduct, -1.0);
    return std::acos(dproduct);
}


/// \brief        calculate the dot product of two vectors
///
/// \param[in]    v1 the 2D vector to dot multiply
/// \param[in]    v2 the 2D vector to dot multiply
///
/// \return       the dot product of two vectors
double DotProduct(const Vector2D& v1, const Vector2D& v2)
{
    return v1.x * v2.x + v1.y * v2.y ;
}


/// \brief Get the magnitude of the vector
///
/// \return the magnitude\length of the vector
double Vector2D::Magnitude() const
{
	return std::sqrt(x * x + y * y);
}

/// \brief Each component of a vector multiplies a scalar, overload * operator
///
/// \param[in] dScale The scalar to be multiplied with current vector
/// \param[in] myVector The vector to multiply with
/// 
/// \return The result vector
Vector2D operator *(double dScale, const Vector2D& myVector)
{
    return Vector2D( dScale * myVector.x,
                              dScale * myVector.y);
}
END_NAMESPACE
