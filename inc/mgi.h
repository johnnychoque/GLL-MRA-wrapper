/* -*- Mode: C++; c-basic-offset: 4 -*- */
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: mgi.h
 *
 * Description: 
 *
 * Authors: Ramon Aguero (ramon@tlmat.unican.es)
 *          Johnny Choque (jchoque@tlmat.unican.es)
 *
 * Organisation: University of Cantabria
 *
 * This software 
 *
 */

#ifndef _mgi_h_
#define _mgi_h_

#include "config.h"

#include <string>
using namespace std;

struct measConfiguration {
    int MeasurementParaType;
    int MeasurementReportEventType;
    int lowerThreshold;
    int upperThreshold;
    int ReportingPeriodicity;
    int NumberOfSamples;
    int AveragingOfSamples;
    int SamplePeriodicity;
};

struct candidateReportIndication {
    string m_deviceID;
    string m_networkID;
    u_int8_t m_cellID[6];
    u_int16_t m_techType;
    u_int16_t m_rssi;
};

// 9.4 Basic Data Format		
#define	MGI_Enumerated			0x100
#define	MGI_Grouped			0x200
#define	MGI_Integer32			0x300
#define	MGI_OctecString			0x400
#define	MGI_Unsigned16			0x500
#define	MGI_Unsigned32			0x600
#define	MGI_Unsigned8			0x700
#define	MGI_UnsignedVariableLength	0x800
#define	MGI_UTF8String			0x900
#define	MGI_Unsigned64			0xA00
#define	MGI_Integer64			0xB00
#define	MGI_Float32			0xC00
#define	MGI_Float64			0xD00

// 9.3.1 Parameter Type Field				
#define	ParaTAG_ResultCode			0x001 | MGI_Enumerated 
#define	ParaTAG_AccessTechnologyType		0x002 | MGI_Enumerated 
#define	ParaTAG_QoSTrafficClass			0x003 | MGI_Enumerated 
#define	ParaTAG_TrafficDirection		0x004 | MGI_Enumerated 
#define	ParaTAG_LinkEventType			0x005 | MGI_Enumerated 
#define	ParaTAG_AttachModifReason		0x006 | MGI_Enumerated 
#define	ParaTAG_MeasParaType			0x007 | MGI_Enumerated 
#define	ParaTAG_MeasReportReason		0x008 | MGI_Enumerated 
#define	ParaTAG_AveragSamples			0x009 | MGI_Enumerated 
#define	ParaTAG_FE_ID				0x010 | MGI_Enumerated 
#define	ParaTAG_MeasConfiguration		0x001 | MGI_Grouped 
#define	ParaTAG_MeasValueReport			0x002 | MGI_Grouped 
#define	ParaTAG_OfferedQoS			0x003 | MGI_Grouped 
#define	ParaTAG_RequestedQoS			0x004 | MGI_Grouped 
#define	ParaTAG_ProvidedQoS			0x005 | MGI_Grouped 
#define	ParaTAG_TechSpecCellInfo		0x006 | MGI_Grouped 
#define	ParaTAG_Latitude			0x001 | MGI_Integer32 
#define	ParaTAG_Longitude			0x002 | MGI_Integer32 
#define	ParaTAG_MeasValueSNR			0x003 | MGI_Integer32 
#define	ParaTAG_DeviceID			0x001 | MGI_OctecString 
#define	ParaTAG_SourceDeviceID			0x002 | MGI_OctecString 
#define	ParaTAG_TargetDeviceID			0x003 | MGI_OctecString 
#define	ParaTAG_GlobalTerminalID		0x004 | MGI_OctecString 
#define	ParaTAG_AccessTechSpecTerminalID	0x005 | MGI_OctecString 
#define	ParaTAG_Threshold			0x006 | MGI_OctecString 
#define	ParaTAG_ContactPort			0x001 | MGI_Unsigned16 
#define	ParaTAG_LinkEventCapabilities		0x001 | MGI_Unsigned32 
#define	ParaTAG_LinkEventConfiguration		0x002 | MGI_Unsigned32 
#define	ParaTAG_MeasCapabilities		0x003 | MGI_Unsigned32 
#define	ParaTAG_TransactionID			0x001 | MGI_Unsigned64 
#define	ParaTAG_CapacityTotal			0x001 | MGI_Unsigned8 
#define	ParaTAG_CapacityDL			0x002 | MGI_Unsigned8 
#define	ParaTAG_CapacityUL			0x003 | MGI_Unsigned8 
#define	ParaTAG_CapacityDL_RT			0x004 | MGI_Unsigned8 
#define	ParaTAG_CapacityUL_RT			0x005 | MGI_Unsigned8 
#define	ParaTAG_CapacityDL_NRT			0x006 | MGI_Unsigned8 
#define	ParaTAG_CapacityUL_NRT			0x007 | MGI_Unsigned8 
#define	ParaTAG_MeasValueLoadTotal		0x008 | MGI_Unsigned8 
#define	ParaTAG_MeasValueLoadDL			0x009 | MGI_Unsigned8 
#define	ParaTAG_MeasValueLoadUL			0x00A | MGI_Unsigned8 
#define	ParaTAG_MeasValueLoadRT_DL		0x00B | MGI_Unsigned8 
#define	ParaTAG_MeasValueLoadRT_UL		0x00C | MGI_Unsigned8 
#define	ParaTAG_MeasValueLoadNRT_DL		0x00D | MGI_Unsigned8 
#define	ParaTAG_MeasValueLoadNRT_UL		0x00E | MGI_Unsigned8 
#define	ParaTAG_MeasValueSignalStrength		0x00F | MGI_Unsigned8 
#define	ParaTAG_MeasValueNoise			0x010 | MGI_Unsigned8 
#define	ParaTAG_GlobalLinkID			0x001 | MGI_UnsignedVariableLength 
#define	ParaTAG_LocalLinkID			0x002 | MGI_UnsignedVariableLength 
#define	ParaTAG_AccessFlowID			0x003 | MGI_UnsignedVariableLength 
#define	ParaTAG_CellID				0x004 | MGI_UnsignedVariableLength 
#define	ParaTAG_CellRadius			0x005 | MGI_UnsignedVariableLength 
#define	ParaTAG_GuaranteedBitRate		0x006 | MGI_UnsignedVariableLength 
#define	ParaTAG_MaxBitRate			0x007 | MGI_UnsignedVariableLength 
#define	ParaTAG_BitErrorRate			0x008 | MGI_UnsignedVariableLength 
#define	ParaTAG_BlockErrorRate			0x009 | MGI_UnsignedVariableLength 
#define	ParaTAG_Delay				0x00A | MGI_UnsignedVariableLength 
#define	ParaTAG_ReportPeriod			0x00B | MGI_UnsignedVariableLength 
#define	ParaTAG_NumberOfSamples			0x00C | MGI_UnsignedVariableLength 
#define	ParaTAG_SamplePeriod			0x00D | MGI_UnsignedVariableLength 
#define	ParaTAG_Rating				0x00E | MGI_UnsignedVariableLength 
#define	ParaTAG_ErrorDescription		0x001 | MGI_UTF8String 
#define	ParaTAG_NetworkID			0x002 | MGI_UTF8String 
				
#define	MSGTYPE_MGI		0x0100		
#define	MSGTYPE_MHI		0x0200		
#define	MSGTYPE_GHI		0x0300		
#define	MSGTYPE_MSI		0x0400		
#define	MSGTYPE_ACSI		0x0500		
				
// 9.2.4 Message Command Code
#define	MSGTYPE_DEVICE_REGISTRATION		0x0001 | MSGTYPE_MGI 
#define	MSGTYPE_DEVICE_DEREGISTRATION		0x0002 | MSGTYPE_MGI 
#define	MSGTYPE_CANDIDATE_REPORT		0x0003 | MSGTYPE_MGI 
#define	MSGTYPE_LOAD_REPORT			0x0004 | MSGTYPE_MGI 
#define	MSGTYPE_LINK_ATTACH			0x0005 | MSGTYPE_MGI 
#define	MSGTYPE_LINK_ATTACH_NOTIFICATION	0x0006 | MSGTYPE_MGI 
#define	MSGTYPE_LINK_DETACH			0x0007 | MSGTYPE_MGI 
#define	MSGTYPE_LINK_DETACH_NOTIFICATION	0x0008 | MSGTYPE_MGI 
#define	MSGTYPE_LINK_CHANGE			0x0009 | MSGTYPE_MGI 
#define	MSGTYPE_LINK_REPORT			0x000B | MSGTYPE_MGI 
#define	MSGTYPE_ACSI_RegisterFE			0x0001 | MSGTYPE_ACSI 
#define	MSGTYPE_ACSI_UnregisterFE		0x0002 | MSGTYPE_ACSI 
#define	MSGTYPE_MHI_HandoverExecution		0x0001 | MSGTYPE_MHI 
#define	MSGTYPE_MHI_LocatorDeletion		0x0002 | MSGTYPE_MHI 
#define	MSGTYPE_MHI_Trigger			0x0003 | MSGTYPE_MHI 
#define	MSGTYPE_MHI_Constraint			0x0004 | MSGTYPE_MHI 
#define	MSGTYPE_MHI_Locator_COA_Config		0x0009 | MSGTYPE_MHI 
#define	MSGTYPE_MSI_AccessFlowSetup		0x0001 | MSGTYPE_MSI 
#define	MSGTYPE_MSI_AccessFlowRelease		0x0002 | MSGTYPE_MSI 
#define	MSGTYPE_MSI_AccessFlowModification	0x0003 | MSGTYPE_MSI 

// 8.8.1 Result-code
#define ResultCode_UNKNOWN               0x00
#define ResultCode_SUCCESS               0x01
#define ResultCode_UNSUPPORTED           0x10
#define ResultCode_FORMATERROR           0x11
#define ResultCode_MANDATORYPARAMMISSING 0x12
#define ResultCode_INVALIDPARAMVALUE     0x13
#define ResultCode_PARAMTOOMANYTIMES     0x14
#define ResultCode_UNABLEDELIVER         0x15
#define ResultCode_TOOBUSY               0x16


// 6.12 Access Technology Type
#define Tech_UNKNOWN                     0x0000

#define Tech_FIXED_Generic               0x0100
#define Tech_FIXED_Ethernet              0x0101
#define Tech_FIXED_DSL                   0x0102

#define Tech_GSM_Generic                 0x0200

#define Tech_UMTS_Generic                0x0300
#define Tech_UMTS_3GPP_UTRAN_FDD         0x0301
#define Tech_UMTS_3GPP_UTRAN_TDD         0x0302
#define Tech_UMTS_3GPP_General_HSDPA     0x0304
#define Tech_UMTS_3GPP_UTRAN_FDD_HSDPA   0x0305
#define Tech_UMTS_3GPP_UTRAN_TDD_HSDPA   0x0306
#define Tech_UMTS_3GPP_General_HSUPA     0x0308
#define Tech_UMTS_3GPP_UTRAN_FDD_HSDPA_HSUPA 0x030D
#define Tech_UMTS_3GPP_UTRAN_TDD_HSDPA_HSUPA 0x030D

#define Tech_WLAN_Generic                0x0400
#define Tech_WLAN_80211a                 0x0401
#define Tech_WLAN_80211b                 0x0402
#define Tech_WLAN_80211ab                0x0403
#define Tech_WLAN_80211g                 0x0404
#define Tech_WLAN_80211abg               0x0407

#define Tech_WIMAX_Generic               0x0500

// 8.30 Link Event Capabilities
#define LinkEvent_LinkUP                 0x00000001
#define LinkEvent_LinkDown               0x00000002
#define LinkEvent_LinkGoingDown          0x00000004
#define LinkEvent_QueuePilingUp          0x00000008
#define LinkEvent_SDU_TransmitFailure    0x00000010
#define LinkEvent_LinkParamChange        0x00000020
#define LinkEvent_LinkHOPossible         0x00000040
#define LinkEvent_LinkHOProceeding       0x00000080
#define LinkEvent_LinkHOComplete         0x00000100
#define LinkEvent_CandidateFound         0x00000200
#define LinkEvent_CandidateLost          0x00000400
#define LinkEvent_CandidateParamChange   0x00000800

// 8.32 AttachModificationReason
#define AttachModifReason_Unknown        0x00
#define AttachModifReason_Connect        0x01
#define AttachModifReason_Disconnect     0x02
#define AttachModifReason_HOexecution    0x10
#define AttachModifReason_HOpreparation  0x11
#define AttachModifReason_HOprepCancel   0x12
#define AttachModifReason_LocationUpdate 0x13
#define AttachModifReason_PagingAnswer   0x14
#define AttachModifReason_ShutDown       0x21
#define AttachModifReason_LowPower       0x22
#define AttachModifReason_Malfunction    0x23
#define AttachModifReason_Maintenance    0x24
#define AttachModifReason_Overload       0x25
#define AttachModifReason_AccessDenied   0x26
#define AttachModifReason_ConnectionLost 0x27

// 8.40 Measurement Capability
#define MeasCapability_TotalLoad         0x00000001
#define MeasCapability_TotalLoadDL       0x00000002
#define MeasCapability_TotalLoadUL       0x00000004
#define MeasCapability_TotalLoadRT       0x00000008
#define MeasCapability_TotalLoatNRT      0x00000010
#define MeasCapability_LoadRT_DL         0x00000020
#define MeasCapability_LoadRT_UL         0x00000040
#define MeasCapability_LoadNRT_DL        0x00000080
#define MeasCapability_LoadNRT_UL        0x00000100 
#define MeasCapability_DelayDL           0x00000200
#define MeasCapability_DelayRT_DL        0x00000400
#define MeasCapability_DelayNRT_DL       0x00000800
#define MeasCapability_RSSI_DL           0x00010000
#define MeasCapability_RSSI_UL           0x00020000
#define MeasCapability_NoiseDL           0x00040000
#define MeasCapability_NoiseUL           0x00080000
#define MeasCapability_SNR_DL            0x00100000
#define MeasCapability_SNR_UL            0x00200000
#define MeasCapability_MaxBitRateDL      0x00400000
#define MeasCapability_MaxBitRateUL      0x00800000
#define MeasCapability_BER_DL            0x01000000
#define MeasCapability_BER_UL            0x02000000
#define MeasCapability_BLER_DL           0x04000000
#define MeasCapability_BLER_UL           0x08000000

// 8.43 Measurement Parameter Type
#define MeasParaType_Unknown             0x00
#define MeasParaType_Load_Total          0x10
#define MeasParaType_Load_DL             0x11
#define MeasParaType_Load_UL             0x12
#define MeasParaType_Load_RT_DL          0x13
#define MeasParaType_Load_RT_UL          0x14
#define MeasParaType_Load_NRT_DL         0x15
#define MeasParaType_Delay               0x20
#define MeasParaType_SignalStrength      0x30
#define MeasParaType_Noise               0x40
#define MeasParaType_SNR                 0x50
#define MeasParaType_BER                 0x60
#define MeasParaType_BlockErrorRate      0x70
#define MeasParaType_MaximumBitRate_DL   0x80
#define MeasParaType_MaximumBitRate_UL   0x81

// 8.44 Measurement Report Event Type
#define MeasReportEventType_Unknown      0x00
#define MeasReportEventType_SingleMeas   0x01
#define MeasReportEventType_ThreshPeriod 0x80
#define MeasReportEventType_CancelMeas   0xFE
#define MeasReportEventType_CancelAll    0xFF

#define MAX_PKT_SIZE                     512

#define CURRENT_VERSION                  0x01
#define MGI_GISTHOPS                     0x01

#define INDICATION                       0x00
#define REQUEST                          0x01
#define ANSWER                           0x02

#define QUERY_GIST_TYPE                  0x00
#define RESPONSE_GIST_TYPE               0x01    
#define DATA_GIST_TYPE                   0x03

struct __attribute__((packed)) mgiHeader {
    u_int8_t version;
    u_int8_t gistHops;
    u_int16_t msgLength;
    u_int16_t nslpID;
    u_int8_t type;
#ifdef LITTLEENDIAN
    u_int8_t res:5, e_:1, r_:1, s_:1;
#else
    u_int8_t s_:1, r_:1, e_:1, res:5;
#endif
};



#endif // _mgi_h_
