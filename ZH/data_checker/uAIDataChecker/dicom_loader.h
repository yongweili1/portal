

#ifndef DICOM_LOADER_H_
#define DICOM_LOADER_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "uai_app_defs.h"
 

BEGIN_NAMESPACE
// DicomLoader
// 用于读取数据
class ILoadingNotifier;
class DicomSlice;
class DicomCodeAssist;
class DicomLoader
{
public:
    DicomLoader();
    virtual ~DicomLoader(void);
	//static boost::shared_ptr<DicomLoader> GetInstance();

    void Cleanup();

	bool ReadDicomFolder(const std::string& sFolder, std::vector<boost::shared_ptr<DicomSlice> >& vDcmImageList);

	bool ReadDicomFiles(const std::vector<std::string>& vecDicomFiles,
        std::vector<boost::shared_ptr<DicomSlice> >& vDcmImageList);

	bool AysncReadDicomDir(const
		std::string& sDicomDir,
		const boost::shared_ptr<ILoadingNotifier> pNotifier);

	bool AysncReadDicomFiles(
		const std::vector<std::string>& vecDicomFiles,
		const boost::shared_ptr<ILoadingNotifier> pNotifier);

	bool CreateFilePathFromDir(
		const std::string,
		std::vector<std::string>& vecFilesPath
		);

    bool LoadDcmFile(const std::string& sFilePath, boost::shared_ptr<DicomSlice>& pDicomImage);
	
    //DCM_PatientName
    //DCM_InstitutionName
    //DCM_Manufacturer
    bool AnonymousDcm(const std::string& sInput, const std::string& sOutput);

	bool AnonymousDcmMore(const std::string& sInput, const std::string& sOutput);

private:

    DicomCodeAssist*        m_pDicomCodeAssist;

private:
	DISALLOW_COPY_AND_ASSIGN(DicomLoader);
    void FindFilesRecursion(const std::string& sFolder, std::vector<std::string>& vecDicomFiles);
};

END_NAMESPACE
#endif
