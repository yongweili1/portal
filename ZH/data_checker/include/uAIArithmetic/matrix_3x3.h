
#ifndef UIH_ARITHMETIC_MATRIX_3X3_H_
#define UIH_ARITHMETIC_MATRIX_3X3_H_

#include "uai_app_defs.h"
#include <memory>

BEGIN_NAMESPACE

// forward declaration
class Vector2D;
class Point2D;

/// \class Matrix3x3 is a class that encapsulates a standard 3x3 homogeneous matrix
/// \brief Matrix3x3 maintains a 9-value array with base vectors laid out 
///        contiguously in memory and the internal layout is like m[col][row]. 
///		   The translation components occupy  the 7th, 8th(m[2][0],m[2][1]) elements of 
///        the 9-element matrix. 
/// 
///        When applying matrix multiplications, column vectors are used.  If vector V  is to
///        be transformed by M1 then M2 then M3, the calculation would 
///        be M3 * M2 * M1 * V.
///        The generic form M * P which shows the layout of the matrix entries is shown below:
///                         [ m[0][0]  m[1][0]  m[2][0] ]   {x}
///                         | m[0][1]  m[1][1]  m[2][1] | * {y}
///                         | m[0][2]  m[1][2]  m[2][2] ]   {1}
class Matrix3x3
{
public:
    /// \brief Default constructor
    Matrix3x3();

    /// \brief Constructor with 16 column-major matrix elements 
    Matrix3x3(double m00, double m01, double m02, 
              double m10, double m11, double m12, 
              double m20, double m21, double m22);
public:

    /// \brief make an identity matrix
    void SetIdintity();

    /// \brief Prepend a matrix
    void Prepend(const Matrix3x3 &myMatrix);

    /// \brief Append a matrix
    void Append(const Matrix3x3 &myMatrix);

    /// \brief Transpose the matrix
    void Transpose();

    /// \brief Test whether the matrix has inverse matrix
    bool HasInverse() const;

    /// \brief Get the inverse of a matrix. Before calling this function, MUST call 
    ///        bool HasInverse() to make sure the inverse exists.
    Matrix3x3 Inverse() const;

    /// \brief Transform a point through the transform matrix
    Point2D Transform(const Point2D &myPoint) const;

    /// \brief Transform a point array through the transform matrix
    void Transform(Point2D *myPoint, int iSize) const;

    /// \brief Transform a vector through the transform matrix
    Vector2D Transform(const Vector2D &myVector) const;

    /// \brief Extracts the translation transformation part of the matrix. 
    void ExtractTranslate(Vector2D &) const;

    /// \brief Calculate the determinant of the 3X3 matrix
    double Determinant() const;

    /// \brief Get ajoint matrix
    Matrix3x3 GetAdjoint() const;

    /// \brief Get transpose of a matrix
    Matrix3x3 GetTranspose() const;

    /// \brief Matrix concatenation using '*'. Overload operator *
    Matrix3x3 operator * (const Matrix3x3& myMatrix) const;

    /// \brief Vector transformation using '*'.Overload operator *
    Vector2D operator * (const Vector2D& myVector) const;

    /// \brief Point transformation using '*'.Overload operator *
    Point2D operator * (const Point2D& myPoint) const;

    /// \brief Matrix multiply a scalar using '*'.Overload operator *
    Matrix3x3 operator * (const double dscale) const;

    /// \brief Matrix addition.Overload operator +
    Matrix3x3 operator + (const Matrix3x3& myMatrix) const;

    /// \brief Matrix subtraction. Overload operator -
    Matrix3x3 operator - (const Matrix3x3& myMatrix) const;

    /// \brief Tests 2 matrices for equality. Overload operator ==
    bool operator == (const Matrix3x3& myMatrix) const;

    /// \brief Tests 2 matrices for inequality. Overload operator ==
    bool operator != (const Matrix3x3& myMatrix) const;

    /// \brief Assignment of a matrix. Overload operator =
    Matrix3x3 & operator = (const Matrix3x3& myMatrix);

    /// \brief overload operator []
     double* operator [] (unsigned int iRow);

    /// \brief overload operator []
     const double *operator [] (unsigned int iRow) const;

    /// \brief Check whether or not the matrix is affine matrix. 
    bool IsAffine() const;

    /// \brief Builds a scaling matrix
    static Matrix3x3 MakeScale(const Vector2D& myScale);
    
    /// \brief Builds a translation matrix
    static Matrix3x3 MakeTranslate(const Vector2D& myTranslate);

	/// \brief Builds a translation matrix
	static Matrix3x3 MakeCenterRotate(const double& myAngle);
private:
    
    /// \fn double Matrix4x4::minor(int r0, int r1, int c0, int c1) const
    ///
    /// \brief Calculate minor determinant of specified rows and columns
     double minor_mx3(int, int, int, int) const;

public:
    union {
             double m[3][3];
             double _m[9];
          };
};
END_NAMESPACE

#endif
