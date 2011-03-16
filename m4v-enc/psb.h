/*
 * psb.h, omx psb component header
 *
 * Copyright (c) 2009-2010 Wind River Systems, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __WRS_OMXIL_INTEL_MRST_PSB
#define __WRS_OMXIL_INTEL_MRST_PSB

#include <OMX_Core.h>
#include <OMX_Component.h>

#include <cmodule.h>
#include <portbase.h>
#include <componentbase.h>

#define oscl_memset	memset

typedef enum _M4vStartCodeType {
    /* video_object_start_code goes from 0x00 to 0x1F */
    MIN_VIDEO_OBJECT_START_CODE = 0x00,
    MAX_VIDEO_OBJECT_START_CODE = 0x1F,
    /* video_object_layer_start_code from 0x20 to 0x2F */
    MIN_VIDEO_OBJECT_LAYER_START_CODE = 0x20,
    MAX_VIDEO_OBJECT_LAYER_START_CODE = 0x2F,
    VISUAL_OBJECT_SEQUENCE_START_CODE = 0xB0,
    VISUAL_OBJECT_SEQUENCE_END_CODE	= 0xB1,
    USER_DATA_START_CODE = 0xB2,
    GROUP_OF_VOP_START_CODE	= 0xB3,
    VISUAL_OBJECT_START_CODE = 0xB5,
    VOP_START_CODE = 0xB6,
    STUFFING_START_CODE = 0xC3,
    UNKNOWN_CODE_TYPE = 0xFF,
} M4vStartCodeType;

#define M4V_VOP_TYPE_MASK 0xC0

typedef enum _M4vVopType {
    I_FRAME = 0x00,
    P_FRAME = 0x40,
    B_FRAME = 0x80,
    S_FRAME = 0xC0,
    UNKNOWN_FRAME = 0xFF,
} M4vVopType;

class MrstPsbComponent : public ComponentBase
{
public:
    /*
     * constructor & destructor
     */
    MrstPsbComponent();
    ~MrstPsbComponent();

private:
    /*
     * component methods & helpers
     */
    /* implement ComponentBase::ComponentAllocatePorts */
    virtual OMX_ERRORTYPE ComponentAllocatePorts(void);

    OMX_ERRORTYPE __AllocateMpeg4Port(OMX_U32 port_index, OMX_DIRTYPE dir);

    OMX_ERRORTYPE __AllocateRawPort(OMX_U32 port_index, OMX_DIRTYPE dir);

    /* implement ComponentBase::ComponentGet/SetPatameter */
    virtual OMX_ERRORTYPE
    ComponentGetParameter(OMX_INDEXTYPE nParamIndex,
                          OMX_PTR pComponentParameterStructure);
    virtual OMX_ERRORTYPE
    ComponentSetParameter(OMX_INDEXTYPE nIndex,
                          OMX_PTR pComponentParameterStructure);

    /* implement ComponentBase::ComponentGet/SetConfig */
    virtual OMX_ERRORTYPE
    ComponentGetConfig(OMX_INDEXTYPE nIndex,
                       OMX_PTR pComponentConfigStructure);
    virtual OMX_ERRORTYPE
    ComponentSetConfig(OMX_INDEXTYPE nIndex,
                       OMX_PTR pComponentConfigStructure);

    /* implement ComponentBase::Processor[*] */
    virtual OMX_ERRORTYPE ProcessorInit(void);  /* Loaded to Idle */
    virtual OMX_ERRORTYPE ProcessorDeinit(void);/* Idle to Loaded */
    virtual OMX_ERRORTYPE ProcessorStart(void); /* Idle to Executing/Pause */
    virtual OMX_ERRORTYPE ProcessorStop(void);  /* Executing/Pause to Idle */
    virtual OMX_ERRORTYPE ProcessorPause(void); /* Executing to Pause */
    virtual OMX_ERRORTYPE ProcessorResume(void);/* Pause to Executing */
    virtual OMX_ERRORTYPE ProcessorFlush(OMX_U32 port_index);
    virtual OMX_ERRORTYPE ProcessorProcess(OMX_BUFFERHEADERTYPE **buffers,
                                           buffer_retain_t *retain,
                                           OMX_U32 nr_buffers);

    /* end of component methods & helpers */

    /*
     * vcp setting helpers
     */
    OMX_ERRORTYPE __RawChangePortParamWithVcp(MixVideoConfigParams *vcp,
            PortVideo *port);

    OMX_ERRORTYPE __Mpeg4ChangePortParamWithVcp(MixVideoConfigParams *vcp,
            PortMpeg4 *port);
    OMX_ERRORTYPE ChangePortParamWithVcp(void);

    OMX_ERRORTYPE __Mpeg4ChangeVcpWithPortParam(MixVideoConfigParams *vcp,
            PortMpeg4 *port, bool *vcp_changed);

    OMX_ERRORTYPE ChangeVcpWithPortParam(MixVideoConfigParams *vcp,
                                         PortVideo *port, bool *vcp_changed);

    static void M4vEncMixBufferCallback(ulong token, uchar *data);

    bool SplitM4vFrameByStartCode(OMX_U8* buf, OMX_U32 len,
                                  OMX_U8** scbuf, OMX_U32* sclen);

    inline M4vStartCodeType GetM4vStartCodeType(OMX_U8* sc);

    inline M4vVopType GetM4vVopType(OMX_U8* vop);

    /* end of vcp setting helpers */

    /* mix video */
    MixVideo *mix;
    MixVideoInitParams *vip;
    MixParams *mvp;
    MixVideoConfigParams *vcp;
    MixDisplayAndroid *display;
    MixBuffer *mixbuffer_in[1];
    MixIOVec *mixiovec_out[1];

    OMX_BOOL is_mixvideodec_configured;
    OMX_U32 inframe_counter;
    OMX_U32 outframe_counter;

    /* for fps */
    OMX_TICKS last_ts;
    float last_fps;

    /* for buffer sharing */
    uint8** share_ptr_array;
    int share_ptr_count;
    int share_ptr_size;
    int share_ptr_stride;
    int share_data_size;

    bool b_config_sent;
    OMX_U8 *mpeg4_enc_buffer;
    OMX_U32 mpeg4_enc_buffer_length;
    OMX_U32 mpeg4_enc_frame_size_left;
    OMX_U8 *temp_coded_data_buffer;
    OMX_U32 temp_coded_data_buffer_size;

    OMX_U32 mpeg4EncPFrames;

    OMX_VIDEO_PARAM_INTEL_BITRATETYPE mpeg4EncParamIntelBitrateType;
    OMX_VIDEO_CONFIG_INTEL_BITRATETYPE mpeg4EncConfigIntelBitrateType;

    OMX_VIDEO_CONFIG_INTEL_AIR mpeg4EncConfigAir;
    OMX_CONFIG_FRAMERATETYPE mpeg4EncFramerate;

    /* constant */
    /* ports */
    const static OMX_U32 NR_PORTS = 2;
    const static OMX_U32 INPORT_INDEX = 0;
    const static OMX_U32 OUTPORT_INDEX = 1;

    /* default buffer */
    const static OMX_U32 INPORT_RAW_ACTUAL_BUFFER_COUNT = 5;
    const static OMX_U32 INPORT_RAW_MIN_BUFFER_COUNT = 1;
    const static OMX_U32 INPORT_RAW_BUFFER_SIZE = 614400;
    const static OMX_U32 OUTPORT_RAW_ACTUAL_BUFFER_COUNT = 2;
    const static OMX_U32 OUTPORT_RAW_MIN_BUFFER_COUNT = 1;
    const static OMX_U32 OUTPORT_RAW_BUFFER_SIZE = 38016;

    const static OMX_U32 INPORT_MPEG4_ACTUAL_BUFFER_COUNT = 10;
    const static OMX_U32 INPORT_MPEG4_MIN_BUFFER_COUNT = 1;

    const static OMX_U32 INPORT_MPEG4_BUFFER_SIZE = 614400;
    const static OMX_U32 OUTPORT_MPEG4_ACTUAL_BUFFER_COUNT = 2;
    const static OMX_U32 OUTPORT_MPEG4_MIN_BUFFER_COUNT = 1;
    const static OMX_U32 OUTPORT_MPEG4_BUFFER_SIZE = 307200;


};

#endif /* __WRS_OMXIL_INTEL_MRST_PSB */
