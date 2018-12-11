
#include "dicom_slice.h"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdatset.h" 
#include "dcmtk/dcmdata/dcelem.h"

#include "common_functor.h"

//#include "uAIDicomParser/dicom_suv_calculator.h"

BEGIN_NAMESPACE;

DicomSlice::DicomSlice( 
const boost::shared_ptr<DcmDataset>& dcmDataset,
const int& iFrameIndex/*=0*/)
:m_spDataset(dcmDataset),
m_sPatientID(""),
m_sSeriesUID(""),
m_sStudyUID(""),
m_sStudyDescription(""),
m_sStudyDate(""),
m_sStudyTime(""),
m_sPatientName(""),
m_sPatientSex("M"),
m_sPatientBirthDate(""),
m_sSeriesDescription(""),
m_sModality("CT"),
m_sInstitutionName(""),
m_sManufacturer(""),
m_iSeriesNumber(0),
m_sInstanceUID(""),
m_iInstanceNumber(iFrameIndex + 1),
m_iNumberOfFrames(1),
m_iRow(512),
m_iColumn(512),
m_iSamplePerPixel(1),
m_iBitAllocated(16),
m_iPixelRepresentation(0),
m_dSlop(1.0),
m_dIntercept(0.0),
m_dMinScalar(0.0),
m_dMaxScalar(0.0),
m_sSopClassUID(""),
m_sFrameOfReferenceUID(""),
m_bExtractedWL(false),
m_dWW(400),
m_dWC(30),
m_pPixelPointer(nullptr),
m_ulPixelDataSize(0)
{
	
	m_spDataset = dcmDataset;
	//ReadDICOMTag_i();
}


DicomSlice::DicomSlice():
m_sPatientID(""),
    m_sSeriesUID(""),
    m_sStudyUID(""),
    m_sStudyDescription(""),
    m_sStudyDate(""),
    m_sStudyTime(""),
    m_sPatientName(""),
    m_sPatientSex("M"),
    m_sPatientBirthDate(""),
    m_sSeriesDescription(""),
    m_sModality("CT"),
	m_sInstitutionName(""),
	m_sManufacturer(""),
    m_iSeriesNumber(0),
    m_sInstanceUID(""),
    m_iInstanceNumber(0),
    m_iNumberOfFrames(1),
    m_iRow(512),
    m_iColumn(512),
    m_iSamplePerPixel(1),
    m_iBitAllocated(16),
    m_iPixelRepresentation(0),
    m_dSlop(1.0),
    m_dIntercept(0.0),
    m_dMinScalar(0.0),
    m_dMaxScalar(0.0),
    m_sSopClassUID(""),
    m_sFrameOfReferenceUID(""),
    m_bExtractedWL(false),
    m_dWW(400),
    m_dWC(30),
    m_pPixelPointer(nullptr),
    m_ulPixelDataSize(0)
{

}

 DicomSlice::~DicomSlice( void )
 {
 
 }
 
 template<typename T>
 void DicomSlice::FindMinMax_i( T *pData )
 {
	 if(nullptr == pData)
	 {
		 return;
	 }
	 T min =  *pData, max = *pData;
	 size_t size = m_iRow * m_iColumn;

	 for(int i = 1;i < size; i++)
	 {
		 T temp = *(pData + i);
		 min = temp < min ? temp : min;
		 max = temp > max ? temp : max;
	 }

	 m_dMinScalar = static_cast<double>(min);
	 m_dMaxScalar = static_cast<double>(max);
 }



 template<typename T>
 void DicomSlice::ModifyPaddingValue_i( T *pData, const int iLimit, const int iValue ) 
 { 
     int iCount = m_iRow * m_iColumn; 
     for (int i = 0; i < iCount; ++i) 
     { 
         if (pData[i] > iLimit) 
         { 
             pData[i] = iValue; 
         } 
     } 
 }

 template<typename T>
 bool DicomSlice::GetPixelValue_i( T *pData, const int& iX, const int& iY,const int iSample, double* dValue )
 {
	 size_t iIndex = (iY * m_iColumn + iX) * iSample;
	 
	 for (int i = 0; i < iSample; ++i)
	 {
		 *(dValue + i) = pData[iIndex + i]; 
	 }
	 return true;
 }

 boost::shared_ptr<DcmDataset> DicomSlice::GetDcmDataset() const
 {
	 return m_spDataset;
 }

 boost::shared_ptr<DcmDataset> DicomSlice::GetFrameDataset() const
 {
	 return m_spFrame;
 }
 
 bool DicomSlice::ReadImageLevel_i()
 {
     OFString sInstanceUID;
     if(m_spDataset->findAndGetOFString(DCM_SOPInstanceUID, sInstanceUID).good())
     {
         m_sInstanceUID = sInstanceUID.c_str();
     }

     OFString sSopClassUID;
     if(m_spDataset->findAndGetOFString(DCM_SOPClassUID, sSopClassUID).good())
     {
         m_sSopClassUID = sSopClassUID.c_str();
     }

     Sint32 iNumberOfFrams;
     if (m_spDataset->findAndGetSint32(DCM_NumberOfFrames, iNumberOfFrams).good())
     {
         m_iNumberOfFrames = (int)iNumberOfFrams;
     }

     Uint16 iRow;
     if(m_spDataset->findAndGetUint16(DCM_Rows, iRow).good())
     {
         m_iRow = (int)iRow;
     }

     Uint16 iCol;
     if(m_spDataset->findAndGetUint16(DCM_Columns, iCol).good())
     {
         m_iColumn = (int)iCol;
     }

     Uint16 iSamplesPerPixel;
     if(m_spDataset->findAndGetUint16(DCM_SamplesPerPixel, iSamplesPerPixel).good())
     {
         m_iSamplePerPixel = (int)iSamplesPerPixel;
     }

     Uint16 iBitAllocated;
     if(m_spDataset->findAndGetUint16(DCM_BitsAllocated, iBitAllocated).good())
     {
         m_iBitAllocated = (int)iBitAllocated;
     }

     Uint16 iBitsStored;
     if(m_spDataset->findAndGetUint16(DCM_BitsStored, iBitsStored).good())
     {
         m_iBitsStored = (int)iBitsStored;
     }

     Uint16 iPixelRepresentation;
     if(m_spDataset->findAndGetUint16(DCM_PixelRepresentation, iPixelRepresentation).good())
     {
         m_iPixelRepresentation = iPixelRepresentation;
     }


     OFString sFrameOfReferenceUID;
     if(m_spDataset->findAndGetOFString(DCM_FrameOfReferenceUID, sFrameOfReferenceUID).good())
     {
         m_sFrameOfReferenceUID = sFrameOfReferenceUID.c_str();
     }

     {
         //Sint32 iInstanceNumber;
         //if(m_spDataset->findAndGetSint32(DCM_InstanceNumber, iInstanceNumber).good())
         //{
         //    m_iInstanceNumber = (int)iInstanceNumber;
         //}
     }
     DcmElement* element = NULL;  
     OFCondition result = m_spDataset->findAndGetElement(DCM_PixelData, element); //获取图像数据  
     if (result.good() && element != NULL)  
     {
         Uint8* image_data = nullptr;  
         result = element->getUint8Array(image_data);     //获取8位的图像数据 
         if (result.good() && nullptr != image_data )
         {
             m_pPixelPointer = image_data;
         }
         else
         {
             printf("DCM_PixelData is not found with m_sSopClassUID=%s", m_sSopClassUID.c_str());
             return false;
         }
     }
     else
     {
         printf("DCM_PixelData is not found with m_sSopClassUID=%s", m_sSopClassUID.c_str());
         return false;
     }

     // row column should have value
     m_ulPixelDataSize = m_iRow * m_iColumn * (m_iBitAllocated >> 3);

     auto pReadDataset = m_spDataset;
     if (m_iNumberOfFrames > 1)
     {
         long long iOffset = (m_iInstanceNumber - 1) * m_iRow * m_iColumn * m_iSamplePerPixel * (long long)m_iBitAllocated / 8;
         m_pPixelPointer = (char*)m_pPixelPointer + iOffset;


         // 拆分功能组 shared以及per_frame
         m_spFrame.reset((DcmDataset*)(m_spDataset->clone()));
         pReadDataset = m_spFrame;

         std::pair<DcmTagKey, int> pSearchArray[2];
         pSearchArray[0] = std::make_pair(DCM_SharedFunctionalGroupsSequence, 0);
         pSearchArray[1] = std::make_pair(DCM_PerFrameFunctionalGroupsSequence, m_iInstanceNumber - 1);
         for (int i = 0 ; i < 2; ++i)
         {
             DcmItem* pSearchGroupElement = NULL;
             if (m_spFrame->findAndGetSequenceItem(pSearchArray[i].first,pSearchGroupElement, pSearchArray[i].second).good())
             {
                 auto pElement = pSearchGroupElement->nextInContainer(nullptr);
                 while(pElement)
                 {
                     auto pSubElement = pElement->nextInContainer(nullptr);
                     while(pSubElement)
                     {
                         auto pSpElement = pSubElement->nextInContainer(nullptr);
                         while(pSpElement)
                         {
                             if (m_spFrame->insert((DcmElement*)pSpElement->clone()).good())
                             {
                                 printf("insert success\n");
                             }
                             pSpElement = pSubElement->nextInContainer(pSpElement);
                         }
                         pSubElement = pElement->nextInContainer(pSubElement);
                     }
                     pElement = pSearchGroupElement->nextInContainer(pElement);
                 }
             }
         }

         // 删除数据以及功能组
         m_spFrame->findAndDeleteElement(DCM_PixelData);
         m_spFrame->findAndDeleteElement(DCM_SharedFunctionalGroupsSequence);
         m_spFrame->findAndDeleteElement(DCM_PerFrameFunctionalGroupsSequence);
     }
     else
     {
         Sint32 iInstanceNumber;
         if(m_spDataset->findAndGetSint32(DCM_InstanceNumber, iInstanceNumber).good())
         {
             m_iInstanceNumber = (int)iInstanceNumber;
         }
     }



     OFString sPixelSpace;
     if(pReadDataset->findAndGetOFStringArray(DCM_PixelSpacing, sPixelSpace).good())
     {
         // 0.5\0.5
         std::string PixelSpacing = sPixelSpace.c_str();
         std::vector<std::string> vecPixelSpace = Split(PixelSpacing, "\\");
         if(2 == vecPixelSpace.size())
         {
             m_dSpacing[0] = atof(vecPixelSpace[0].c_str());
             m_dSpacing[1] = atof(vecPixelSpace[1].c_str());
         }
     }

     OFString sWindowWidth,sWindowCenter;
     if(pReadDataset->findAndGetOFString(DCM_WindowWidth, sWindowWidth).good()
         && pReadDataset->findAndGetOFString(DCM_WindowCenter, sWindowCenter).good())
     {
         std::string sTemp1 = sWindowWidth.c_str();
         std::vector<std::string> vecTemp1 = Split(sTemp1, "\\");

         std::string sTemp2 = sWindowCenter.c_str();
         std::vector<std::string> vecTemp2 = Split(sTemp2, "\\");

         if(!vecTemp1.empty() && vecTemp1.size() == vecTemp2.size())
         {
             m_bExtractedWL = true;
             m_dWW = atof(vecTemp1[0].c_str());
             m_dWC = atof(vecTemp2[0].c_str());
         }
     }



     OFString sSlop;
     if(pReadDataset->findAndGetOFString(DCM_RescaleSlope, sSlop).good())
     {
         m_dSlop = atof(sSlop.c_str());
     }

     OFString sIntercept;
     if(pReadDataset->findAndGetOFString(DCM_RescaleIntercept, sIntercept).good())
     {
         m_dIntercept = atof(sIntercept.c_str());
     }


     OFString sSliceThickness;
     if(pReadDataset->findAndGetOFString(DCM_SliceThickness, sSliceThickness).good())
     {
         m_SliceThickness = sSliceThickness.c_str();
     }

#pragma region image level

     OFString sImagePosition;
     if(pReadDataset->findAndGetOFStringArray(DCM_ImagePositionPatient, sImagePosition).good())
     {
         std::string m_sImagePositionPatient = sImagePosition.c_str();
         std::vector<std::string> vecImagePosition =Split(m_sImagePositionPatient, "\\");
         if(3 == vecImagePosition.size())
         {
             m_ImagePositionPatient.x = atof(vecImagePosition[0].c_str());
             m_ImagePositionPatient.y = atof(vecImagePosition[1].c_str());
             m_ImagePositionPatient.z = atof(vecImagePosition[2].c_str());
         }
     }

     OFString sImageOritentation;
     if(pReadDataset->findAndGetOFStringArray(DCM_ImageOrientationPatient, sImageOritentation).good())
     {
         std::string ImageOrientationPatient = sImageOritentation.c_str();
         std::vector<std::string> vecImageOritentation = Split(ImageOrientationPatient, "\\");
         if(6 == vecImageOritentation.size())
         {
             m_vetRow.x = atof(vecImageOritentation[0].c_str());
             m_vetRow.y = atof(vecImageOritentation[1].c_str());
             m_vetRow.z = atof(vecImageOritentation[2].c_str());

             m_vetColumn.x = atof(vecImageOritentation[3].c_str());
             m_vetColumn.y = atof(vecImageOritentation[4].c_str());
             m_vetColumn.z = atof(vecImageOritentation[5].c_str());
         }

     }

#pragma endregion image level


     // 计算Pixel2Patient
     m_matPixel2Patient.SetIdintity();
     auto vetZ = m_vetRow.CrossProduct(m_vetColumn);
     vetZ.Normalize();
     m_matPixel2Patient.m[0][0] = m_vetRow.x * m_dSpacing[1];
     m_matPixel2Patient.m[0][1] = m_vetRow.y * m_dSpacing[1];
     m_matPixel2Patient.m[0][2] = m_vetRow.z * m_dSpacing[1];

     m_matPixel2Patient.m[1][0] = m_vetColumn.x * m_dSpacing[0];
     m_matPixel2Patient.m[1][1] = m_vetColumn.y * m_dSpacing[0];
     m_matPixel2Patient.m[1][2] = m_vetColumn.z * m_dSpacing[0];

     m_matPixel2Patient.m[2][0] = vetZ.x;
     m_matPixel2Patient.m[2][1] = vetZ.y;
     m_matPixel2Patient.m[2][2] = vetZ.z;

     m_matPixel2Patient.m[3][0] = m_ImagePositionPatient.x;
     m_matPixel2Patient.m[3][1] = m_ImagePositionPatient.y;
     m_matPixel2Patient.m[3][2] = m_ImagePositionPatient.z;

     // 计算Pixel2World
     // 由于Spacing相同所以仅需要考虑Pixel的变换
     double dXLength = m_iColumn * m_dSpacing[1];
     double dYLength = m_iRow	* m_dSpacing[0];

     double dMaxLength = dXLength > dYLength ? dXLength : dYLength;
     m_matPixel2World.SetIdintity();
     Vector2D vetCenter(-(m_iColumn - 1) / 2.0,-(m_iRow - 1) / 2.0);
     m_matPixel2World.Prepend(Matrix3x3::MakeTranslate(vetCenter));
     Vector2D vetScale(m_dSpacing[0] / dMaxLength,m_dSpacing[1] / dMaxLength);
     m_matPixel2World.Prepend(Matrix3x3::MakeScale(vetScale));

     int iPaddingValue = 0; 
     bool bFindPadding = false; Uint16 uiValue = 0; Sint16 siValue = 0; 
     if (m_spDataset->findAndGetUint16(DCM_PixelPaddingValue, uiValue).good()) 
     { 
         bFindPadding = true; 
         iPaddingValue = uiValue; 
     } 
     else if (m_spDataset->findAndGetSint16(DCM_PixelPaddingValue, siValue).good()) 
     { 
         bFindPadding = true; 
         iPaddingValue = siValue; 
     } 

     if(bFindPadding)
     {
         Uint16 iHighBit = 0; 
         if(m_spDataset->findAndGetUint16(DCM_HighBit, iHighBit).bad()) 
         { 
             iHighBit = m_iBitAllocated - 1; 
         } 
         int iLimit = (int)(std::pow(2.0, (int)iHighBit) + 0.5); 
         if (m_spDataset->findAndGetUint16(DCM_PixelPaddingRangeLimit, uiValue).good()) 
         { 
             iLimit = uiValue; 
         } 
         else if (m_spDataset->findAndGetSint16(DCM_PixelPaddingRangeLimit, siValue).good()) 
         { 
             iLimit = uiValue; 
         } 
         if(m_iBitAllocated == 16 && m_iPixelRepresentation == 0)
         {
             ModifyPaddingValue_i((unsigned short*)(m_pPixelPointer), iLimit, iPaddingValue); 
         }
         else if(m_iBitAllocated == 16 && m_iPixelRepresentation == 1)
         {
             ModifyPaddingValue_i((short*)(m_pPixelPointer), iLimit, iPaddingValue); 
         }
     }
     if (m_iSamplePerPixel == 1)
     {
         if (m_iBitAllocated == 8 && m_iPixelRepresentation == 0)
         {
             FindMinMax_i((unsigned char*)(m_pPixelPointer));
         }
         else if (m_iBitAllocated == 8 && m_iPixelRepresentation == 1)
         {
             FindMinMax_i((char*)(m_pPixelPointer));
         }
         else if (m_iBitAllocated == 16 && m_iPixelRepresentation == 0)
         {
             FindMinMax_i((unsigned short*)(m_pPixelPointer));
         }
         else if (m_iBitAllocated == 16 && m_iPixelRepresentation == 1)
         {
             FindMinMax_i((short*)(m_pPixelPointer));
         }
     }
     return true;
 }

 bool DicomSlice::ReadDICOMTag()
 {
 	if (!m_spDataset)
 	{
 		return false;
 	}


	OFString sPatientID;
	if(m_spDataset->findAndGetOFString(DCM_PatientID, sPatientID).good())
	{
		m_sPatientID = sPatientID.c_str();
	}

 	//OFString sPatientName;
 	//if(m_spDataset->findAndGetOFString(DCM_PatientName, sPatientName).good())
 	//{
 	//	m_sPatientName = sPatientName.c_str();
		//m_sPatientName = ChangeTextFromUTF8ToAnsi(m_sPatientName);
 	//}

    //Anonymous dicom by qiangqiang.zhou@20180827
    m_sPatientName = "Anonymous";
 
 	OFString sPatientSex;
 	if(m_spDataset->findAndGetOFString(DCM_PatientSex, sPatientSex).good())
 	{
 		m_sPatientSex = sPatientSex.c_str();
 	}

	OFString sPatientBirthDate;
	if(m_spDataset->findAndGetOFString(DCM_PatientBirthDate, sPatientBirthDate).good())
	{
		m_sPatientBirthDate = sPatientBirthDate.c_str();
	}
 
 	OFString sStudyUID;
 	if(m_spDataset->findAndGetOFString(DCM_StudyInstanceUID, sStudyUID).good())
 	{
 		m_sStudyUID = sStudyUID.c_str();
 	}


	OFString sStudyDescription;
	if(m_spDataset->findAndGetOFString(DCM_StudyDescription, sStudyDescription).good())
	{
		m_sStudyDescription = sStudyDescription.c_str();
		m_sStudyDescription = ChangeTextFromUTF8ToAnsi(m_sStudyDescription);
	}

	OFString sStudyDate;
	if(m_spDataset->findAndGetOFString(DCM_StudyDate, sStudyDate).good())
	{
		m_sStudyDate = sStudyDate.c_str();
	}

	OFString sSeriesDate;
	if(m_spDataset->findAndGetOFString(DCM_SeriesDate, sSeriesDate).good())
	{
		m_sSeriesDate = sSeriesDate.c_str();
	}

	OFString sStudyTime;
	if(m_spDataset->findAndGetOFString(DCM_StudyTime, sStudyTime).good())
	{
		m_sStudyTime = sStudyTime.c_str(); 
	}

	OFString sSeriesTime;
	if(m_spDataset->findAndGetOFString(DCM_SeriesTime, sSeriesTime).good())
	{
		m_sSeriesTime = sSeriesTime.c_str();
	}
 
 	OFString sSeriesUID;
 	if(m_spDataset->findAndGetOFString(DCM_SeriesInstanceUID, sSeriesUID).good())
 	{
 		m_sSeriesUID = sSeriesUID.c_str();
 	}


#pragma region series level

    OFString sSeriesDescription;
    if(m_spDataset->findAndGetOFString(DCM_SeriesDescription, sSeriesDescription).good())
    {
        m_sSeriesDescription = sSeriesDescription.c_str();
        m_sSeriesDescription = ChangeTextFromUTF8ToAnsi(m_sSeriesDescription);
    }

    OFString sModality;
    if(m_spDataset->findAndGetOFString(DCM_Modality, sModality).good())
    {
        m_sModality = sModality.c_str();
    }

	OFString sInstitutionName;
	if(m_spDataset->findAndGetOFString(DCM_InstitutionName,sInstitutionName).good())
	{
		m_sInstitutionName = sInstitutionName.c_str();
	}

	OFString sManufacturer;
	if(m_spDataset->findAndGetOFString(DCM_Manufacturer,sManufacturer).good())
	{
		m_sManufacturer = sManufacturer.c_str();
	}

    OFString sPatientPosition;
    if(m_spDataset->findAndGetOFString(DCM_PatientPosition, sPatientPosition).good())
    {
        m_sPatientPosition = sPatientPosition.c_str();
    }

    OFString BodyPartExamined;
    if(m_spDataset->findAndGetOFString(DCM_BodyPartExamined, BodyPartExamined).good())
    {
        m_sBodyPartExamined = BodyPartExamined.c_str();
    }
    OFString ProtocolName;
    if(m_spDataset->findAndGetOFString(DCM_ProtocolName, ProtocolName).good())
    {
        m_sProtocolName = ProtocolName.c_str();
    }  	
    Sint32 iSeriesNumber;
 	if(m_spDataset->findAndGetSint32(DCM_SeriesNumber, iSeriesNumber).good())
 	{
 		m_iSeriesNumber = (int)iSeriesNumber;
 	}

#pragma endregion series level

	

    return ReadImageLevel_i();

	
 }
 

 std::string DicomSlice::GetInstanceUID() const
 {
	return m_sInstanceUID;
 }


 std::string DicomSlice::GetSeriesUID() const
 {
 	return m_sSeriesUID;
 }
 
 std::string DicomSlice::GetSeriesDescription() const
 {
	 return m_sSeriesDescription;
 }

 std::string DicomSlice::GetStudyUID() const
 {
 	return m_sStudyUID;
 }

 std::string DicomSlice::GetStudyDescription() const
 {
	 return m_sStudyDescription;
 }

 std::string DicomSlice::GetStudyDate() const
 {
	 return m_sStudyDate;
 }

 std::string DicomSlice::GetStudyTime() const
 {
	 return m_sStudyTime;
 }
 
 int DicomSlice::GetSeriesNumber() const
 {
 	return m_iSeriesNumber;
 }

 std::string DicomSlice::GetSeriesDate() const
 {
	 return m_sSeriesDate;
 }

 std::string DicomSlice::GetSeriesTime() const
 {
	 return m_sSeriesTime;
 }
 
 std::string DicomSlice::GetPatientID() const
 {
	 return m_sPatientID;
 }

 std::string DicomSlice::GetPatientUID() const
 {
     return m_sPatientUID;
 }

 std::string DicomSlice::GetPatientName() const
 {
 	return m_sPatientName;
 }
 
 std::string DicomSlice::GetPatientSex() const
 {
 	return m_sPatientSex;
 }

 std::string DicomSlice::GetPatientBirthDate() const
 {
	 return m_sPatientBirthDate;
 }
 
 std::string DicomSlice::GetModality() const
 {
 	return m_sModality;
 }

 std::string DicomSlice::GetInstitutionName() const
 {
	 return m_sInstitutionName;
 }

 std::string DicomSlice::GetManufactuer() const
 {
	 return m_sManufacturer;
 }
 
 int DicomSlice::GetInstanceNumber() const
 {
 	return m_iInstanceNumber;
 }
 
 int DicomSlice::GetNumberOfFrames() const
 {
 	return m_iNumberOfFrames;
 }

 int DicomSlice::GetRow() const
 {
	 return m_iRow;
 }

 int DicomSlice::GetColumn() const
 {
	 return m_iColumn;
 }

 int DicomSlice::GetSamplesPerPixel() const
 {
	 return m_iSamplePerPixel;
 }

 int DicomSlice::GetBitAllocated() const
 {
	 return m_iBitAllocated;
 }

 int DicomSlice::GetPixelRepresentation() const
 {
	 return m_iPixelRepresentation;
 }

 void DicomSlice::GetPixelSpacing( double* dSpacing ) const
 {
	 std::memcpy(dSpacing, m_dSpacing, 2 * sizeof(double));
 }
 
 Point3D DicomSlice::GetImagePositionPatient() const
 {
 	return m_ImagePositionPatient;
 }
 
 void DicomSlice::GetImageOrientationPatient( 
	 Vector3D& rowDir, 
	 Vector3D& columnDir ) const
 {
	 rowDir = m_vetRow;
	 columnDir = m_vetColumn;
 }

 void* DicomSlice::GetPixelPointer() const
 {
	 return m_pPixelPointer;
 }

 double DicomSlice::GetSlop() const
 {
	 return m_dSlop;
 }

 double DicomSlice::GetIntercept() const
 {
	 return m_dIntercept;
 }

 Matrix4x4 DicomSlice::GetImage2Patient() const
 {
	 return m_matPixel2Patient;
 }

 Matrix3x3 DicomSlice::GetImage2World() const
 {
	 return m_matPixel2World;
 }

 void DicomSlice::SetImage2World(
	 Matrix3x3& matImage2World )
 {
	 m_matPixel2World = matImage2World;
 }

 bool DicomSlice::CalculateMinMax()
 {
	 if (m_iSamplePerPixel != 1)
	 {
		 return false;
	 }
	 if (m_iBitAllocated == 8 && m_iPixelRepresentation == 0)
	 {
		 FindMinMax_i((unsigned char*)(m_pPixelPointer));
	 }
	 else if (m_iBitAllocated == 8 && m_iPixelRepresentation == 1)
	 {
		 FindMinMax_i((char*)(m_pPixelPointer));
	 }
	 else if (m_iBitAllocated == 16 && m_iPixelRepresentation == 0)
	 {
		 FindMinMax_i((unsigned short*)(m_pPixelPointer));
	 }
	 else if (m_iBitAllocated == 16 && m_iPixelRepresentation == 1)
	 {
		 FindMinMax_i((short*)(m_pPixelPointer));
	 }
	 else
	 {
		 return false;
	 }
	 return true;
 }

 double DicomSlice::GetMinScalar() const
 {
	  return m_dMinScalar;
 }

 double DicomSlice::GetMaxScalar() const
 {
	 return m_dMaxScalar;
 }

 std::string DicomSlice::GetSopClassUID() const
 {
	 return m_sSopClassUID;
 }

 std::string DicomSlice::GetFrameOfReferenceUID() const
 {
	 return m_sFrameOfReferenceUID;
 }

 bool DicomSlice::GetWindowLevel( double& dWW, double& dWC )
 {
	dWW =m_dWW;dWC = m_dWC;
	return m_bExtractedWL;
 }

 bool DicomSlice::GetPixelValue( const int& iX,const int& iY, double* dValue )
 {
	 if (iX >= m_iColumn || iY >= m_iRow)
	 {
		 return false;
	 }

	 if (m_iSamplePerPixel == 1)
	 {
		 if (m_iBitAllocated == 8 && m_iPixelRepresentation == 0)
		 {
			 return GetPixelValue_i((unsigned char*)(m_pPixelPointer), iX, iY, m_iSamplePerPixel, dValue);
		 }
		 else if (m_iBitAllocated == 8 && m_iPixelRepresentation == 1)
		 {
			 return GetPixelValue_i((char*)(m_pPixelPointer),iX, iY, m_iSamplePerPixel, dValue);
		 }
		 else if (m_iBitAllocated == 16 && m_iPixelRepresentation == 0)
		 {
			 return GetPixelValue_i((unsigned short*)(m_pPixelPointer),iX, iY, m_iSamplePerPixel, dValue);
		 }
		 else if (m_iBitAllocated == 16 && m_iPixelRepresentation == 1)
		 {
			 return GetPixelValue_i((short*)(m_pPixelPointer),iX, iY, m_iSamplePerPixel, dValue);
		 }
		 else
		 {
			 return false;
		 }
	 }
	 else if (m_iSamplePerPixel == 3)
	 {
		 return GetPixelValue_i((unsigned char*)(m_pPixelPointer), iX, iY, m_iSamplePerPixel, dValue);
	 }
	 else
	 {
		 return false;
	 }
 }

 bool DicomSlice::FindAndGetString(
	 const unsigned short& groupID,
	 const unsigned short& elmtID,
	 std::string& sResult )
 {
	 DcmElement *pDcmElement;
	 auto pReadDataset = m_spFrame ? m_spFrame : m_spDataset;
	 OFCondition oc = pReadDataset->findAndGetElement(
		 DcmTagKey(groupID, elmtID),pDcmElement);
	 if (oc.bad())
	 {
		 return false;
	 }
	 // get value representation
	 DcmEVR mVR(pDcmElement->getVR());
	 switch (mVR)
	 {
	 case EVR_SS:/// signed short
		 {
			 Sint16 iValue = 0;
			 oc = pDcmElement->getSint16(iValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 char cValue[20];
			 sprintf(cValue,"%d",iValue);
			 sResult = std::string(cValue);
		 }
		 break;
	 case EVR_US:/// unsigned short
		 {
			 Uint16 iValue = 0;
			 oc = pDcmElement->getUint16(iValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 char cValue[20];
			 sprintf(cValue,"%d",iValue);
			 sResult = std::string(cValue);
		 }
		 break;
	 case EVR_SL:/// signed long
		 {
			 Sint32 iValue = 0;
			 oc = pDcmElement->getSint32(iValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 char cValue[32];
			 sprintf(cValue,"%d",iValue);
			 sResult = std::string(cValue);
		 }
		 break;
	 case EVR_UL:/// unsigned long
		 {
			 Uint32 iValue = 0;
			 oc = pDcmElement->getUint32(iValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 char cValue[32];
			 sprintf(cValue,"%d",iValue);
			 sResult = std::string(cValue);
		 }
		 break;
	 case EVR_FL:/// float single-precision
		 {
			 Float32 fValue = 0;
			 oc = pDcmElement->getFloat32(fValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 char cValue[32];
			 sprintf(cValue,"%.2f",fValue);
			 sResult = std::string(cValue);
		 }
		 break;
	 case EVR_FD:/// float double-precision
		 {
			 Float64 fValue = 0;
			 oc = pDcmElement->getFloat64(fValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 char cValue[32];
			 sprintf(cValue,"%.2f",fValue);
			 sResult = std::string(cValue);
		 }
		 break;
	 case EVR_OF:/// other float
		 {
			 Float32 fValue = 0;
			 oc = pDcmElement->getFloat32(fValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 char cValue[32];
			 sprintf(cValue,"%.2f",fValue);
			 sResult = std::string(cValue);
		 }
		 break;
	 case EVR_DT:/// date time string
	 case EVR_DS:/// decimal string
	 case EVR_IS:/// integer string
	 case EVR_DA:/// date string
	 case EVR_TM:/// time string
	 case EVR_AS:/// age string
	 case EVR_CS:/// code string
		 {
			 OFString sValue;
			 oc = pDcmElement->getOFStringArray(sValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 sResult = std::string(sValue.c_str());
		 }
		 break;
	 case EVR_AE:/// application entity title
	 case EVR_LO:/// long string
	 case EVR_LT:/// long text
	 case EVR_PN:/// person name
	 case EVR_SH:/// short string
	 case EVR_ST:/// short text
	 case EVR_UI:/// unique identifier
	 case EVR_UT:/// unlimited text
		 {
			 OFString sValue;
			 oc = pDcmElement->getOFStringArray(sValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 sResult = std::string(sValue.c_str());
			 sResult = ChangeTextFromUTF8ToAnsi(sResult);
		 }
		 break;
	 default:// VR is always Unknown or UN
		 return false;
		 break;
	 }
	 return true;
 }

 bool DicomSlice::FindAndGetDouble( 
	 const unsigned short& groupID,
	 const unsigned short& elmtID,
	 double& dResult )
 {
	 DcmElement *pDcmElement;
	 auto pReadDataset = m_spFrame ? m_spFrame : m_spDataset;
	 OFCondition oc = pReadDataset->findAndGetElement(
		 DcmTagKey(groupID, elmtID),pDcmElement);
	 if (oc.bad())
	 {
		 return false;
	 }
	 //pDcmElement->getLength()
		 
	 // get value representation
	 DcmEVR mVR(pDcmElement->getVR());
	 switch (mVR)
	 {
	 case EVR_SS:/// signed short
		 {
			 Sint16 iValue = 0;
			 oc = pDcmElement->getSint16(iValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 dResult = (double)iValue;
		 }
		 break;
	 case EVR_US:/// unsigned short
		 {
			 Uint16 iValue = 0;
			 oc = pDcmElement->getUint16(iValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 dResult = (double)iValue;
		 }
		 break;
	 case EVR_SL:/// signed long
		 {
			 Sint32 iValue = 0;
			 oc = pDcmElement->getSint32(iValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 dResult = (double)iValue;
		 }
		 break;
	 case EVR_UL:/// unsigned long
		 {
			 Uint32 iValue = 0;
			 oc = pDcmElement->getUint32(iValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 dResult = (double)iValue;
		 }
		 break;
	 case EVR_FL:/// float single-precision
		 {
			 Float32 fValue = 0;
			 oc = pDcmElement->getFloat32(fValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 dResult = (double)fValue;
		 }
		 break;
	 case EVR_FD:/// float double-precision
		 {
			 Float64 fValue = 0;
			 oc = pDcmElement->getFloat64(fValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 dResult = (double)fValue;
		 }
		 break;
	 case EVR_OF:/// other float
		 {
			 Float32 fValue = 0;
			 oc = pDcmElement->getFloat32(fValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 dResult = (double)fValue;
		 }
		 break;
	 case EVR_DT:/// date time string
	 case EVR_DS:/// decimal string
	 case EVR_IS:/// integer string
	 case EVR_DA:/// date string
	 case EVR_TM:/// time string
	 case EVR_AS:/// age string
	 case EVR_AE:/// application entity title
	 case EVR_CS:/// code string
	 case EVR_LO:/// long string
	 case EVR_LT:/// long text
	 case EVR_PN:/// person name
	 case EVR_SH:/// short string
	 case EVR_ST:/// short text
	 case EVR_UI:/// unique identifier
	 case EVR_UT:/// unlimited text
		 {
			 OFString sValue;
			 oc = pDcmElement->getOFString(sValue,0);
			 if (oc.bad())
			 {
				 return false;
			 }
			 dResult = atof(sValue.c_str());
		 }
		 break;
	 default:// VR is always Unknown or UN
		 return false;
		 break;
	 }
	 return true;
 }

 bool DicomSlice::FindAndGetDateTime(
	 const unsigned short& groupID,
	 const unsigned short& elmtID, 
	 tm& timeResult )
 {
	 DcmElement *pDcmElement;
	 auto pReadDataset = m_spFrame ? m_spFrame : m_spDataset;
	 OFCondition oc = pReadDataset->findAndGetElement(
		 DcmTagKey(groupID, elmtID),pDcmElement);
	 if (oc.bad())
	 {
		 return false;
	 }
	 // get value representation
	 DcmEVR mVR(pDcmElement->getVR());
	 switch (mVR)
	 {
	 case EVR_DT:/// date time string
		 {
			 OFString sValue;
			 oc = pDcmElement->getOFStringArray(sValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 double dDateTime = atof(sValue.c_str());
			 return ConvertDT(dDateTime, timeResult);
		 }
		 break;
	 case EVR_DA:/// date string
		 {
			 OFString sValue;
			 oc = pDcmElement->getOFStringArray(sValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 unsigned long long iYYYYMMDD = atol(sValue.c_str());
			 return ConvertDA(iYYYYMMDD, timeResult);
		 }
		 break;
	 case EVR_TM:/// time string
		 {
			 OFString sValue;
			 oc = pDcmElement->getOFStringArray(sValue);
			 if (oc.bad())
			 {
				 return false;
			 }
			 double dHHMMSSFF = atof(sValue.c_str());
			 return ConvertTM(dHHMMSSFF, timeResult);
		 }
		 break;
	 default:// VR is always Unknown or UN
		 return false;
		 break;
	 }
 }

 void DicomSlice::SetImagePositionPatient(const Point3D& ImagePositionPatient) 
 {
     m_ImagePositionPatient = ImagePositionPatient;
 }


 END_NAMESPACE
