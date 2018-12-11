#include "uai_app_logger.h"
#include "uai_dicom_slicer_checker.h"
#include "uai_dicom_slicer_check_api.h"
    
BEGIN_NAMESPACE;
    
namespace
{
    inline bool GetImagePosition( const DicomSlicePtr& pImage , float (&fImgPos)[ 3 ] )
    {
        Point3D pt = pImage->GetImagePositionPatient();
        fImgPos[0] = pt.x;
        fImgPos[1] = pt.y;
        fImgPos[2] = pt.z;
        return true;
    }
    
    bool GetFirstAndLastSliceLocation( 
        const std::vector< DicomSlicePtr >& vecImageList, 
        float(&fFristSliceLocation)[3], float(&fLastSliceLocation)[3] )
    {
        int iNumOfImage = static_cast<int>(vecImageList.size());
        if (0 == iNumOfImage)
        {
            return false;
        }
        //Get the location of the First slice
        if (!GetImagePosition(vecImageList.front(), fFristSliceLocation))
        {
            _LOG_DEV_ERROR << "Fail to Get FristSliceLocation";
            return false;
        }
        //Get the location of the Last slice
        DicomSlicePtr pImageTmp;
        float fSliceLocation[3] = { 0.0f };
        for (int i = 1; i < iNumOfImage; i++)
        {
            pImageTmp = vecImageList.at(i);
            if (!pImageTmp)
            {
                return false;
            }
            if (!GetImagePosition(pImageTmp, fSliceLocation))
            {
                _LOG_DEV_ERROR << "Fail to Get LastSliceLocation. Image Num: " << i;
                return false;
            }
            if ((fabs(fSliceLocation[0] - fFristSliceLocation[0]) < dMRThicknessError) &&
                (fabs(fSliceLocation[1] - fFristSliceLocation[1]) < dMRThicknessError) &&
                (fabs(fSliceLocation[2] - fFristSliceLocation[2]) < dMRThicknessError))
            {
                GetImagePosition(vecImageList.at(i - 1), fLastSliceLocation);      // i-1
                break;
            }
        }
        return true;
    }
}

bool DynamicSeriesGroup(
    const std::vector< DicomSlicePtr >& vecImageList, 
    std::vector< std::vector< DicomSlicePtr > >& vecVolumeList )
{
    // If multiple volume is contained in this series, only keep the 1st volume
    int iNumOfImage = static_cast<int>(vecImageList.size());
    if ( 0 == iNumOfImage )
    {
        return false;
    }

    float fFristSliceLocation[3] = { 0.0f };
    float fLastSliceLocation[3] = { 0.0f };
    float fSliceLocation[3] = { 0.0f };

    if (!GetFirstAndLastSliceLocation(vecImageList, fFristSliceLocation, fLastSliceLocation))
    {
        _LOG_DEV_ERROR << "Fail to GetFirstAndLastSliceLocation";
        return false;
    }

    DicomSlicePtr pImageTmp;
    std::vector< DicomSlicePtr > vecImgTemp;

    for (int i = 0; i < iNumOfImage; i++)
    {
        pImageTmp = vecImageList.at( i );
        if ( !pImageTmp )
        {
            return false;
        }
        if ( !GetImagePosition( pImageTmp, fSliceLocation ) )
        {
            _LOG_DEV_ERROR << "Fail to Get Image SliceLocation. Image Num: " << i;
            return false;
        }

        vecImgTemp.push_back(pImageTmp);

        // If the slice location is the same as the last one, then we complete get a volume
        // for medical usage, we treat difference < 0.1 mm as the same. Fix 881696
        if ( (fabs(fSliceLocation[0] - fLastSliceLocation[0]) < dMRThicknessError) &&
            (fabs(fSliceLocation[1] - fLastSliceLocation[1]) < dMRThicknessError) &&
            (fabs(fSliceLocation[2] - fLastSliceLocation[2]) < dMRThicknessError) )
        {
            vecVolumeList.push_back( vecImgTemp );
            vecImgTemp.clear();
        }
    }

    if ( !vecImgTemp.empty() )
    {
        vecVolumeList.push_back( vecImgTemp );
    }
    return true;
}

int VerifyDynamicSeries(
    const std::vector< DicomSlicePtr >& vecImageList, 
    std::vector< std::vector< DicomSlicePtr > >& vecVolumeList )
{
    std::vector< std::vector< DicomSlicePtr > > vecVolumeListTmp;
    if ( !DynamicSeriesGroup( vecImageList, vecVolumeListTmp ))
    {
        _LOG_DEV_ERROR << "Fail to do DynamicSeriesGroup !" ;
        return -1;
    }
    if ( vecVolumeListTmp.empty() )
    {
        _LOG_DEV_ERROR << "The count of vecVolumeListTmp is empty!" ;
        return -1;
    }
    auto itorBeg = vecVolumeListTmp.begin();
    for ( ; itorBeg != vecVolumeListTmp.end(); ++itorBeg )
    {
        auto vecVolume = *itorBeg;
        if ( 3 > vecVolume.size() )
        {
            _LOG_DEV_ERROR << "The count of images in vecVolume is " << vecVolume.size() ;
            continue;
        }
        vecVolumeList.push_back( vecVolume );
    }
    return vecVolumeList.empty() ? -1 : 0;
}

DataCheckingBase::DataCheckingBase()
{
    m_DynamicSeriesDataChecking = boost::bind( VerifyDynamicSeries, _1, _2 );
}

DataCheckingBase::~DataCheckingBase()
{
    
}


double LegalRoundOffError( DicomSlicePtr ptrSlice )
{
    std::string sSopClassUID = ptrSlice->GetSopClassUID();
    if(SOPClassUID_MRImageStorage == sSopClassUID)
    {
        //MR
        return dMRThicknessError;
    }
    else if(SOPClassUID_CTImageStorage == sSopClassUID)
    {
        //CT
        return dCTSpacingBetweenSlicesError;
    }
    else if(SOPClassUID_PositronEmissionTomographyImageStorage == sSopClassUID )
    {
        //PET
        return dPETSpacingBetweenSlicesError;
    }
    else
    {
        _LOG_DEV_WARNING << "Cannot Find the Modality" ;
       return dMRThicknessError; 
    }
}

int DataCheckingBase::DataChecking(
        const std::vector<DicomSlicePtr> vSliceList, 
        std::string& sReplyInfo )
{
    if ( 3 > vSliceList.size() )
    {
        sReplyInfo = "UID_AppCommon_Too_Little_Images";
        return -1;
    }
    int iRtn = VerifyCommonIllegalCondition_i( vSliceList, sReplyInfo);
    if ( 0 > iRtn )
    {
        _LOG_DEV_ERROR << "VerifyCommonIllegalCondition_i Failed! Reason: " << sReplyInfo ;
        return iRtn;
    }

    if ( !m_AdditionalDataChecking.empty() )
    {
        iRtn = m_AdditionalDataChecking( vSliceList, sReplyInfo);
    }
    return iRtn;
}


inline void GetImageSize_i( const DicomSlicePtr& pImage, int (&iImgSize)[2] )
{
    iImgSize[ 0 ] = pImage->GetRow();
    iImgSize[ 1 ] = pImage->GetColumn();
}

inline bool ImageSizeEqual_i( int (&iLftSize)[ 2 ], int (&iRhtSize)[ 2 ] )
{
    return ( iLftSize[ 0 ] == iRhtSize[ 0 ] && iLftSize[ 1 ] == iRhtSize[ 1 ] );
}

inline bool IsIOPEqual_i( double (&dLft)[ 6 ], double (&dRht)[ 6 ], double dLegalError )
{
    return ( DoubleEqual(dLft[ 0 ], dRht[ 0 ], dLegalError ) && 
        DoubleEqual( dLft[ 1 ], dRht[ 1 ], dLegalError ) &&
        DoubleEqual( dLft[ 2 ], dRht[ 2 ], dLegalError ) &&
        DoubleEqual( dLft[ 3 ], dRht[ 3 ], dLegalError ) &&
        DoubleEqual( dLft[ 4 ], dRht[ 4 ], dLegalError ) &&
        DoubleEqual( dLft[ 5 ], dRht[ 5 ], dLegalError ) );
}

inline bool IsImageSizeEqual_i( int (&iImageSize)[ 2 ], const DicomSlicePtr& pImage )
{
    int iImgSizeTmp[ 2 ] = { 0 };
    GetImageSize_i( pImage, iImgSizeTmp);
    return ImageSizeEqual_i( iImageSize, iImgSizeTmp);
}

inline bool IsImageOrientationPatientEqual_i( double (&dStandardIOP)[ 6 ], const DicomSlicePtr& pImage, std::string& sReplyInfo, double dLegalError )
{
    double dIOPTmp[ 6 ] = { 0.0 };
    if ( !GetImageOrientationPatient( pImage, dIOPTmp ) )
    {
        sReplyInfo= "UID_AppCommon_Fail_To_Get_Image_Orientation_Patient";
        return false;
    }
    if ( !IsIOPEqual_i( dStandardIOP, dIOPTmp, dLegalError ) )
    {
        sReplyInfo = "UID_AppCommon_Image_Orientation_Not_Consistent";
        return false;
    }
    return true;
}

inline bool VerifyPixelSpacing_i( double (&dPxlSpacing1)[ 2 ], double (&dPxlSpacing2)[ 2 ], double dLegalError )
{
    return DoubleEqual( dPxlSpacing1[ 0 ], dPxlSpacing2[ 0 ], dLegalError ) &&
        DoubleEqual( dPxlSpacing1[ 1 ], dPxlSpacing2[ 1 ], dLegalError );
}

bool IsPixelSpacingEqual_i( const DicomSlicePtr& pImage, double (&dPixelSpacing)[ 2 ], double dLegalError )
{
    double dPxelSpacingTmp[ 2 ] = { 0.0 };
    if ( !GetPixelSpacing( pImage, dPxelSpacingTmp) )
    {
        return false;
    }
    return VerifyPixelSpacing_i( dPxelSpacingTmp, dPixelSpacing, dLegalError);
}

bool IsSliceThicknessEqual_i( const DicomSlicePtr& pImage, double &dSlsThickness, double dLegalError )
{
    double dSlsThicknessTmp = 0.0;
    if ( !GetSliceThickness( pImage, dSlsThicknessTmp) )
    {
        return false;
    }

    return std::fabs( dLegalError ) > std::fabs( dSlsThicknessTmp - dSlsThickness);
}

int DataCheckingBase::VerifyCommonIllegalCondition_i(
    const std::vector< DicomSlicePtr >& vecImageList, 
    std::string& sReplyInfo )
{
    //检查序列中所有的图像,若其中有二次捕获的图像则为非法序列,
    //以Check在2D中通过保存追加功能产生既有原始图像
    //又有二次捕获的非法序列
    for (auto iter = vecImageList.begin();iter != vecImageList.end();++iter)
    {
        auto sSopClassUID = (*iter)->GetSopClassUID();
        if (SOPClassUID_SecondaryCaptureImageStorage == sSopClassUID)
        {
            sReplyInfo = "UID_AppCommon_Illegal_Series";
            _LOG_DEV_ERROR << "Fail to VerifyDynamicSeries_i_1";
            return -1;
        }
    }

    std::vector< std::vector< DicomSlicePtr > > vecVolumeList;
    if ( !m_DynamicSeriesDataChecking.empty() )
    {
        if ( 0 != m_DynamicSeriesDataChecking( vecImageList, vecVolumeList) )
        {
            sReplyInfo = "UID_AppCommon_Illegal_Series";
            _LOG_DEV_ERROR << "Fail to VerifyDynamicSeries_i_2";
            return -1;
        }
    }
    else
    {
        vecVolumeList.push_back( vecImageList );
    }

    int iFindLegalVolume = -1;
    auto itorBeg = vecVolumeList.begin();
    for ( ; itorBeg != vecVolumeList.end(); ++itorBeg )
    {
        int iRtn = VerifyOneVolume_i( *itorBeg, sReplyInfo );
        if ( 0 > iRtn )
        {
            _LOG_DEV_ERROR << "Fail to VerifyOneVolume_i: Reason:" << sReplyInfo;
            continue;
        }
        iFindLegalVolume = iRtn;
    }
    return iFindLegalVolume;
}

bool DataCheckingBase::SetAdditionalDataChecking(
    const AdditionalDataChecking& additionalDataChecking )
{
    m_AdditionalDataChecking = additionalDataChecking;
    return true;
}

void DataCheckingBase::SetDynamicSeriesChecking(
    const DynamicSeriesDataChecking& dynamicSeriesDataChecking )
{
    m_DynamicSeriesDataChecking = dynamicSeriesDataChecking;
}

int DataCheckingBase::VerifyModality_i(
    const std::vector< DicomSlicePtr >& vecImageList )
{
    auto itorBeg = vecImageList.begin();
    auto pFstImg = *itorBeg;
    ++itorBeg;
    std::string sSopClassUID = pFstImg->GetSopClassUID();
    if ( sSopClassUID.empty() )
    {
        return -1;
    }
    auto itorEnd = vecImageList.end();
    for ( ; itorBeg != itorEnd; ++itorBeg )
    {
        std::string sSopClassUIDTmp = (*itorBeg)->GetSopClassUID();
        if ( sSopClassUIDTmp != sSopClassUID )
        {
            return -1;
        }
    }
    return 0;
}

int DataCheckingBase::VerifyOneVolume_i(
    const std::vector< DicomSlicePtr >& vecOneVolume, std::string& sReplyInfo )
{
    int iRtn = 0;
    size_t uiImgCount = vecOneVolume.size();
    if ( 3 > uiImgCount )
    {
        sReplyInfo = "UID_AppCommon_Illegal_Series";
        return -1;
    }
    // Verify Image size, Image orientation and if images are Secondary Capture
    auto pImage = vecOneVolume.at( 0 );
    if ( !pImage )
    {
        sReplyInfo = "UID_AppCommon_Illegal_Image";
        return -1;
    }

    std::string sSopClassUID = pImage->GetSopClassUID();
    if ( sSopClassUID == SOPClassUID_SecondaryCaptureImageStorage )
    {
        sReplyInfo = "UID_AppCommon_Image_Is_SC";
        return -1;
    }

    if ( 0 != VerifyModality_i( vecOneVolume ) )
    {
        sReplyInfo = "UID_AppCommon_Sop_Class_UID_Not_Consistent";
        return -1;
    }

    // Get 1st image's ImagePositionPatient, size and PixelSpacing
    double dFstImagePostion[3]={0.0};
    double dFstIOP[6] = { 0.0 };
    int    iFstImageSize[ 2 ] = { 0 };
    double dPixelSpacing[ 2 ] = { 0.0 };
    double dSlsThickness = 0.0;
    GetImageSize_i( pImage, iFstImageSize);


    if (!GetImagePositionPatient(pImage, dFstImagePostion))
    {
        sReplyInfo= "UID_AppCommon_Fail_To_Get_Image_Position";
        return -1;
    }


    if ( !GetImageOrientationPatient( pImage, dFstIOP) )
    {
        sReplyInfo= "UID_AppCommon_Fail_To_Get_Image_Orientation_Patient";
        return -1;
    }


    if ( !GetPixelSpacing( pImage, dPixelSpacing) )
    {
        sReplyInfo = "UID_AppCommon_Fail_To_Get_Pixel_Spacing";
        return -1;
    }

    if ( !DoubleEqual( dPixelSpacing[ 0 ], dPixelSpacing[ 1 ], 0.001) )
    {
        sReplyInfo = "UID_AppCommon_Pixel_Spacing_Not_Equal";
        iRtn = 1;
    }

    if ( !GetSliceThickness( pImage, dSlsThickness) )
    {
        sReplyInfo = "UID_AppCommon_Fail_To_Get_Slice_Thickness";
        return -1;
    }

    for ( size_t uiTmp = 1; uiTmp < uiImgCount; ++uiTmp )
    {
        pImage = vecOneVolume.at( uiTmp );
        if ( !pImage )
        {
            sReplyInfo = "UID_AppCommon_Illegal_Image";
            return -1;
        }

        double dTestImagePostion[3]={0.0};
        double dTestIOP[6] = { 0.0 };
        double dTestPixelSpacing[ 2 ] = { 0.0 }; 

        if (!GetImagePositionPatient(pImage, dTestImagePostion))
        {
            sReplyInfo= "UID_AppCommon_Fail_To_Get_Image_Position";
            return -1;
        }

        if ( !GetImageOrientationPatient( pImage, dTestIOP) )
        {
            sReplyInfo= "UID_AppCommon_Fail_To_Get_Image_Orientation_Patient";
            return -1;
        }

        if ( !GetPixelSpacing( pImage, dTestPixelSpacing) )
        {
            sReplyInfo = "UID_AppCommon_Fail_To_Get_Pixel_Spacing";
            return -1;
        }

        if ( !IsImageSizeEqual_i( iFstImageSize, pImage) )
        {
            sReplyInfo = "UID_AppCommon_Image_Size_Not_Equal";
            return -1;
        }
        /*
        if ( !IsIOPEqual_i( dFstIOP, dTestIOP, LegalRoundOffError(pImage) ) )
        {
            sReplyInfo = "UID_AppCommon_Image_Orientation_Not_Consistent";
            return -1;
        }
        
        if (!VerifyPixelSpacing_i( dPixelSpacing,dTestPixelSpacing , LegalRoundOffError(pImage)))
        {
            sReplyInfo = "UID_AppCommon_Size_Pixel_Spacing_Not_Equal";
            return -1;
        }
        
        if ( !IsSliceThicknessEqual_i( pImage, dSlsThickness, LegalRoundOffError(pImage) ) )
        {
            sReplyInfo = "UID_AppCommon_Slice_Thickness_Not_Equal";
            return -1;
        }
        */
    }
    return iRtn;
}

int VerifyNonDynamicSeries( 
    const std::vector< DicomSlicePtr >& vecImageList, 
    std::vector< std::vector< DicomSlicePtr > >& vecVolumeList )
{
    std::vector< std::vector< DicomSlicePtr > > vecVolumeListTmp;
    if ( !DynamicSeriesGroup( vecImageList, vecVolumeListTmp ))
    {
        _LOG_DEV_ERROR << "Fail to do DynamicSeriesGroup !";
        return -1;
    }
    if ( vecVolumeListTmp.empty() )
    {
        _LOG_DEV_ERROR << "The count of vecVolumeListTmp is empty!";
        return -1;
    }
    auto itorBeg = vecVolumeListTmp.begin();
    for ( ; itorBeg != vecVolumeListTmp.end(); ++itorBeg )
    {
        auto vecVolume = *itorBeg;
        if ( 3 > vecVolume.size() )
        {
            _LOG_DEV_ERROR << "The count of images in vecVolume is " << vecVolume.size() ;
            continue;
        }
        vecVolumeList.push_back( vecVolume );
    }
    return (vecVolumeList.empty() || 1 < vecVolumeList.size()) ? -1 : 0;
}


END_NAMESPACE
