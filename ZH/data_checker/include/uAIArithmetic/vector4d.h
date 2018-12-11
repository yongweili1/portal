
#ifndef ARITHMETIC_VECTOR4D_H_
#define ARITHMETIC_VECTOR4D_H_

#include "uai_app_defs.h"
#include <memory>

BEGIN_NAMESPACE

class Matrix4x4;

/// \Class Vector3D
/// \brief Standard 3-dimensional vector
class Vector4D
{
public:
    
    /// \brief Default Constructor
    Vector4D();

    /// \brief Constructor with x, y, z components
    Vector4D(const double dx, const double dy, const double dz, const double dw);

    /// \brief Copy Constructor
    Vector4D(const Vector4D& myOther);

    /// \brief Default destructor
    ~Vector4D();

public:
    
    /// \brief overload += operator
    Vector4D & operator += (const Vector4D& myVector);

    /// \brief overload + operator
    Vector4D operator + (const Vector4D& myVector) const;
    
    /// \brief overload -= operator
    Vector4D & operator -= (const Vector4D& myVector);
    
    /// \brief overload - operator
    Vector4D   operator - (const Vector4D& myVector) const;

    /// \brief overload - operator
    Vector4D   operator - () const;
    
    /// \brief Each component multiplies a scalar, overload *= operator 
    Vector4D & operator *= (const double dScale);

    /// \brief Each component multiplies a scalar, overload *= operator
    Vector4D operator * (const double dScale) const;
    
    /// \brief overload != operator
    bool operator != (const Vector4D& myVector) const;

    /// \brief overload == operator
    bool operator == (const Vector4D& myVector) const;

    /// \brief overload = operator
    Vector4D & operator = (const Vector4D& myVector);

public:

    /// \fn double AngleBetween(const Vector3D&)
    ///
    /// \brief Calculates the angle between 2 vectors.
    ///        Vectors do not have to be unit-length.
    double AngleBetween(const Vector4D& myOther) const;

	/// \fn double RotateAngleBetween(const Vector3D&)
	///
	/// \brief Calculates the angle between 2 vectors.
	///        Diffenerent from AngleBetween, the angle
	//         between always returns value in 0~PI,
	//         this interface could return -PI~PI
	//         Vectors do not have to be unit-length.
	//         users should appoint the Axis direction
	//         that the plane are facing with
	//         All parameter do not need to be normalized
	double RotateAngleBetween(const Vector4D& myOther, const Vector4D& myAxis) const;

	/// \brief Calculates the projection vector over the target vector
	///
	/// \param[in] src, target
	///
	/// \return the projection vector over target vector
	Vector4D Projection(const Vector4D& myOther);

    /// \fn Vector3D CrossProduct(const Vector3D&)
    ///
    /// \brief Calculates the angle between 2 vectors.
    ///        Vectors do not have to be unit-length.
    Vector4D CrossProduct(const Vector4D& myVector) const;

    /// \fn DotProduct(const Vector3D&)
    ///
    /// \brief Calculates the Dot Product between 2 vectors.
    ///         If both are unit vectors, the dot product is the cosine of the angle; 
    ///         otherwise the dot product must be divided by the product of the lengths 
    ///         of both vectors to get the cosine of the angle.
    double DotProduct(const Vector4D& myVector) const;
    
    /// \fn double Magnitude()
    ///
    /// \brief Get the magnitude of the vector
    double Magnitude() const;

    /// \fn Vector3D Normalize()
    /// \brief This method normalizes the vector such that it's length / magnitude is 1. 
    ///        The result is called a unit vector.
    bool Normalize();

    /// \fn Vector3D Normalize()
    /// \brief This method normalizes the vector such that it's length / magnitude is 1. 
    ///        The result is called a unit vector.
    bool GetNormalize(Vector4D& myNormal) const;

    /// \fn Vector3D Reflect(const Vector3D& normal) const
    ///
    /// \brief Calculates a reflection vector to the plane with the given normal.
    ///        The normal need to be normalized.
    Vector4D Reflect(const Vector4D& normal) const;

    /// \brief check whether 2 vectors is parallel to each other
    /// return true if they are parallel
    ///        false if they are not parallel
    bool IsParallel2(const Vector4D& myVector) const;

    /// \brief check whether 2 vectors is orthotropic to each other
    /// return true if they are orthotropic
    ///        false if they are not orthotropic
    bool IsOrthotropic2(const Vector4D& myVector) const;

public:
    /// \fn double AngleBetween(const Vector3D&)
    ///
    /// \brief Calculates the angle between 2 vectors.
    ///        Vectors do not have to be unit-length.
    friend double AngleBetween(const Vector4D& myVector1, const Vector4D& myVector2);

	/// \fn double CitaBetween(const Vector3D& myVector1, const Vector3D& myVector2, const Vector3D& myAxis)
	///
	/// \brief Calculates the angle between 2 vectors.
	///        Diffenerent from AngleBetween, the angle
	//         between always returns value in 0~PI,
	//         this interface could return -PI~PI
	//         users should appoint the Axis direction
	//         that the plane are facing with
	//         All parameter do not need to be normalized
	friend double RotateAngleBetween(const Vector4D& myVector1, const Vector4D& myVector2, const Vector4D& myAxis);

	/// \brief Calculates the projection vector over the target vector
	///
	/// \param[in] src, target
	///
	/// \return the projection vector over target vector
	friend Vector4D Projection(const Vector4D& src, const Vector4D& target);

    /// \fn Vector3D CrossProduct(const Vector3D&, const Vector3D&)
    ///
    /// \brief Calculates the cross-product of 2 vectors, i.e.
    ///        the vector that lies perpendicular to them both.
    friend Vector4D CrossProduct(const Vector4D& myVector1, const Vector4D& myVector2);
    
    /// \fn Vector3D DotProduct(const Vector3D&, const Vector3D&)
    ///
    /// \brief Calculates the Dot Product between 2 vectors.
    ///         If both are unit vectors, the dot product is the cosine of the angle; 
    ///         otherwise the dot product must be divided by the product of the lengths 
    ///         of both vectors to get the cosine of the angle.
    friend double DotProduct(const Vector4D& myVector1, const Vector4D& myVector2);

    /// \fn Vector3D operator *(double, const Vector3D&)
    ///
    /// \brief overload * operator, scalar multiply a vector
    friend Vector4D operator *(double dScale, const Vector4D& myVector);

public:
    double x;
    double y;
    double z;
    double w;
};

END_NAMESPACE

#endif