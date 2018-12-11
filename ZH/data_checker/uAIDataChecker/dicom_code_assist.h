#ifndef DICOM_CODE_ASSIST_H_
#define DICOM_CODE_ASSIST_H_
#include "uai_app_defs.h"
#include "dcmtk/oflog/nullap.h"
#include "dcmtk/dcmjpeg/djencode.h"                   // for all supported JPEG processes encode
#include "dcmtk/dcmjpeg/djdecode.h"                   // for all supported JPEG processes decode
#include "dcmtk/dcmdata/dcrleerg.h"                   // for registers RLE encoder
#include "dcmtk/dcmdata/dcrledrg.h"                   // for registers RLE decode
#include "dcmtk/dcmjpls/djencode.h"                   // for all supported JPEG-LS processes encode
#include "dcmtk/dcmjpls/djdecode.h"                   // for all supported JPEG-LS processes decode

#include "dcmtk/dcmjp2k/d2init.h"                     // for D2JPEG2000 library
#include "dcmtk/dcmjp2k/d2encode.h"                   // for registers JPEG2000 encoder
#include "dcmtk/dcmjp2k/d2decode.h"                   // for registers JPEG2000 decode


BEGIN_NAMESPACE

class DicomCodeAssist
{
public:
    DicomCodeAssist():m_bInitialize(false){}
    virtual ~DicomCodeAssist(void){}
	void Initialize(){
        if (!m_bInitialize)
        {
            return;
        }

        // deregister JPEG decoder
        //qiangqian.zhou@20180807 TODO fix bug
        DJDecoderRegistration::cleanup();
        //deregister JPEG encoder
        DJEncoderRegistration::cleanup();

        // deregister RLE decoder
        DcmRLEDecoderRegistration::cleanup();
        // deregister RLE encoder
        DcmRLEEncoderRegistration::cleanup();

        // deregister JPEG-LS decoder
        DJLSDecoderRegistration::cleanup();
        // deregister JPEG-LS encoder
        DJLSEncoderRegistration::cleanup();

        //deregister JPEG2000 decoder
        D2DecoderRegistration::cleanup();
        // deregister JPEG2000 encoder
        D2EncoderRegistration::cleanup();
        // cleanup JPEG2000 library
        D2JPEG2000Library::cleanup();
    }
    void Cleanup(){
        if (m_bInitialize)
        {
            return;
        }
        m_bInitialize = true;
        // register JPEG decoder
        DJDecoderRegistration::registerCodecs();
        // register JPEG encoder
        DJEncoderRegistration ::registerCodecs();

        // register RLE decoder
        DcmRLEDecoderRegistration::registerCodecs();
        // register RLE encoder
        DcmRLEEncoderRegistration::registerCodecs();

        // register JPEG-LS decoder
        DJLSDecoderRegistration::registerCodecs();
        // register JPEG-LS encoder
        DJLSEncoderRegistration::registerCodecs();

        // initialize JPEG2000 library
        const OFCondition cond = D2JPEG2000Library::initialize();
        if (cond.good())
        {
            // register JPEG2000 decoder
            D2DecoderRegistration::registerCodecs();
            // register JPEG2000 encoder
            D2EncoderRegistration::registerCodecs();
        }
    }
private:
	bool m_bInitialize;
private:
	DISALLOW_COPY_AND_ASSIGN(DicomCodeAssist);
};

END_NAMESPACE
#endif

