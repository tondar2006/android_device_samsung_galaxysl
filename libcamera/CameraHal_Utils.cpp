/*
 * Copyright (C) Texas Instruments - http://www.ti.com/
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
/**
* @file CameraHal_Utils.cpp
*
* This file maps the Camera Hardware Interface to V4L2.
*
*/

#define LOG_TAG "CameraHalUtils"

#include "CameraHal.h"
#include "zoom_step.inc"

namespace android {

#ifdef FW3A
int CameraHal::FW3A_Create()
{
    int err = 0;

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME

#endif

    fobj = (lib3atest_obj*)malloc(sizeof(*fobj));
    if (!fobj) {
        ALOGE("cannot alloc fobj");
        goto exit;
    }
    memset(fobj, 0 , sizeof(*fobj));

    err = ICam_Create(&fobj->hnd);
    if (err < 0) {
        ALOGE("Can't Create2A");
        goto exit;
    }

#ifdef DEBUG_LOG

    ALOGD("FW3A Create - %d   fobj=%p", err, fobj);

    LOG_FUNCTION_NAME_EXIT

#endif

    return 0;

exit:
    ALOGD("Can't create 3A FW");
    return -1;
}

int CameraHal::FW3A_Init()
{
    int err = 0;

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME

#endif

    /* Init 2A framework */
    err = ICam_Init(fobj->hnd);
    if (err < 0) {
        ALOGE("Can't ICam_Init, will try to release first");
        err = ICam_Release(fobj->hnd, ICAM_RELEASE);
        if (!err) {
            err = ICam_Init(fobj->hnd);
            if (err < 0) {
                ALOGE("Can't ICam_Init");

                err = ICam_Destroy(fobj->hnd);
                goto exit;
            }
        }
    }

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME_EXIT

#endif

    return 0;

exit:
    ALOGE("Can't init 3A FW");
    return -1;
}

int CameraHal::FW3A_Release()
{
    int ret;

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME

#endif

    ret = ICam_Release(fobj->hnd, ICAM_RESTART);
    if (ret < 0) {
        ALOGE("Cannot Release2A");
        goto exit;
    } else {
        ALOGD("2A released");
    }

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME_EXIT

#endif

    return 0;

exit:

    return -1;
}

int CameraHal::FW3A_Destroy()
{
    int ret;

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME

#endif

    ret = ICam_Destroy(fobj->hnd);
    if (ret < 0) {
        ALOGE("Cannot Destroy2A");
    } else {
        ALOGD("2A destroyed");
    }

    free(fobj->mnote.buffer);
  	free(fobj);

    fobj = NULL;

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME_EXIT

#endif

    return 0;
}

int CameraHal::FW3A_Start()
{
    int ret;

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME

#endif

    if (isStart_FW3A!=0) {
        ALOGE("3A FW is already started");
        return -1;
    }

    //Start 3AFW
    ret = ICam_ViewFinder(fobj->hnd, ICAM_ENABLE);
    if (0 > ret) {
        ALOGE("Cannot Start 2A");
        return -1;
    } else {
        ALOGE("3A FW Start - success");
    }

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME_EXIT

#endif

    isStart_FW3A = 1;
    return 0;
}

int CameraHal::FW3A_Stop()
{
    int ret;

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME

#endif

    if (isStart_FW3A==0) {
        ALOGE("3A FW is already stopped");
        return -1;
    }

    //Stop 3AFW
    ret = ICam_ViewFinder(fobj->hnd, ICAM_DISABLE);
    if (0 > ret) {
        ALOGE("Cannot Stop 3A");
        return -1;
    }

#ifdef DEBUG_LOG
     else {
        ALOGE("3A FW Stop - success");
    }
#endif

    isStart_FW3A = 0;

#ifdef DEBUG_LOG
    LOG_FUNCTION_NAME_EXIT
#endif

    return 0;
}

int CameraHal::FW3A_Start_CAF()
{
    int ret;

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME

#endif

    if (isStart_FW3A_CAF!=0) {
        ALOGE("3A FW CAF is already started");
        return -1;
    }

    if (isStart_FW3A == 0) {
        ALOGE("3A FW is not started");
        return -1;
    }

    ret = ICam_CFocus(fobj->hnd, ICAM_ENABLE);

    if (ret != 0) {
        ALOGE("Cannot Start CAF");
        return -1;
    } else {
        isStart_FW3A_CAF = 1;
#ifdef DEBUG_LOG
        ALOGD("3A FW CAF Start - success");
#endif
    }

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME_EXIT

#endif

    return 0;
}

int CameraHal::FW3A_Stop_CAF()
{
    int ret, prev = isStart_FW3A_CAF;

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME

#endif

    if (isStart_FW3A_CAF==0) {
        ALOGE("3A FW CAF is already stopped");
        return prev;
    }

    ret = ICam_CFocus(fobj->hnd, ICAM_DISABLE);
    if (0 > ret) {
        ALOGE("Cannot Stop CAF");
        return -1;
    }


     else {
        isStart_FW3A_CAF = 0;
#ifdef DEBUG_LOG
        ALOGD("3A FW CAF Stop - success");
#endif
    }


#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME_EXIT

#endif

    return prev;
}

//TODO: Add mode argument
int CameraHal::FW3A_Start_AF()
{
    int ret = 0;

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME

#endif
    if (isStart_FW3A == 0) {
        ALOGE("3A FW is not started");
        return -1;
    }

    if (isStart_FW3A_AF!=0) {
        ALOGE("3A FW AF is already started");
        return -1;
    }

    ret = ICam_AFocus(fobj->hnd, ICAM_ENABLE);

    if (0 != ret) {
        ALOGE("Cannot Start AF");
        return -1;
    } else {
        isStart_FW3A_AF = 1;
        ALOGD("3A FW AF Start - success");
    }

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME_EXIT

#endif

    return 0;
}

int CameraHal::FW3A_Stop_AF()
{
    int ret, prev = isStart_FW3A_AF;

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME

#endif

    if (isStart_FW3A_AF == 0) {
        ALOGE("3A FW AF is already stopped");
        return isStart_FW3A_AF;
    }

    //Stop 3AFW
    ret = ICam_AFocus(fobj->hnd, ICAM_DISABLE);
    if (0 > ret) {
        ALOGE("Cannot Stop AF");
        return -1;
    } else {
        isStart_FW3A_AF = 0;
        ALOGE("3A FW AF Stop - success");
    }

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME_EXIT

#endif

    return prev;
}

int CameraHal::FW3A_GetSettings() const
{
    int err = 0;

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME

#endif

    err = ICam_ReadSettings(fobj->hnd, &fobj->settings);

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME_EXIT

#endif

    return err;
}

int CameraHal::FW3A_SetSettings()
{
    int err = 0;

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME

#endif

    err = ICam_WriteSettings(fobj->hnd, &fobj->settings);

#ifdef DEBUG_LOG

    LOG_FUNCTION_NAME_EXIT

#endif

    return err;
}

#endif

#ifdef IMAGE_PROCESSING_PIPELINE

int CameraHal::InitIPP(int w, int h, int fmt, int ippMode)
{
    int eError = 0;

	if( (ippMode != IPP_CromaSupression_Mode) && (ippMode != IPP_EdgeEnhancement_Mode) ){
		ALOGE("Error unsupported mode=%d", ippMode);
		return -1;
	}

    pIPP.hIPP = IPP_Create();
    ALOGD("IPP Handle: %p",pIPP.hIPP);

	if( !pIPP.hIPP ){
		ALOGE("ERROR IPP_Create");
		return -1;
	}

    if( fmt == PIX_YUV420P)
	    pIPP.ippconfig.numberOfAlgos = 3;
	else
	    pIPP.ippconfig.numberOfAlgos = 4;

    pIPP.ippconfig.orderOfAlgos[0]=IPP_START_ID;

    if( fmt != PIX_YUV420P ){
        pIPP.ippconfig.orderOfAlgos[1]=IPP_YUVC_422iTO422p_ID;

	    if(ippMode == IPP_CromaSupression_Mode ){
		    pIPP.ippconfig.orderOfAlgos[2]=IPP_CRCBS_ID;
	    }
	    else if(ippMode == IPP_EdgeEnhancement_Mode){
		    pIPP.ippconfig.orderOfAlgos[2]=IPP_EENF_ID;
	    }

        pIPP.ippconfig.orderOfAlgos[3]=IPP_YUVC_422pTO422i_ID;
        pIPP.ippconfig.isINPLACE=INPLACE_ON;
	} else {
	    if(ippMode == IPP_CromaSupression_Mode ){
		    pIPP.ippconfig.orderOfAlgos[1]=IPP_CRCBS_ID;
	    }
	    else if(ippMode == IPP_EdgeEnhancement_Mode){
		    pIPP.ippconfig.orderOfAlgos[1]=IPP_EENF_ID;
	    }

        pIPP.ippconfig.orderOfAlgos[2]=IPP_YUVC_422pTO422i_ID;
        pIPP.ippconfig.isINPLACE=INPLACE_OFF;
	}

	pIPP.outputBufferSize = (w*h*2);

    ALOGD("IPP_SetProcessingConfiguration");
    eError = IPP_SetProcessingConfiguration(pIPP.hIPP, pIPP.ippconfig);
	if(eError != 0){
		ALOGE("ERROR IPP_SetProcessingConfiguration");
	}

	if(ippMode == IPP_CromaSupression_Mode ){
		pIPP.CRCBptr.size = sizeof(IPP_CRCBSAlgoCreateParams);
		pIPP.CRCBptr.maxWidth = w;
		pIPP.CRCBptr.maxHeight = h;
		pIPP.CRCBptr.errorCode = 0;

		ALOGD("IPP_SetAlgoConfig CRBS");
		eError = IPP_SetAlgoConfig(pIPP.hIPP, IPP_CRCBS_CREATEPRMS_CFGID, &(pIPP.CRCBptr));
		if(eError != 0){
			ALOGE("ERROR IPP_SetAlgoConfig");
		}
	}else if(ippMode == IPP_EdgeEnhancement_Mode ){
		pIPP.EENFcreate.size = sizeof(IPP_EENFAlgoCreateParams);
		pIPP.EENFcreate.maxImageSizeH = w;
		pIPP.EENFcreate.maxImageSizeV = h;
		pIPP.EENFcreate.errorCode = 0;
		pIPP.EENFcreate.inPlace = 0;
		pIPP.EENFcreate.inputBufferSizeForInPlace = 0;

		ALOGD("IPP_SetAlgoConfig EENF");
		eError = IPP_SetAlgoConfig(pIPP.hIPP, IPP_EENF_CREATEPRMS_CFGID, &(pIPP.EENFcreate));
		if(eError != 0){
			ALOGE("ERROR IPP_SetAlgoConfig");
		}
	}

    pIPP.YUVCcreate.size = sizeof(IPP_YUVCAlgoCreateParams);
    pIPP.YUVCcreate.maxWidth = w;
    pIPP.YUVCcreate.maxHeight = h;
    pIPP.YUVCcreate.errorCode = 0;

    if ( fmt != PIX_YUV420P ) {
        ALOGD("IPP_SetAlgoConfig: IPP_YUVC_422TO420_CREATEPRMS_CFGID");
        eError = IPP_SetAlgoConfig(pIPP.hIPP, IPP_YUVC_422TO420_CREATEPRMS_CFGID, &(pIPP.YUVCcreate));
	    if(eError != 0){
		    ALOGE("ERROR IPP_SetAlgoConfig: IPP_YUVC_422TO420_CREATEPRMS_CFGID");
	    }
	}

    ALOGD("IPP_SetAlgoConfig: IPP_YUVC_420TO422_CREATEPRMS_CFGID");
    eError = IPP_SetAlgoConfig(pIPP.hIPP, IPP_YUVC_420TO422_CREATEPRMS_CFGID, &(pIPP.YUVCcreate));
    if(eError != 0){
        ALOGE("ERROR IPP_SetAlgoConfig: IPP_YUVC_420TO422_CREATEPRMS_CFGID");
    }

    ALOGD("IPP_InitializeImagePipe");
    eError = IPP_InitializeImagePipe(pIPP.hIPP);
	if(eError != 0){
		ALOGE("ERROR IPP_InitializeImagePipe");
	} else {
        mIPPInitAlgoState = true;
    }

    pIPP.iStarInArgs = (IPP_StarAlgoInArgs*)((char*)DSP_CACHE_ALIGN_MEM_ALLOC(sizeof(IPP_StarAlgoInArgs)));
    pIPP.iStarOutArgs = (IPP_StarAlgoOutArgs*)((char*)DSP_CACHE_ALIGN_MEM_ALLOC(sizeof(IPP_StarAlgoOutArgs)));

	if(ippMode == IPP_CromaSupression_Mode ){
        pIPP.iCrcbsInArgs = (IPP_CRCBSAlgoInArgs*)((char*)DSP_CACHE_ALIGN_MEM_ALLOC(sizeof(IPP_CRCBSAlgoInArgs)));
        pIPP.iCrcbsOutArgs = (IPP_CRCBSAlgoOutArgs*)((char*)DSP_CACHE_ALIGN_MEM_ALLOC(sizeof(IPP_CRCBSAlgoOutArgs)));
	}

	if(ippMode == IPP_EdgeEnhancement_Mode){
        pIPP.iEenfInArgs = (IPP_EENFAlgoInArgs*)((char*)DSP_CACHE_ALIGN_MEM_ALLOC(sizeof(IPP_EENFAlgoInArgs)));
        pIPP.iEenfOutArgs = (IPP_EENFAlgoOutArgs*)((char*)DSP_CACHE_ALIGN_MEM_ALLOC(sizeof(IPP_EENFAlgoOutArgs)));
	}

    pIPP.iYuvcInArgs1 = (IPP_YUVCAlgoInArgs*)((char*)DSP_CACHE_ALIGN_MEM_ALLOC(sizeof(IPP_YUVCAlgoInArgs)));
    pIPP.iYuvcOutArgs1 = (IPP_YUVCAlgoOutArgs*)((char*)DSP_CACHE_ALIGN_MEM_ALLOC(sizeof(IPP_YUVCAlgoOutArgs)));
    pIPP.iYuvcInArgs2 = (IPP_YUVCAlgoInArgs*)((char*)DSP_CACHE_ALIGN_MEM_ALLOC(sizeof(IPP_YUVCAlgoInArgs)));
    pIPP.iYuvcOutArgs2 = (IPP_YUVCAlgoOutArgs*)((char*)DSP_CACHE_ALIGN_MEM_ALLOC(sizeof(IPP_YUVCAlgoOutArgs)));

	if(ippMode == IPP_EdgeEnhancement_Mode){
        pIPP.dynEENF = (IPP_EENFAlgoDynamicParams*)((char*)DSP_CACHE_ALIGN_MEM_ALLOC(sizeof(IPP_EENFAlgoDynamicParams)));
	}

	if( !(pIPP.ippconfig.isINPLACE) ){
		pIPP.pIppOutputBuffer= (unsigned char*)DSP_CACHE_ALIGN_MEM_ALLOC(pIPP.outputBufferSize); // TODO make it dependent on the output format
	}

    return eError;
}
/*-------------------------------------------------------------------*/
/**
  * DeInitIPP()
  *
  *
  *
  * @param pComponentPrivate component private data structure.
  *
  * @retval OMX_ErrorNone       success, ready to roll
  *         OMX_ErrorHardware   if video driver API fails
  **/
/*-------------------------------------------------------------------*/
int CameraHal::DeInitIPP(int ippMode)
{
    int eError = 0;

    if(mIPPInitAlgoState){
        ALOGD("IPP_DeinitializePipe");
        eError = IPP_DeinitializePipe(pIPP.hIPP);
        ALOGD("IPP_DeinitializePipe");
        if( eError != 0){
            ALOGE("ERROR IPP_DeinitializePipe");
        }
        mIPPInitAlgoState = false;
    }

    ALOGD("IPP_Delete");
    IPP_Delete(&(pIPP.hIPP));

    free(((char*)pIPP.iStarInArgs));
    free(((char*)pIPP.iStarOutArgs));

	if(ippMode == IPP_CromaSupression_Mode ){
        free(((char*)pIPP.iCrcbsInArgs));
        free(((char*)pIPP.iCrcbsOutArgs));
	}

	if(ippMode == IPP_EdgeEnhancement_Mode){
        free(((char*)pIPP.iEenfInArgs));
        free(((char*)pIPP.iEenfOutArgs));
	}

    free(((char*)pIPP.iYuvcInArgs1));
    free(((char*)pIPP.iYuvcOutArgs1));
    free(((char*)pIPP.iYuvcInArgs2));
    free(((char*)pIPP.iYuvcOutArgs2));

	if(ippMode == IPP_EdgeEnhancement_Mode){
        free(((char*)pIPP.dynEENF));
	}

	if(!(pIPP.ippconfig.isINPLACE)){
		free(pIPP.pIppOutputBuffer);
	}

    ALOGD("Terminating IPP");

    return eError;
}

int CameraHal::PopulateArgsIPP(int w, int h, int fmt, int ippMode)
{
    int eError = 0;

    ALOGD("IPP: PopulateArgs ENTER");

	//configuring size of input and output structures
    pIPP.iStarInArgs->size = sizeof(IPP_StarAlgoInArgs);
	if(ippMode == IPP_CromaSupression_Mode ){
	    pIPP.iCrcbsInArgs->size = sizeof(IPP_CRCBSAlgoInArgs);
	}
	if(ippMode == IPP_EdgeEnhancement_Mode){
	    pIPP.iEenfInArgs->size = sizeof(IPP_EENFAlgoInArgs);
	}

    pIPP.iYuvcInArgs1->size = sizeof(IPP_YUVCAlgoInArgs);
    pIPP.iYuvcInArgs2->size = sizeof(IPP_YUVCAlgoInArgs);

    pIPP.iStarOutArgs->size = sizeof(IPP_StarAlgoOutArgs);
	if(ippMode == IPP_CromaSupression_Mode ){
    	pIPP.iCrcbsOutArgs->size = sizeof(IPP_CRCBSAlgoOutArgs);
	}
	if(ippMode == IPP_EdgeEnhancement_Mode){
    	pIPP.iEenfOutArgs->size = sizeof(IPP_EENFAlgoOutArgs);
	}

    pIPP.iYuvcOutArgs1->size = sizeof(IPP_YUVCAlgoOutArgs);
    pIPP.iYuvcOutArgs2->size = sizeof(IPP_YUVCAlgoOutArgs);

	//Configuring specific data of algorithms
	if(ippMode == IPP_CromaSupression_Mode ){
	    pIPP.iCrcbsInArgs->inputHeight = h;
	    pIPP.iCrcbsInArgs->inputWidth = w;
	    pIPP.iCrcbsInArgs->inputChromaFormat = IPP_YUV_420P;
	}

	if(ippMode == IPP_EdgeEnhancement_Mode){
		pIPP.iEenfInArgs->inputChromaFormat = IPP_YUV_420P;
		pIPP.iEenfInArgs->inFullWidth = w;
		pIPP.iEenfInArgs->inFullHeight = h;
		pIPP.iEenfInArgs->inOffsetV = 0;
		pIPP.iEenfInArgs->inOffsetH = 0;
		pIPP.iEenfInArgs->inputWidth = w;
		pIPP.iEenfInArgs->inputHeight = h;
		pIPP.iEenfInArgs->inPlace = 0;
		pIPP.iEenfInArgs->NFprocessing = 0;
    }

    if ( fmt == PIX_YUV422I ) {
        pIPP.iYuvcInArgs1->inputChromaFormat = IPP_YUV_422ILE;
        pIPP.iYuvcInArgs1->outputChromaFormat = IPP_YUV_420P;
        pIPP.iYuvcInArgs1->inputHeight = h;
        pIPP.iYuvcInArgs1->inputWidth = w;
    }

    pIPP.iYuvcInArgs2->inputChromaFormat = IPP_YUV_420P;
    pIPP.iYuvcInArgs2->outputChromaFormat = IPP_YUV_422ILE;
    pIPP.iYuvcInArgs2->inputHeight = h;
    pIPP.iYuvcInArgs2->inputWidth = w;

	pIPP.iStarOutArgs->extendedError= 0;
	pIPP.iYuvcOutArgs1->extendedError = 0;
	pIPP.iYuvcOutArgs2->extendedError = 0;
	if(ippMode == IPP_EdgeEnhancement_Mode)
		pIPP.iEenfOutArgs->extendedError = 0;
	if(ippMode == IPP_CromaSupression_Mode )
		pIPP.iCrcbsOutArgs->extendedError = 0;

	//Filling ipp status structure
    pIPP.starStatus.size = sizeof(IPP_StarAlgoStatus);
	if(ippMode == IPP_CromaSupression_Mode ){
    	pIPP.CRCBSStatus.size = sizeof(IPP_CRCBSAlgoStatus);
	}
	if(ippMode == IPP_EdgeEnhancement_Mode){
    	pIPP.EENFStatus.size = sizeof(IPP_EENFAlgoStatus);
	}

    pIPP.statusDesc.statusPtr[0] = &(pIPP.starStatus);
	if(ippMode == IPP_CromaSupression_Mode ){
	    pIPP.statusDesc.statusPtr[1] = &(pIPP.CRCBSStatus);
	}
	if(ippMode == IPP_EdgeEnhancement_Mode){
	    pIPP.statusDesc.statusPtr[1] = &(pIPP.EENFStatus);
	}

    pIPP.statusDesc.numParams = 2;
    pIPP.statusDesc.algoNum[0] = 0;
    pIPP.statusDesc.algoNum[1] = 1;
    pIPP.statusDesc.algoNum[1] = 1;

    ALOGD("IPP: PopulateArgs EXIT");

    return eError;
}

int CameraHal::ProcessBufferIPP(void *pBuffer, long int nAllocLen, int fmt, int ippMode,
                               int EdgeEnhancementStrength, int WeakEdgeThreshold, int StrongEdgeThreshold,
                                int LowFreqLumaNoiseFilterStrength, int MidFreqLumaNoiseFilterStrength, int HighFreqLumaNoiseFilterStrength,
                                int LowFreqCbNoiseFilterStrength, int MidFreqCbNoiseFilterStrength, int HighFreqCbNoiseFilterStrength,
                                int LowFreqCrNoiseFilterStrength, int MidFreqCrNoiseFilterStrength, int HighFreqCrNoiseFilterStrength,
                                int shadingVertParam1, int shadingVertParam2, int shadingHorzParam1, int shadingHorzParam2,
                                int shadingGainScale, int shadingGainOffset, int shadingGainMaxValue,
                                int ratioDownsampleCbCr)
{
    int eError = 0;
    IPP_EENFAlgoDynamicParams IPPEENFAlgoDynamicParamsCfg =
    {
        sizeof(IPP_EENFAlgoDynamicParams),
        0,//  inPlace
        150,//  EdgeEnhancementStrength;
        100,//  WeakEdgeThreshold;
        300,//  StrongEdgeThreshold;
        30,//  LowFreqLumaNoiseFilterStrength;
        80,//  MidFreqLumaNoiseFilterStrength;
        20,//  HighFreqLumaNoiseFilterStrength;
        60,//  LowFreqCbNoiseFilterStrength;
        40,//  MidFreqCbNoiseFilterStrength;
        30,//  HighFreqCbNoiseFilterStrength;
        50,//  LowFreqCrNoiseFilterStrength;
        30,//  MidFreqCrNoiseFilterStrength;
        20,//  HighFreqCrNoiseFilterStrength;
        1,//  shadingVertParam1;
        800,//  shadingVertParam2;
        1,//  shadingHorzParam1;
        800,//  shadingHorzParam2;
        128,//  shadingGainScale;
        4096,//  shadingGainOffset;
        24576,//  shadingGainMaxValue;
        1//  ratioDownsampleCbCr;
    };//2

    ALOGD("IPP_StartProcessing");
    eError = IPP_StartProcessing(pIPP.hIPP);
    if(eError != 0){
        ALOGE("ERROR IPP_SetAlgoConfig");
    }

	if(ippMode == IPP_EdgeEnhancement_Mode){
       IPPEENFAlgoDynamicParamsCfg.inPlace = 0;
        NONNEG_ASSIGN(EdgeEnhancementStrength, IPPEENFAlgoDynamicParamsCfg.EdgeEnhancementStrength);
        NONNEG_ASSIGN(WeakEdgeThreshold, IPPEENFAlgoDynamicParamsCfg.WeakEdgeThreshold);
        NONNEG_ASSIGN(StrongEdgeThreshold, IPPEENFAlgoDynamicParamsCfg.StrongEdgeThreshold);
        NONNEG_ASSIGN(LowFreqLumaNoiseFilterStrength, IPPEENFAlgoDynamicParamsCfg.LowFreqLumaNoiseFilterStrength);
        NONNEG_ASSIGN(MidFreqLumaNoiseFilterStrength, IPPEENFAlgoDynamicParamsCfg.MidFreqLumaNoiseFilterStrength);
        NONNEG_ASSIGN(HighFreqLumaNoiseFilterStrength, IPPEENFAlgoDynamicParamsCfg.HighFreqLumaNoiseFilterStrength);
        NONNEG_ASSIGN(LowFreqCbNoiseFilterStrength, IPPEENFAlgoDynamicParamsCfg.LowFreqCbNoiseFilterStrength);
        NONNEG_ASSIGN(MidFreqCbNoiseFilterStrength, IPPEENFAlgoDynamicParamsCfg.MidFreqCbNoiseFilterStrength);
        NONNEG_ASSIGN(HighFreqCbNoiseFilterStrength, IPPEENFAlgoDynamicParamsCfg.HighFreqCbNoiseFilterStrength);
        NONNEG_ASSIGN(LowFreqCrNoiseFilterStrength, IPPEENFAlgoDynamicParamsCfg.LowFreqCrNoiseFilterStrength);
        NONNEG_ASSIGN(MidFreqCrNoiseFilterStrength, IPPEENFAlgoDynamicParamsCfg.MidFreqCrNoiseFilterStrength);
        NONNEG_ASSIGN(HighFreqCrNoiseFilterStrength, IPPEENFAlgoDynamicParamsCfg.HighFreqCrNoiseFilterStrength);
        NONNEG_ASSIGN(shadingVertParam1, IPPEENFAlgoDynamicParamsCfg.shadingVertParam1);
        NONNEG_ASSIGN(shadingVertParam2, IPPEENFAlgoDynamicParamsCfg.shadingVertParam2);
        NONNEG_ASSIGN(shadingHorzParam1, IPPEENFAlgoDynamicParamsCfg.shadingHorzParam1);
        NONNEG_ASSIGN(shadingHorzParam2, IPPEENFAlgoDynamicParamsCfg.shadingHorzParam2);
        NONNEG_ASSIGN(shadingGainScale, IPPEENFAlgoDynamicParamsCfg.shadingGainScale);
        NONNEG_ASSIGN(shadingGainOffset, IPPEENFAlgoDynamicParamsCfg.shadingGainOffset);
        NONNEG_ASSIGN(shadingGainMaxValue, IPPEENFAlgoDynamicParamsCfg.shadingGainMaxValue);
        NONNEG_ASSIGN(ratioDownsampleCbCr, IPPEENFAlgoDynamicParamsCfg.ratioDownsampleCbCr);

        ALOGD("Set EENF Dynamics Params:");
        ALOGD("\tInPlace                      = %d", (int)IPPEENFAlgoDynamicParamsCfg.inPlace);
        ALOGD("\tEdge Enhancement Strength    = %d", (int)IPPEENFAlgoDynamicParamsCfg.EdgeEnhancementStrength);
        ALOGD("\tWeak Edge Threshold          = %d", (int)IPPEENFAlgoDynamicParamsCfg.WeakEdgeThreshold);
        ALOGD("\tStrong Edge Threshold        = %d", (int)IPPEENFAlgoDynamicParamsCfg.StrongEdgeThreshold);
        ALOGD("\tLuma Noise Filter Low Freq Strength   = %d", (int)IPPEENFAlgoDynamicParamsCfg.LowFreqLumaNoiseFilterStrength );
        ALOGD("\tLuma Noise Filter Mid Freq Strength   = %d", (int)IPPEENFAlgoDynamicParamsCfg.MidFreqLumaNoiseFilterStrength );
        ALOGD("\tLuma Noise Filter High Freq Strength   = %d", (int)IPPEENFAlgoDynamicParamsCfg.HighFreqLumaNoiseFilterStrength );
        ALOGD("\tChroma Noise Filter Low Freq Cb Strength = %d", (int)IPPEENFAlgoDynamicParamsCfg.LowFreqCbNoiseFilterStrength);
        ALOGD("\tChroma Noise Filter Mid Freq Cb Strength = %d", (int)IPPEENFAlgoDynamicParamsCfg.MidFreqCbNoiseFilterStrength);
        ALOGD("\tChroma Noise Filter High Freq Cb Strength = %d", (int)IPPEENFAlgoDynamicParamsCfg.HighFreqCbNoiseFilterStrength);
        ALOGD("\tChroma Noise Filter Low Freq Cr Strength = %d", (int)IPPEENFAlgoDynamicParamsCfg.LowFreqCrNoiseFilterStrength);
        ALOGD("\tChroma Noise Filter Mid Freq Cr Strength = %d", (int)IPPEENFAlgoDynamicParamsCfg.MidFreqCrNoiseFilterStrength);
        ALOGD("\tChroma Noise Filter High Freq Cr Strength = %d", (int)IPPEENFAlgoDynamicParamsCfg.HighFreqCrNoiseFilterStrength);
        ALOGD("\tShading Vert 1 = %d", (int)IPPEENFAlgoDynamicParamsCfg.shadingVertParam1);
        ALOGD("\tShading Vert 2 = %d", (int)IPPEENFAlgoDynamicParamsCfg.shadingVertParam2);
        ALOGD("\tShading Horz 1 = %d", (int)IPPEENFAlgoDynamicParamsCfg.shadingHorzParam1);
        ALOGD("\tShading Horz 2 = %d", (int)IPPEENFAlgoDynamicParamsCfg.shadingHorzParam2);
        ALOGD("\tShading Gain Scale = %d", (int)IPPEENFAlgoDynamicParamsCfg.shadingGainScale);
        ALOGD("\tShading Gain Offset = %d", (int)IPPEENFAlgoDynamicParamsCfg.shadingGainOffset);
        ALOGD("\tShading Gain Max Val = %d", (int)IPPEENFAlgoDynamicParamsCfg.shadingGainMaxValue);
        ALOGD("\tRatio Downsample CbCr = %d", (int)IPPEENFAlgoDynamicParamsCfg.ratioDownsampleCbCr);


		/*Set Dynamic Parameter*/
		memcpy(pIPP.dynEENF,
		       (void*)&IPPEENFAlgoDynamicParamsCfg,
		       sizeof(IPPEENFAlgoDynamicParamsCfg));


		ALOGD("IPP_SetAlgoConfig");
		eError = IPP_SetAlgoConfig(pIPP.hIPP, IPP_EENF_DYNPRMS_CFGID, (void*)pIPP.dynEENF);
		if( eError != 0){
			ALOGE("ERROR IPP_SetAlgoConfig");
		}
	}

    pIPP.iInputBufferDesc.numBuffers = 1;
    pIPP.iInputBufferDesc.bufPtr[0] = pBuffer;
    pIPP.iInputBufferDesc.bufSize[0] = nAllocLen;
    pIPP.iInputBufferDesc.usedSize[0] = nAllocLen;
    pIPP.iInputBufferDesc.port[0] = 0;
    pIPP.iInputBufferDesc.reuseAllowed[0] = 0;

	if(!(pIPP.ippconfig.isINPLACE)){
		pIPP.iOutputBufferDesc.numBuffers = 1;
		pIPP.iOutputBufferDesc.bufPtr[0] = pIPP.pIppOutputBuffer;						/*TODO, depend on pix format*/
		pIPP.iOutputBufferDesc.bufSize[0] = pIPP.outputBufferSize;
		pIPP.iOutputBufferDesc.usedSize[0] = pIPP.outputBufferSize;
		pIPP.iOutputBufferDesc.port[0] = 1;
		pIPP.iOutputBufferDesc.reuseAllowed[0] = 0;
	}

    if ( fmt == PIX_YUV422I){
	    pIPP.iInputArgs.numArgs = 4;
	    pIPP.iOutputArgs.numArgs = 4;

        pIPP.iInputArgs.argsArray[0] = pIPP.iStarInArgs;
        pIPP.iInputArgs.argsArray[1] = pIPP.iYuvcInArgs1;
	    if(ippMode == IPP_CromaSupression_Mode ){
	        pIPP.iInputArgs.argsArray[2] = pIPP.iCrcbsInArgs;
	    }
	    if(ippMode == IPP_EdgeEnhancement_Mode){
		    pIPP.iInputArgs.argsArray[2] = pIPP.iEenfInArgs;
	    }
        pIPP.iInputArgs.argsArray[3] = pIPP.iYuvcInArgs2;

        pIPP.iOutputArgs.argsArray[0] = pIPP.iStarOutArgs;
        pIPP.iOutputArgs.argsArray[1] = pIPP.iYuvcOutArgs1;
        if(ippMode == IPP_CromaSupression_Mode ){
            pIPP.iOutputArgs.argsArray[2] = pIPP.iCrcbsOutArgs;
        }
        if(ippMode == IPP_EdgeEnhancement_Mode){
            pIPP.iOutputArgs.argsArray[2] = pIPP.iEenfOutArgs;
        }
        pIPP.iOutputArgs.argsArray[3] = pIPP.iYuvcOutArgs2;
	 } else {
        pIPP.iInputArgs.numArgs = 3;
        pIPP.iOutputArgs.numArgs = 3;

        pIPP.iInputArgs.argsArray[0] = pIPP.iStarInArgs;
        if(ippMode == IPP_CromaSupression_Mode ){
            pIPP.iInputArgs.argsArray[1] = pIPP.iCrcbsInArgs;
        }
        if(ippMode == IPP_EdgeEnhancement_Mode){
            pIPP.iInputArgs.argsArray[1] = pIPP.iEenfInArgs;
        }
        pIPP.iInputArgs.argsArray[2] = pIPP.iYuvcInArgs2;

        pIPP.iOutputArgs.argsArray[0] = pIPP.iStarOutArgs;
        if(ippMode == IPP_CromaSupression_Mode ){
            pIPP.iOutputArgs.argsArray[1] = pIPP.iCrcbsOutArgs;
        }
        if(ippMode == IPP_EdgeEnhancement_Mode){
            pIPP.iOutputArgs.argsArray[1] = pIPP.iEenfOutArgs;
        }
        pIPP.iOutputArgs.argsArray[2] = pIPP.iYuvcOutArgs2;
    }

    ALOGD("IPP_ProcessImage");
	if((pIPP.ippconfig.isINPLACE)){
		eError = IPP_ProcessImage(pIPP.hIPP, &(pIPP.iInputBufferDesc), &(pIPP.iInputArgs), NULL, &(pIPP.iOutputArgs));
	}
	else{
		eError = IPP_ProcessImage(pIPP.hIPP, &(pIPP.iInputBufferDesc), &(pIPP.iInputArgs), &(pIPP.iOutputBufferDesc),&(pIPP.iOutputArgs));
	}
    if( eError != 0){
		ALOGE("ERROR IPP_ProcessImage");
	}

    ALOGD("IPP_StopProcessing");
    eError = IPP_StopProcessing(pIPP.hIPP);
    if( eError != 0){
        ALOGE("ERROR IPP_StopProcessing");
    }

	ALOGD("IPP_ProcessImage Done");

    return eError;
}

#endif

int CameraHal::ZoomPerform(float zoom)
{
    int delta_x, delta_y;
    int ret;
    int dst_width, dst_height;
    float normalizedZoom = 0.0;

    LOG_FUNCTION_NAME

    LOG_FUNCTION_NAME_EXIT

    return 0;
}

void CameraHal::SetDSPKHz(unsigned int KHz)
{
    char command[100];
    sprintf(command, "echo %u > /sys/power/dsp_freq", KHz);
    system(command);
    // ALOGD("command: %s", command);
}

};




