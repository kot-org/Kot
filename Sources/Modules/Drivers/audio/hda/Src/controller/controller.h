#pragma once

#include <kot/sys.h>
#include <kot/bits.h>
#include <kot/assert.h>
#include <kot/utils/vector.h>

#include <kot/uisd/srvs/pci.h>
#include <kot/uisd/srvs/time.h>
#include <kot/uisd/srvs/audio.h>
#include <kot/uisd/srvs/system.h>

#include <kot++/printf.h>

#include <srv/srv.h>

extern process_t Proc;

#define HDA_CLASS           0x4
#define HDA_SUBCLASS        0x3

#define HDA_BDL_SIZE        0x1000

#define HDA_GCTL_RESET                          (1 << 0)
#define HDA_GCTL_UNSOLICITED_RESPONSES          (1 << 8)
#define HDA_INTCTL_GLOBAL_INTERRUPT             (1 << 31)
#define HDA_INTCTL_CONTROLLER_INTERRUPT         (1 << 30)
#define HDA_RIRB_AND_CORB_DMA                   (1 << 1)
#define HDA_CORB_GENERATE_ERROR_INTERRUPT       (1 << 0)
#define HDA_RIRB_GENERATE_RESPONSE_INTERRUPT    (1 << 0)

#define HDA_GCAP_OUTPUT_STREAMS(x)          ((x >> 12) & 0xf)
#define HDA_GCAP_INPUT_STREAMS(x)           ((x >> 8) & 0xf)
#define HDA_GCAP_BIDIRECTIONAL_STREAMS(x)   ((x >> 3) & 0xf)
#define HDA_GCAP_64BIT(x)                   (x & 0x1)

#define HDA_SUPPORTED_CORB_SIZE_MASK        (7 << 4)
#define HDA_SUPPORTED_CORB_SIZE_2           (1 << 4)
#define HDA_SUPPORTED_CORB_SIZE_16          (1 << 5)
#define HDA_SUPPORTED_CORB_SIZE_256         (1 << 6)

#define HDA_SUPPORTED_RIRB_SIZE_MASK        (7 << 4)
#define HDA_SUPPORTED_RIRB_SIZE_2           (1 << 4)
#define HDA_SUPPORTED_RIRB_SIZE_16          (1 << 5)
#define HDA_SUPPORTED_RIRB_SIZE_256         (1 << 6)

#define HDA_RB_SIZE_MASK                    0x3

#define HDA_CORB_SIZE_2                     0
#define HDA_CORB_SIZE_16                    1
#define HDA_CORB_SIZE_256                   2

#define HDA_RIRB_SIZE_2                     0
#define HDA_RIRB_SIZE_16                    1
#define HDA_RIRB_SIZE_256                   2

#define HDA_CORB_READP_RESET                (1 << 15) 
#define HDA_RIRB_WRITEP_RESET               (1 << 15) 

#define HDA_CORB_WRITEP_MASK                (0xff)
#define HDA_RIRB_WRITEP_MASK                (0xff)

#define HDA_RIRB_RESPONSEINTERRUPT          (1 << 0)
#define HDA_RIRB_OVERRUN                    (1 << 2)

// Stream reset
#define HDA_STREAM_CTL_RST                  0x1
// Stream run
#define HDA_STREAM_CTL_RUN                  0x2
// Interrupt on completion enable
#define HDA_STREAM_CTL_ICOE                 0x4
// FIFO Error Interrupt
#define HDA_STREAM_CTL_FEIE                 0x8
// Descriptor error interuupt
#define HDA_STREAM_CTL_DEIE                 0x10

#define HDA_STREAM_CTL_STRIPE_MASK          (3 << 16)
#define HDA_STREAM_CTL_TRAFFIC_PRIORITY     (1 << 18)
// Bid direction control
#define HDA_STREAM_CTL_BIDIRECTIONAL_DIR    (1 << 19)
// ID associated with stream data
#define HDA_STREAM_CTL_STREAM_NUM_MASK      (0xf << 20)
#define HDA_STREAM_CTL_STREAM_NUM(x)        ((x & 0xf) << 20)

#define HDA_STREAM_FMT_CHAN_MASK            (0xf)

// Buffer completion interrupt status
#define HDA_STREAM_STS_BCIS                 (1 << 2)
#define HDA_STREAM_STS_FIFO_ERROR           (1 << 3)
#define HDA_STREAM_STS_DESC_ERROR           (1 << 4)
#define HDA_STREAM_STS_FIFO_READY           (1 << 5)

#define HDA_MAX_CODECS                          15
#define HDA_MAX_STREAMS                         15

// Payload for set stream, channel command
#define HDA_CODEC_SET_STREAM_CHAN_PAYLOAD(stream, chan) (((stream & 0xf) << 4) | (chan & 0xf))

// Audio widget capabilities
#define HDA_W_AUDIO_CHAN_COUNT              0x1
#define HDA_W_AUDIO_AMP_IN                  0x2
#define HDA_W_AUDIO_AMP_OUT                 0x4
#define HDA_W_AUDIO_FORMAT_OVERRIDE         0x8
#define HDA_W_AUDIO_STRIPE                  0x10

#define HDA_W_AUDIO_TYPE(x)                 ((x >> 20) & 0xf)
#define HDA_W_AUDIO_STEP_OFFSET(x)          ((x >> 0) & 0x7f)
#define HDA_W_AUDIO_GAIN_STEP(x)            ((x >> 8) & 0x7f)
#define HDA_W_AUDIO_STEP_SIZE(x)            ((x >> 16) & 0x7f)
#define HDA_W_AUDIO_OUTPUT                  0x0
#define HDA_W_AUDIO_INPUT                   0x1
#define HDA_W_AUDIO_MIXER                   0x2
#define HDA_W_AUDIO_SELECTOR                0x3
#define HDA_W_AUDIO_PIN_COMPLEX             0x4
#define HDA_W_AUDIO_POWER_WIDGET            0x5
#define HDA_W_AUDIO_VOLUME_KNOB             0x6
#define HDA_W_AUDIO_BEEP_GENERATOR          0x7
#define HDA_W_AUDIO_VENDOR_DEFINED          0xf

// Default stream number for output
#define STREAM_ID_PCMOUT                    0x1

#define HDA_STREAM_DESCRIPTOR_LVI_MASK          0xff

// VERBS
#define HDA_VERB_GET_STREAM_FORMAT		        0x0a00
#define HDA_VERB_GET_AMP_GAIN_MUTE		        0x0b00
#define HDA_VERB_GET_PROC_COEF			        0x0c00
#define HDA_VERB_GET_COEF_INDEX			        0x0d00
#define HDA_VERB_GET_PARAMETERS			        0x0f00
#define HDA_VERB_GET_CONNECT_SEL			    0x0f01
#define HDA_VERB_GET_CONNECT_LIST		        0x0f02
#define HDA_VERB_GET_PROC_STATE			        0x0f03
#define HDA_VERB_GET_SDI_SELECT			        0x0f04
#define HDA_VERB_GET_POWER_STATE			    0x0f05
#define HDA_VERB_GET_CONV			            0x0f06
#define HDA_VERB_GET_PIN_WIDGET_CONTROL		    0x0f07
#define HDA_VERB_GET_UNSOLICITED_RESPONSE	    0x0f08
#define HDA_VERB_GET_PIN_SENSE			        0x0f09
#define HDA_VERB_GET_BEEP_CONTROL		        0x0f0a
#define HDA_VERB_GET_EAPD_BTLENABLE		        0x0f0c
#define HDA_VERB_GET_DIGI_CONVERT_1		        0x0f0d
#define HDA_VERB_GET_DIGI_CONVERT_2		        0x0f0e
#define HDA_VERB_GET_VOLUME_KNOB_CONTROL		0x0f0f
#define HDA_VERB_GET_GPIO_DATA			        0x0f15
#define HDA_VERB_GET_GPIO_MASK			        0x0f16
#define HDA_VERB_GET_GPIO_DIRECTION		        0x0f17
#define HDA_VERB_GET_GPIO_WAKE_MASK		        0x0f18
#define HDA_VERB_GET_GPIO_UNSOLICITED_RSP_MASK	0x0f19
#define HDA_VERB_GET_GPIO_STICKY_MASK		    0x0f1a
#define HDA_VERB_GET_CONFIG_DEFAULT		        0x0f1c
#define HDA_VERB_GET_SUBSYSTEM_ID		        0x0f20
#define HDA_VERB_GET_CVT_CHAN_COUNT		        0x0f2d
#define HDA_VERB_GET_HDMI_DIP_SIZE		        0x0f2e
#define HDA_VERB_GET_HDMI_ELDD			        0x0f2f
#define HDA_VERB_GET_HDMI_DIP_INDEX		        0x0f30
#define HDA_VERB_GET_HDMI_DIP_DATA		        0x0f31
#define HDA_VERB_GET_HDMI_DIP_XMIT		        0x0f32
#define HDA_VERB_GET_HDMI_CP_CTRL		        0x0f33
#define HDA_VERB_GET_HDMI_CHAN_SLOT		        0x0f34
#define HDA_VERB_SET_STREAM_FORMAT		        0x200
#define HDA_VERB_SET_AMP_GAIN_MUTE		        0x300
#define HDA_VERB_SET_PROC_COEF			        0x400
#define HDA_VERB_SET_COEF_INDEX			        0x500
#define HDA_VERB_SET_CONNECT_SEL			    0x701
#define HDA_VERB_SET_PROC_STATE			        0x703
#define HDA_VERB_SET_SDI_SELECT			        0x704
#define HDA_VERB_SET_POWER_STATE			    0x705
#define HDA_VERB_SET_CHANNEL_STREAMID		    0x706
#define HDA_VERB_SET_PIN_WIDGET_CONTROL		    0x707
#define HDA_VERB_SET_UNSOLICITED_ENABLE		    0x708
#define HDA_VERB_SET_PIN_SENSE			        0x709
#define HDA_VERB_SET_BEEP_CONTROL		        0x70a
#define HDA_VERB_SET_EAPD_BTLENABLE		        0x70c
#define HDA_VERB_SET_DIGI_CONVERT_1		        0x70d
#define HDA_VERB_SET_DIGI_CONVERT_2		        0x70e
#define HDA_VERB_SET_VOLUME_KNOB_CONTROL		0x70f
#define HDA_VERB_SET_GPIO_DATA			        0x715
#define HDA_VERB_SET_GPIO_MASK			        0x716
#define HDA_VERB_SET_GPIO_DIRECTION		        0x717
#define HDA_VERB_SET_GPIO_WAKE_MASK		        0x718
#define HDA_VERB_SET_GPIO_UNSOLICITED_RSP_MASK	0x719
#define HDA_VERB_SET_GPIO_STICKY_MASK		    0x71a
#define HDA_VERB_SET_CONFIG_DEFAULT_BYTES_0	    0x71c
#define HDA_VERB_SET_CONFIG_DEFAULT_BYTES_1	    0x71d
#define HDA_VERB_SET_CONFIG_DEFAULT_BYTES_2	    0x71e
#define HDA_VERB_SET_CONFIG_DEFAULT_BYTES_3	    0x71f
#define HDA_VERB_SET_EAPD				        0x788
#define HDA_VERB_SET_CODEC_RESET			    0x7ff
#define HDA_VERB_SET_CVT_CHAN_COUNT		        0x72d
#define HDA_VERB_SET_HDMI_DIP_INDEX		        0x730
#define HDA_VERB_SET_HDMI_DIP_DATA		        0x731
#define HDA_VERB_SET_HDMI_DIP_XMIT		        0x732
#define HDA_VERB_SET_HDMI_CP_CTRL		        0x733
#define HDA_VERB_SET_HDMI_CHAN_SLOT		        0x734


#define HDA_PARAMETER_VENDOR_ID		                0x00
#define HDA_PARAMETER_SUBSYSTEM_ID		            0x01
#define HDA_PARAMETER_REV_ID			            0x02
#define HDA_PARAMETER_NODE_COUNT		            0x04
#define HDA_PARAMETER_FUNCTION_TYPE		            0x05
#define HDA_PARAMETER_AUDIO_FG_CAP		            0x08
#define HDA_PARAMETER_AUDIO_WIDGET_CAP		        0x09
#define HDA_PARAMETER_PCM			                0x0a
#define HDA_PARAMETER_STREAM			            0x0b
#define HDA_PARAMETER_PIN_CAP			            0x0c
#define HDA_PARAMETER_AMP_IN_CAP		            0x0d
#define HDA_PARAMETER_CONNLIST_LEN		            0x0e
#define HDA_PARAMETER_POWER_STATE		            0x0f
#define HDA_PARAMETER_PROC_CAP			            0x10
#define HDA_PARAMETER_GPIO_CAP			            0x11
#define HDA_PARAMETER_AMP_OUT_CAP		            0x12
#define HDA_PARAMETER_VOL_KNB_CAP		            0x13
#define HDA_PARAMETER_HDMI_LPCM_CAP		            0x20

/* Pin widget capabilies */
#define HDA_PINCAP_IMP_SENSE    (1 << 0)	/* impedance sense capable */
#define HDA_PINCAP_TRIG_REQ		(1 << 1)	/* trigger required */
#define HDA_PINCAP_PRES_DETECT	(1 << 2)	/* presence detect capable */
#define HDA_PINCAP_HP_DRV		(1 << 3)	/* headphone drive capable */
#define HDA_PINCAP_OUT			(1 << 4)	/* output capable */
#define HDA_PINCAP_IN			(1 << 5)	/* input capable */
#define HDA_PINCAP_BALANCE		(1 << 6)	/* balanced I/O capable */

#define HDA_SUPPCM_RATES		(0xfff << 0)
#define HDA_SUPPCM_BITS_8		(1<<16)
#define HDA_SUPPCM_BITS_16		(1<<17)
#define HDA_SUPPCM_BITS_20		(1<<18)
#define HDA_SUPPCM_BITS_24		(1<<19)
#define HDA_SUPPCM_BITS_32		(1<<20)

enum {
	HDA_GRP_AUDIO_FUNCTION = 0x01,
	HDA_GRP_MODEM_FUNCTION = 0x02,
};

enum {
	HDA_WID_AUD_OUT     = 0x0,		    /* Audio Out */
	HDA_WID_AUD_IN      = 0x1,		    /* Audio In */
	HDA_WID_AUD_MIX     = 0x2,		    /* Audio Mixer */
	HDA_WID_AUD_SEL     = 0x3,		    /* Audio Selector */
	HDA_WID_PIN         = 0x4,		    /* Pin Complex */
	HDA_WID_POWER       = 0x5,		    /* Power */
	HDA_WID_VOL_KNB     = 0x6,		    /* Volume Knob */
	HDA_WID_BEEP        = 0x7,		    /* Beep Generator */
	HDA_WID_VENDOR      = 0x0f	        /* Vendor specific */
};

struct HDAStreamFormat{
    uint8_t NumberOfChannels:4;
    uint8_t BitsPerSample:3;
    uint8_t SampleBaseRateDivisor:3;
    uint8_t SampleBaseRateMultiplier:3;
    uint8_t SampleBaseRate:1;
} __attribute__((packed));

struct HDAStreamDescriptor {
    volatile uint32_t Control:24;
    volatile uint8_t Status;
    volatile uint32_t CurrentBufferLinkPosition;
    volatile uint32_t CyclicBufferSize;
    volatile uint16_t LastValidIndex;
    volatile uint8_t Reserved1[2];
    volatile uint16_t StreamFIFOSize;
    volatile uint16_t Format;
    volatile uint8_t Reserved2[4];
    volatile uint64_t BufferDescriptorListPointer;
}__attribute__((packed));

struct HDABufferDescriptorEntry{
    volatile uint64_t Address;
    volatile uint32_t Length;
    volatile uint32_t InterruptOnCompletion:1;
    volatile uint32_t Reserved:31;
}__attribute__((packed));

struct HDAControllerRegs {
    volatile uint16_t GlobalCapabilities;
    volatile uint8_t MinorVersion;
    volatile uint8_t MajorVersion;
    volatile uint16_t OutputPayloadCapability;
    volatile uint16_t InputPayloadCapability;
    volatile uint32_t GlobalControl;
    volatile uint16_t WakeEnable;
    volatile uint16_t WakeStatus;
    volatile uint16_t GlobalStatus;
    volatile uint8_t Reserved1[6];
    volatile uint16_t OutputStreamPayloadCapability;
    volatile uint16_t InputStreamPayloadCapability;
    volatile uint8_t Reserved2[4];
    volatile uint32_t InterruptControl;
    volatile uint32_t InterruptStatus;
    volatile uint8_t Reserved3[8];
    volatile uint32_t WallClockCounter;
    volatile uint8_t Reserved4[4];
    volatile uint32_t StreamSynchronization;
    volatile uint8_t Reserved5[4];
    volatile uint64_t CORBBaseAddress;
    volatile uint16_t CORBWritePointer;
    volatile uint16_t CORBReadPointer;
    volatile uint8_t CORBControl;
    volatile uint8_t CORBStatus;
    volatile uint8_t CORBSize;
    volatile uint8_t Reserved6;
    volatile uint64_t RIRBBaseAddress;
    volatile uint16_t RIRBWritePointer;
    volatile uint16_t ResponseInterruptCount;
    volatile uint8_t RIRBControl;
    volatile uint8_t RIRBStatus;
    volatile uint8_t RIRBSize;
    volatile uint8_t Reserved7;
    volatile uint32_t ImmediateCommandOutputInterface;
    volatile uint32_t ImmediateCommandInputInterface;
    volatile uint16_t ImmediateCommandStatus;
    volatile uint8_t Reserved8[6];
    volatile uint64_t DMAPositionBuffer;
    volatile uint8_t Reserved9[8];
    HDAStreamDescriptor Streams[];
}__attribute__((packed));


struct NodeConfiguration{
    uint8_t Sequence:4;
    uint8_t DefaultAssociation:4;
    uint8_t Misc:4;
    uint8_t Color:4;
    uint8_t ConnectionType:4;
    uint8_t DefaultDevice:4;
    uint8_t Location:6;
    uint8_t PortConnectivity:2;
}__attribute__((packed));

enum StreamType{
    Output = 0,
    Input = 1,
    Bidirectional = 2,
};

struct HDAWidget{
    uint32_t Node;
    uint8_t Type;
    uint32_t AudioCapabilities;
    struct HDAFunction* Function;
};

struct HDAFunction{
    uint32_t Node;
    uint8_t Type;
    uint32_t WidgetsStart;
    uint32_t WidgetsCount;
    vector_t* Widgets;
    uint32_t GainStepOut;
    uint32_t AMPOutNode;
    struct HDACodec* Codec;
};

struct HDACodec{
    uint32_t Index;
    uint32_t FunctionsStart;
    uint32_t FunctionsCount;
    vector_t* Functions;
};

struct HDAOutput{
    class HDAController* ControllerParent;

    bool IsCurrentRunning;

    uint32_t SupportedPCM;

    HDAFunction* Function;

    struct HDAStream* Stream;

    HDAStreamFormat Format;

    AudioEncoding SampleFormat;

    event_t OffsetUpdateEvent;

    srv_audio_device_t AudioDevice;
};

struct HDAStream{
    HDAStreamDescriptor* Descriptor;

    uint32_t StreamNumber;

    uintptr_t BufferDescriptorListPhysicalAddress;
    uint32_t BufferDescriptorListEntries;
    HDABufferDescriptorEntry* BufferDescriptorList;

    ksmem_t BufferKey;
    uintptr_t Buffer;
    size64_t Size;
};

class HDAController{
    public:
        uintptr_t HDABaseAddress;
        HDAControllerRegs* Registers;

        bool Supports64Bit;

        uint32_t NumInputStreams;
        uint32_t NumOutputStreams;
        uint32_t NumBidirectionalStreams;
        uint32_t StreamCount;

        uint16_t CORBEntries;
        uint32_t* CORB;
        uint16_t RIRBEntries;
        uint64_t* RIRB;

        uint64_t Lock;

        thread_t InterruptThread;

        HDACodec Codecs[HDA_MAX_CODECS];

        HDAOutput* Outputs[HDA_MAX_STREAMS];
        
        KResult SetupCORB();
        KResult SetupRIRB();

        HDAController(PCIDeviceID_t Device);
        ~HDAController();

        KResult ConfigureStreamFormat(HDAOutput* Output);
        KResult SetSampleRate(HDAOutput* Output, uint32_t SampleRate);
        KResult SetChannel(HDAOutput* Output, uint8_t Channels);
        KResult SetVolume(HDAOutput* Output, uint8_t Volume);
        KResult SetSoundEncoding(HDAOutput* Output, AudioEncoding Encoding);
        KResult GetNodeConfiguration(HDAWidget* Widget, NodeConfiguration* Config);

        KResult TransferData(HDAOutput* Output, uintptr_t Buffer, size64_t Size, uint64_t Offset);
        KResult ChangeStatus(HDAOutput* Output, bool IsRunning);
        KResult GetOffset(HDAOutput* Output, uint64_t* Offset);

        void DetectCodec(uint8_t Codec);
        void WidgetInitialize(HDAFunction* Function, uint8_t WidgetNode);
        void AddOutputFunction(HDAFunction* Function);

        HDAStream* CreateStream(HDACodec* Codec, StreamType Type);

        uint32_t GetCodecParameter(uint32_t Codec, uint32_t Node, uint32_t Parameter);
        KResult SendVerb(uint32_t Verb, uint64_t* Response);
        
        inline uint32_t MakeVerb(uint32_t Verb, uint8_t NodeID, uint8_t CodecAddress) {
            return (Verb & 0xfffff) | ((NodeID & 0xff) << 20) | ((CodecAddress & 0xf) << 28);
        }
};
