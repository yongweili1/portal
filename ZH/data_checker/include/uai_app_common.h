//////////////////////////////////////////////////////////////////////////
/// \defgroup uAI
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2018
///  All rights reserved.
///
///  \author  qiangqiang.zhou     mailto:qiangqiang.zhou@united-imaging.com
///
///  \file    uAI App error code
///  \brief   
///
///  \version 1.0
///  \date    June 15, 2018
///  \{
//////////////////////////////////////////////////////////////////////////

#ifndef UAI_APP_ERROR_H_
#define UAI_APP_ERROR_H_

#include <string>
#include "uai_app_defs.h"

namespace uai
{
    enum APP_ERROR_CODES
    {
        //common error range [0, 99]
        APP_ER_SUCCESS				= 0						,
        APP_ER_FAILURE                                      ,
        APP_ER_INPUT_INVALID                                ,
      
		APP_ER_DATABASE				= 100					,

        APP_ER_UNKNOWN_ERROR		= 5000				    ,
    };

    //app type for all AI applications
    enum APP_TYPE
    {
        APP_TYPE_DR_CHEST           = 0,    //  for Chest.cshtml
        APP_TYPE_DR_HAND            = 1,    //  for BoneAge.cshtml
        APP_TYPE_CT_RIB             = 2,    //  for Fracture.cshtml
        APP_TYPE_CT_LUNG            = 3,    //  for Lung.cshtml
        APP_TYPE_CT_HEART           = 4,
        APP_TYPE_MR_BRAIN           = 5,
        APP_TYPE_UNSUPPORTED        = 6,
    };

    static const char * ENUM_TO_STRING_APP_TYPE[] = 
    {
        "DR_CHEST",
        "DR_HAND",
        "CT_RIB",
        "CT_LUNG",
        "CT_HEART",
        "MR_BRAIN",
        "UNSUPPORTED",
    };
    APP_TYPE UAI_EXPORT String2Enum_APP_TYPE(const char* user_input);

    //////////////////////////////////////////////////////////////////////////
    //std::string m_arrBone[7]={"骨化中心的个数","尺骨远侧骨垢线闭合","挠骨远侧骨垢线闭合","掌指骨骨垢线闭合","拇指内侧籽骨出现","钩状骨沟突发育成熟","尺骨茎突出现"};
    enum BONEAGE_TYPE
    {
        BONEAGE_NUMBER_OF_OSSIFICATION          = 0 ,
        BONEAGE_ULINA_DISTAL_BONE_LINE              ,
        BONEAGE_DISTAL_BONE_SCALE_LINE              ,
        BONEAGE_METACARPOPHALANGEAL_LINE            ,
        BONEAGE_MEDIAL_SKELETAL_BONE                ,
        BONEAGE_HOOKED_BONE_SULCUS                  ,
        BONEAGE_ULINA_STYLOID_PROCESS               , 
        BONEAGE_UNSUPPORTED
    };

    static const char * ENUM_TO_STRING_BONEAGE_TYPE[] = 
    {
        "BONEAGE_NUMBER_OF_OSSIFICATION",
        "BONEAGE_ULINA_DISTAL_BONE_LINE",
        "BONEAGE_DISTAL_BONE_SCALE_LINE",
        "BONEAGE_METACARPOPHALANGEAL_LINE",
        "BONEAGE_MEDIAL_SKELETAL_BONE",
        "BONEAGE_HOOKED_BONE_SULCUS",
        "BONEAGE_ULINA_STYLOID_PROCESS",
        "BONEAGE_UNSUPPORTED"
    };

    //////////////////////////////////////////////////////////////////////////
    //TODO update later
    //by qiangqiang.zhou@20180820
    enum APP_PROTOCOL_KEY
    {
        APP_PROTOCOL_KEY_CHEST           = 0,  
        APP_PROTOCOL_KEY_LUNG           ,  
        APP_PROTOCOL_KEY_HAND               ,   
        APP_PROTOCOL_KEY_RIB                ,
        APP_PROTOCOL_KEY_CHEST_CN           ,
        APP_PROTOCOL_KEY_UNSUPPORTED        ,
    };
    static const char * ENUM_TO_STRING_APP_PROTOCOL_KEY[] = 
    {
        "CHEST",
        "LUNG",
        "HAND",
        "RIB",
        "胸部",
        "UNSUPPORTED",
    };
    APP_PROTOCOL_KEY UAI_EXPORT String2Enum_APP_PROTOCOL_KEY(const char* user_input);

    struct PacsConnection
    {
        std::string     PacsAE;
        std::string     PacsHostname;
        unsigned int    PacsPort;
        std::string     AETitle;
        unsigned int    AEPort;
        std::string     OutputDirectory;
        unsigned int    nTimeout;
    };

    enum DATATYPE
    {
        DATA_TYPE_UNSIGNED_CHAR  = 0,
        DATA_TYPE_UNSIGNED_SHORT = 1,
        DATA_TYPE_CHAR = 2,
        DATA_TYPE_SHORT = 3,
        DATA_TYPE_FLOAT = 4,
        DATA_TYPE_DOUBLE = 5,
    };

    enum DATA_SOURCE
    {
        //common error range [0, 99]
        DATA_SOURCE_PACS				= 0,
        DATA_SOURCE_DISK
    };


    //form  UAI/AppData/uAIConfig.xml
    struct UAI_EXPORT AppConfig
    {
        AppConfig()
        {
            DicomSource = DATA_SOURCE_PACS;
            DicomFilesInput = "";
            OutputDirectory = "";
            SeriesUID = "";
            ModalitiesInStudy = "";
            m_sSeriesTime = "19710101";
            m_sStudyTime = "19710101";
            StudyDescription = "";
            RuntimeCallAlg = false;
            AlgorithmsEnable = 0;
            StudyDateStart="";
            StudyDateEnd="";
            IsAutoDeleted = false;
            DaysToKeepData = 180;
            RatioToDeleteFiles = 0.94f;
        }

        AppConfig& AppConfig::operator=(const AppConfig& src)
        {
            if (this != &src)
            {
                this->DicomSource = src.DicomSource;
                this->DicomFilesInput = src.DicomFilesInput;
                this->OutputDirectory = src.OutputDirectory;
                this->SeriesUID = src.SeriesUID;
                this->RuntimeCallAlg = src.RuntimeCallAlg;
                this->ModalitiesInStudy = src.ModalitiesInStudy;
                this->StudyDateStart = src.StudyDateStart;
                this->StudyDateEnd = src.StudyDateEnd;
                this->m_sStudyTime = src.m_sStudyTime;
                this->m_sSeriesTime = src.m_sSeriesTime;
                this->StudyDescription = src.StudyDescription;
                this->AlgorithmsEnable = src.AlgorithmsEnable;
                this->IsAutoDeleted = src.IsAutoDeleted;
                this->DaysToKeepData = src.DaysToKeepData;
                this->RatioToDeleteFiles = src.RatioToDeleteFiles;
            }
            return *this;
        }

         //<!-- 0 from PACS 1 DicomFilesInput from  -->
        DATA_SOURCE    DicomSource;
        std::string     DicomFilesInput;
        std::string     OutputDirectory;
        std::string     SeriesUID;
        std::string     m_sStudyTime;
        std::string     m_sSeriesTime;
        std::string     ModalitiesInStudy;
        std::string     StudyDateStart;  
        std::string     StudyDateEnd;  
        std::string     StudyDescription;
        bool            RuntimeCallAlg;
        //according to enum AlgorithmName
        unsigned int    AlgorithmsEnable;
        bool            IsAutoDeleted;
        int             DaysToKeepData;
        float           RatioToDeleteFiles;

    };

#define MIN_SLICE_COUNT 20

#define BIT_CHECK(a,b) ((a) & (1<<(b)))

    enum AlgorithmName
    {
        VSEG,
        LUNGNODULE,
        CHESTCLF,
        BONEAP,
        RIBSEGMENT,
        CHESTCLFSCOL,
        CARDIOTHORACIC,
		RIBFRACTURE,
    };

    static const char * ENUM_TO_STRING_AlgorithmName[] = 
    {
        "VSEG",
        "LUNGNODULE",
        "CHESTCLF",
        "BONEAP",
        "RIBSEGMENT",
        "CHESTCLFSCOL",
        "CARDIOTHORACIC",
		"RIBFRACTURE",
    };

    enum AlgoModelName
    {
        LEFTLUNGMODEL,
        RIGHTLUNGMODEL,
        LUNGNODULEMODEL,
        CHESTXRMODEL,
        BONEAGEPREDMODEL,
        RIBSEGMENTMODEL,
        CHESTCLFSCOLMODEL,
        HEARTMODEL,
		RIBFRACTUREMODEL,
    };

    AlgorithmName UAI_EXPORT String2Enum_AlgorithmName(const char* user_input);

}
#endif //UAI_APP_ERROR_H_
