
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

