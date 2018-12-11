#ifndef UIH_ARITHMETIC_PLANE_2D_H_
#define UIH_ARITHMETIC_PLANE_2D_H_

#include "uai_app_defs.h"
#include "uAIArithmetic/arithmetic.h"
#include <memory>

BEGIN_NAMESPACE

// forward declaration
///            m_Origion/---------------------------/m_Point1
///                    /                           /
///                   /                           /
///        m_Point2  /---------------------------/
///
class Plane2D
{
public:
    /// \brief Default Constructor. By default, the plane is centered 
    ///        at the origin of the 2D coordinate,
    ///        with width and height of length 1 and resolutions set to 1.
    Plane2D();
    
    /// \brief Constructor defined an origin point,and  two other points
    ///        that define two axes for the plane.
    Plane2D(const Point2D& myOrigin, 
        const Point2D& myPoint1, 
        const Point2D& myPoint2);

    /// \brief Rotate a plane
    void Rotate(const double& dAngle);

    /// \brief Translate the whole plane
    void Translate(const Vector2D& myVector);
    
    /// \brief Transform the plane through a transform matrix
    void Transform(Matrix3x3 myMatrix);

    /// \brief Set the center of the plane. Works in conjunction with the plane 
    //         normal to position the plane. Don't use this method to define the plane.
    //         Instead, use it to move the plane to a new center point.
    void SetCenter(const Point2D& myCenter);

    /// \brief Get the Center point of the plane
    Point2D GetCenter() const;

    /// \brief Get the four vertices of the plane
    void GetVertices(Point2D *pVertices) const;

     /// \brief overload operator ==. judge whether the two planes are the same plane
     ///        in the computer meaning. 
     /// \note  IsSamePlane
     bool operator == (const Plane2D& myPlane);

     /// \brief overload operator !=
     bool operator != (const Plane2D& myPlane);

     /// \brief overload operator =
     Plane2D& operator = (const Plane2D& myPlane);
private:
    Point2D m_Origion;
    Point2D m_Point1;
    Point2D m_Point2;
};
END_NAMESPACE
#endif
