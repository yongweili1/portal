
#include "uAIArithmetic/arithmetic.h"
#include "uAIArithmetic/plane2d.h"

BEGIN_NAMESPACE

Plane2D::Plane2D()
	:m_Origion(Point2D(-0.5, -0.5)),
	m_Point1(Point2D(0.5, -0.5)),
	m_Point2(Point2D(-0.5, 0.5))
{

}

Plane2D::Plane2D( 
	const Point2D& myOrigin, 
	const Point2D& myPoint1,
	const Point2D& myPoint2 )
	:m_Origion(myOrigin)
	,m_Point1(myPoint1)
	,m_Point2(myPoint2)
{

}

/// \brief Get the four vertices of the plane
///
/// \param[out] pVertices pointer to the four vertices.This pointer  
///             should be maintained by the calling function
///
void Plane2D::GetVertices(Point2D *pVertices) const
{
    if (nullptr == pVertices)
    {
        return;
    }
    Point2D temp = Point2D(0.0,0.0);
    
    Vector2D v1 = m_Point1 - m_Origion;
    Vector2D v2 = m_Point2 - m_Origion;

    temp = m_Origion + ( v1 + v2 );

    *pVertices = m_Origion;
    *(pVertices + 1) = m_Point1;
    *(pVertices + 2) = temp;
    *(pVertices +3) = m_Point2;
}

/// \brief Rotate a plane
void Plane2D::Rotate(const double& dAngle)
{
    Point2D center = GetCenter();

    Vector2D vTranslate = Vector2D(center.x, center.y);
    
    Matrix3x3 myTransform = Matrix3x3::MakeTranslate(-vTranslate);
    myTransform.Prepend(Matrix3x3::MakeCenterRotate(dAngle));
    myTransform.Prepend(Matrix3x3::MakeTranslate(vTranslate));

    m_Origion = myTransform.Transform(m_Origion);
    m_Point1 = myTransform.Transform(m_Point1);
    m_Point2 = myTransform.Transform(m_Point2);
}

/// \brief Translate the whole plane
///
/// \param[in] myVector Translation vector
///
void Plane2D::Translate(const Vector2D& myVector)
{
    m_Origion += myVector;
    m_Point1 += myVector;
    m_Point2 += myVector;
}

/// \brief Set the center of the plane. Works in conjunction with the plane 
///        normal to position the plane. Don't use this method to define the plane.
///        Instead, use it to move the plane to a new center point.
///
/// \param[in] myCenter The new center of the plane
void Plane2D::SetCenter(const Point2D& myCenter)
{
    Point2D previousCenter =  GetCenter();
    Translate(myCenter - previousCenter);
}

/// \brief Transform the plane through a transform matrix
void Plane2D::Transform(Matrix3x3 myMatrix)
{
    m_Origion = myMatrix.Transform(m_Origion);
    m_Point1 = myMatrix.Transform(m_Point1);
    m_Point2 = myMatrix.Transform(m_Point2);
}

/// \brief Get the Center point of the plane
Point2D Plane2D::GetCenter() const
{
    return m_Origion + 
        ((m_Point1 - m_Origion) + (m_Point2 - m_Origion)) * 0.5;
}

/// \brief overload operator ==
bool Plane2D::operator == (const Plane2D& myPlane)
{
    return (m_Origion == myPlane.m_Origion && 
            m_Point1 == myPlane.m_Point1 &&
            m_Point2 == myPlane.m_Point2);
}

/// \brief overload operator !=
bool Plane2D::operator != (const Plane2D& myPlane)
{
    return (m_Origion != myPlane.m_Origion || 
        m_Point1 != myPlane.m_Point1 ||
        m_Point2 != myPlane.m_Point2);
}

/// \brief overload operator =
Plane2D& Plane2D::operator = (const Plane2D& myPlane)
{
    m_Origion = myPlane.m_Origion;
    m_Point1 = myPlane.m_Point1;
    m_Point2 = myPlane.m_Point2;
    return *this;
}

END_NAMESPACE
