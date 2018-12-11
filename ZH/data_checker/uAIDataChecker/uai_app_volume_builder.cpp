#include "uai_app_volume_builder.h"
#include "dicom_loader.h"
#include "dicom_slice.h"
#include "uai_dicom_slicer_checker.h"
#include <boost/bind.hpp>

namespace uai {
	typedef boost::shared_ptr<DicomSlice> DicomSlicePtr;
	int data_checking(const char *pSeriesDir)
	{
		boost::shared_ptr<DicomLoader> m_pLoader(new DicomLoader);
		boost::shared_ptr<DataCheckingBase> m_pDataCheckingBase(new DataCheckingBase);
		std::vector<DicomSlicePtr> vDcmImageList;

		if (!m_pLoader->ReadDicomFolder(pSeriesDir, vDcmImageList))
		{
			printf("ReadDicomFolder failed.\n");
			return -1;
		}

		if (vDcmImageList.empty())
		{
			printf("vDcmImageList.empty()!\n");
			return -1;
		}

		std::sort(vDcmImageList.begin(), vDcmImageList.end(),
			boost::bind(std::less<int>(),
				boost::bind(&DicomSlice::GetInstanceNumber, _1),
				boost::bind(&DicomSlice::GetInstanceNumber, _2))
		);

		std::string sReplyInfo;

		int rtn = m_pDataCheckingBase->DataChecking(vDcmImageList, sReplyInfo);
		if (rtn != 0 && !sReplyInfo.empty())
		{
			printf("data checking return %d for %s\n", rtn, sReplyInfo.c_str());
		}
		return rtn;
	}

}
