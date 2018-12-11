#ifndef UAI_DICOM_SLICER_CHECKER_IMAGE_H_
#define UAI_DICOM_SLICER_CHECKER_IMAGE_H_

#include <memory>
#include <string>
#include <ctime>
#include <vector>

#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/noncopyable.hpp"

#include "uai_app_defs.h"
#include "uAIArithmetic/arithmetic.h"

#include "dicom_slice.h"
#include "uai_dicom_slicer_check_api.h"

BEGIN_NAMESPACE
    
class IDataChecking
{
public:
    
    ///\brief: callback function of additionalDataChecking
    typedef boost::function<int(
        const std::vector<DicomSlicePtr>&, 
        std::string&)>     AdditionalDataChecking;
    
    
    typedef   boost::function< int(
        const std::vector<DicomSlicePtr>& vecImageList, 
        std::vector< std::vector< DicomSlicePtr> >& vecVolumeList)>  DynamicSeriesDataChecking;
    
    virtual int DataChecking(
        const std::vector<DicomSlicePtr> vSliceList, 
        std::string& sReplyInfo ) = 0;
    
    ///\brief: Register AdditionalDataChecking Functions
    virtual bool SetAdditionalDataChecking(
        const AdditionalDataChecking& additionalDataChecking) = 0;
    
};

///\brief: 检查同一序列中的DicomSlicer是否满足DataChecking的要求
class DataCheckingBase : public IDataChecking
{
public:
    
    DataCheckingBase();
    
    virtual ~DataCheckingBase();
    
    int DataChecking(
        const std::vector<DicomSlicePtr> vSliceList, 
        std::string& sReplyInfo );
    
    bool SetAdditionalDataChecking(
        const AdditionalDataChecking& additionalDataChecking );
    
    
   void SetDynamicSeriesChecking( 
       const DynamicSeriesDataChecking& dynamicSeriesDataChecking );
    
    //////////////////////////////////////////////////////////////////////////
    /// \brief      As the legal round-off error of different modality is not the same, in the
    ///              derived classes for different modality should rewrite this method
    ///
    /// \return     Legal round-off error
    //////////////////////////////////////////////////////////////////////////
    double LegalRoundOffError(DicomSlicePtr ptrSlice);
    
protected:
    
    //////////////////////////////////////////////////////////////////////////
    /// \brief      To verify illegal condition for MR, CT and PET, Image size and
    ///             Image Orientation Patient are been verified in this method
    ///
    /// \param[in]  vecImageList, all images of the selected series
    /// \param[out] sReplyInfo, a string which depict the result for NLS
    ///
    /// \return     0, if the selected series is legal for 3D reconstruct
    ///             -1, if the selected series is illegal
    ///
    /// 
    //////////////////////////////////////////////////////////////////////////
    virtual int VerifyCommonIllegalCondition_i(
        const std::vector< DicomSlicePtr >& vecImageList,
        std::string& sReplyInfo );
    
    //////////////////////////////////////////////////////////////////////////
    /// \brief      This method is used to verify if all modality of images in the imagelist
    ///             is same
    ///
    /// \param[in]  vecImageList, all images of the selected series
    ///
    /// \return     0, if the modality of all images in vecImageList is same
    ///             -1, otherwise
    //////////////////////////////////////////////////////////////////////////
    int VerifyModality_i( const std::vector< DicomSlicePtr >& vecImageList );
    
    
    int VerifyOneVolume_i( const std::vector< DicomSlicePtr >& vecOneVolume, 
        std::string& sReplyInfo );
    
private:
    AdditionalDataChecking     m_AdditionalDataChecking;
    DynamicSeriesDataChecking  m_DynamicSeriesDataChecking;
    
};

END_NAMESPACE
#endif
