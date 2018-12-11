
#ifndef UIH_ARITHMETIC_MATRIX_4X4_H_
#define UIH_ARITHMETIC_MATRIX_4X4_H_

#include "uai_app_defs.h"
#include <memory>

BEGIN_NAMESPACE

// forward declaration
class Quaternion;
class Vector3D;
class Vector4D;
class Point3D;

/// \class Matrix4x4 is a class that encapsulates a standard 4x4 homogeneous matrix
/// \brief OpenGL's matrix is column-major while c++ array is row-major. To keep consistent
///        with openGL, Matrix4x4 maintains a 16-value array with base vectors laid out 
///        contiguously in memory and the internal layout is like m[col][row]. The translation 
///        components occupy  the 13th, 14th, and 15th(m[3][1],m[3][2],m[3][3]) elements of 
///        the 16-element matrix. 
/// 
///        When applying matrix multiplications, column vectors are used.  If vector V  is to
///        be transformed by M1 then M2 then M3, the calculation would 
///        be M3 * M2 * M1 * V.
///        The generic form M * P which shows the layout of the matrix entries is shown below:
///                         [ m[0][0]  m[1][0]  m[2][0]  m[3][0] ]   {x}
///                         | m[0][1]  m[1][1]  m[2][1]  m[3][1] | * {y}
///                         | m[0][2]  m[1][2]  m[2][2]  m[3][2] |   {z}
///                         [ m[0][3]  m[1][3]  m[2][3]  m[3][3] ]   {1}
class Matrix4x4
{
public:
    /// \brief Default constructor
    Matrix4x4();

    /// \brief Constructor with 16 column-major matrix elements 
    Matrix4x4(double m00, double m01, double m02, double m03, 
                          double m10, double m11, double m12, double m13, 
                          double m20, double m21, double m22, double m23, 
                          double m30, double m31, double m32, double m33);

    Matrix4x4(Vector4D col0,Vector4D col1,Vector4D col2,Vector4D col3);

    /// \brief Creates a standard 4x4 transformation matrix with 
    ///        a zero translation part from a rotation/scaling
    Matrix4x4(const Quaternion&);

public:

    /// \brief make an identity matrix
    void SetIdintity();

    /// \brief Prepend a matrix
    void Prepend(const Matrix4x4 &myMatrix);

    /// \brief Append a matrix
    void Append(const Matrix4x4 &myMatrix);

    /// \brief Transpose the matrix
    void Transpose();

    /// \brief Test whether the matrix has inverse matrix
    bool HasInverse() const;

    void GetElements(double* m);
    void GetVector4D(Vector4D& col0,Vector4D& col1,Vector4D& col2,Vector4D& col3);

    /// \brief Get the inverse of a matrix. Before calling this function, MUST call 
    ///        bool HasInverse() to make sure the inverse exists.
    Matrix4x4 Inverse() const;

    /// \brief Transform a point through the transform matrix
    Point3D Transform(const Point3D &myPoint) const;

    /// \brief Transform a point array through the transform matrix
    void Transform(Point3D *myPoint, int iSize) const;

    /// \brief Transform a vector through the transform matrix
    Vector3D Transform(const Vector3D &myVector) const;

    ///// \brief Decompose a Matrix4 to orientation / scale / origin.
    //void Decomposition(Vector3D &myOrigin, Vector3D &myScale, Quaternion &myQuaternion) const;

    /// \brief Extracts the translation transformation part of the matrix. 
    void ExtractTranslate(Vector3D &) const;

    /// \brief Calculate the determinant of the 4X4 matrix
    double Determinant() const;

    /// \brief Get ajoint matrix
    Matrix4x4 GetAdjoint() const;

    /// \brief Get transpose of a matrix
    Matrix4x4 GetTranspose() const;

    /// \brief Matrix concatenation using '*'. Overload operator *
    Matrix4x4 operator * (const Matrix4x4& myMatrix) const;

    /// \brief Vector transformation using '*'.Overload operator *
    Vector3D operator * (const Vector3D& myVector) const;

    /// \brief Point transformation using '*'.Overload operator *
    Point3D operator * (const Point3D& myPoint) const;

    /// \brief Matrix multiply a scalar using '*'.Overload operator *
    Matrix4x4 operator * (const double dscale) const;

    /// \brief Matrix addition.Overload operator +
    Matrix4x4 operator + (const Matrix4x4& myMatrix) const;

    /// \brief Matrix subtraction. Overload operator -
    Matrix4x4 operator - (const Matrix4x4& myMatrix) const;

    /// \brief Tests 2 matrices for equality. Overload operator ==
    bool operator == (const Matrix4x4& myMatrix) const;

    /// \brief Tests 2 matrices for inequality. Overload operator ==
    bool operator != (const Matrix4x4& myMatrix) const;

    /// \brief Assignment of a matrix. Overload operator =
    Matrix4x4 & operator = (const Matrix4x4& myMatrix);

    /// \brief overload operator []
    double* operator[] (unsigned int  iRow);

    /// \brief overload operator []
    const double *operator[] (unsigned int iRow) const;

    /// \brief Check whether or not the matrix is affine matrix. 
    bool IsAffine() const;

private:
    
    /// \fn double Matrix4x4::minor(int r0, int r1,  int r2, int c0, int c1, int c2) const
    ///
    /// \brief Calculate minor determinant of specified rows and columns
     double minor_mx4(int, int, int, int, int, int) const;

public:
    union {
             double m[4][4];
             double _m[16];
          };
};
/// \brief Builds a scaling matrix
Matrix4x4 MakeScale(const Vector3D& myScale);

/// \brief Builds a translation matrix
Matrix4x4 MakeTranslate(const Vector3D& myTranslate);

/// \brief Builds a matrix through Quaternion
Matrix4x4 MakeQuaternion(const Quaternion& myQuaternion);

END_NAMESPACE

#endif
