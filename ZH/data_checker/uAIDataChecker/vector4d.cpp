#include "uAIArithmetic/arithmetic.h"
#include "uAIArithmetic/vector4d.h"
#include <cmath>

BEGIN_NAMESPACE

/// \brief Default Constructor
Vector4D::Vector4D()
    :x(0.0),y(1.0),z(0.0),w(0.0)
{
}

/// \brief Constructor with x, y, z components
Vector4D::Vector4D(const double dx, const double dy, const double dz, const double dw)
    :x(dx),y(dy),z(dz),w(dw)
{
}

/// \brief Copy Constructor
Vector4D::Vector4D(const Vector4D& myOther)
{
    x = myOther.x;
    y = myOther.y;
    z = myOther.z;
    w = myOther.w;
}

/// \brief Default destructor
Vector4D::~Vector4D()
{
}


/// \brief overload += operator
///
/// \param[in]   myVector  The vector to be added on this object
///
/// \return      The result vector
Vector4D & Vector4D::operator += (const Vector4D& myVector)
{
    x += myVector.x;
    y += myVector.y;
    z += myVector.z;
    w += myVector.w;
    return *this;
}


/// \brief overload + operator
///
/// \param[in] myVector The vector to be added on this object
///
/// \return      The result vector
Vector4D Vector4D:: operator + (const Vector4D& myVector) const
{
    return Vector4D(x + myVector.x, y + myVector.y, z + myVector.z, w + myVector.w);
}


/// \brief overload -= operator
///
/// \param[in] myVector The vector to be subtracted from this object
///
/// \return The result vector
Vector4D & Vector4D::operator -= (const Vector4D& myVector)
{
    x -= myVector.x;
    y -= myVector.y;
    z -= myVector.z;
    w -= myVector.w;
    return *this;
}


/// \brief overload - operator
///
/// \param[in] myVector The vector to be subtracted from this object
///
/// \return The result vector
Vector4D Vector4D::operator - (const Vector4D& myVector) const
{
    return Vector4D(x - myVector.x,
                              y - myVector.y,
                              z - myVector.z,
                              w - myVector.w);
}


/// \brief overload - operator
///
/// \return The result vector
Vector4D Vector4D::operator - () const
{
    return Vector4D(-x, -y, -z, -w);
}


/// \brief Each component multiplies a scalar, overload *= operator 
///
/// \param[in] dScale The scalar to be multiplied with current vector
///
/// \return The result vector
Vector4D & Vector4D::operator *= (const double dScale)
{
    x *= dScale;
    y *= dScale;
    z *= dScale;
    w *= dScale;
    return *this;
}


/// \brief Each component multiplies a scalar, overload * operator
///
/// \param[in] dScale The scalar to be multiplied with current vector
///
/// \return The result vector
Vector4D Vector4D::operator * (const double dScale) const
{
    return Vector4D(x * dScale,
                              y * dScale,
                              z * dScale,
                              w * dScale);
}


/// \brief overload != operator
///
/// \param[in] myVector Input vector to compare with the current object
///
/// \return true if the two vectors are not equal,
///         false if the two vectors equals
bool Vector4D::operator != (const Vector4D& myVector) const
{
    return (std:: fabs(x - myVector.x) > ARITHMETIC_DOUBLE_MICRO || 
             std:: fabs(y - myVector.y) > ARITHMETIC_DOUBLE_MICRO || 
             std:: fabs(z - myVector.z) > ARITHMETIC_DOUBLE_MICRO ||
             std:: fabs(w - myVector.w) > ARITHMETIC_DOUBLE_MICRO);
}


/// \brief overload == operator
///
/// \param[in] myVector Input vector to compare with the current object
///
/// \return false if the two vectors are not equal
///         true if the two vectors equals
bool Vector4D::operator == (const Vector4D& myVector) const
{
     return (std:: fabs(x - myVector.x) < ARITHMETIC_DOUBLE_MICRO && 
              std:: fabs(y - myVector.y) < ARITHMETIC_DOUBLE_MICRO && 
              std:: fabs(z - myVector.z) < ARITHMETIC_DOUBLE_MICRO &&
              std:: fabs(w - myVector.w) < ARITHMETIC_DOUBLE_MICRO);
}


/// \brief overload = operator. Assigns the value of the other vector
///
/// \param[in] myVector The other vector
Vector4D & Vector4D::operator = (const Vector4D& myVector)
{
    x = myVector.x;
    y = myVector.y;
    z = myVector.z;
    w = myVector.w;
    return *this;
}

/// \brief Calculates the angle between 2 vectors.
///        Vectors do not have to be unit-length.
///
/// \param[in] myVector The other vector
///
/// \return the angle between 2 vectors
double Vector4D::AngleBetween(const Vector4D& myOther) const
{
    double dlen = Magnitude() * myOther.Magnitude();

    dlen = (dlen > ARITHMETIC_DOUBLE_MICRO) ? dlen : ARITHMETIC_DOUBLE_MICRO;

    double dproduct = DotProduct(myOther) / dlen;
    dproduct = std::min(dproduct, 1.0);
    dproduct = std::max(dproduct, -1.0);
    return std::acos(dproduct);
}

/// \brief Calculates the angle between 2 vectors.
///        Vectors do not have to be unit-length.
///
/// \param[in] myVector The other vector
///
/// \return the angle between 2 vectors
double Vector4D::RotateAngleBetween(const Vector4D& myOther, const Vector4D& myAxis) const
{
	Vector4D targetAxis = myAxis;

	targetAxis.Normalize();

	double dlen = myAxis.DotProduct(this->CrossProduct(myOther));

	double dProduct = DotProduct(myOther);

	return std::atan2(dlen, dProduct);
}

/// \brief Calculates the projection vector over the target vector
///
/// \param[in] src, target
///
/// \return the projection vector over target vector
Vector4D Vector4D::Projection(const Vector4D& myOther)
{
	Vector4D targetNormalize = myOther;
	targetNormalize.Normalize();
	Vector4D vecProjection = DotProduct(targetNormalize) * myOther;
	return vecProjection;
}

/// \brief Calculates the cross-product of 2 vectors, i.e.
///        the vector that lies perpendicular to them both.
///
/// \param[in] myVector Another vector
///
/// \return the result vector
Vector4D  Vector4D::CrossProduct(const Vector4D& myVector) const
{
    return Vector4D(y * myVector.z - z * myVector.y,
                              z * myVector.x - x * myVector.z,
                              x * myVector.y - y * myVector.x,
                              w * myVector.w - w * myVector.w);
}


/// \brief Calculates the Dot Product between 2 vectors.
///         If both are unit vectors, the dot product is the cosine of the angle; 
///         otherwise the dot product must be divided by the product of the lengths 
///         of both vectors to get the cosine of the angle.
///
/// \param[in] myVector Another vector
///
/// \return the dot product of the two vectors
double Vector4D::DotProduct(const Vector4D& myVector) const
{
    return x * myVector.x + y * myVector.y + z * myVector.z + w * myVector.w;
}


/// \brief Get the magnitude of the vector
///
/// \return the magnitude\length of the vector
double Vector4D::Magnitude() const
{
    return std::sqrt(x * x + y * y + z * z + w * w);
}

/// \fn Vector4D Normalize()
///
/// \brief This method normalizes the vector such that it's length / magnitude is 1. 
///        The result is called a unit vector.
///
/// \return true if successfully turned into a unit vector
///         false if the vector's magnitude if zero
bool Vector4D::Normalize()
{
    double dlength = Magnitude();
    if (dlength < ARITHMETIC_DOUBLE_MICRO)
    {
        return false;
    }
    double dinvlength = 1.0 / dlength;
    x *= dinvlength;
    y *= dinvlength;
    z *= dinvlength;
    w *= dinvlength;
    return true;
}

/// \fn Vector4D GetNormalize()
///
/// \brief This method normalizes the vector such that it's length / magnitude is 1. 
///        The result is called a unit vector.
///
///param[out] myNormal the normalize result.
/// \return true if successfully turned into a unit vector
///         false if the vector's magnitude if zero
bool Vector4D::GetNormalize(Vector4D& myNormal) const
{
    double dlength = Magnitude();
    if (dlength < ARITHMETIC_DOUBLE_MICRO)
    {
        return false;
    }
    double dinvlength = 1.0 / dlength;
    myNormal.x = x * dinvlength;
    myNormal.y = y * dinvlength;
    myNormal.z = z * dinvlength;
    myNormal.w = w * dinvlength;
    return true;
}

/// \brief Calculates a reflection vector to the plane with the given normal.
///        The normal need to be normalized.
///
/// \param[in] normal a plane's normal, need to be normalized before calling this function
///
/// \return The reflected vector
Vector4D Vector4D::Reflect(const Vector4D& normal) const
{
     return Vector4D(*this - (2 * this->DotProduct(normal) * normal));
}


/// \brief Calculates the angle between 2 vectors.
///        Vectors do not have to be unit-length.
///
/// \param[in] myVector1 The first vector
/// \param[in] myVector2 The second vector
///
/// \return the angle between the vectors
double AngleBetween(const Vector4D& myVector1, 
                            const Vector4D& myVector2)
{
    double dlen = myVector1.Magnitude() * myVector2.Magnitude();

    dlen = dlen > ARITHMETIC_DOUBLE_MICRO? dlen : ARITHMETIC_DOUBLE_MICRO;

    double dproduct = myVector1.DotProduct(myVector2) / dlen;
    dproduct = std::min(dproduct, 1.0);
    dproduct = std::max(dproduct, -1.0);
    return std::acos(dproduct);
}

/// \brief Calculates the angle between 2 vectors.
///        Vectors do not have to be unit-length.
///
/// \param[in] myVector The other vector
///
/// \return the angle between 2 vectors
double RotateAngleBetween(const Vector4D& myVector1, const Vector4D& myVector2, const Vector4D& myAxis)
{
	Vector4D targetAxis = myAxis;

	targetAxis.Normalize();

	double dlen = DotProduct((CrossProduct(myVector1, myVector2)), myAxis);

	double dProduct = DotProduct(myVector1, myVector2);

	return std::atan2(dlen, dProduct);
}

/// \brief Calculates the projection vector over the target vector
///
/// \param[in] src, target
///
/// \return the projection vector over target vector
Vector4D Projection(const Vector4D& src, const Vector4D& target)
{
	Vector4D targetNormalize = target;
	targetNormalize.Normalize();
	Vector4D vecProjection = DotProduct(src, targetNormalize) * target;
	return vecProjection;
}

/// \brief Calculates the cross-product of 2 vectors, i.e.
///        the vector that lies perpendicular to them both.
/// \param[in] myVector1 The first vector
/// \param[in] myVector2 Another vector
///
/// \return the result vector
Vector4D CrossProduct(const Vector4D& myVector1, 
                                       const Vector4D& myVector2)
{
    return Vector4D(
        myVector1.y * myVector2.z - myVector1.z * myVector2.y,
        myVector1.z * myVector2.x - myVector1.x * myVector2.z,
        myVector1.x * myVector2.y - myVector1.y * myVector2.x,
        myVector1.w * myVector2.w - myVector1.w * myVector2.w);
}

double DotProduct(const Vector4D& myVector1, 
                          const Vector4D& myVector2)
{
    return myVector1.x * myVector2.x + myVector1.y * myVector2.y + myVector1.z * myVector2.z + myVector1.w * myVector2.w;
}

/// \brief Each component of a vector multiplies a scalar, overload * operator
///
/// \param[in] dScale The scalar to be multiplied with current vector
/// \param[in] myVector The vector to multiply with
/// 
/// \return The result vector
Vector4D operator *(double dScale, const Vector4D& myVector)
{
    return Vector4D(dScale * myVector.x,
                              dScale * myVector.y,
                              dScale * myVector.z,
                              dScale * myVector.w);
}

/// \brief check whether 2 vectors is parallel to each other
/// \param[in] myVector Input vector to compare with the current object
/// return true if they are parallel
///        false if they are not parallel
bool Vector4D::IsParallel2(const Vector4D& myVector) const
{
    return this->CrossProduct(myVector) == Vector4D(0.0, 0.0, 0.0,0.0);
}

/// \brief check whether 2 vectors is orthotropic to each other
/// \param[in] myVector Input vector to compare with the current object
/// return true if they are orthotropic
///        false if they are not orthotropic
bool Vector4D::IsOrthotropic2(const Vector4D& myVector) const
{
    return std:: fabs(this->DotProduct(myVector)) < ARITHMETIC_DOUBLE_MICRO;
}

END_NAMESPACE

