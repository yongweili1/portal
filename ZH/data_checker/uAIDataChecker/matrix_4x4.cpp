#include "uAIArithmetic/arithmetic.h"
#include "uAIArithmetic/matrix_4x4.h"
#include <cmath>
#include <string.h>

BEGIN_NAMESPACE

/// \brief Default Constructor
Matrix4x4::Matrix4x4()
{
}

/// \brief the order is column-major
Matrix4x4::Matrix4x4(double m00, double m01, double m02, double m03, 
        double m10, double m11, double m12, double m13, 
        double m20, double m21, double m22, double m23, 
        double m30, double m31, double m32, double m33)
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[0][3] = m03;
    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[1][3] = m13;
    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
    m[2][3] = m23;
    m[3][0] = m30;
    m[3][1] = m31;
    m[3][2] = m32;
    m[3][3] = m33;
}


Matrix4x4::Matrix4x4(Vector4D col0,Vector4D col1,Vector4D col2,Vector4D col3)
{
    _m[0] = col0.x;
    _m[1] = col0.y;
    _m[2] = col0.z;
    _m[3] = col0.w;

    _m[4] = col1.x;
    _m[5] = col1.y;
    _m[6] = col1.z;
    _m[7] = col1.w;

    _m[8] = col2.x;
    _m[9] = col2.y;
    _m[10] = col2.z;
    _m[11] = col2.w;

    _m[12] = col3.x;
    _m[13] = col3.y;
    _m[14] = col3.z;
    _m[15] = col3.w;
}

void Matrix4x4::GetVector4D(Vector4D& col0,Vector4D& col1,Vector4D& col2,Vector4D& col3)
{
    col0.x  =   _m[0] ;
    col0.y  =   _m[1] ;
    col0.z  =   _m[2] ;
    col0.w  =   _m[3] ;

    col1.x  =   _m[4] ;
    col1.y  =   _m[5] ;
    col1.z  =   _m[6] ;
    col1.w  =   _m[7] ;

    col2.x  =   _m[8] ;
    col2.y  =   _m[9] ;
    col2.z  =   _m[10];
    col2.w  =   _m[11];

    col3.x  =   _m[12];
    col3.y  =   _m[13];
    col3.z  =   _m[14];
    col3.w  =   _m[15];
}

/// \brief Creates a standard 4x4 transformation matrix with 
///        a zero translation part from a rotation/scaling
Matrix4x4::Matrix4x4(const Quaternion &myQuaternion)
{
    *this = myQuaternion.ToMatrix();
}


/// \brief make an identity matrix
void Matrix4x4::SetIdintity()
{
    m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = 0.0;
    m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = 0.0;
    m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = 0.0;
    m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
}


/// \brief Prepend a matrix
///
/// \param[in] myMatrix The matrix to be multiplied with current matrix
void Matrix4x4::Prepend(const Matrix4x4 &myMatrix)
{
    *this = myMatrix * (*this);
}

/// \brief Append a matrix
///
/// \param[in] myMatrix The matrix to be appended
void Matrix4x4::Append(const Matrix4x4 &myMatrix)
{
    *this = (*this) * myMatrix;
}


/// \brief Transpose the matrix
void Matrix4x4::Transpose()
{
    std::swap(_m[1], _m[4]);
    std::swap(_m[2], _m[8]);
    std::swap(_m[3], _m[12]);
    std::swap(_m[6], _m[9]);
    std::swap(_m[7], _m[13]);
    std::swap(_m[11], _m[14]);
}


/// \brief Get transpose of a matrix
///
/// \return The matrix's transpose
Matrix4x4 Matrix4x4::GetTranspose() const
{
    return Matrix4x4(_m[0], _m[4], _m[8], _m[12],
                                _m[1], _m[5], _m[9], _m[13],
                                _m[2], _m[6], _m[10], _m[14],
                                _m[3], _m[7], _m[11], _m[15]);
}


/// \brief Test whether the matrix has inverse matrix
///
/// \return true if has inverse ,or return false
bool Matrix4x4::HasInverse() const
{
    return (std::fabs(Determinant()) > UIH_ARITHMETIC_DOUBLE_ZERO);
    
}


/// \brief Get the inverse of a matrix. Before calling this function, MUST call 
///        bool HasInverse() to make sure the inverse exists.
///
/// \return The inverse of a matrix
Matrix4x4 Matrix4x4::Inverse() const
{
    return GetAdjoint() * (1.0 / Determinant());
}


/// \brief Check whether or not the matrix is affine matrix.
/// 
/// \return true if is an affine matrix, or return false
bool Matrix4x4::IsAffine() const
{
    return (std::fabs(_m[3]) < ARITHMETIC_DOUBLE_MICRO && 
        std::fabs(_m[7]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[11]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[15]-1.0) < ARITHMETIC_DOUBLE_MICRO);
}


/// \brief Transform a point through the transform matrix
///
/// \param[in] myPoint The point to be transformed
///
/// \return Transformed point
Point3D Matrix4x4::Transform(const Point3D &myPoint) const
{
    return (*this) * myPoint;
}

/// \brief Transform a point array through the transform matrix
void Matrix4x4::Transform(Point3D *myPoint, int iSize) const
{
    if (nullptr == myPoint )
    {
        return;
    }
    for (int i=0;i<iSize;i++)
    {
        myPoint[i] = (*this) * myPoint[i];
    }
}

/// \brief Transform a vector through the transform matrix
///
/// \param[in] myVector The vector to be transformed
///
/// \return Transformed vector
Vector3D Matrix4x4::Transform(const Vector3D & myVector) const
{
    return (*this) * myVector;
}


/// \brief Calculate the determinant of the 4X4 matrix
///
/// \return The determinant of the matrix
double Matrix4x4::Determinant() const
{
    return m[0][0] * minor_mx4(1, 2, 3, 1, 2, 3) -
           m[0][1] * minor_mx4(1, 2, 3, 0, 2, 3) +
           m[0][2] * minor_mx4(1, 2, 3, 0, 1, 3) -
           m[0][3] * minor_mx4(1, 2, 3, 0, 1, 2);
}


/// \brief Builds a scaling matrix
///
/// \param[in] myScale scaling vector
///
/// \return The scaling matrix
Matrix4x4 MakeScale(const Vector3D & myScale)
{
    return Matrix4x4(myScale.x, 0.0, 0.0, 0.0 ,
                                 0.0, myScale.y, 0.0, 0.0 ,
                                 0.0, 0.0, myScale.z, 0.0,
                                 0.0, 0.0, 0.0, 1.0);

}

/// \brief Builds a translation matrix
///
/// \param[in] myTranslate Translation vector
///
/// \return Translation matrix
Matrix4x4 MakeTranslate(const Vector3D &myTranslate)
{
    return Matrix4x4(1.0, 0.0, 0.0, 0.0 ,
        0.0, 1.0, 0.0, 0.0 ,
        0.0, 0.0, 1.0, 0.0,
        myTranslate.x, myTranslate.y, myTranslate.z, 1.0);
}


/// \brief Builds a matrix through Quaternion
///
/// \param[in] myQuaternion The input Quaternion
///
/// \return matrix built through Quaternion
Matrix4x4 MakeQuaternion(const Quaternion &myQuaternion)
{
    return myQuaternion.ToMatrix();
}

///// \brief Decompose a Matrix4 to orientation / scale / origin.
//void Matrix4x4::Decomposition(Vector3D &myOrigin, 
//                                          Vector3D &myScale, 
//                                          Quaternion &myQuaternion) const
//{
//}


/// \brief Extracts the translation transformation part of the matrix.
///
/// \param[out] myVector The translation part of the matrix
void Matrix4x4::ExtractTranslate(Vector3D &myVector) const
{
    myVector.x = _m[12];
    myVector.y = _m[13];
    myVector.z = _m[14];

}


/// \brief Matrix concatenation using '*'. Overload operator *
///
/// \param[in] m2 The matrix to be appended with
///
/// \return The result matrix
Matrix4x4 Matrix4x4::operator * (const Matrix4x4 & m2) const
{
    return Matrix4x4(
        _m[0] * m2._m[0] + _m[4] * m2._m[1] + _m[8] * m2._m[2] + _m[12] * m2._m[3],
        _m[1] * m2._m[0] + _m[5] * m2._m[1] + _m[9] * m2._m[2] + _m[13] * m2._m[3],
        _m[2] * m2._m[0] + _m[6] * m2._m[1] + _m[10] * m2._m[2] + _m[14] * m2._m[3],
        _m[3] * m2._m[0] + _m[7] * m2._m[1] + _m[11] * m2._m[2] + _m[15] * m2._m[3],
        _m[0] * m2._m[4] + _m[4] * m2._m[5] + _m[8] * m2._m[6] + _m[12] * m2._m[7],
        _m[1] * m2._m[4] + _m[5] * m2._m[5] + _m[9] * m2._m[6] + _m[13] * m2._m[7],
        _m[2] * m2._m[4] + _m[6] * m2._m[5] + _m[10] * m2._m[6] + _m[14] * m2._m[7],
        _m[3] * m2._m[4] + _m[7] * m2._m[5] + _m[11] * m2._m[6] + _m[15] * m2._m[7],
        _m[0] * m2._m[8] + _m[4] * m2._m[9] + _m[8] * m2._m[10] + _m[12] * m2._m[11],
        _m[1] * m2._m[8] + _m[5] * m2._m[9] + _m[9] * m2._m[10] + _m[13] * m2._m[11],
        _m[2] * m2._m[8] + _m[6] * m2._m[9] + _m[10] * m2._m[10] + _m[14] * m2._m[11],
        _m[3] * m2._m[8] + _m[7] * m2._m[9] + _m[11] * m2._m[10] + _m[15] * m2._m[11],
        _m[0] * m2._m[12] + _m[4] * m2._m[13] + _m[8] * m2._m[14] + _m[12] * m2._m[15],
        _m[1] * m2._m[12] + _m[5] * m2._m[13] + _m[9] * m2._m[14] + _m[13] * m2._m[15],
        _m[2] * m2._m[12] + _m[6] * m2._m[13] + _m[10] * m2._m[14] + _m[14] * m2._m[15],
        _m[3] * m2._m[12] + _m[7] * m2._m[13] + _m[11] * m2._m[14] + _m[15] * m2._m[15]
    );
}


/// \brief Vector transformation using '*'.Overload operator *
///
/// \param[in] myVector Input vector
///
/// \return Transformed vector
Vector3D Matrix4x4::operator * (const Vector3D &myVector) const
{
    return  Vector3D(_m[0] * myVector.x + _m[4] * myVector.y + _m[8] * myVector.z,
                               _m[1] * myVector.x + _m[5] * myVector.y + _m[9] * myVector.z,
                               _m[2] * myVector.x + _m[6] * myVector.y + _m[10] * myVector.z);
}


/// \brief Point transformation using '*'.Overload operator *
///
/// \param[in] myPoint Input point
/// 
/// \return Transformed Vector
Point3D Matrix4x4::operator * (const Point3D& myPoint) const
{
    double dw = _m[3] * myPoint.x + _m[7] * myPoint.y + _m[11] * myPoint.z + _m[15];
    dw = fabs(dw) > UIH_ARITHMETIC_DOUBLE_ZERO? dw: UIH_ARITHMETIC_DOUBLE_ZERO;
    dw = 1.0 / dw;
    return  Point3D(
        (_m[0] * myPoint.x + _m[4] * myPoint.y + _m[8] * myPoint.z + _m[12]) * dw,
        (_m[1] * myPoint.x + _m[5] * myPoint.y + _m[9] * myPoint.z + _m[13]) * dw,
        (_m[2] * myPoint.x + _m[6] * myPoint.y + _m[10] * myPoint.z + _m[14]) * dw);
}


/// \brief Matrix multiply a scalar using '*'.Overload operator *
///        Each element of the matrix multiply with a scale
///
/// \param[in] dscale The scaling factor
///
///\ return  Result matrix
Matrix4x4 Matrix4x4::operator * (const double dscale) const
{
    return Matrix4x4(
        dscale * _m[0], dscale * _m[1], dscale * _m[2], dscale *_m[3],
        dscale * _m[4], dscale * _m[5], dscale * _m[6], dscale *_m[7],
        dscale * _m[8], dscale * _m[9], dscale * _m[10], dscale *_m[11],
        dscale * _m[12], dscale * _m[13], dscale * _m[14], dscale *_m[15]
        );
}


/// \brief Matrix addition.Overload operator +
///
/// \param[in] myMatrix The matrix to be added
///
/// \return Result matrix
Matrix4x4 Matrix4x4::operator + (const Matrix4x4& myMatrix) const
{
    Matrix4x4 myResult;
    myResult._m[0] = _m[0] + myMatrix._m[0];
    myResult._m[1] = _m[1] + myMatrix._m[1];
    myResult._m[2] = _m[2] + myMatrix._m[2];
    myResult._m[3] = _m[3] + myMatrix._m[3];

    myResult._m[4] = _m[4] + myMatrix._m[4];
    myResult._m[5] = _m[5] + myMatrix._m[5];
    myResult._m[6] = _m[6] + myMatrix._m[6];
    myResult._m[7] = _m[7] + myMatrix._m[7];

    myResult._m[8] = _m[8] + myMatrix._m[8];
    myResult._m[9] = _m[9] + myMatrix._m[9];
    myResult._m[10] = _m[10] + myMatrix._m[10];
    myResult._m[11] = _m[11] + myMatrix._m[11];

    myResult._m[12] = _m[12] + myMatrix._m[12];
    myResult._m[13] = _m[13] + myMatrix._m[13];
    myResult._m[14] = _m[14] + myMatrix._m[14];
    myResult._m[15] = _m[15] + myMatrix._m[15];
    return myResult;
}


/// \brief Matrix subtraction. Overload operator -
///
/// \param[in] myMatrix The matrix to be subtracted
///
/// \return Result matrix
Matrix4x4 Matrix4x4::operator - (const Matrix4x4& myMatrix) const
{
    Matrix4x4 myResult;
    myResult._m[0] = _m[0] - myMatrix._m[0];
    myResult._m[1] = _m[1] - myMatrix._m[1];
    myResult._m[2] = _m[2] - myMatrix._m[2];
    myResult._m[3] = _m[3] - myMatrix._m[3];

    myResult._m[4] = _m[4] - myMatrix._m[4];
    myResult._m[5] = _m[5] - myMatrix._m[5];
    myResult._m[6] = _m[6] - myMatrix._m[6];
    myResult._m[7] = _m[7] - myMatrix._m[7];

    myResult._m[8] = _m[8] - myMatrix._m[8];
    myResult._m[9] = _m[9] - myMatrix._m[9];
    myResult._m[10] = _m[10] - myMatrix._m[10];
    myResult._m[11] = _m[11] - myMatrix._m[11];

    myResult._m[12] = _m[12] - myMatrix._m[12];
    myResult._m[13] = _m[13] - myMatrix._m[13];
    myResult._m[14] = _m[14] - myMatrix._m[14];
    myResult._m[15] = _m[15] - myMatrix._m[15];
    return myResult;
}


/// \brief Tests 2 matrices for equality. Overload operator ==
bool Matrix4x4::operator == (const Matrix4x4& myMatrix) const
{
    return (std::fabs(_m[0] - myMatrix._m[0]) < ARITHMETIC_DOUBLE_MICRO && 
        std::fabs(_m[1] - myMatrix._m[1]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[2] - myMatrix._m[2]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[3] - myMatrix._m[3]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[4] - myMatrix._m[4]) < ARITHMETIC_DOUBLE_MICRO && 
        std::fabs(_m[5] - myMatrix._m[5]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[6] - myMatrix._m[6]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[7] - myMatrix._m[7]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[8] - myMatrix._m[8]) < ARITHMETIC_DOUBLE_MICRO && 
        std::fabs(_m[9] - myMatrix._m[9]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[10] - myMatrix._m[10]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[11] - myMatrix._m[11]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[12] - myMatrix._m[12]) < ARITHMETIC_DOUBLE_MICRO && 
        std::fabs(_m[13] - myMatrix._m[13]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[14] - myMatrix._m[14]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[15] - myMatrix._m[15]) < ARITHMETIC_DOUBLE_MICRO);
}

/// \brief Tests 2 matrices for inequality. Overload operator ==
bool Matrix4x4::operator != (const Matrix4x4& myMatrix) const
{
    return (std::fabs(_m[0] - myMatrix._m[0]) > ARITHMETIC_DOUBLE_MICRO || 
        std::fabs(_m[1] - myMatrix._m[1]) > ARITHMETIC_DOUBLE_MICRO ||
        std::fabs(_m[2] - myMatrix._m[2]) > ARITHMETIC_DOUBLE_MICRO ||
        std::fabs(_m[3] - myMatrix._m[3]) > ARITHMETIC_DOUBLE_MICRO ||
        std::fabs(_m[4] - myMatrix._m[4]) > ARITHMETIC_DOUBLE_MICRO || 
        std::fabs(_m[5] - myMatrix._m[5]) > ARITHMETIC_DOUBLE_MICRO ||
        std::fabs(_m[6] - myMatrix._m[6]) > ARITHMETIC_DOUBLE_MICRO ||
        std::fabs(_m[7] - myMatrix._m[7]) > ARITHMETIC_DOUBLE_MICRO ||
        std::fabs(_m[8] - myMatrix._m[8]) > ARITHMETIC_DOUBLE_MICRO || 
        std::fabs(_m[9] - myMatrix._m[9]) > ARITHMETIC_DOUBLE_MICRO ||
        std::fabs(_m[10] - myMatrix._m[10]) > ARITHMETIC_DOUBLE_MICRO ||
        std::fabs(_m[11] - myMatrix._m[11]) > ARITHMETIC_DOUBLE_MICRO ||
        std::fabs(_m[12] - myMatrix._m[12]) > ARITHMETIC_DOUBLE_MICRO || 
        std::fabs(_m[13] - myMatrix._m[13]) > ARITHMETIC_DOUBLE_MICRO ||
        std::fabs(_m[14] - myMatrix._m[14]) > ARITHMETIC_DOUBLE_MICRO ||
        std::fabs(_m[15] - myMatrix._m[15]) > ARITHMETIC_DOUBLE_MICRO);
}


/// \brief Assignment of a matrix. Overload operator =
Matrix4x4 & Matrix4x4::operator = (const Matrix4x4& myMatrix)
{
    _m[0] = myMatrix._m[0];
    _m[1] = myMatrix._m[1];
    _m[2] = myMatrix._m[2];
    _m[3] = myMatrix._m[3];

    _m[4] = myMatrix._m[4];
    _m[5] = myMatrix._m[5];
    _m[6] = myMatrix._m[6];
    _m[7] = myMatrix._m[7];

    _m[8] = myMatrix._m[8];
    _m[9] = myMatrix._m[9];
    _m[10] = myMatrix._m[10];
    _m[11] = myMatrix._m[11];

    _m[12] = myMatrix._m[12];
    _m[13] = myMatrix._m[13];
    _m[14] = myMatrix._m[14];
    _m[15] = myMatrix._m[15];
    
    return *this;
}

/// \brief overload operator []
///
/// \param[in] iRow Input row number
///
/// \return Pointer to the specified row
double* Matrix4x4::operator [] (unsigned int iRow)
{
    if(iRow > 4)
        return nullptr;
    else
        return m[iRow];
}

    /// \brief overload operator []
const double *Matrix4x4::operator [] (unsigned int iRow) const
{
    if(iRow > 4)
        return nullptr;
    else
        return m[iRow];
}

/// \brief Calculate minor_mx4 determinant of specified rows and columns
///
/// \param[in] r0, r1, r2 The three rows number
/// \param[in] c0, c1, c2 The three columns number
///
/// \return minor_mx4 determinant of specified rows and columns
double Matrix4x4::minor_mx4(int r0, int r1,  int r2, int c0, int c1, int c2) const
{
    return m[r0][c0] * (m[r1][c1] * m[r2][c2] - m[r2][c1] * m[r1][c2]) -
           m[r0][c1] * (m[r1][c0] * m[r2][c2] - m[r2][c0] * m[r1][c2]) +
           m[r0][c2] * (m[r1][c0] * m[r2][c1] - m[r2][c0] * m[r1][c1]);
}


void Matrix4x4::GetElements(double* m)
{
     memcpy(m, _m, 16*sizeof(double));
}

/// \brief Get ajoint matrix
///
/// \return The ajoint matrix 
Matrix4x4 Matrix4x4::GetAdjoint() const
{
    return Matrix4x4(minor_mx4(1, 2, 3, 1, 2, 3),
                                 -minor_mx4(0, 2, 3, 1, 2, 3),
                                 minor_mx4(0, 1, 3, 1, 2, 3),
                                 -minor_mx4(0, 1, 2, 1, 2, 3),

                                 -minor_mx4(1, 2, 3, 0, 2, 3),
                                 minor_mx4(0, 2, 3, 0, 2, 3),
                                 -minor_mx4(0, 1, 3, 0, 2, 3),
                                 minor_mx4(0, 1, 2, 0, 2, 3),

                                 minor_mx4(1, 2, 3, 0, 1, 3),
                                 -minor_mx4(0, 2, 3, 0, 1, 3),
                                 minor_mx4(0, 1, 3, 0, 1, 3),
                                -minor_mx4(0, 1, 2, 0, 1, 3),

                                -minor_mx4(1, 2, 3, 0, 1, 2),
                                minor_mx4(0, 2, 3, 0, 1, 2),
                                -minor_mx4(0, 1, 3, 0, 1, 2),
                                minor_mx4(0, 1, 2, 0, 1, 2));
}
END_NAMESPACE

