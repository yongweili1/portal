#include "uAIArithmetic/arithmetic.h"
#include "uAIArithmetic/matrix_3x3.h"
#include <cmath>


BEGIN_NAMESPACE

/// \brief Default Constructor
Matrix3x3::Matrix3x3()
{
}

/// \brief the order is column-major
Matrix3x3::Matrix3x3(double m00, double m01, double m02, 
					 double m10, double m11, double m12, 
					 double m20, double m21, double m22)
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
}

/// \brief make an identity matrix
void Matrix3x3::SetIdintity()
{
    m[0][0] = 1.0; m[1][0] = 0.0; m[2][0] = 0.0;
    m[0][1] = 0.0; m[1][1] = 1.0; m[2][1] = 0.0;
    m[0][2] = 0.0; m[1][2] = 0.0; m[2][2] = 1.0;
}

/// \brief Prepend a matrix
///
/// \param[in] myMatrix The matrix to be multiplied with current matrix
void Matrix3x3::Prepend(const Matrix3x3 &myMatrix)
{
    *this = myMatrix * (*this);
}

/// \brief Append a matrix
///
/// \param[in] myMatrix The matrix to be appended
void Matrix3x3::Append(const Matrix3x3 &myMatrix)
{
    *this = (*this) * myMatrix;
}


/// \brief Transpose the matrix
void Matrix3x3::Transpose()
{
    std::swap(_m[1], _m[3]);
    std::swap(_m[2], _m[6]);
    std::swap(_m[5], _m[7]);
}


/// \brief Get transpose of a matrix
///
/// \return The matrix's transpose
Matrix3x3 Matrix3x3::GetTranspose() const
{
    return Matrix3x3(_m[0], _m[3], _m[6],
                     _m[1], _m[4], _m[7],
                     _m[2], _m[5], _m[8]);
}


/// \brief Test whether the matrix has inverse matrix
///
/// \return true if has inverse ,or return false
bool Matrix3x3::HasInverse() const
{
    return (std::fabs(Determinant()) > UIH_ARITHMETIC_DOUBLE_ZERO);
}


/// \brief Get the inverse of a matrix. Before calling this function, MUST call 
///        bool HasInverse() to make sure the inverse exists.
///
/// \return The inverse of a matrix
Matrix3x3 Matrix3x3::Inverse() const
{
    return GetAdjoint() * (1.0 / Determinant());
}


/// \brief Check whether or not the matrix is affine matrix.
/// 
/// \return true if is an affine matrix, or return false
bool Matrix3x3::IsAffine() const
{
    return (std::fabs(_m[2]) < ARITHMETIC_DOUBLE_MICRO && 
			std::fabs(_m[5]) < ARITHMETIC_DOUBLE_MICRO &&
			std::fabs(_m[8]-1.0) < ARITHMETIC_DOUBLE_MICRO);
}


/// \brief Transform a point through the transform matrix
///
/// \param[in] myPoint The point to be transformed
///
/// \return Transformed point
Point2D Matrix3x3::Transform(const Point2D &myPoint) const
{
    return (*this) * myPoint;
}

/// \brief Transform a point array through the transform matrix
void Matrix3x3::Transform(Point2D *myPoint, int iSize) const
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
Vector2D Matrix3x3::Transform(const Vector2D & myVector) const
{
    return (*this) * myVector;
}


/// \brief Calculate the determinant of the 4X4 matrix
///
/// \return The determinant of the matrix
double Matrix3x3::Determinant() const
{
    return m[0][0] * minor_mx3(1, 2, 1, 2) -
           m[0][1] * minor_mx3(1, 2, 0, 2) +
           m[0][2] * minor_mx3(1, 2, 0, 1);
}


/// \brief Builds a scaling matrix
///
/// \param[in] myScale scaling vector
///
/// \return The scaling matrix
Matrix3x3 Matrix3x3::MakeScale(const Vector2D & myScale)
{
    return Matrix3x3(myScale.x,		0.0,	0.0,
                     0.0,		myScale.y,  0.0 ,
                     0.0,			0.0,	1.0);

}

/// \brief Builds a translation matrix
///
/// \param[in] myTranslate Translation vector
///
/// \return Translation matrix
Matrix3x3 Matrix3x3::MakeTranslate(const Vector2D &myTranslate)
{
    return Matrix3x3(1.0,			0.0,		  0.0,
					 0.0,			1.0,		  0.0 ,
					myTranslate.x, myTranslate.y, 1.0);
}


Matrix3x3 Matrix3x3::MakeCenterRotate( const double& angle )
{
	return Matrix3x3(cos(angle),	sin(angle),		0.0,
					-sin(angle),	cos(angle),		0.0 ,
						0,				0,			1.0);
}

/// \brief Extracts the translation transformation part of the matrix.
///
/// \param[out] myVector The translation part of the matrix
void Matrix3x3::ExtractTranslate(Vector2D &myVector) const
{
    myVector.x = _m[7];
    myVector.y = _m[8];
}


/// \brief Matrix concatenation using '*'. Overload operator *
///
/// \param[in] m2 The matrix to be appended with
///
/// \return The result matrix
Matrix3x3 Matrix3x3::operator * (const Matrix3x3 & m2) const
{
    return Matrix3x3(
        _m[0] * m2._m[0] + _m[3] * m2._m[1] + _m[6] * m2._m[2],
        _m[1] * m2._m[0] + _m[4] * m2._m[1] + _m[7] * m2._m[2],
        _m[2] * m2._m[0] + _m[5] * m2._m[1] + _m[8] * m2._m[2],
        _m[0] * m2._m[3] + _m[3] * m2._m[4] + _m[6] * m2._m[5],
        _m[1] * m2._m[3] + _m[4] * m2._m[4] + _m[7] * m2._m[5],
        _m[2] * m2._m[3] + _m[5] * m2._m[4] + _m[8] * m2._m[5],
        _m[0] * m2._m[6] + _m[3] * m2._m[7] + _m[6] * m2._m[8],
        _m[1] * m2._m[6] + _m[4] * m2._m[7] + _m[7] * m2._m[8],
        _m[2] * m2._m[6] + _m[5] * m2._m[7] + _m[8] * m2._m[8]
    );
}


/// \brief Vector transformation using '*'.Overload operator *
///
/// \param[in] myVector Input vector
///
/// \return Transformed vector
Vector2D Matrix3x3::operator * (const Vector2D &myVector) const
{
    return  Vector2D(_m[0] * myVector.x + _m[3] * myVector.y,
                               _m[1] * myVector.x + _m[4] * myVector.y);
}


/// \brief Point transformation using '*'.Overload operator *
///
/// \param[in] myPoint Input point
/// 
/// \return Transformed Vector
Point2D Matrix3x3::operator * (const Point2D& myPoint) const
{
    double dw = _m[2] * myPoint.x + _m[5] * myPoint.y + _m[8];
    dw = fabs(dw) > UIH_ARITHMETIC_DOUBLE_ZERO? dw: UIH_ARITHMETIC_DOUBLE_ZERO;
    dw = 1.0 / dw;
    return  Point2D(
        (_m[0] * myPoint.x + _m[3] * myPoint.y + _m[6]) * dw,
        (_m[1] * myPoint.x + _m[4] * myPoint.y + _m[7]) * dw);
}


/// \brief Matrix multiply a scalar using '*'.Overload operator *
///        Each element of the matrix multiply with a scale
///
/// \param[in] dscale The scaling factor
///
///\ return  Result matrix
Matrix3x3 Matrix3x3::operator * (const double dscale) const
{
    return Matrix3x3(
        dscale * _m[0], dscale * _m[1], dscale * _m[2],
        dscale * _m[3], dscale * _m[4], dscale * _m[5],
        dscale * _m[6], dscale * _m[7], dscale * _m[8]);
}


/// \brief Matrix addition.Overload operator +
///
/// \param[in] myMatrix The matrix to be added
///
/// \return Result matrix
Matrix3x3 Matrix3x3::operator + (const Matrix3x3& myMatrix) const
{
    Matrix3x3 myResult;
    myResult._m[0] = _m[0] + myMatrix._m[0];
    myResult._m[1] = _m[1] + myMatrix._m[1];
    myResult._m[2] = _m[2] + myMatrix._m[2];

    myResult._m[3] = _m[3] + myMatrix._m[3];
    myResult._m[4] = _m[4] + myMatrix._m[4];
    myResult._m[5] = _m[5] + myMatrix._m[5];

    myResult._m[6] = _m[6] + myMatrix._m[6];
    myResult._m[7] = _m[7] + myMatrix._m[7];
    myResult._m[8] = _m[8] + myMatrix._m[8];
    return myResult;
}


/// \brief Matrix subtraction. Overload operator -
///
/// \param[in] myMatrix The matrix to be subtracted
///
/// \return Result matrix
Matrix3x3 Matrix3x3::operator - (const Matrix3x3& myMatrix) const
{
    Matrix3x3 myResult;
    myResult._m[0] = _m[0] - myMatrix._m[0];
    myResult._m[1] = _m[1] - myMatrix._m[1];
    myResult._m[2] = _m[2] - myMatrix._m[2];

    myResult._m[3] = _m[3] - myMatrix._m[3];
    myResult._m[4] = _m[4] - myMatrix._m[4];
    myResult._m[5] = _m[5] - myMatrix._m[5];

    myResult._m[6] = _m[6] - myMatrix._m[6];
    myResult._m[7] = _m[7] - myMatrix._m[7];
    myResult._m[8] = _m[8] - myMatrix._m[8];
    return myResult;
}


/// \brief Tests 2 matrices for equality. Overload operator ==
bool Matrix3x3::operator == (const Matrix3x3& myMatrix) const
{
    return std::fabs(_m[0] - myMatrix._m[0]) < ARITHMETIC_DOUBLE_MICRO && 
        std::fabs(_m[1] - myMatrix._m[1]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[2] - myMatrix._m[2]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[3] - myMatrix._m[3]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[4] - myMatrix._m[4]) < ARITHMETIC_DOUBLE_MICRO && 
        std::fabs(_m[5] - myMatrix._m[5]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[6] - myMatrix._m[6]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[7] - myMatrix._m[7]) < ARITHMETIC_DOUBLE_MICRO &&
        std::fabs(_m[8] - myMatrix._m[8]) < ARITHMETIC_DOUBLE_MICRO ;
}

/// \brief Tests 2 matrices for inequality. Overload operator ==
bool Matrix3x3::operator != (const Matrix3x3& myMatrix) const
{
    return std::fabs(_m[0] - myMatrix._m[0]) > ARITHMETIC_DOUBLE_MICRO || 
		std::fabs(_m[1] - myMatrix._m[1]) > ARITHMETIC_DOUBLE_MICRO ||
		std::fabs(_m[2] - myMatrix._m[2]) > ARITHMETIC_DOUBLE_MICRO ||
		std::fabs(_m[3] - myMatrix._m[3]) > ARITHMETIC_DOUBLE_MICRO ||
		std::fabs(_m[4] - myMatrix._m[4]) > ARITHMETIC_DOUBLE_MICRO || 
		std::fabs(_m[5] - myMatrix._m[5]) > ARITHMETIC_DOUBLE_MICRO ||
		std::fabs(_m[6] - myMatrix._m[6]) > ARITHMETIC_DOUBLE_MICRO ||
		std::fabs(_m[7] - myMatrix._m[7]) > ARITHMETIC_DOUBLE_MICRO ||
		std::fabs(_m[8] - myMatrix._m[8]) > ARITHMETIC_DOUBLE_MICRO ;
}


/// \brief Assignment of a matrix. Overload operator =
Matrix3x3 & Matrix3x3::operator = (const Matrix3x3& myMatrix)
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
    
    return *this;
}

/// \brief overload operator []
///
/// \param[in] iRow Input row number
///
/// \return Pointer to the specified row
double* Matrix3x3::operator [] (unsigned int iRow)
{
    if(iRow > 3)
        return nullptr;
    else
        return m[iRow];
}

    /// \brief overload operator []
const double *Matrix3x3::operator [] (unsigned int iRow) const
{
    if(iRow > 3)
        return nullptr;
    else
        return m[iRow];
}

/// \brief Calculate minor_mx3 determinant of specified rows and columns
///
/// \param[in] r0, r1, r2 The three rows number
/// \param[in] c0, c1, c2 The three columns number
///
/// \return minor_mx3 determinant of specified rows and columns
double Matrix3x3::minor_mx3(int r0, int r1, int c0, int c1) const
{
    return m[r0][c0] * m[r1][c1] - m[r1][c0] * m[r0][c1];
}


/// \brief Get ajoint matrix
///
/// \return The ajoint matrix 
Matrix3x3 Matrix3x3::GetAdjoint() const
{
    return Matrix3x3(minor_mx3(1, 2, 1, 2),
                     -minor_mx3(0, 2, 1, 2),
                     minor_mx3(0, 1, 1, 2),

					 -minor_mx3(1, 2, 0, 2),
					 minor_mx3(0, 2, 0, 2),
					 -minor_mx3(0, 1, 0, 2),

					 minor_mx3(1, 2, 0, 1),
					 -minor_mx3(0, 2, 0, 1),
					 minor_mx3(0, 1, 0, 1));
}

END_NAMESPACE
