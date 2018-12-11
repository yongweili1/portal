#include "dicom_loader.h"

#include <algorithm>    // std::transform
#include <boost/filesystem.hpp>
#include "dcmtk/dcmdata/dcistrmb.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmimage/diregist.h"
#include "dcmtk/dcmimgle/dcmimage.h"

#include "dicom_code_assist.h"
#include "uai_app_logger.h"
#include "dicom_slice.h"

BEGIN_NAMESPACE;

DicomLoader::DicomLoader():m_pDicomCodeAssist(nullptr)
{                   
    m_pDicomCodeAssist = new DicomCodeAssist();
}

DicomLoader::~DicomLoader( void )
{
   DEL_PTR(m_pDicomCodeAssist);
}

void DicomLoader::Cleanup()
{
   m_pDicomCodeAssist->Cleanup();
}

bool DicomLoader::ReadDicomFolder(const std::string& sFolder, std::vector<boost::shared_ptr<DicomSlice>>& vDcmImageList)
{
    if (sFolder.empty())
    {
        _LOG_DEV_ERROR<<"sFolder is empty.";
        return false;
    }
	std::vector<std::string> vecDicomFiles;
	FindFilesRecursion(sFolder, vecDicomFiles);
	return ReadDicomFiles(vecDicomFiles, vDcmImageList);
}

bool DicomLoader::LoadDcmFile(const std::string& sFilePath, boost::shared_ptr<DicomSlice>& pDicomImage) 
{
    DicomCodeAssist DicomCA;
    DicomCA.Initialize();
    FILE* pFile = fopen(sFilePath.c_str(), "rb");
    if(NULL == pFile)
    {
        return false;
    }
    // move to the begin
    (void)fseek(pFile, 0, SEEK_SET);
    // move to the end
    (void)fseek(pFile, 0, SEEK_END);
    // get file stream size
    unsigned long lFileLength = ftell(pFile);
    // move to the begin
    (void)fseek(pFile, 0, SEEK_SET);

    // check file stream size to verify open_file successfully
    if (lFileLength < 1)
    {
        return false;
    }
    // create file buffer
    boost::shared_ptr<char> spFileBuffer(
        new char[lFileLength],
        std::default_delete<char[]>());
    if (!spFileBuffer)
    {
        return false;
    }
    char* pFileBuffer = spFileBuffer.get();
    std::memset(pFileBuffer, 0, 
        static_cast<size_t>(lFileLength*sizeof(char)));

    const size_t READ_DATA_COUNT = 1;
    // attempt to read in lFileLength characters
    const size_t stReadLength = fread(
        pFileBuffer,     
        static_cast<size_t>(lFileLength),
        READ_DATA_COUNT,
        pFile);
    (void)fclose(pFile);

    if (READ_DATA_COUNT != stReadLength)
    {
        return false;
    }
    DcmInputBufferStream dcmInputBufferStream;
    // adds the content of the given buffer to the input stream
    dcmInputBufferStream.setBuffer(
        pFileBuffer, static_cast<offile_off_t>(lFileLength));
    // marks the end of stream
    dcmInputBufferStream.setEos();

    // check status of the stream
    OFCondition cond = dcmInputBufferStream.status();
    if (cond.bad())
    {
        return false;
    }

    DcmFileFormat fileformat; 
    fileformat.transferInit();
    cond = fileformat.read(dcmInputBufferStream);
    //OFCondition oc = fileformat.loadFile(m_sFilePath.c_str());                    //读取Dicom图像  
    if (cond.bad())     //判断Dicom文件是否读取成功  
    {   
        return false;  
    }
    fileformat.transferEnd();
    cond = fileformat.loadAllDataIntoMemory();
    if (cond.bad())
    {
        return false;
    }
    DcmDataset *dataset = fileformat.getAndRemoveDataset();                              //得到Dicom的数据集  
    E_TransferSyntax xfer = dataset->getOriginalXfer();                          //得到传输语法  

    if (xfer != EXS_LittleEndianExplicit)
    {
        OFCondition cond = dataset->chooseRepresentation(
            EXS_LittleEndianExplicit, NULL);  
        if (cond.bad())
        {
            return false;
        }
    }
    auto pDataset = boost::shared_ptr<DcmDataset>(dataset);
    try
    {
        boost::shared_ptr<DicomSlice> pDcmSlice(new DicomSlice(pDataset));
        bool bRet = pDcmSlice->ReadDICOMTag();
        if (!bRet)
        {
            //_LOG_DEV_ERROR<<"pDcmSlice->ReadDICOMTag failed FilePath="<<sFilePath;
            return bRet;
        }
        pDicomImage = pDcmSlice;
        pDicomImage->SetDcmFilePath(sFilePath);
    }
    catch(...)
    {
        ;
    }

    // *dProgress = 

    return true;
}

bool DicomLoader::ReadDicomFiles(const std::vector<std::string>& vecDicomFiles, std::vector<boost::shared_ptr<DicomSlice>>& vDcmImageList)
{
    if (vecDicomFiles.empty())
    {
        return false;
    }

    bool bRet = false;

    size_t iCount = vecDicomFiles.size();
    vDcmImageList.clear();
    for (int i = 0; i < iCount; ++i)
    {
        boost::shared_ptr<DicomSlice> pDicomImage;
        bRet = LoadDcmFile(vecDicomFiles[i], pDicomImage); //  (i + 1) / (double)iCount
        if (!bRet)
        {
            //_LOG_DEV_ERROR<<"ERROR_CODES_DCM_FILE_INVALID to Load DcmFile:"<<vecDicomFiles[i];
            continue;
        }

        vDcmImageList.push_back(pDicomImage);
    }
    return bRet;
}

bool DicomLoader::AysncReadDicomDir(const std::string& path,
	const boost::shared_ptr<ILoadingNotifier> pNotifier )
{
// 	std::vector<std::string> vecDicomFiles;
// 	AppUtils::FindFilesRecursion(path, vecDicomFiles);
// 	return AysncReadDicomFiles(vecDicomFiles ,pNotifier);
    return false;
}

bool DicomLoader::AysncReadDicomFiles( 
	const std::vector<std::string>& vecDicomFiles, 
	const boost::shared_ptr<ILoadingNotifier> pNotifier )
{
	if (vecDicomFiles.empty()
		|| !pNotifier)
	{
		return false;
	}
	/*if (!m_spThread)
	{
		m_spThread = UIHCommon::ActiveObjectFIFO::Create();
		if(!m_spThread->Activate())
		{
			return false;
		}
	}
	auto pLoadingStartCommand = std::make_shared<LoadingStartCommand>(pNotifier);
	m_spThread->PutCommand(pLoadingStartCommand);

	int iCount = vecDicomFiles.size();
	std::vector<std::string> vecDicomFilesPath;
	for (int i = 0; i < iCount; ++i)
	{
		std::vector<std::string> vecFilesPath;
		if (CreateFilePathFromDir(vecDicomFiles[i],vecFilesPath))
		{
			vecDicomFilesPath.insert(vecDicomFilesPath.end(),vecFilesPath.begin(),vecFilesPath.end());
		}
		else
		{
			vecDicomFilesPath.push_back(vecDicomFiles[i]);
		}
	}
	iCount = vecDicomFilesPath.size();
	for (int i = 0; i < iCount; ++i)
	{
		auto pLoadingCommand = std::make_shared<DicomLoadingCommand>(
			vecDicomFilesPath[i],(i + 1) / (double)iCount,pNotifier);
		m_spThread->PutCommand(pLoadingCommand);
	}
	auto pLoadingCompletedCommand = std::make_shared<LoadingCompletedCommand>(pNotifier);
	m_spThread->PutCommand(pLoadingCompletedCommand);*/

	return true;
}

bool DicomLoader::CreateFilePathFromDir(
	const std::string filePath,
	std::vector<std::string>& vecFilesPath)
{
		FILE* pFile = fopen(filePath.c_str(), "rb");
		if(NULL == pFile)
		{
                	return false;
		}
		// move to the begin
		(void)fseek(pFile, 0, SEEK_SET);
		// move to the end
		(void)fseek(pFile, 0, SEEK_END);
		// get file stream size
		unsigned long lFileLength = ftell(pFile);
		// move to the begin
		(void)fseek(pFile, 0, SEEK_SET);

		// check file stream size to verify open_file successfully
		if (lFileLength < 1)
		{
			return false;
		}
		// create file buffer
		boost::shared_ptr<char> spFileBuffer(
			new char[lFileLength],
			std::default_delete<char[]>());
		if (!spFileBuffer)
		{
			return false;
		}
		char* pFileBuffer = spFileBuffer.get();
		std::memset(pFileBuffer, 0, 
			static_cast<size_t>(lFileLength*sizeof(char)));

		const size_t READ_DATA_COUNT = 1;
		// attempt to read in lFileLength characters
		const size_t stReadLength = fread(
			pFileBuffer,
			static_cast<size_t>(lFileLength),
			READ_DATA_COUNT,
			pFile);
		(void)fclose(pFile);

		if (READ_DATA_COUNT != stReadLength)
		{
			return false;
		}
		DcmInputBufferStream dcmInputBufferStream;
		// adds the content of the given buffer to the input stream
		dcmInputBufferStream.setBuffer(
			pFileBuffer, static_cast<offile_off_t>(lFileLength));
		// marks the end of stream
		dcmInputBufferStream.setEos();

		// check status of the stream
		OFCondition cond = dcmInputBufferStream.status();
		if (cond.bad())
		{
			return false;
		}

		DcmFileFormat fileformat; 
		fileformat.transferInit();
		cond = fileformat.read(dcmInputBufferStream);
		//OFCondition oc = fileformat.loadFile(m_sFilePath.c_str());                    //读取Dicom图像  
		if (cond.bad())     //判断Dicom文件是否读取成功  
		{   
			return false;  
		}
		fileformat.transferEnd();
		cond = fileformat.loadAllDataIntoMemory();
		if (cond.bad())
		{
			return false;
		}
		DcmDataset *dataset = fileformat.getAndRemoveDataset();                              //得到Dicom的数据集  
		//E_TransferSyntax xfer = dataset->getOriginalXfer();                          //得到传输语法  
		DcmElement *element = NULL;
		cond = dataset->findAndGetElement(DCM_PixelData, element); 
		if (cond.bad())
		{
			auto pDataset = boost::shared_ptr<DcmDataset>(dataset);
			pDataset->findAndGetElement(DCM_PixelData,element);
			auto pElemnet = pDataset->nextInContainer(nullptr);
			while (pElemnet)
			{
				auto pSubElement = pElemnet->nextInContainer(nullptr);
				while (pSubElement)
				{
					auto pSpElement = pSubElement->nextInContainer(nullptr);
					while (pSpElement)
					{
						auto dGroup = pSpElement->getTag().getGroup();
						auto dElement = pSpElement->getTag().getElement();
						if (dGroup == 0x0004 && dElement == 0x1500)
						{
							OFString sfileID;
							auto spElement = (DcmElement*)pSpElement;
							OFString sValue;
							auto oc = spElement->getOFStringArray(sValue);
							if (oc.bad())
							{
								return false;
							}
							auto sResult = std::string(sValue.c_str());
							auto iPosition = filePath.find_last_of("\\");
							auto tempPath = filePath.substr(0,iPosition+1);
							sResult = tempPath + sResult;
							vecFilesPath.push_back(sResult);
						}
						pSpElement = pSubElement->nextInContainer(pSpElement);
					}
					pSubElement = pElemnet->nextInContainer(pSubElement);
				}
				pElemnet = pDataset->nextInContainer(pElemnet);
			}
			return true;
		}
	return false;
}

bool DicomLoader::AnonymousDcm(const std::string& sInput, const std::string& sOutput)
{
    //load file
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(sInput.c_str());
    if (!status.good())
    {
        std::cerr << "Error: cannot read DICOM file (" << status.text() << ")" << std::endl;
        return false;
    }


    DcmDataset *dataset = fileformat.getDataset();                              //得到Dicom的数据集  
    E_TransferSyntax xfer = dataset->getOriginalXfer();                          //得到传输语法  

    if (xfer != EXS_LittleEndianExplicit)
    {
        OFCondition cond = dataset->chooseRepresentation(
            EXS_LittleEndianExplicit, NULL);  
        if (cond.bad())
        {
            return false;
        }
    }

    //anonymous 
    OFString Anonymous = "Anonymous";  
    
    //DCM_PatientName
    dataset->putAndInsertString(DCM_PatientName, Anonymous.c_str());

    //DCM_InstitutionName
    dataset->putAndInsertString(DCM_InstitutionName, Anonymous.c_str());

    //DCM_Manufacturer
    dataset->putAndInsertString(DCM_Manufacturer, Anonymous.c_str());

    //save file
    status = fileformat.saveFile(sOutput.c_str(), EXS_LittleEndianExplicit);
    if (status.bad())
    {
        _LOG_DEV_ERROR<<"Error: saveFile cannot write DICOM file ("<<status.text()<<")";
        std::cerr << "Error: saveFile cannot write DICOM file (" << status.text() << ")" << std::endl;
        
        return false;
    }
    return true;
}

bool DicomLoader::AnonymousDcmMore(const std::string& sInput, const std::string& sOutput)
{
	//load file
	DicomCodeAssist DicomCA;
	DicomCA.Initialize();

    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(sInput.c_str());
    if (!status.good())
    {
        std::cerr << "Error: cannot read DICOM file (" << status.text() << ")" << std::endl;
        return false;
    }
	DcmDataset *dataset = fileformat.getDataset();                              //得到Dicom的数据集  
    E_TransferSyntax xfer = dataset->getOriginalXfer();                          //得到传输语法  

    if (xfer != EXS_LittleEndianExplicit)
    {
        OFCondition cond = dataset->chooseRepresentation(
            EXS_LittleEndianExplicit, NULL);  
        if (cond.bad())
        {
            return false;
        }
		dataset = fileformat.getDataset();
    }

    //anonymous 
    OFString Anonymous = "Anonymous";  
    //DCM_PatientName
	delete dataset->remove(DCM_PatientName);
    dataset->putAndInsertString(DCM_PatientName, Anonymous.c_str());

    //DCM_InstitutionName
	delete dataset->remove(DCM_InstitutionName);
    dataset->putAndInsertString(DCM_InstitutionName, Anonymous.c_str());

    //DCM_Manufacturer
	delete dataset->remove(DCM_Manufacturer);
    dataset->putAndInsertString(DCM_Manufacturer, Anonymous.c_str());

	//DCM_Accession_Number
	delete dataset->remove(DCM_AccessionNumber);
	dataset->putAndInsertString(DCM_AccessionNumber, Anonymous.c_str());

	//DCM_StudyData
	delete dataset->remove(DCM_StudyDate);
	dataset->putAndInsertString(DCM_StudyDate, Anonymous.c_str());

	//DCM_SeriesData
	delete dataset->remove(DCM_SeriesDate);
	dataset->putAndInsertString(DCM_SeriesDate, Anonymous.c_str());

	//DCM_AcquisitionDate
	delete dataset->remove(DCM_AcquisitionDate);
	dataset->putAndInsertString(DCM_AcquisitionDate, Anonymous.c_str());

	//DCM_ContentDate
	delete dataset->remove(DCM_ContentDate);
	dataset->putAndInsertString(DCM_ContentDate, Anonymous.c_str());

	//DCM_InstitutionAddress
	delete dataset->remove(DCM_InstitutionAddress);
	dataset->putAndInsertString(DCM_InstitutionAddress, Anonymous.c_str());

	//DCM_OperatorsName
	delete dataset->remove(DCM_OperatorsName);
	dataset->putAndInsertString(DCM_OperatorsName, Anonymous.c_str());

	//DCM_PatientBirthTime
	delete dataset->remove(DCM_PatientBirthTime);
	dataset->putAndInsertString(DCM_PatientBirthTime, Anonymous.c_str());

	//DCM_PatientSex
	delete dataset->remove(DCM_PatientSex);
	dataset->putAndInsertString(DCM_PatientSex, Anonymous.c_str());
	
	//DCM_PatientAge
	delete dataset->remove(DCM_PatientAge);
	dataset->putAndInsertString(DCM_PatientAge, Anonymous.c_str());

	//DCM_StudyID
	delete dataset->remove(DCM_StudyID);
	dataset->putAndInsertString(DCM_StudyID, Anonymous.c_str());

    //save file
    status = fileformat.saveFile(sOutput.c_str(), EXS_LittleEndianExplicit);
    if (status.bad())
    {
        _LOG_DEV_ERROR<<"Error: saveFile cannot write DICOM file ("<<status.text()<<")";
        std::cerr << "Error: saveFile cannot write DICOM file (" << status.text() << ")" << std::endl;
        
        return false;
    }
    return true;
}

void DicomLoader::FindFilesRecursion(const std::string& sFolder, std::vector<std::string> &vecDicomFiles)
{
    namespace fs = boost::filesystem;
    fs::directory_iterator end_iter;
    try
    {
        for(fs::directory_iterator iter(sFolder);iter!=end_iter;iter++)
        {
            if (fs::is_directory( *iter ) ){
                FindFilesRecursion(iter->path().string(), vecDicomFiles);
            }
            else {
                vecDicomFiles.push_back(iter->path().string());
            }
        }
    }
    catch (fs::filesystem_error& e)
    {
        printf("%s\n", e.what());
    }
}
END_NAMESPACE
