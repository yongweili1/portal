#ifndef UAI_DICOM_SLICER_CHECKER_API_H_
#define UAI_DICOM_SLICER_CHECKER_API_H_

#include <memory>
#include <string>
#include <ctime>
#include <cmath>
#include <stdio.h>

#include "uai_app_defs.h"

#include "boost/function.hpp"
#include "boost/noncopyable.hpp"

#include "uAIArithmetic/arithmetic.h"
#include "dicom_slice.h"
#include "uai_dicom_sopclassuid_def.h"

BEGIN_NAMESPACE

// double类型数据是否相等的误差，如果两个double类型数据只差小于 dDoubleError，则认为二者相等
static const double dDoubleError = 1e-5;
static const double dMREPI = 0.010001;
static const double dCTEPI = 0.010001;
static const double dPETEPI = 0.010001;

// MR数据检查中，要求层厚的误差小于0.1，为了舍入误差，将此值精确到小数点后6位
static const double dMRThicknessError = 0.0100001;

// CT 数据检查中，要求层间距的误差小于0.1mm，为了舍入误差，将此值精确到小数点后6位
static const double dCTSpacingBetweenSlicesError = 0.10001;
// CT数据检查中，要求层间距不能大于10mm,
static const double dCTMaxSpacing                = 10.000001;

// PET数据检查中，要求层间距的误差小于0.01mm，为了舍入误差，将此值精确到小数点后6位
static const double dPETSpacingBetweenSlicesError = 0.11;

//定义PI
static const double dPI = 3.14;

typedef boost::shared_ptr<DicomSlice>  DicomSlicePtr;

inline double GetEPIBYModality(const std::string& scsopuid)
{
    if(SOPClassUID_MRImageStorage == scsopuid)
    {
        //MR
        return dMREPI;
    }
    else if(SOPClassUID_CTImageStorage == scsopuid)
    {
        //CT
        return dCTEPI;
    }
    else
    {
        //PET
        return dPETEPI;
    }
}

inline double GetMinSeriesSliceSpacing(const std::string& scsopuid,double spacing)
{
    if(SOPClassUID_MRImageStorage == scsopuid)
    {
        //MR
        return 4 * spacing;
    }
    else if(SOPClassUID_CTImageStorage == scsopuid)
    {
        //CT
        return 5 * spacing;
    }
    else
    {
        //PET
        return 5 * spacing;
    }
}

inline bool DoubleNotEqual( double dLft, double dRht, double dLegalError = dDoubleError )
{
    return ( dLegalError < std::fabs( dLft - dRht ) );
}

inline bool DoubleEqual( double dLft, double dRht, double dLegalError = dDoubleError )
{
    return ( dLegalError > std::fabs( dLft - dRht ) );
}

inline bool GetImageOrientationPatient( const DicomSlicePtr& pImage, double (&dIOP)[6] )
{
    Vector3D v_X;
    Vector3D v_Y;
    pImage->GetImageOrientationPatient(v_X, v_Y);
    dIOP[0] = v_X.x;
    dIOP[1] = v_X.y;
    dIOP[2] = v_X.z;
    dIOP[3] = v_Y.x;
    dIOP[4] = v_Y.y;
    dIOP[5] = v_Y.z;
    return true;
    
    /*
    return ( 6 == sscanf_s( sIOP.c_str(), "%lf\\%lf\\%lf\\%lf\\%lf\\%lf", dIOP, dIOP + 1, dIOP + 2,
        dIOP + 3, dIOP + 4, dIOP + 5) );
    */
    
}

inline bool IsImageOrientationPatientParallelZAxi(const DicomSlicePtr& pImage,const double& thita)
{

    Vector3D vRow;
    Vector3D vCol;
    
    pImage->GetImageOrientationPatient(vRow, vCol);
    
    double angleInPi = vRow.CrossProduct(vCol).AngleBetween(Vector3D(0.0,0.0,1.0));
    return std::fabs(angleInPi) < std::fabs(thita);
}
    

inline bool GetImagePositionPatient( const DicomSlicePtr& pImage, double (&dIPP)[ 3 ] )
{
    Point3D pt = pImage->GetImagePositionPatient();
    dIPP[0] = pt.x;
    dIPP[1] = pt.y;
    dIPP[2] = pt.z;
    return true;
}

    
inline bool IsImageShift(const DicomSlicePtr& pImage1,const DicomSlicePtr& pImage2,const double& thita)
{
    double dipp1[3];
    if(!GetImagePositionPatient(pImage1,dipp1))
    {
        return true;
    }
    
    Vector3D v1(dipp1[0],dipp1[1],dipp1[2]);
    double dipp2[3];
    if(!GetImagePositionPatient(pImage2,dipp2))
    {
        return true;
    }
    
    Vector3D v2(dipp2[0],dipp2[1],dipp2[2]);
    Vector3D dir = v1-v2;

    Vector3D v3;
    Vector3D v4;
    pImage1->GetImageOrientationPatient(v3, v4);
    
    double dAngle = v3.CrossProduct(v4).AngleBetween(dir);
    return std::fabs( dAngle ) > std::fabs(thita) &&
        std::fabs( dAngle - dPI ) > std::fabs( thita );
}

inline bool GetSliceThickness( const DicomSlicePtr& pImage, double& dSlsThickness )
{
    std::string sThickness = pImage->GetSliceThickness();
    return ( 1 == sscanf( sThickness.c_str(), "%lf", &dSlsThickness) );
}

inline bool ContainInRange(const double& min,const double& max,const double& val)
{
    return min < val && val <= max;
}

inline bool SliceThicknessEqual(
    const DicomSlicePtr& pImage1, 
    const DicomSlicePtr& pImage2,
    const double& epi)
{
    double sliceThickness1;
    if(!GetSliceThickness(pImage1,sliceThickness1))
    {
        return false;
    }
    double sliceThickness2;
    if(!GetSliceThickness(pImage2,sliceThickness2))
    {
        return false;
    }
    return std::fabs( sliceThickness1 - sliceThickness2 ) < epi;
}

inline bool GetPixelSpacing( const DicomSlicePtr pImage, double (&dSpacing)[2] )
{
    pImage->GetPixelSpacing(dSpacing);
    return true;
    /*
    std::string sSpacing = pImage->GetPixelSpacing();
    return ( 2 == sscanf_s( sSpacing.c_str(), "%lf\\%lf", dSpacing, dSpacing + 1) );
    */
}

inline bool ThicknessEqual( double dThickness1, double dThickness2 )
{
    return ( dMRThicknessError > std::fabs( dThickness1 - dThickness2 ) );
}

inline bool GetSpacingBetweenSlices(
    const DicomSlicePtr& pImage1,
    const DicomSlicePtr& pImage2,
    double& dSpacingBtSlices,
    std::string& sReplyInfo,
    int* sign=0)
{
    double dFstIPP[ 3 ] = { 0.0 }, dSndIPP[ 3 ] = { 0.0 };
    if ( !GetImagePositionPatient( pImage1, dFstIPP) ||
        !GetImagePositionPatient( pImage2, dSndIPP ) )
    {
        sReplyInfo = "UID_AppCommon_Fail_To_Get_Image_Position";
        return false;
    }
    dSpacingBtSlices = std::sqrt( 
        ( dFstIPP[ 0 ] - dSndIPP[ 0 ] ) * ( dFstIPP[ 0 ] - dSndIPP[ 0 ] ) +
        ( dFstIPP[ 1 ] - dSndIPP[ 1 ] ) * ( dFstIPP[ 1 ] - dSndIPP[ 1 ] ) +
        ( dFstIPP[ 2 ] - dSndIPP[ 2 ] ) * ( dFstIPP[ 2 ] - dSndIPP[ 2 ] ) );
    double zIntercept = dFstIPP[ 2 ] - dSndIPP[ 2 ];
    if(0 != sign)
    {
        *sign = static_cast<int>((zIntercept)/(std::fabs(zIntercept)));
    }
    return true;
}

inline bool SpacingEqual( double dVal1, double dVal2 ,const double& epi)
{
    return ( epi > std::fabs( dVal1 - dVal2 ) );
}

inline bool Ascending( const DicomSlicePtr& pImage1, const DicomSlicePtr& pImage2 )
{
    return pImage1->GetInstanceNumber() < pImage2->GetInstanceNumber();
}

inline double GetSpacingBetweenSlices( const DicomSlicePtr& pImage1, const DicomSlicePtr& pImage2)
{
    double dFstIPP[ 3 ] = { 0.0 }, dSndIPP[ 3 ] = { 0.0 };
    if ( !GetImagePositionPatient( pImage1, dFstIPP) ||
        !GetImagePositionPatient( pImage2, dSndIPP ) )
    {
        return false;
    }
    return std::sqrt( 
        ( dFstIPP[ 0 ] - dSndIPP[ 0 ] ) * ( dFstIPP[ 0 ] - dSndIPP[ 0 ] ) +
        ( dFstIPP[ 1 ] - dSndIPP[ 1 ] ) * ( dFstIPP[ 1 ] - dSndIPP[ 1 ] ) +
        ( dFstIPP[ 2 ] - dSndIPP[ 2 ] ) * ( dFstIPP[ 2 ] - dSndIPP[ 2 ] ) );
}

inline bool IsPixelSpacingEqual(const DicomSlicePtr& pImage1,const DicomSlicePtr& pImage2,const double& epi)
{
    double ps1[2];
    if(!GetPixelSpacing(pImage1,ps1))
    {
        return false;
    }
    double ps2[2];
    if(!GetPixelSpacing(pImage2,ps2))
    {
        return false;
    }
    return std::fabs(ps1[0] - ps2[0]) < epi &&
        std::fabs(ps1[1] - ps2[1]) < epi;
}

inline bool IsFOVEqual( 
    const DicomSlicePtr& pImage1,const DicomSlicePtr& pImage2,
    const double& epi )
{
    double ps1[2];
    if(!GetPixelSpacing(pImage1,ps1))
    {
        return false;
    }
    int row1 = pImage1->GetRow();
    int col1 = pImage1->GetColumn();
    double ps2[2];
    if(!GetPixelSpacing(pImage2,ps2))
    {
        return false;
    }
    int row2 = pImage2->GetRow();
    int col2 = pImage2->GetColumn();
    return std::fabs(row1*ps1[0] - row2*ps2[0]) < epi &&
        std::fabs(col1*ps1[1] - col2*ps2[1]) < epi;
}

END_NAMESPACE
#endif
