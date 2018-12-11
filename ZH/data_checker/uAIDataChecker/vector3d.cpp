#include "uAIArithmetic/arithmetic.h"
#include "uAIArithmetic/vector3d.h"
#include <cmath>

BEGIN_NAMESPACE

/// \brief Default Constructor
Vector3D::Vector3D()
    :x(0.0),y(1.0),z(0.0)
{
}

/// \brief Constructor with x, y, z components
Vector3D::Vector3D(const double dx, const double dy, const double dz)
    :x(dx),y(dy),z(dz)
{
}

/// \brief Copy Constructor
Vector3D::Vector3D(const Vector3D& myOther)
{
    x = myOther.x;
    y = myOther.y;
    z = myOther.z;
}

/// \brief Default destructor
Vector3D::~Vector3D()
{
}


/// \brief overload += operator
///
/// \param[in]   myVector  The vector to be added on this object
///
/// \return      The result vector
Vector3D & Vector3D::operator += (const Vector3D& myVector)
{
    x += myVector.x;
    y += myVector.y;
    z += myVector.z;
    return *this;
}


/// \brief overload + operator
///
/// \param[in] myVector The vector to be added on this object
///
/// \return      The result vector
Vector3D Vector3D:: operator + (const Vector3D& myVector) const
{
    return Vector3D(x + myVector.x, y + myVector.y, z + myVector.z);
}


/// \brief overload -= operator
///
/// \param[in] myVector The vector to be subtracted from this object
///
/// \return The result vector
Vector3D & Vector3D::operator -= (const Vector3D& myVector)
{
    x -= myVector.x;
    y -= myVector.y;
    z -= myVector.z;
    return *this;
}


/// \brief overload - operator
///
/// \param[in] myVector The vector to be subtracted from this object
///
/// \return The result vector
Vector3D Vector3D::operator - (const Vector3D& myVector) const
{
    return Vector3D(x - myVector.x,
                              y - myVector.y,
                              z - myVector.z);
}


/// \brief overload - operator
///
/// \return The result vector
Vector3D Vector3D::operator - () const
{
    return Vector3D(-x, -y, -z);
}


/// \brief Each component multiplies a scalar, overload *= operator 
///
/// \param[in] dScale The scalar to be multiplied with current vector
///
/// \return The result vector
Vector3D & Vector3D::operator *= (const double dScale)
{
    x *= dScale;
    y *= dScale;
    z *= dScale;
    return *this;
}


/// \brief Each component multiplies a scalar, overload * operator
///
/// \param[in] dScale The scalar to be multiplied with current vector
///
/// \return The result vector
Vector3D Vector3D::operator * (const double dScale) const
{
    return Vector3D(x * dScale,
                              y * dScale,
                              z * dScale);
}


/// \brief overload != operator
///
/// \param[in] myVector Input vector to compare with the current object
///
/// \return true if the two vectors are not equal,
///         false if the two vectors equals
bool Vector3D::operator != (const Vector3D& myVector) const
{
    return (std:: fabs(x - myVector.x) > ARITHMETIC_DOUBLE_MICRO || 
             std:: fabs(y - myVector.y) > ARITHMETIC_DOUBLE_MICRO || 
             std:: fabs(z - myVector.z) > ARITHMETIC_DOUBLE_MICRO);
}


/// \brief overload == operator
///
/// \param[in] myVector Input vector to compare with the current object
///
/// \return false if the two vectors are not equal
///         true if the two vectors equals
bool Vector3D::operator == (const Vector3D& myVector) const
{
     return (std:: fabs(x - myVector.x) < ARITHMETIC_DOUBLE_MICRO && 
              std:: fabs(y - myVector.y) < ARITHMETIC_DOUBLE_MICRO && 
              std:: fabs(z - myVector.z) < ARITHMETIC_DOUBLE_MICRO);
}


/// \brief overload = operator. Assigns the value of the other vector
///
/// \param[in] myVector The other vector
Vector3D & Vector3D::operator = (const Vector3D& myVector)
{
    x = myVector.x;
    y = myVector.y;
    z = myVector.z;
    return *this;
}

/// \brief Calculates the angle between 2 vectors.
///        Vectors do not have to be unit-length.
///
/// \param[in] myVector The other vector
///
/// \return the angle between 2 vectors
double Vector3D::AngleBetween(const Vector3D& myOther) const
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
double Vector3D::RotateAngleBetween(const Vector3D& myOther, const Vector3D& myAxis) const
{
	Vector3D targetAxis = myAxis;

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
Vector3D Vector3D::Projection(const Vector3D& myOther)
{
	Vector3D targetNormalize = myOther;
	targetNormalize.Normalize();
	Vector3D vecProjection = DotProduct(targetNormalize) * myOther;
	return vecProjection;
}

/// \brief Calculates the cross-product of 2 vectors, i.e.
///        the vector that lies perpendicular to them both.
///
/// \param[in] myVector Another vector
///
/// \return the result vector
Vector3D  Vector3D::CrossProduct(const Vector3D& myVector) const
{
    return Vector3D(y * myVector.z - z * myVector.y,
                              z * myVector.x - x * myVector.z,
                              x * myVector.y - y * myVector.x);
}


/// \brief Calculates the Dot Product between 2 vectors.
///         If both are unit vectors, the dot product is the cosine of the angle; 
///         otherwise the dot product must be divided by the product of the lengths 
///         of both vectors to get the cosine of the angle.
///
/// \param[in] myVector Another vector
///
/// \return the dot product of the two vectors
double Vector3D::DotProduct(const Vector3D& myVector) const
{
    return x * myVector.x + y * myVector.y + z * myVector.z;
}


/// \brief Get the magnitude of the vector
///
/// \return the magnitude\length of the vector
double Vector3D::Magnitude() const
{
    return std::sqrt(x * x + y * y + z * z);
}

/// \fn Vector3D Normalize()
///
/// \brief This method normalizes the vector such that it's length / magnitude is 1. 
///        The result is called a unit vector.
///
/// \return true if successfully turned into a unit vector
///         false if the vector's magnitude if zero
bool Vector3D::Normalize()
{
    double dlength = std::sqrt(x * x + y * y + z * z);
    if (dlength < ARITHMETIC_DOUBLE_MICRO)
    {
        return false;
    }
    double dinvlength = 1.0 / dlength;
    x *= dinvlength;
    y *= dinvlength;
    z *= dinvlength;
    return true;
}

/// \fn Vector3D GetNormalize()
///
/// \brief This method normalizes the vector such that it's length / magnitude is 1. 
///        The result is called a unit vector.
///
///param[out] myNormal the normalize result.
/// \return true if successfully turned into a unit vector
///         false if the vector's magnitude if zero
bool Vector3D::GetNormalize(Vector3D& myNormal) const
{
    double dlength = std::sqrt(x * x + y * y + z * z);
    if (dlength < ARITHMETIC_DOUBLE_MICRO)
    {
        return false;
    }
    double dinvlength = 1.0 / dlength;
    myNormal.x = x * dinvlength;
    myNormal.y = y * dinvlength;
    myNormal.z = z * dinvlength;
    return true;
}

/// \brief Calculates a reflection vector to the plane with the given normal.
///        The normal need to be normalized.
///
/// \param[in] normal a plane's normal, need to be normalized before calling this function
///
/// \return The reflected vector
Vector3D Vector3D::Reflect(const Vector3D& normal) const
{
     return Vector3D(*this - (2 * this->DotProduct(normal) * normal));
}


/// \brief Calculates the angle between 2 vectors.
///        Vectors do not have to be unit-length.
///
/// \param[in] myVector1 The first vector
/// \param[in] myVector2 The second vector
///
/// \return the angle between the vectors
double AngleBetween(const Vector3D& myVector1, 
                            const Vector3D& myVector2)
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
double RotateAngleBetween(const Vector3D& myVector1, const Vector3D& myVector2, const Vector3D& myAxis)
{
	Vector3D targetAxis = myAxis;

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
Vector3D Projection(const Vector3D& src, const Vector3D& target)
{
	Vector3D targetNormalize = target;
	targetNormalize.Normalize();
	Vector3D vecProjection = DotProduct(src, targetNormalize) * target;
	return vecProjection;
}

/// \brief Calculates the cross-product of 2 vectors, i.e.
///        the vector that lies perpendicular to them both.
/// \param[in] myVector1 The first vector
/// \param[in] myVector2 Another vector
///
/// \return the result vector
Vector3D CrossProduct(const Vector3D& myVector1, 
                                       const Vector3D& myVector2)
{
    return Vector3D(myVector1.y * myVector2.z - myVector1.z * myVector2.y,
                              myVector1.z * myVector2.x - myVector1.x * myVector2.z,
                              myVector1.x * myVector2.y - myVector1.y * myVector2.x);
}

double DotProduct(const Vector3D& myVector1, 
                          const Vector3D& myVector2)
{
    return myVector1.x * myVector2.x + myVector1.y * myVector2.y + myVector1.z * myVector2.z;
}

/// \brief Each component of a vector multiplies a scalar, overload * operator
///
/// \param[in] dScale The scalar to be multiplied with current vector
/// \param[in] myVector The vector to multiply with
/// 
/// \return The result vector
Vector3D operator *(double dScale, const Vector3D& myVector)
{
    return Vector3D(dScale * myVector.x,
                              dScale * myVector.y,
                              dScale * myVector.z);
}

/// \brief check whether 2 vectors is parallel to each other
/// \param[in] myVector Input vector to compare with the current object
/// return true if they are parallel
///        false if they are not parallel
bool Vector3D::IsParallel2(const Vector3D& myVector) const
{
    return this->CrossProduct(myVector) == Vector3D(0.0, 0.0, 0.0);
}

/// \brief check whether 2 vectors is orthotropic to each other
/// \param[in] myVector Input vector to compare with the current object
/// return true if they are orthotropic
///        false if they are not orthotropic
bool Vector3D::IsOrthotropic2(const Vector3D& myVector) const
{
    return std:: fabs(this->DotProduct(myVector)) < ARITHMETIC_DOUBLE_MICRO;
}

END_NAMESPACE

