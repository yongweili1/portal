#ifndef UIH_ARITHMETIC_PLANE_H_
#define UIH_ARITHMETIC_PLANE_H_

#include "uai_app_defs.h"
#include "uAIArithmetic/point3d.h"
#include <memory>

BEGIN_NAMESPACE

// forward declaration
/// \class MedViewer3DPlane represents the geometry of a finite plane in 3D space.
/// \brief The plane is defined by specifying an origin point,and then two
//   other points that, together with the origin, define two axes for the 
//   plane. These axes do not have to be orthogonal, so you can create
//   a parallelogram.
///  To Rotate the plane, you can call Rotate(const Quaternion& myQuaternion) or
///  reset the normal using SetNormal(const Vector3D& myNoraml).
///            m_Origion /---------------------------/ m_Point1
///                     /                           /
///                    /                           /
///        m_Point2   /---------------------------/
///
class Plane
{
public:
    /// \brief Default Constructor. By default, the plane is centered 
    ///        at the origin of the 3D coordinate, and perpendicular to the
    ///        z-axis, with width and height of length 1 and resolutions set to 1.
    Plane();
    
    /// \brief Constructor defined an origin point,and  two other points
    ///        that define two axes for the plane.
    Plane(const Point3D& myOrigin, 
        const Point3D& myPoint1, 
        const Point3D& myPoint2);

    /// \brief Rotate a plane
    void Rotate(const Quaternion& myQuaternion);

    /// \brief Translate the whole plane
    void Translate(const Vector3D& myVector);
    
    /// \brief Transform the plane through a transform matrix
    void Transform(Matrix4x4 myMatrix);

    /// \brief Set the new normal to plane. Transform the vertices according to 
    ///         the new input normal vector
    void SetNormal(Vector3D& myNormal);
    
    /// \brief Set the center of the plane. Works in conjunction with the plane 
    //         normal to position the plane. Don't use this method to define the plane.
    //         Instead, use it to move the plane to a new center point.
    void SetCenter(const Point3D& myCenter);

    /// \brief Get the plane's normal
    Vector3D GetNormal() const;

    /// \brief Get the Center point of the plane
    Point3D GetCenter() const;

    /// \brief Get the four vertices of the plane
    void GetVertices(Point3D *pVertices) const;

    /// \brief Test whether the vector and the plane intersects
    bool Intersect(const Vector3D& myVector, Point3D &IntersectPoint);

     /// \brief Test whether intersects with a plane 
    bool Intersect(const Plane &myPlane, 
         Point3D &myPoint1,
         Point3D &myPoint2);

    /// \brief Test whether a point is on a plane
     bool IsOnPlane(const Point3D &myPoint);
     bool IsOnPlane(const Point3D& point, const double dTolerance);

     /// calculate the distance from a point to plane
     double DistancePointToPlane(const Point3D& point);
     double DistancePointToPlane(const Point3D& point) const;

     /// judge whether the two planes are the same plane in the mathematics meaning.
     bool IsSamePlane(const Plane& plane, double dTolerance = 1e-5);

     /// \brief overload operator ==. judge whether the two planes are the same plane
     ///        in the computer meaning. 
     /// \note  IsSamePlane
     bool operator == (const Plane& myPlane);

     /// \brief overload operator !=
     bool operator != (const Plane& myPlane);

     /// \brief overload operator =
     Plane& operator = (const Plane& myPlane);
protected:
    double DistancePointToPlane_i(const Point3D& point) const;

private:
    Point3D m_Origion;
    Point3D m_Point1;
    Point3D m_Point2;
};

double DistancePointToPlane(const Point3D& point, const Plane& plane);

END_NAMESPACE
#endif
