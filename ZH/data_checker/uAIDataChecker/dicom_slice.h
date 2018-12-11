#ifndef DICOM_IMAGE_H_
#define DICOM_IMAGE_H_

#include <memory>
#include <string>
#include <string.h>
#include <ctime>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>


#include "uai_app_defs.h"
#include "uAIArithmetic/arithmetic.h"

class DcmDataset;

BEGIN_NAMESPACE

class DicomSeries;
class DicomSUVCalculator;

// DicomSlice,为了保证线程安全，不提供Set方法，构造时根据传入的参数进行初始化
// 若DicomSlice插入到DicomSeries的时,设置m_wpDicomSeries
class DicomSlice
{
public:
    DicomSlice();
    DicomSlice(const boost::shared_ptr<DcmDataset>& dcmDataset, const int& iFrameIndex=0);
    virtual ~DicomSlice(void);
public:
	// Series 
	boost::shared_ptr<DicomSeries> GetDcmSeries() const;

	////////////////////////////////////////////////////////////////////

#pragma region patient level

	std::string GetPatientID() const;
	void SetPatientID(const std::string sPatientID){m_sPatientID=sPatientID;}

    // Patient Name
	std::string GetPatientUID() const;
    void SetPatientUID(const std::string sUid) {m_sPatientUID = sUid;}

    // Patient Name
    std::string GetPatientName() const;
	void SetPatientName(const std::string&sPatientName)
	{
		m_sPatientName = sPatientName;
	}

    // Patient Sex
    std::string GetPatientSex() const;
	void SetPatientSex(const std::string&sPatientSex) {m_sPatientSex = sPatientSex;}

    // Patient BirthDate
    std::string GetPatientBirthDate() const;
	void SetPatientBirthDate(const std::string&sPatientBirthDate) {m_sPatientBirthDate = sPatientBirthDate;}

#pragma endregion patient level


	// Series Description
	std::string GetSeriesDescription() const;

	// StudyUID
	std::string GetStudyUID() const;
	void SetStudyUID(const std::string&sStudyUID) {m_sStudyUID = sStudyUID;}

	// Study Description
	std::string GetStudyDescription() const;
	void SetStudyDescription(const std::string&sStudyDescription) {m_sStudyDescription = sStudyDescription;}

	// Study Date
	std::string GetStudyDate() const;
	void SetStudyDate(const std::string&sStudyDate) {m_sStudyDate = sStudyDate;}

	// Study Time
	std::string GetStudyTime() const;
	void SetStudyTime(const std::string&sStudyTime) {m_sStudyTime = sStudyTime;}

	// InstanceUID
	std::string GetInstanceUID()const;
    void SetInstanceUID(const std::string& InstanceUID){m_sInstanceUID = InstanceUID;}

	// SopClassUID
	std::string GetSopClassUID()const;
	void SetSopClassUID(const std::string& sSopClassUID) {m_sSopClassUID = sSopClassUID;}

	// FrameOfReferenceUID
	std::string GetFrameOfReferenceUID() const;
	void SetFrameOfReferenceUID(const std::string& sFrameOfReferenceUID) {m_sFrameOfReferenceUID = sFrameOfReferenceUID;}

	//InstitutionName
	std::string GetInstitutionName() const;
	void SetInstitutionName(const std::string& sInstitutionName) {m_sInstitutionName = sInstitutionName;}

	//Manufacture
	std::string GetManufactuer() const;
	void SetManufacturer(const std::string& sManufacturer) {m_sManufacturer = sManufacturer;}

#pragma region series level
    // SeriesUID
	std::string GetSeriesUID() const;
    void SetSeriesUID(const std::string& seriesUID){m_sSeriesUID = seriesUID;}
    // PatientPosition 0018,5100
    std::string GetPatientPosition() const { return m_sPatientPosition;}
	void SetPatientPosition(const std::string& sPatientPosition) {m_sPatientPosition = sPatientPosition;}

    //#define DCM_BodyPartExamined                     DcmTagKey(0x0018, 0x0015)
    std::string GetBodyPartExamined() const { return m_sBodyPartExamined;}
    void SetBodyPartExamined(const std::string& BodyPartExamined) { m_sBodyPartExamined = BodyPartExamined;}

    //#define DCM_ProtocolName                         DcmTagKey(0x0018, 0x1030)
    std::string GetProtocolName() const { return m_sProtocolName ;}
    void SetProtocolName(const std::string& ProtocolName ) { m_sProtocolName  = ProtocolName ;}

#pragma endregion series level

	// SeriesNumber
	int GetSeriesNumber() const;
	void SetSeriesNumber(const int& sSeriesNumber) {m_iSeriesNumber = sSeriesNumber;}

	// Series Date
	std::string GetSeriesDate() const;
	void SetSeriesDate(const std::string&sSeriesDate) {m_sSeriesDate = sSeriesDate;}

	// Series Time
	std::string GetSeriesTime() const;
	void SetSeriesTime(const std::string& sSeriesTime) {m_sSeriesTime = sSeriesTime;}


	// Modality
	std::string GetModality() const;
	void SetModality(const std::string& sModality) {m_sModality = sModality;}

	// InstanceNumber
	int GetInstanceNumber() const;
    void SetInstanceNumber(int InstanceNumber) { m_iInstanceNumber = InstanceNumber;}

	// NumberOfFrames
	int GetNumberOfFrames() const;
	void SetNumberOfFrames(int sNumberOfFrames) {m_iNumberOfFrames = sNumberOfFrames;}

	// Row
	int GetRow() const;
    void SetRow(int row) { m_iRow = row;}

	// Colunm
	int GetColumn() const;    
    void SetColumn(int colunm) { m_iColumn = colunm;}

	// SamplesPerPixel
	int GetSamplesPerPixel() const;
	void SetSamplesPerPixel(int sSamplesPerPixel) {m_iSamplePerPixel = sSamplesPerPixel;}

	// BitsAllocated
	int GetBitAllocated() const;
    void SetBitAllocated(int BitAllocated) { m_iBitAllocated = BitAllocated;}

    //#define DCM_BitsStored DcmTagKey(0x0028, 0x0101)
    int GetBitsStored() const { return m_iBitsStored;}
    void SetBitsStored(int BitsStored) { m_iBitsStored = BitsStored;}

	// PixelRepresentation
	int GetPixelRepresentation() const;
    void SetPixelRepresentation(int PixelRepresentation) {m_iPixelRepresentation = PixelRepresentation;}

    //#define DCM_PixelSpacing DcmTagKey(0x0028, 0x0030)
    void GetPixelSpacing(double* dSpacing) const;
    void SetPixelSpacing(double PixelSpacing[2] ) 
    {        
        memcpy(m_dSpacing, PixelSpacing, 2 * sizeof(double));
    }

    //#define DCM_SliceThickness                       DcmTagKey(0x0018, 0x0050)
    std::string GetSliceThickness()const {return m_SliceThickness;}
    void SetSliceThickness(const std::string& SliceThickness){ m_SliceThickness = SliceThickness;}

	// Slop
	double GetSlop() const;
    void SetSlop(double Slop) {m_dSlop = Slop;}

	// Intercept
	double GetIntercept() const;
    void SetIntercept(double Intercept) {m_dIntercept = Intercept;}

#pragma region image level

    
    void SetImagePositionPatient(const Point3D& ImagePositionPatient);

    //0020,0037
    //// ImageOrientation
    void GetImageOrientationPatient(
        Vector3D& rowDir,
        Vector3D& columnDir) const;

    void SetImageOrientationPatient(Vector3D rowDir,Vector3D columnDir) 
    {
        m_vetRow = rowDir;
        m_vetColumn = columnDir;
    }

	// 0020,0032
    Point3D GetImagePositionPatient() const;

    std::string GetDcmFilePath( ) const { return m_sDcmfilepath;}
    void SetDcmFilePath(std::string dcmfilepath) { m_sDcmfilepath = dcmfilepath;}

#pragma endregion image level


	void* GetPixelPointer() const;

    unsigned long GetPixelDataSize() const { return m_ulPixelDataSize;}
	void SetPixelDataSize(unsigned long sPixelDataSize) {m_ulPixelDataSize = sPixelDataSize;}

	// dValue Length is same with m_iSamplePerPixel
	bool GetPixelValue(const int& iX,const int& iY, double* dValue);

	////////////////////////////////////////////////////////////////////

	// Dataset,包含图像的所有Tag信息
	boost::shared_ptr<DcmDataset> GetDcmDataset() const;

	// Dataset,包含当前帧的所有Tag信息
	boost::shared_ptr<DcmDataset> GetFrameDataset() const;

	// 获取像素信息到病人坐标系转换(第三维度不计算)若需要计算Pixel位置应使用（x,y,0）
	Matrix4x4 GetImage2Patient() const;

	// 设置像素信息到归一化坐标系转换
	void SetImage2World(Matrix3x3& matImage2World);

	// 获取像素信息到归一化坐标转换
	Matrix3x3 GetImage2World() const;

	// 获取图像的最大值
	bool CalculateMinMax();

	// 使用之前需要提前调用计算Max和Min
	double GetMinScalar() const;

	// 使用之前需要提前调用计算Max和Min
	double GetMaxScalar() const;

	// 获取图像存储的WindowLevel
	bool GetWindowLevel(double& dWW, double& dWC);
    void SetWindowWidth(double dWW) {m_dWW = dWW;}
    void SetWindowCenter(double dWC) {m_dWC = dWC;}

	bool FindAndGetString(
		const unsigned short& groupID,
		const unsigned short& elmtID,
		std::string& sResult);

	bool FindAndGetDouble(
		const unsigned short& groupID, 
		const unsigned short& elmtID,
		double& dResult);

	bool FindAndGetDateTime(
		const unsigned short& groupID, 
		const unsigned short& elmtID,
		tm& timeResult);
	bool ReadDICOMTag();
protected:

    bool ReadImageLevel_i();

	template<typename T>
	void FindMinMax_i(T *pData);

    template<typename T> 
    void ModifyPaddingValue_i( T *pData, const int iLimit, const int iValue);


	template<typename T>
	inline bool GetPixelValue_i(
		T *pData,const int& iX,const int& iY,
		const int iSample, double* dValue);
protected:

#pragma region patient level

    std::string m_sPatientID;
    std::string m_sPatientUID;
    std::string m_sPatientName;
    std::string m_sPatientSex;
    std::string m_sPatientBirthDate;
#pragma endregion patient level

	std::string m_sStudyUID;
	std::string m_sStudyDescription;
	std::string m_sStudyDate;
	std::string m_sStudyTime;
	std::string m_sFrameOfReferenceUID;
	std::string m_sModality;
	std::string m_sInstitutionName;
	std::string m_sManufacturer;

#pragma region series level
    std::string m_sPatientPosition;
    std::string m_sSeriesDate;
    std::string m_sSeriesTime;
    std::string m_sSeriesDescription;
    std::string m_sSeriesUID;
    std::string m_sBodyPartExamined;
    std::string m_sProtocolName;
#pragma endregion series level
	int m_iSeriesNumber;

	std::string m_sSopClassUID;
	std::string m_sInstanceUID;
	int m_iInstanceNumber;
	int m_iNumberOfFrames;
	double m_dSpacing[2];
	int m_iRow;
	int m_iColumn;
	int m_iSamplePerPixel;
	int m_iBitAllocated;
    int m_iBitsStored;
	int m_iPixelRepresentation;
	double m_dSlop;
	double m_dIntercept;
	bool m_bExtractedWL;
	double m_dWW;
	double m_dWC;

	void* m_pPixelPointer;
    unsigned long m_ulPixelDataSize;
	double m_dMaxScalar;
	double m_dMinScalar;

#pragma region image level

    Vector3D m_vetRow;
    Vector3D m_vetColumn;

    Point3D m_ImagePositionPatient;
    std::string m_sDcmfilepath;

#pragma endregion image level



	Matrix4x4 m_matPixel2Patient;
	Matrix3x3 m_matPixel2World;

	// 存储读取到的DcmDataset
	boost::shared_ptr<DcmDataset> m_spDataset;

	// 若为多帧图像，存储对应帧的DcmDataset
	// dcmDataset拆分m_spDataset多帧功能组得到
	// 内部不包含PixelData以及多帧功能组item
	boost::shared_ptr<DcmDataset> m_spFrame;
	boost::weak_ptr<DicomSeries> m_wpDicomSeries;

    std::string m_SliceThickness;
private:
	DISALLOW_COPY_AND_ASSIGN(DicomSlice);
};

END_NAMESPACE
#endif
