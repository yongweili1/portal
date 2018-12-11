#ifndef UIH_ARITHMETIC_VECTOR2D_H_
#define UIH_ARITHMETIC_VECTOR2D_H_

#include "uai_app_defs.h"

BEGIN_NAMESPACE

/// \class Vector2D
/// \brief Vector2D defines the operations of 2D Vector.
class Vector2D
{
public:
    /// \brief        default constructor of the class Vector2D
    Vector2D();

    /// \brief        constructor of the class Vector2D
    Vector2D(double xValue,double yValue);

    /// \brief        copy constructor of the class Vector2D
    Vector2D(const Vector2D & myVector2D);

    /// \brief        destructor of the class Vector2D
    ~Vector2D();

public:
    /// \brief overload operator += for class Vector2D
    Vector2D & operator += (const Vector2D& v2);

    /// \brief overload operator + for class Vector2D
    Vector2D   operator + (const Vector2D& v2) const;

    /// \brief overload operator -= for class Vector2D
    Vector2D & operator -= (const Vector2D& v2);

    /// \brief overload operator - for class Vector2D
    Vector2D   operator - (const Vector2D& v2) const;

	/// \brief overload - operator
	Vector2D   operator - () const;

    /// \brief Each component multiplies a scalar, overload *= operator 
    Vector2D & operator *= ( double dScale);

    /// \brief Each component multiplies a scalar, overload *= operator
    Vector2D operator * ( double dScale) const;

    /// \brief overload operator = for class Vector2D
    Vector2D & operator = (const Vector2D& v2);

    /// \brief overload operator != for class Vector2D
    bool operator != (const Vector2D & v2) const;

    /// \brief overload operator == for class Vector2D
    bool operator == (const Vector2D & v2) const;

public:
    /// \brief Calculates the angle between 2 vectors.
    ///        Vectors do not have to be unit-length.
    double AngleBetween( const Vector2D& myOther) const;

    /// \brief Calculates the Dot Product between 2 vectors.
    ///         If both are unit vectors, the dot product is the cosine of the angle; 
    ///         otherwise the dot product must be divided by the product of the lengths 
    ///         of both vectors to get the cosine of the angle.
    double DotProduct(const Vector2D& myVector) const;

    /// \brief calculate the norm
    double Norm() const;

    /// \brief normalize the 2D vector.
    bool Normalize();

	/// \fn double Magnitude()
	///
	/// \brief Get the magnitude of the vector
	double Magnitude() const;
public:
	friend double AngleBetween(const Vector2D&, const Vector2D&);

	/// \brief calculate the dot product of two vectors.
	friend double DotProduct(const Vector2D&, const Vector2D&);

	/// \brief overload * operator, scalar multiply a vector
	friend Vector2D operator *(double, const Vector2D&);
public:
    double x;
    double y;
};

END_NAMESPACE

#endif