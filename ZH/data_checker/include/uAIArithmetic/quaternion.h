
#ifndef UIH_ARITHMETIC_QUATERNION_H_
#define UIH_ARITHMETIC_QUATERNION_H_

#include "uai_app_defs.h"
#include <memory>

BEGIN_NAMESPACE

class Vector3D;
class Matrix4x4;

/// \class Quaternion
/// \brief The Quaternion is an implementation of quaternion.
///        Quaternion is a kind of representation of rotation, which is
///        more effective and more intuitive than rotation matrix.
///        A quaternion is represented as below
///             q = w + x i + y j + z k
class Quaternion
{
public:
    /// \brief  default constructor
    Quaternion();

    /// \brief  overload constructor
    //Quaternion(double, double, double, double);

    /// \brief  overload constructor
    Quaternion(double dAngle, const Vector3D& vAxis);

    /// \brief  copy constructor
    Quaternion(const Quaternion& quaterion);

    /// \brief  destructor
    ~Quaternion();

public:
    /// \fn operator - ()
    ///
    /// \brief  overload the negative operator
    Quaternion   operator - () const;

    /// \fn operator *=( const Quaternion& )
    ///
    /// \brief  overload the multiply compound assign operator
    Quaternion & operator *=(const Quaternion& quaternion);

    /// \fn operator * ( const Quaternion& )
    ///
    /// \brief  overload the multiply operator
    Quaternion operator * (const Quaternion& quaternion) const;

    /// \fn operator = ( const Quaternion& )
    ///
    /// \brief  overload the assign operator
    Quaternion & operator = (const Quaternion& quaternion );

    /// \fn operator != ( const Quaternion& )
    ///
    /// \brief  overload the inequality operator
    bool operator != (const Quaternion& quaternion) const;

    /// \fn operator == ( const Quaternion& )
    ///
    /// \brief  overload the equality operator
    bool operator == (const Quaternion& quaternion) const;

    /// \fn ToMatrix()
    ///
    /// \brief  converter the quaternion to a rotation matrix
    Matrix4x4  ToMatrix() const;

    /// \fn Conjugate()
    ///
    /// \brief  return the conjugate of the quaternion
    Quaternion  Conjugate() const;

    /// \fn Normalize()
    ///
    /// \brief  normalize the magnitude to 1
    bool Normalize();
public:
    double m_dW;
    double m_dX;
    double m_dY;
    double m_dZ;

private:
    bool m_bNormalized;
};

END_NAMESPACE

#endif
