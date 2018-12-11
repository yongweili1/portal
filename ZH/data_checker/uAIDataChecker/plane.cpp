
#include "uAIArithmetic/arithmetic.h"
#include "uAIArithmetic/plane.h"
#include <limits> 

BEGIN_NAMESPACE

/// \brief Default Constructor. By default, the plane is centered 
///        at the origin of the 3D coordinate, and perpendicular to the
///        z-axis, with width and height of length 1 and resolutions set to 1.
///
Plane::Plane()
:m_Origion(Point3D(-0.5, -0.5, 0.0)),
 m_Point1(Point3D(0.5, -0.5, 0.0)),
 m_Point2(Point3D(-0.5, 0.5, 0.0))
{
}

/// \brief Constructor defined an origin point,and  two other points
///        that define two axes for the plane.
///
/// \param[in] myOrigion Origin point
/// \param[in] myPoint1 The first vertex
/// \param[in] myPoint2 The second vertex
///
Plane::Plane(const Point3D& myOrigin, 
    const Point3D& myPoint1, 
    const Point3D& myPoint2)
    :m_Origion(myOrigin),m_Point1(myPoint1),m_Point2(myPoint2)
{
}

/// \brief Get the four vertices of the plane
///
/// \param[out] pVertices pointer to the four vertices.This pointer  
///             should be maintained by the calling function
///
void Plane::GetVertices(Point3D *pVertices) const
{
    if (nullptr == pVertices)
    {
        return;
    }
    Point3D temp = Point3D(0.0,0.0,0.0);
    
    Vector3D v1 = m_Point1 - m_Origion;
    Vector3D v2 = m_Point2 - m_Origion;

    temp = m_Origion + ( v1 + v2 );

    *pVertices = m_Origion;
    *(pVertices + 1) = m_Point1;
    *(pVertices + 2) = temp;
    *(pVertices +3) = m_Point2;
}

/// \brief Rotate a plane
///
/// \param[in] myQuaternion Rotate quaternion
///
void Plane::Rotate(const Quaternion& myQuaternion)
{
    Point3D center = GetCenter();

    Vector3D vTranslate = Vector3D(center.x, center.y, center.z);
    
    Matrix4x4 myTransform = MakeTranslate(-vTranslate);
    myTransform.Prepend(Matrix4x4(myQuaternion));
    myTransform.Prepend(MakeTranslate(vTranslate));

    m_Origion = myTransform.Transform(m_Origion);
    m_Point1 = myTransform.Transform(m_Point1);
    m_Point2 = myTransform.Transform(m_Point2);
}

/// \brief Translate the whole plane
///
/// \param[in] myVector Translation vector
///
void Plane::Translate(const Vector3D& myVector)
{
    m_Origion += myVector;
    m_Point1 += myVector;
    m_Point2 += myVector;
}


/// \brief Set the new normal to plane. Transform the vertices according to 
///         the new input normal vector
///
/// \param[in] myNormal Input normal
void Plane::SetNormal(Vector3D& myNormal)
{
    if (!myNormal.Normalize())
    {
        return;
    }

    Vector3D thisNormal = GetNormal();
    double dp = thisNormal.DotProduct(myNormal);

    double theta = 0.0 ;
    Vector3D myRotateVec(0.0,1.0,0.0);

    if (dp > 0.9999)
    {
        return; // zero rotation
    }
    else if (dp < -0.9999)
    {
        theta = UIH_ARITHMETIC_PI;
        myRotateVec = m_Point1 - m_Origion;
    }
    else
    {
        theta = AngleBetween(thisNormal, myNormal);
        myRotateVec = CrossProduct(thisNormal, myNormal);
    }
    Rotate(Quaternion(theta,myRotateVec));
}

/// \brief Set the center of the plane. Works in conjunction with the plane 
///        normal to position the plane. Don't use this method to define the plane.
///        Instead, use it to move the plane to a new center point.
///
/// \param[in] myCenter The new center of the plane
void Plane::SetCenter(const Point3D& myCenter)
{
    Point3D previousCenter =  GetCenter();
    Translate(myCenter - previousCenter);
}

/// \brief Transform the plane through a transform matrix
void Plane::Transform(Matrix4x4 myMatrix)
{
    m_Origion = myMatrix.Transform(m_Origion);
    m_Point1 = myMatrix.Transform(m_Point1);
    m_Point2 = myMatrix.Transform(m_Point2);
}

/// \brief Get the plane's normal
Vector3D Plane::GetNormal() const
{
    Vector3D axes1 = m_Point1 - m_Origion;
    Vector3D axes2 = m_Point2 - m_Origion;
    
    Vector3D myVector = CrossProduct(axes1,axes2);
    myVector.Normalize();
    return myVector;
}

/// \brief Get the Center point of the plane
Point3D Plane::GetCenter() const
{
    return m_Origion + 
        ((m_Point1 - m_Origion) + (m_Point2 - m_Origion)) * 0.5;
}

/// \brief Test whether the vector and the plane intersects
/// 
/// \param[in] myVector Test vector
/// \param[out] IntersectPoint if has an intersection, record the intersect point
///
/// \return true if the plane has an intersection with the vector, or return false
bool Plane::Intersect(const Vector3D& /*myVector*/, Point3D & /*IntersectPoint*/)
{
    /// \todo TODO(wenqing.liu@united-imaging.com): Intersect with a vector
    return false;
}

/// \brief Test whether intersects with a plane  
///
/// \param[in] myPlane Test plane
/// \param[out] myPoint1,myPoint2 represent two endpoints of a line
///
/// \return true if the plane has an intersection with the test, 
///         test plane, or return false.
bool Plane::Intersect(const Plane &/*myPlane*/, 
         Point3D &/*myPoint1*/,
         Point3D &/*myPoint2*/)
{
    /// \todo TODO(wenqing.liu@united-imaging.com): Intersect with a plane
    return false;
}

/// \brief Test whether a point is on a plane
///
/// param[in] myPoint Test point
///
/// \return true if the point is on the plane,
///         or return false 
bool Plane::IsOnPlane(const Point3D & myPoint)
{
    return this->IsOnPlane(myPoint, 1e-5);
}

bool Plane::IsOnPlane( const Point3D& point, const double dTolerance )
{
    double distance = this->DistancePointToPlane(point);
    if (std::fabs(distance) < dTolerance)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/// \brief overload operator ==
bool Plane::operator == (const Plane& myPlane)
{
    return (m_Origion == myPlane.m_Origion && 
            m_Point1 == myPlane.m_Point1 &&
            m_Point2 == myPlane.m_Point2);
}

/// \brief overload operator !=
bool Plane::operator != (const Plane& myPlane)
{
    return (m_Origion != myPlane.m_Origion || 
        m_Point1 != myPlane.m_Point1 ||
        m_Point2 != myPlane.m_Point2);
}

/// \brief overload operator =
Plane& Plane::operator = (const Plane& myPlane)
{
    m_Origion = myPlane.m_Origion;
    m_Point1 = myPlane.m_Point1;
    m_Point2 = myPlane.m_Point2;
    return *this;
}

double Plane::DistancePointToPlane( const Point3D& point )
{
    return this->DistancePointToPlane_i(point);
}

double Plane::DistancePointToPlane( const Point3D& point ) const
{
    return this->DistancePointToPlane_i(point);
}

bool Plane::IsSamePlane( const Plane& plane, double dTolerance /*= 1e-5*/ )
{
    Vector3D normal1 =  this->GetNormal();
    Vector3D normal2 = plane.GetNormal();
    if (!normal2.IsParallel2(normal1))
    {
        return false;
    }

    // 关于原点对称的两个平面，原点到平面的距离是相同的
    //double distanceOrigin1 = this->DistancePointToPlane(Point3D(0.0, 0.0, 0.0));
    //double distanceOrigin2 = plane.DistancePointToPlane(Point3D(0.0, 0.0, 0.0));

    //if (std::fabs(distanceOrigin1 - distanceOrigin2) > dTolerance)
    //{
    //    return false;
    //}

    // 两个平面中心点的向量点乘法向量，若为0时是平行的
    Vector3D myDis = this->GetCenter() - plane.GetCenter();

    if (std::abs(normal1.DotProduct(myDis)) > dTolerance)
    {
        return false;
    }

    return true;
}

double Plane::DistancePointToPlane_i( const Point3D& point ) const
{
    Vector3D edge1 = m_Point1 - m_Origion;
    Vector3D edge2 = m_Point2 - m_Origion;

    if (edge1.IsParallel2(edge2))
    {
        return std::numeric_limits<double>::min();
    }

    Vector3D normal = CrossProduct(edge1, edge2);
    double dAbsolute = std::fabs(normal.DotProduct(Vector3D(point - m_Origion)));
    double distance = dAbsolute / normal.Magnitude();

    return distance;
}

double DistancePointToPlane( const Point3D& point, const Plane& plane )
{
    return plane.DistancePointToPlane(point);
}

END_NAMESPACE
