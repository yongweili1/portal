
#include "uAIArithmetic/arithmetic.h"
#include "uAIArithmetic/quaternion.h"

BEGIN_NAMESPACE


/// \brief        default constructor of the class Quaternion
///
/// \return       none
Quaternion::Quaternion()
    : m_dW(0), m_dX(1), m_dY(0), m_dZ(0), m_bNormalized(false)
{

}


/// \brief        constructor of the class Quaternion
///
/// \param[in]    dW: the w part of the quaternion
/// \param[in]    dX: the x part of the quaternion
/// \param[in]    dY: the y part of the quaternion
/// \param[in]    dZ: the z part of the quaternion
///
/// \return       none
//Quaternion::Quaternion(double dW, double dX, double dY, double dZ)
//    : m_dW(dW), m_dX(dX), m_dY(dY), m_dZ(dZ), m_bNormalized(false)
//{
//    if (std::abs(dX) < ARITHMETIC_DOUBLE_MICRO &&
//        std::abs(dY) < ARITHMETIC_DOUBLE_MICRO &&
//        std::abs(dZ) < ARITHMETIC_DOUBLE_MICRO)
//    {
//        m_dW = 0;
//        m_dX = 1;
//        m_dY = 0;
//        m_dZ = 0;
//    }
//}


/// \brief        constructor of the class Quaternion
///
/// \param[in]    dAngle: the angle the rotate
/// \param[in]    vAxis:  the axis to rotate around
///
/// \return       none
Quaternion::Quaternion(double dAngle, const Vector3D& vAxis)
    : m_dW(std::cos(dAngle / 2)),
      m_bNormalized(true)
{
    double dMagnitude = vAxis.Magnitude();
    if (dMagnitude < ARITHMETIC_DOUBLE_MICRO)
    {
        m_dX = 0;
        m_dY = 0;
        m_dZ = 0;
    }
    else
    {
        double dSin = std::sin(dAngle / 2);
        m_dX = dSin * vAxis.x / dMagnitude;
        m_dY = dSin * vAxis.y / dMagnitude;
        m_dZ = dSin * vAxis.z / dMagnitude;
    }
}


/// \brief        copy constructor of the class Quaternion
///
/// \return       none
Quaternion::Quaternion(const Quaternion& quaterion)
    : m_dW(quaterion.m_dW),
      m_dX(quaterion.m_dX),
      m_dY(quaterion.m_dY),
      m_dZ(quaterion.m_dZ),
      m_bNormalized(false)
{

}


/// \brief        destructor of the class Quaternion
///
/// \return       none
Quaternion::~Quaternion()
{

}


/// \brief        overload the negative operator
///
/// \return       the negative quaternion
Quaternion Quaternion::operator-() const
{
    Quaternion myQuaternion;
    myQuaternion.m_dW = -m_dW;
    myQuaternion.m_dX = -m_dX;
    myQuaternion.m_dY = -m_dY;
    myQuaternion.m_dZ = -m_dZ;
    return myQuaternion;
}


/// \brief        overload the multiply operator
///
/// \param[in]    quaternion: the quaternion to multiply with
///
/// \return       Quaternion: the product of two quaternions
Quaternion Quaternion::operator*(const Quaternion& quaternion) const
{
    Quaternion myQuaternion;
    myQuaternion.m_dW = 
        m_dW * quaternion.m_dW - m_dX * quaternion.m_dX - m_dY * quaternion.m_dY - m_dZ * quaternion.m_dZ;
    myQuaternion.m_dX = 
        m_dW * quaternion.m_dX + m_dX * quaternion.m_dW + m_dY * quaternion.m_dZ - m_dZ * quaternion.m_dY;
    myQuaternion.m_dY = 
        m_dW * quaternion.m_dY - m_dX * quaternion.m_dZ + m_dY * quaternion.m_dW + m_dZ * quaternion.m_dX;
    myQuaternion.m_dZ = 
        m_dW * quaternion.m_dZ + m_dX * quaternion.m_dY - m_dY * quaternion.m_dX + m_dZ * quaternion.m_dW;

    return myQuaternion;
}


/// \brief        overload the multiply compound assign operator
///
/// \param[in]    quaternion: the quaternion to multiply
///
/// \return       Quaternion&: the product of two quaternions
Quaternion& Quaternion::operator*=(const Quaternion& quaternion)
{
    *this = *this * quaternion;

    return *this;
}


/// \brief        convert the quaternion to a rotation matrix
///               for a normalized quaternion, the matrix is
///               [ ww+xx-yy-zz   2xy-2wz       2xz+2wy       0
///                 2xy+2wz       ww-xx+yy-zz   2yz-2wx       0
///                 2xz-2wy       2yz+2wx       ww-xx-yy+zz   0
///                 0             0             0             1 ]
///
/// \return       Matrix4x4: the resulting matrix
Matrix4x4 Quaternion::ToMatrix() const
{
    Quaternion quaternionTmp(*this);
    quaternionTmp.Normalize();

    double ww = quaternionTmp.m_dW * quaternionTmp.m_dW;
    double xx = quaternionTmp.m_dX * quaternionTmp.m_dX;
    double yy = quaternionTmp.m_dY * quaternionTmp.m_dY;
    double zz = quaternionTmp.m_dZ * quaternionTmp.m_dZ;
    double xy = 2 * quaternionTmp.m_dX * quaternionTmp.m_dY;
    double yz = 2 * quaternionTmp.m_dY * quaternionTmp.m_dZ;
    double xz = 2 * quaternionTmp.m_dX * quaternionTmp.m_dZ;
    double wx = 2 * quaternionTmp.m_dW * quaternionTmp.m_dX;
    double wy = 2 * quaternionTmp.m_dW * quaternionTmp.m_dY;
    double wz = 2 * quaternionTmp.m_dW * quaternionTmp.m_dZ;

    return Matrix4x4(
            ww + xx - yy - zz, xy + wz, xz - wy, 0,
            xy - wz, ww - xx + yy - zz, yz + wx, 0,
            xz + wy, yz - wx, ww - xx - yy + zz, 0,
            0, 0, 0, 1
        );
}


/// \brief        normalize the magnitude to 1
///
/// \return       bool: if the quaternion has 0 magnitude
bool Quaternion::Normalize()
{
    if (m_bNormalized)
    {
        return true;
    }

    double dNorm = sqrt(m_dW * m_dW + m_dX * m_dX + m_dY * m_dY + m_dZ * m_dZ);
    if (ARITHMETIC_DOUBLE_MICRO > dNorm)
    {
        return false;
    }

    m_dW /= dNorm;
    m_dX /= dNorm;
    m_dY /= dNorm;
    m_dZ /= dNorm;

    return true;
}


/// \brief        calculate the conjugate of the quaternion, 
///               q* = w - xi - yj - zk
///               which represent a rotation to the anti direction
///
/// \return       Quaternion: the conjugate
Quaternion Quaternion::Conjugate() const
{
    Quaternion myQuaternion;
    myQuaternion.m_dW = m_dW;
    myQuaternion.m_dX = -m_dX;
    myQuaternion.m_dY = -m_dY;
    myQuaternion.m_dZ = -m_dZ;
    return myQuaternion;
}


/// \brief        overload the equality operator
///
/// \param[in]    quaternion: the quaternion to compare with
///
/// \return       bool: if the two quaternions equal to each other
bool Quaternion::operator==(const Quaternion& quaternion) const
{
    return (std::abs(m_dW - quaternion.m_dW) < ARITHMETIC_DOUBLE_MICRO &&
            std::abs(m_dX - quaternion.m_dX) < ARITHMETIC_DOUBLE_MICRO &&
            std::abs(m_dY - quaternion.m_dY) < ARITHMETIC_DOUBLE_MICRO &&
            std::abs(m_dZ - quaternion.m_dZ) < ARITHMETIC_DOUBLE_MICRO);
}


/// \brief        overload the inequality operator
///
/// \param[in]    quaternion: the quaternion to compare with
///
/// \return       bool: if the two quaternions unequal to each other
bool Quaternion::operator!=(const Quaternion& quaternion) const
{
    return (std::abs(m_dW - quaternion.m_dW) > ARITHMETIC_DOUBLE_MICRO ||
            std::abs(m_dX - quaternion.m_dX) > ARITHMETIC_DOUBLE_MICRO ||
            std::abs(m_dY - quaternion.m_dY) > ARITHMETIC_DOUBLE_MICRO ||
            std::abs(m_dZ - quaternion.m_dZ) > ARITHMETIC_DOUBLE_MICRO);
}


/// \brief        overload the assign operator
///
/// \param[in]    quaternion: the quaternion to assign from
///
/// \return       Quaternion&: the assigned instance
Quaternion& Quaternion::operator=(const Quaternion& quaternion)
{
    m_dW = quaternion.m_dW;
    m_dX = quaternion.m_dX;
    m_dY = quaternion.m_dY;
    m_dZ = quaternion.m_dZ;

    return *this;
}

END_NAMESPACE
