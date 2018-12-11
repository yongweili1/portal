#ifndef UIH_ARITHMETIC_POINT2D_H_
#define UIH_ARITHMETIC_POINT2D_H_

#include "uai_app_defs.h"
#include <memory>

BEGIN_NAMESPACE

class Vector2D;

/// \class Point2D
/// \brief Point2D defines the operations of 2D point.
class Point2D
{
public:
    /// \brief default constructor of the class Point2D
    Point2D();

    /// \brief constructor of the class Point2D
    Point2D(double xValue,double yValue);

    /// \brief copy constructor of the class Point2D
    Point2D(const Point2D & myPoint);

    /// \brief destructor of the class Point2D
    ~Point2D();

public:
    /// \brief overload operator += for class Point2D
    Point2D & operator += (const Point2D& v2);

    /// \brief overload operator + for class Point2D
    Point2D   operator + (const Point2D& v2) const;

    /// \brief overload operator += for class Point2D
    Point2D & operator += (const Vector2D& v2);

    /// \brief overload operator + for class Point2D
    Point2D   operator + (const Vector2D& v2) const;

    /// \brief overload operator -= for class Point2D
    Point2D & operator -= (const Vector2D& v2);

    /// \brief overload operator - for class Point2D
    Vector2D   operator - (const Point2D& v2) const;

    /// \brief Each component multiplies a scalar, overload *= operator 
    Point2D & operator *= ( double dScale);

    /// \brief Each component multiplies a scalar, overload *= operator
    Point2D operator * ( double dScale) const;

    /// \brief overload operator = for class Point2D
    Point2D & operator = (const Point2D& v2);

    /// \brief overload operator != for class Point2D
    bool operator != (const Point2D & v2) const;

    /// \brief overload operator == for class Point2D
    bool operator == (const Point2D & v2) const;
public:
    double x;
    double y;
};

END_NAMESPACE

#endif
