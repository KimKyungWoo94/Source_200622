/* Automatically generated file - do not edit */

#include "asn1defs.h"
#include "dot3-asn.h"

const ASN1CType asn1_type_EXT_TYPE[] = {
  (ASN1_CTYPE_OBJECT_CLASS << ASN1_CTYPE_SHIFT) | 0x0 | 0x0,
  2,

  0x1,
  (intptr_t)asn1_type_RefExt,
  0,
  (intptr_t)"&extRef",

  0x0,
  0,
  0,
  (intptr_t)"&ExtValue",
};

const ASN1CType asn1_type_RefExt[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x2,
  0x0,
  0xff,
};

const ASN1CType asn1_type_DataRate80211[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x2,
  0x0,
  0xff,
};

const ASN1CType asn1_type_TXpower80211[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x2,
  0xffffff80,
  0x7f,
};

const ASN1CType asn1_type_ChannelNumber80211[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x2,
  0x0,
  0xff,
};

const ASN1CType asn1_type_RepeatRate[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x2,
  0x0,
  0xff,
};

const ASN1CType asn1_type_TwoDLocation[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  2,
  sizeof(TwoDLocation),

  offsetof(TwoDLocation, latitude) | 0x0,
  (intptr_t)asn1_type_Latitude,
  0,
  (intptr_t)"latitude",

  offsetof(TwoDLocation, longitude) | 0x0,
  (intptr_t)asn1_type_Longitude,
  0,
  (intptr_t)"longitude",

};

const ASN1CType asn1_type_ThreeDLocation[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  3,
  sizeof(ThreeDLocation),

  offsetof(ThreeDLocation, latitude) | 0x0,
  (intptr_t)asn1_type_Latitude,
  0,
  (intptr_t)"latitude",

  offsetof(ThreeDLocation, longitude) | 0x0,
  (intptr_t)asn1_type_Longitude,
  0,
  (intptr_t)"longitude",

  offsetof(ThreeDLocation, elevation) | 0x0,
  (intptr_t)asn1_type_Elevation,
  0,
  (intptr_t)"elevation",

};

const ASN1CType asn1_type_AdvertiserIdentifier[] = {
  (ASN1_CTYPE_CHAR_STRING << ASN1_CTYPE_SHIFT) | 0x0 | 0xc,
  ASN1_CSTR_UTF8String,
};

static const ASN1CType asn1_type__local_0[] = {
  (ASN1_CTYPE_BIT_STRING << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x3,
  0x3,
};

static const ASN1CType asn1_type__local_1[] = {
  (ASN1_CTYPE_OCTET_STRING << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100001,
  0x0,
  0x1f,
};

const ASN1CType asn1_type_ProviderServiceContext[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  2,
  sizeof(ProviderServiceContext),

  offsetof(ProviderServiceContext, fillBit) | 0x0,
  (intptr_t)asn1_type__local_0,
  0,
  (intptr_t)"fillBit",

  offsetof(ProviderServiceContext, psc) | 0x0,
  (intptr_t)asn1_type__local_1,
  0,
  (intptr_t)"psc",

};

const ASN1CType asn1_type_IPv6Address[] = {
  (ASN1_CTYPE_OCTET_STRING << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x4,
  0x10,
  0x10,
};

const ASN1CType asn1_type_ServicePort[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x2,
  0x0,
  0xffff,
};

const ASN1CType asn1_type_MACaddress[] = {
  (ASN1_CTYPE_OCTET_STRING << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x4,
  0x6,
  0x6,
};

const ASN1CType asn1_type_RcpiThreshold[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x2,
  0x0,
  0xff,
};

const ASN1CType asn1_type_WsaCountThreshold[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x2,
  0x0,
  0xff,
};

const ASN1CType asn1_type_WsaCountThresholdInterval[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x2,
  0x0,
  0xff,
};

static const ASN1CType asn1_type__local_2[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  (intptr_t)asn1_type_EdcaParameterRecord,
};

static const ASN1CType asn1_type__local_3[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100001,
  (intptr_t)asn1_type_EdcaParameterRecord,
};

static const ASN1CType asn1_type__local_4[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100002,
  (intptr_t)asn1_type_EdcaParameterRecord,
};

static const ASN1CType asn1_type__local_5[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100003,
  (intptr_t)asn1_type_EdcaParameterRecord,
};

const ASN1CType asn1_type_EdcaParameterSet[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  4,
  sizeof(EdcaParameterSet),

  offsetof(EdcaParameterSet, acbeRecord) | 0x0,
  (intptr_t)asn1_type__local_2,
  0,
  (intptr_t)"acbeRecord",

  offsetof(EdcaParameterSet, acbkRecord) | 0x0,
  (intptr_t)asn1_type__local_3,
  0,
  (intptr_t)"acbkRecord",

  offsetof(EdcaParameterSet, acviRecord) | 0x0,
  (intptr_t)asn1_type__local_4,
  0,
  (intptr_t)"acviRecord",

  offsetof(EdcaParameterSet, acvoRecord) | 0x0,
  (intptr_t)asn1_type__local_5,
  0,
  (intptr_t)"acvoRecord",

};

static const ASN1CType asn1_type__local_6[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x0,
  0x1,
};

static const ASN1CType asn1_type__local_7[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100001,
  0x0,
  0x3,
};

static const ASN1CType asn1_type__local_8[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100002,
  0x0,
  0x1,
};

static const ASN1CType asn1_type__local_9[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100003,
  0x0,
  0xf,
};

static const ASN1CType asn1_type__local_10[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100004,
  0x0,
  0xf,
};

static const ASN1CType asn1_type__local_11[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100005,
  0x0,
  0xf,
};

static const ASN1CType asn1_type__local_12[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100006,
  0x0,
  0xffff,
};

const ASN1CType asn1_type_EdcaParameterRecord[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  7,
  sizeof(EdcaParameterRecord),

  offsetof(EdcaParameterRecord, res) | 0x0,
  (intptr_t)asn1_type__local_6,
  0,
  (intptr_t)"res",

  offsetof(EdcaParameterRecord, aci) | 0x0,
  (intptr_t)asn1_type__local_7,
  0,
  (intptr_t)"aci",

  offsetof(EdcaParameterRecord, acm) | 0x0,
  (intptr_t)asn1_type__local_8,
  0,
  (intptr_t)"acm",

  offsetof(EdcaParameterRecord, aifsn) | 0x0,
  (intptr_t)asn1_type__local_9,
  0,
  (intptr_t)"aifsn",

  offsetof(EdcaParameterRecord, ecwMax) | 0x0,
  (intptr_t)asn1_type__local_10,
  0,
  (intptr_t)"ecwMax",

  offsetof(EdcaParameterRecord, ecwMin) | 0x0,
  (intptr_t)asn1_type__local_11,
  0,
  (intptr_t)"ecwMin",

  offsetof(EdcaParameterRecord, txopLimit) | 0x0,
  (intptr_t)asn1_type__local_12,
  0,
  (intptr_t)"txopLimit",

};

const ASN1CType asn1_type_ChannelAccess80211[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x2,
  0x0,
  0xff,
};

static const ASN1CType asn1_type__local_13[] = {
  (ASN1_CTYPE_BIT_STRING << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x1,
  0x1,
};

static const ASN1CType asn1_type__local_14[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100001,
  0xca5b1700,
  0x35a4e901,
};

const ASN1CType asn1_type_Latitude[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  2,
  sizeof(Latitude),

  offsetof(Latitude, fill) | 0x0,
  (intptr_t)asn1_type__local_13,
  0,
  (intptr_t)"fill",

  offsetof(Latitude, lat) | 0x0,
  (intptr_t)asn1_type__local_14,
  0,
  (intptr_t)"lat",

};

const ASN1CType asn1_type_Longitude[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100001,
  0x94b62e01,
  0x6b49d201,
};

const ASN1CType asn1_type_Elevation[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100002,
  0xfffff000,
  0xefff,
};

const ASN1CType asn1_type_SrvAdvMsg[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  2,
  sizeof(SrvAdvMsg),

  offsetof(SrvAdvMsg, version) | 0x0,
  (intptr_t)asn1_type_SrvAdvPrtVersion,
  0,
  (intptr_t)"version",

  offsetof(SrvAdvMsg, body) | 0x0,
  (intptr_t)asn1_type_SrvAdvBody,
  0,
  (intptr_t)"body",

};

const ASN1CType asn1_type_SrvAdvPrtVersion[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  2,
  sizeof(SrvAdvPrtVersion),

  offsetof(SrvAdvPrtVersion, messageID) | 0x0,
  (intptr_t)asn1_type_SrvAdvMessageType,
  0,
  (intptr_t)"messageID",

  offsetof(SrvAdvPrtVersion, rsvAdvPrtVersion) | 0x0,
  (intptr_t)asn1_type_RsvAdvPrtVersion,
  0,
  (intptr_t)"rsvAdvPrtVersion",

};

const ASN1CType asn1_type_RsvAdvPrtVersion[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100001,
  0x0,
  0x7,
};

const ASN1CType asn1_type_SrvAdvMessageType[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x0,
  0x1,
};

const ASN1CType asn1_type_SrvAdvBody[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100001,
  5,
  sizeof(SrvAdvBody),

  offsetof(SrvAdvBody, changeCount) | 0x0,
  (intptr_t)asn1_type_SrvAdvChangeCount,
  0,
  (intptr_t)"changeCount",

  offsetof(SrvAdvBody, extensions) | 0x8000000,
  (intptr_t)asn1_type_SrvAdvMsgHeaderExts,
  offsetof(SrvAdvBody, extensions_option),
  (intptr_t)"extensions",

  offsetof(SrvAdvBody, serviceInfos) | 0x8000000,
  (intptr_t)asn1_type_ServiceInfos,
  offsetof(SrvAdvBody, serviceInfos_option),
  (intptr_t)"serviceInfos",

  offsetof(SrvAdvBody, channelInfos) | 0x8000000,
  (intptr_t)asn1_type_ChannelInfos,
  offsetof(SrvAdvBody, channelInfos_option),
  (intptr_t)"channelInfos",

  offsetof(SrvAdvBody, routingAdvertisement) | 0x8000000,
  (intptr_t)asn1_type_RoutingAdvertisement,
  offsetof(SrvAdvBody, routingAdvertisement_option),
  (intptr_t)"routingAdvertisement",

};

const ASN1CType asn1_type_SrvAdvChangeCount[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  2,
  sizeof(SrvAdvChangeCount),

  offsetof(SrvAdvChangeCount, saID) | 0x0,
  (intptr_t)asn1_type_SrvAdvID,
  0,
  (intptr_t)"saID",

  offsetof(SrvAdvChangeCount, contentCount) | 0x0,
  (intptr_t)asn1_type_SrvAdvContentCount,
  0,
  (intptr_t)"contentCount",

};

const ASN1CType asn1_type_SrvAdvID[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x0,
  0xf,
};

const ASN1CType asn1_type_SrvAdvContentCount[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100001,
  0x0,
  0xf,
};

const ASN1CType asn1_type_SrvAdvMsgHeaderExts[] = {
  (ASN1_CTYPE_SEQUENCE_OF << ASN1_CTYPE_SHIFT) | 0x1000000 | 0x100001,
  0x0,
  sizeof(SrvAdvMsgHeaderExt),
  (intptr_t)asn1_type_SrvAdvMsgHeaderExt,
  0,
};

static const ASN1CType asn1_type__local_15[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  (intptr_t)asn1_type_RefExt,
};

static const RefExt asn1_value__local_19 = 17;

static const RefExt asn1_value__local_20 = 5;

static const RefExt asn1_value__local_21 = 6;

static const RefExt asn1_value__local_22 = 7;

static const EXT_TYPE asn1_value__local_18[] = {
{
  (intptr_t)&asn1_value__local_19,
  (intptr_t)asn1_type_RepeatRate,
},
{
  (intptr_t)&asn1_value__local_20,
  (intptr_t)asn1_type_TwoDLocation,
},
{
  (intptr_t)&asn1_value__local_21,
  (intptr_t)asn1_type_ThreeDLocation,
},
{
  (intptr_t)&asn1_value__local_22,
  (intptr_t)asn1_type_AdvertiserIdentifier,
},
};

static const ASN1CType asn1_constraint__local_17[] = {
  ASN1_CCONSTRAINT_TABLE,
  (intptr_t)asn1_type_EXT_TYPE,
  1,
  (intptr_t)asn1_value__local_18,
  4,
  1,
  0,
  0x80000000,
  0,
};

static const ASN1CType asn1_type__local_16[] = {
  (ASN1_CTYPE_ANY << ASN1_CTYPE_SHIFT) | 0x200000 | 0x100001,
  (intptr_t)asn1_constraint__local_17,
};

const ASN1CType asn1_type_SrvAdvMsgHeaderExt[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  2,
  sizeof(SrvAdvMsgHeaderExt),

  offsetof(SrvAdvMsgHeaderExt, extensionId) | 0x0,
  (intptr_t)asn1_type__local_15,
  0,
  (intptr_t)"extensionId",

  offsetof(SrvAdvMsgHeaderExt, value) | 0x0,
  (intptr_t)asn1_type__local_16,
  0,
  (intptr_t)"value",

};

const ASN1CType asn1_type_ServiceInfos[] = {
  (ASN1_CTYPE_SEQUENCE_OF << ASN1_CTYPE_SHIFT) | 0x1000000 | 0x100002,
  0x0,
  sizeof(ServiceInfo),
  (intptr_t)asn1_type_ServiceInfo,
  0,
};

const ASN1CType asn1_type_ServiceInfo[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  3,
  sizeof(ServiceInfo),

  offsetof(ServiceInfo, serviceID) | 0x0,
  (intptr_t)asn1_type_VarLengthNumber,
  0,
  (intptr_t)"serviceID",

  offsetof(ServiceInfo, channelIndex) | 0x0,
  (intptr_t)asn1_type_ChannelIndex,
  0,
  (intptr_t)"channelIndex",

  offsetof(ServiceInfo, chOptions) | 0x0,
  (intptr_t)asn1_type_ChannelOptions,
  0,
  (intptr_t)"chOptions",

};

const ASN1CType asn1_type_ChannelOptions[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100002,
  3,
  sizeof(ChannelOptions),

  0 | 0x8000000,
  (intptr_t)asn1_type_MandApp,
  offsetof(ChannelOptions, mandApp_option),
  (intptr_t)"mandApp",

  0 | 0x8000000,
  (intptr_t)asn1_type_ReplyAddress,
  offsetof(ChannelOptions, serviceProviderPort_option),
  (intptr_t)"serviceProviderPort",

  offsetof(ChannelOptions, extensions) | 0x8000000,
  (intptr_t)asn1_type_ServiceInfoExts,
  offsetof(ChannelOptions, extensions_option),
  (intptr_t)"extensions",

};

const ASN1CType asn1_type_ChannelIndex[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100001,
  0x0,
  0x1f,
};

const ASN1CType asn1_type_ReplyAddress[] = {
  (ASN1_CTYPE_NULL << ASN1_CTYPE_SHIFT) | 0x0 | 0x100001,
};

const ASN1CType asn1_type_MandApp[] = {
  (ASN1_CTYPE_NULL << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
};

const ASN1CType asn1_type_ServiceInfoExts[] = {
  (ASN1_CTYPE_SEQUENCE_OF << ASN1_CTYPE_SHIFT) | 0x1000000 | 0x100002,
  0x0,
  sizeof(ServiceInfoExt),
  (intptr_t)asn1_type_ServiceInfoExt,
  0,
};

static const ASN1CType asn1_type__local_23[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  (intptr_t)asn1_type_RefExt,
};

static const RefExt asn1_value__local_27 = 8;

static const RefExt asn1_value__local_28 = 9;

static const RefExt asn1_value__local_29 = 10;

static const RefExt asn1_value__local_30 = 11;

static const RefExt asn1_value__local_31 = 19;

static const RefExt asn1_value__local_32 = 20;

static const RefExt asn1_value__local_33 = 22;

static const EXT_TYPE asn1_value__local_26[] = {
{
  (intptr_t)&asn1_value__local_27,
  (intptr_t)asn1_type_ProviderServiceContext,
},
{
  (intptr_t)&asn1_value__local_28,
  (intptr_t)asn1_type_IPv6Address,
},
{
  (intptr_t)&asn1_value__local_29,
  (intptr_t)asn1_type_ServicePort,
},
{
  (intptr_t)&asn1_value__local_30,
  (intptr_t)asn1_type_ProviderMacAddress,
},
{
  (intptr_t)&asn1_value__local_31,
  (intptr_t)asn1_type_RcpiThreshold,
},
{
  (intptr_t)&asn1_value__local_32,
  (intptr_t)asn1_type_WsaCountThreshold,
},
{
  (intptr_t)&asn1_value__local_33,
  (intptr_t)asn1_type_WsaCountThresholdInterval,
},
};

static const ASN1CType asn1_constraint__local_25[] = {
  ASN1_CCONSTRAINT_TABLE,
  (intptr_t)asn1_type_EXT_TYPE,
  1,
  (intptr_t)asn1_value__local_26,
  7,
  1,
  0,
  0x80000000,
  0,
};

static const ASN1CType asn1_type__local_24[] = {
  (ASN1_CTYPE_ANY << ASN1_CTYPE_SHIFT) | 0x200000 | 0x100001,
  (intptr_t)asn1_constraint__local_25,
};

const ASN1CType asn1_type_ServiceInfoExt[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  2,
  sizeof(ServiceInfoExt),

  offsetof(ServiceInfoExt, extensionId) | 0x0,
  (intptr_t)asn1_type__local_23,
  0,
  (intptr_t)"extensionId",

  offsetof(ServiceInfoExt, value) | 0x0,
  (intptr_t)asn1_type__local_24,
  0,
  (intptr_t)"value",

};

const ASN1CType asn1_type_ChannelInfos[] = {
  (ASN1_CTYPE_SEQUENCE_OF << ASN1_CTYPE_SHIFT) | 0x1000000 | 0x100003,
  0x0,
  sizeof(ChannelInfo),
  (intptr_t)asn1_type_ChannelInfo,
  0,
};

static const ASN1CType asn1_type__local_34[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100001,
  (intptr_t)asn1_type_ChannelNumber80211,
};

static const ASN1CType asn1_type__local_35[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100002,
  (intptr_t)asn1_type_TXpower80211,
};

const ASN1CType asn1_type_ChannelInfo[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  5,
  sizeof(ChannelInfo),

  offsetof(ChannelInfo, operatingClass) | 0x0,
  (intptr_t)asn1_type_OperatingClass80211,
  0,
  (intptr_t)"operatingClass",

  offsetof(ChannelInfo, channelNumber) | 0x0,
  (intptr_t)asn1_type__local_34,
  0,
  (intptr_t)"channelNumber",

  offsetof(ChannelInfo, powerLevel) | 0x0,
  (intptr_t)asn1_type__local_35,
  0,
  (intptr_t)"powerLevel",

  offsetof(ChannelInfo, dataRate) | 0x0,
  (intptr_t)asn1_type_WsaChInfoDataRate,
  0,
  (intptr_t)"dataRate",

  offsetof(ChannelInfo, extensions) | 0x0,
  (intptr_t)asn1_type_ChInfoOptions,
  0,
  (intptr_t)"extensions",

};

const ASN1CType asn1_type_OperatingClass80211[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x0,
  0xff,
};

static const ASN1CType asn1_type__local_36[] = {
  (ASN1_CTYPE_BIT_STRING << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x1,
  0x1,
};

static const ASN1CType asn1_type__local_37[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100001,
  0x0,
  0x7f,
};

const ASN1CType asn1_type_WsaChInfoDataRate[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100003,
  2,
  sizeof(WsaChInfoDataRate),

  offsetof(WsaChInfoDataRate, adaptable) | 0x0,
  (intptr_t)asn1_type__local_36,
  0,
  (intptr_t)"adaptable",

  offsetof(WsaChInfoDataRate, dataRate) | 0x0,
  (intptr_t)asn1_type__local_37,
  0,
  (intptr_t)"dataRate",

};

static const ASN1CType asn1_type__local_38[] = {
  (ASN1_CTYPE_NULL << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
};

static const ASN1CType asn1_type__local_39[] = {
  (ASN1_CTYPE_NULL << ASN1_CTYPE_SHIFT) | 0x0 | 0x100001,
};

static const ASN1CType asn1_type__local_40[] = {
  (ASN1_CTYPE_NULL << ASN1_CTYPE_SHIFT) | 0x0 | 0x100002,
};

static const ASN1CType asn1_type__local_41[] = {
  (ASN1_CTYPE_NULL << ASN1_CTYPE_SHIFT) | 0x0 | 0x100003,
};

static const ASN1CType asn1_type__local_42[] = {
  (ASN1_CTYPE_NULL << ASN1_CTYPE_SHIFT) | 0x0 | 0x100004,
};

static const ASN1CType asn1_type__local_43[] = {
  (ASN1_CTYPE_NULL << ASN1_CTYPE_SHIFT) | 0x0 | 0x100005,
};

static const ASN1CType asn1_type__local_44[] = {
  (ASN1_CTYPE_NULL << ASN1_CTYPE_SHIFT) | 0x0 | 0x100006,
};

const ASN1CType asn1_type_ChInfoOptions[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100004,
  8,
  sizeof(ChInfoOptions),

  0 | 0x8000000,
  (intptr_t)asn1_type__local_38,
  offsetof(ChInfoOptions, option1_option),
  (intptr_t)"option1",

  0 | 0x8000000,
  (intptr_t)asn1_type__local_39,
  offsetof(ChInfoOptions, option2_option),
  (intptr_t)"option2",

  0 | 0x8000000,
  (intptr_t)asn1_type__local_40,
  offsetof(ChInfoOptions, option3_option),
  (intptr_t)"option3",

  0 | 0x8000000,
  (intptr_t)asn1_type__local_41,
  offsetof(ChInfoOptions, option4_option),
  (intptr_t)"option4",

  0 | 0x8000000,
  (intptr_t)asn1_type__local_42,
  offsetof(ChInfoOptions, option5_option),
  (intptr_t)"option5",

  0 | 0x8000000,
  (intptr_t)asn1_type__local_43,
  offsetof(ChInfoOptions, option6_option),
  (intptr_t)"option6",

  0 | 0x8000000,
  (intptr_t)asn1_type__local_44,
  offsetof(ChInfoOptions, option7_option),
  (intptr_t)"option7",

  offsetof(ChInfoOptions, extensions) | 0x8000000,
  (intptr_t)asn1_type_ChannelInfoExts,
  offsetof(ChInfoOptions, extensions_option),
  (intptr_t)"extensions",

};

const ASN1CType asn1_type_ChannelInfoExts[] = {
  (ASN1_CTYPE_SEQUENCE_OF << ASN1_CTYPE_SHIFT) | 0x1000000 | 0x100007,
  0x0,
  sizeof(ChannelInfoExt),
  (intptr_t)asn1_type_ChannelInfoExt,
  0,
};

static const ASN1CType asn1_type__local_45[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  (intptr_t)asn1_type_RefExt,
};

static const RefExt asn1_value__local_49 = 12;

static const RefExt asn1_value__local_50 = 21;

static const EXT_TYPE asn1_value__local_48[] = {
{
  (intptr_t)&asn1_value__local_49,
  (intptr_t)asn1_type_EdcaParameterSet,
},
{
  (intptr_t)&asn1_value__local_50,
  (intptr_t)asn1_type_ChannelAccess80211,
},
};

static const ASN1CType asn1_constraint__local_47[] = {
  ASN1_CCONSTRAINT_TABLE,
  (intptr_t)asn1_type_EXT_TYPE,
  1,
  (intptr_t)asn1_value__local_48,
  2,
  1,
  0,
  0x80000000,
  0,
};

static const ASN1CType asn1_type__local_46[] = {
  (ASN1_CTYPE_ANY << ASN1_CTYPE_SHIFT) | 0x200000 | 0x100001,
  (intptr_t)asn1_constraint__local_47,
};

const ASN1CType asn1_type_ChannelInfoExt[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  2,
  sizeof(ChannelInfoExt),

  offsetof(ChannelInfoExt, extensionId) | 0x0,
  (intptr_t)asn1_type__local_45,
  0,
  (intptr_t)"extensionId",

  offsetof(ChannelInfoExt, value) | 0x0,
  (intptr_t)asn1_type__local_46,
  0,
  (intptr_t)"value",

};

static const ASN1CType asn1_type__local_51[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100003,
  (intptr_t)asn1_type_IPv6Address,
};

static const ASN1CType asn1_type__local_52[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100004,
  (intptr_t)asn1_type_IPv6Address,
};

const ASN1CType asn1_type_RoutingAdvertisement[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100004,
  6,
  sizeof(RoutingAdvertisement),

  offsetof(RoutingAdvertisement, lifetime) | 0x0,
  (intptr_t)asn1_type_RouterLifetime,
  0,
  (intptr_t)"lifetime",

  offsetof(RoutingAdvertisement, ipPrefix) | 0x0,
  (intptr_t)asn1_type_IpV6Prefix,
  0,
  (intptr_t)"ipPrefix",

  offsetof(RoutingAdvertisement, ipPrefixLength) | 0x0,
  (intptr_t)asn1_type_IpV6PrefixLength,
  0,
  (intptr_t)"ipPrefixLength",

  offsetof(RoutingAdvertisement, defaultGateway) | 0x0,
  (intptr_t)asn1_type__local_51,
  0,
  (intptr_t)"defaultGateway",

  offsetof(RoutingAdvertisement, primaryDns) | 0x0,
  (intptr_t)asn1_type__local_52,
  0,
  (intptr_t)"primaryDns",

  offsetof(RoutingAdvertisement, extensions) | 0x0,
  (intptr_t)asn1_type_RoutAdvertExts,
  0,
  (intptr_t)"extensions",

};

const ASN1CType asn1_type_RouterLifetime[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x0,
  0xffff,
};

const ASN1CType asn1_type_IpV6Prefix[] = {
  (ASN1_CTYPE_OCTET_STRING << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100001,
  0x10,
  0x10,
};

const ASN1CType asn1_type_IpV6PrefixLength[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100002,
  0x0,
  0xff,
};

const ASN1CType asn1_type_RoutAdvertExts[] = {
  (ASN1_CTYPE_SEQUENCE_OF << ASN1_CTYPE_SHIFT) | 0x1000000 | 0x100005,
  0x0,
  sizeof(RoutAdvertExt),
  (intptr_t)asn1_type_RoutAdvertExt,
  0,
};

static const ASN1CType asn1_type__local_53[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  (intptr_t)asn1_type_RefExt,
};

static const RefExt asn1_value__local_57 = 13;

static const RefExt asn1_value__local_58 = 14;

static const EXT_TYPE asn1_value__local_56[] = {
{
  (intptr_t)&asn1_value__local_57,
  (intptr_t)asn1_type_SecondaryDns,
},
{
  (intptr_t)&asn1_value__local_58,
  (intptr_t)asn1_type_GatewayMacAddress,
},
};

static const ASN1CType asn1_constraint__local_55[] = {
  ASN1_CCONSTRAINT_TABLE,
  (intptr_t)asn1_type_EXT_TYPE,
  1,
  (intptr_t)asn1_value__local_56,
  2,
  1,
  0,
  0x80000000,
  0,
};

static const ASN1CType asn1_type__local_54[] = {
  (ASN1_CTYPE_ANY << ASN1_CTYPE_SHIFT) | 0x200000 | 0x100001,
  (intptr_t)asn1_constraint__local_55,
};

const ASN1CType asn1_type_RoutAdvertExt[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  2,
  sizeof(RoutAdvertExt),

  offsetof(RoutAdvertExt, extensionId) | 0x0,
  (intptr_t)asn1_type__local_53,
  0,
  (intptr_t)"extensionId",

  offsetof(RoutAdvertExt, value) | 0x0,
  (intptr_t)asn1_type__local_54,
  0,
  (intptr_t)"value",

};

const ASN1CType asn1_type_ShortMsgNpdu[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  3,
  sizeof(ShortMsgNpdu),

  offsetof(ShortMsgNpdu, subtype) | 0x0,
  (intptr_t)asn1_type_ShortMsgSubtype,
  0,
  (intptr_t)"subtype",

  offsetof(ShortMsgNpdu, transport) | 0x0,
  (intptr_t)asn1_type_ShortMsgTpdus,
  0,
  (intptr_t)"transport",

  offsetof(ShortMsgNpdu, body) | 0x0,
  (intptr_t)asn1_type_ShortMsgData,
  0,
  (intptr_t)"body",

};

static const ASN1CType asn1_type__local_59[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100001,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_60[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100002,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_61[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100003,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_62[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100004,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_63[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100005,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_64[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100006,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_65[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100007,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_66[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100008,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_67[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100009,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_68[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10000a,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_69[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10000b,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_70[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10000c,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_71[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10000d,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_72[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10000e,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

static const ASN1CType asn1_type__local_73[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10000f,
  (intptr_t)asn1_type_NoSubtypeProcessing,
};

const ASN1CType asn1_type_ShortMsgSubtype[] = {
  (ASN1_CTYPE_CHOICE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  16,
  sizeof(ShortMsgSubtype),
  offsetof(ShortMsgSubtype, choice),
  offsetof(ShortMsgSubtype, u),
  (intptr_t)asn1_type_NullNetworking,
  (intptr_t)"nullNetworking",
  (intptr_t)asn1_type__local_59,
  (intptr_t)"subTypeReserved1",
  (intptr_t)asn1_type__local_60,
  (intptr_t)"subTypeReserved2",
  (intptr_t)asn1_type__local_61,
  (intptr_t)"subTypeReserved3",
  (intptr_t)asn1_type__local_62,
  (intptr_t)"subTypeReserved4",
  (intptr_t)asn1_type__local_63,
  (intptr_t)"subTypeReserved5",
  (intptr_t)asn1_type__local_64,
  (intptr_t)"subTypeReserved6",
  (intptr_t)asn1_type__local_65,
  (intptr_t)"subTypeReserved7",
  (intptr_t)asn1_type__local_66,
  (intptr_t)"subTypeReserved8",
  (intptr_t)asn1_type__local_67,
  (intptr_t)"subTypeReserved9",
  (intptr_t)asn1_type__local_68,
  (intptr_t)"subTypeReserved19",
  (intptr_t)asn1_type__local_69,
  (intptr_t)"subTypeReserved11",
  (intptr_t)asn1_type__local_70,
  (intptr_t)"subTypeReserved12",
  (intptr_t)asn1_type__local_71,
  (intptr_t)"subTypeReserved13",
  (intptr_t)asn1_type__local_72,
  (intptr_t)"subTypeReserved14",
  (intptr_t)asn1_type__local_73,
  (intptr_t)"subTypeReserved15",
};

static const ASN1CType asn1_type__local_74[] = {
  (ASN1_CTYPE_BIT_STRING << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x1,
  0x1,
};

static const ASN1CType asn1_type__local_75[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100001,
  (intptr_t)asn1_type_ShortMsgVersion,
};

const ASN1CType asn1_type_NoSubtypeProcessing[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  2,
  sizeof(NoSubtypeProcessing),

  offsetof(NoSubtypeProcessing, optBit) | 0x0,
  (intptr_t)asn1_type__local_74,
  0,
  (intptr_t)"optBit",

  offsetof(NoSubtypeProcessing, version) | 0x0,
  (intptr_t)asn1_type__local_75,
  0,
  (intptr_t)"version",

};

const ASN1CType asn1_type_ShortMsgVersion[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x2,
  0x0,
  0x7,
};

static const ASN1CType asn1_type__local_76[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  (intptr_t)asn1_type_ShortMsgVersion,
};

const ASN1CType asn1_type_NullNetworking[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  2,
  sizeof(NullNetworking),

  offsetof(NullNetworking, version) | 0x0,
  (intptr_t)asn1_type__local_76,
  0,
  (intptr_t)"version",

  offsetof(NullNetworking, nExtensions) | 0x8000000,
  (intptr_t)asn1_type_ShortMsgNextensions,
  offsetof(NullNetworking, nExtensions_option),
  (intptr_t)"nExtensions",

};

const ASN1CType asn1_type_ShortMsgNextensions[] = {
  (ASN1_CTYPE_SEQUENCE_OF << ASN1_CTYPE_SHIFT) | 0x1000000 | 0x100001,
  0x0,
  sizeof(ShortMsgNextension),
  (intptr_t)asn1_type_ShortMsgNextension,
  0,
};

static const ASN1CType asn1_type__local_77[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  (intptr_t)asn1_type_RefExt,
};

static const RefExt asn1_value__local_81 = 4;

static const RefExt asn1_value__local_82 = 15;

static const RefExt asn1_value__local_83 = 16;

static const EXT_TYPE asn1_value__local_80[] = {
{
  (intptr_t)&asn1_value__local_81,
  (intptr_t)asn1_type_TXpower80211,
},
{
  (intptr_t)&asn1_value__local_82,
  (intptr_t)asn1_type_ChannelNumber80211,
},
{
  (intptr_t)&asn1_value__local_83,
  (intptr_t)asn1_type_DataRate80211,
},
};

static const ASN1CType asn1_constraint__local_79[] = {
  ASN1_CCONSTRAINT_TABLE,
  (intptr_t)asn1_type_EXT_TYPE,
  1,
  (intptr_t)asn1_value__local_80,
  3,
  1,
  0,
  0x80000000,
  0,
};

static const ASN1CType asn1_type__local_78[] = {
  (ASN1_CTYPE_ANY << ASN1_CTYPE_SHIFT) | 0x200000 | 0x100001,
  (intptr_t)asn1_constraint__local_79,
};

const ASN1CType asn1_type_ShortMsgNextension[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  2,
  sizeof(ShortMsgNextension),

  offsetof(ShortMsgNextension, extensionId) | 0x0,
  (intptr_t)asn1_type__local_77,
  0,
  (intptr_t)"extensionId",

  offsetof(ShortMsgNextension, value) | 0x0,
  (intptr_t)asn1_type__local_78,
  0,
  (intptr_t)"value",

};

static const ASN1CType asn1_type__local_84[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100001,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_85[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100002,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_86[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100003,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_87[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100004,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_88[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100005,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_89[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100006,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_90[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100007,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_91[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100008,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_92[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100009,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_93[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10000a,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_94[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10000b,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_95[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10000c,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_96[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10000d,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_97[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10000e,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_98[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10000f,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_99[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100010,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_100[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100011,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_101[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100012,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_102[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100013,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_103[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100014,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_104[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100015,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_105[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100016,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_106[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100017,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_107[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100018,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_108[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100019,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_109[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10001a,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_110[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10001b,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_111[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10001c,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_112[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10001d,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_113[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10001e,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_114[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10001f,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_115[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100020,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_116[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100021,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_117[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100022,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_118[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100023,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_119[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100024,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_120[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100025,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_121[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100026,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_122[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100027,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_123[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100028,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_124[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100029,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_125[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10002a,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_126[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10002b,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_127[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10002c,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_128[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10002d,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_129[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10002e,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_130[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10002f,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_131[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100030,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_132[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100031,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_133[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100032,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_134[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100033,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_135[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100034,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_136[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100035,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_137[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100036,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_138[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100037,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_139[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100038,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_140[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100039,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_141[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10003a,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_142[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10003b,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_143[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10003c,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_144[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10003d,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_145[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10003e,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_146[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10003f,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_147[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100040,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_148[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100041,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_149[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100042,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_150[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100043,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_151[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100044,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_152[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100045,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_153[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100046,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_154[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100047,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_155[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100048,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_156[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100049,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_157[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10004a,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_158[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10004b,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_159[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10004c,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_160[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10004d,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_161[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10004e,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_162[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10004f,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_163[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100050,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_164[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100051,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_165[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100052,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_166[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100053,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_167[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100054,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_168[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100055,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_169[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100056,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_170[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100057,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_171[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100058,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_172[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100059,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_173[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10005a,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_174[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10005b,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_175[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10005c,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_176[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10005d,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_177[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10005e,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_178[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10005f,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_179[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100060,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_180[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100061,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_181[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100062,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_182[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100063,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_183[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100064,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_184[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100065,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_185[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100066,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_186[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100067,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_187[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100068,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_188[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100069,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_189[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10006a,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_190[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10006b,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_191[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10006c,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_192[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10006d,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_193[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10006e,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_194[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10006f,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_195[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100070,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_196[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100071,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_197[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100072,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_198[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100073,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_199[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100074,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_200[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100075,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_201[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100076,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_202[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100077,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_203[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100078,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_204[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100079,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_205[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10007a,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_206[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10007b,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_207[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10007c,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_208[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10007d,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_209[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10007e,
  (intptr_t)asn1_type_NoTpidProcessing,
};

static const ASN1CType asn1_type__local_210[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x10007f,
  (intptr_t)asn1_type_NoTpidProcessing,
};

const ASN1CType asn1_type_ShortMsgTpdus[] = {
  (ASN1_CTYPE_CHOICE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100001,
  128,
  sizeof(ShortMsgTpdus),
  offsetof(ShortMsgTpdus, choice),
  offsetof(ShortMsgTpdus, u),
  (intptr_t)asn1_type_ShortMsgBcPDU,
  (intptr_t)"bcMode",
  (intptr_t)asn1_type__local_84,
  (intptr_t)"tpidReserved1",
  (intptr_t)asn1_type__local_85,
  (intptr_t)"tpidReserved2",
  (intptr_t)asn1_type__local_86,
  (intptr_t)"tpidReserved3",
  (intptr_t)asn1_type__local_87,
  (intptr_t)"tpidReserved4",
  (intptr_t)asn1_type__local_88,
  (intptr_t)"tpidReserved5",
  (intptr_t)asn1_type__local_89,
  (intptr_t)"tpidReserved6",
  (intptr_t)asn1_type__local_90,
  (intptr_t)"tpidReserved7",
  (intptr_t)asn1_type__local_91,
  (intptr_t)"tpidReserved8",
  (intptr_t)asn1_type__local_92,
  (intptr_t)"tpidReserved9",
  (intptr_t)asn1_type__local_93,
  (intptr_t)"tpidReserved10",
  (intptr_t)asn1_type__local_94,
  (intptr_t)"tpidReserved11",
  (intptr_t)asn1_type__local_95,
  (intptr_t)"tpidReserved12",
  (intptr_t)asn1_type__local_96,
  (intptr_t)"tpidReserved13",
  (intptr_t)asn1_type__local_97,
  (intptr_t)"tpidReserved14",
  (intptr_t)asn1_type__local_98,
  (intptr_t)"tpidReserved15",
  (intptr_t)asn1_type__local_99,
  (intptr_t)"tpidReserved16",
  (intptr_t)asn1_type__local_100,
  (intptr_t)"tpidReserved17",
  (intptr_t)asn1_type__local_101,
  (intptr_t)"tpidReserved18",
  (intptr_t)asn1_type__local_102,
  (intptr_t)"tpidReserved19",
  (intptr_t)asn1_type__local_103,
  (intptr_t)"tpidReserved20",
  (intptr_t)asn1_type__local_104,
  (intptr_t)"tpidReserved21",
  (intptr_t)asn1_type__local_105,
  (intptr_t)"tpidReserved22",
  (intptr_t)asn1_type__local_106,
  (intptr_t)"tpidReserved23",
  (intptr_t)asn1_type__local_107,
  (intptr_t)"tpidReserved24",
  (intptr_t)asn1_type__local_108,
  (intptr_t)"tpidReserved25",
  (intptr_t)asn1_type__local_109,
  (intptr_t)"tpidReserved26",
  (intptr_t)asn1_type__local_110,
  (intptr_t)"tpidReserved27",
  (intptr_t)asn1_type__local_111,
  (intptr_t)"tpidReserved28",
  (intptr_t)asn1_type__local_112,
  (intptr_t)"tpidReserved29",
  (intptr_t)asn1_type__local_113,
  (intptr_t)"tpidReserved30",
  (intptr_t)asn1_type__local_114,
  (intptr_t)"tpidReserved31",
  (intptr_t)asn1_type__local_115,
  (intptr_t)"tpidReserved32",
  (intptr_t)asn1_type__local_116,
  (intptr_t)"tpidReserved33",
  (intptr_t)asn1_type__local_117,
  (intptr_t)"tpidReserved34",
  (intptr_t)asn1_type__local_118,
  (intptr_t)"tpidReserved35",
  (intptr_t)asn1_type__local_119,
  (intptr_t)"tpidReserved36",
  (intptr_t)asn1_type__local_120,
  (intptr_t)"tpidReserved37",
  (intptr_t)asn1_type__local_121,
  (intptr_t)"tpidReserved38",
  (intptr_t)asn1_type__local_122,
  (intptr_t)"tpidReserved39",
  (intptr_t)asn1_type__local_123,
  (intptr_t)"tpidReserved40",
  (intptr_t)asn1_type__local_124,
  (intptr_t)"tpidReserved41",
  (intptr_t)asn1_type__local_125,
  (intptr_t)"tpidReserved42",
  (intptr_t)asn1_type__local_126,
  (intptr_t)"tpidReserved43",
  (intptr_t)asn1_type__local_127,
  (intptr_t)"tpidReserved44",
  (intptr_t)asn1_type__local_128,
  (intptr_t)"tpidReserved45",
  (intptr_t)asn1_type__local_129,
  (intptr_t)"tpidReserved46",
  (intptr_t)asn1_type__local_130,
  (intptr_t)"tpidReserved47",
  (intptr_t)asn1_type__local_131,
  (intptr_t)"tpidReserved48",
  (intptr_t)asn1_type__local_132,
  (intptr_t)"tpidReserved49",
  (intptr_t)asn1_type__local_133,
  (intptr_t)"tpidReserved50",
  (intptr_t)asn1_type__local_134,
  (intptr_t)"tpidReserved51",
  (intptr_t)asn1_type__local_135,
  (intptr_t)"tpidReserved52",
  (intptr_t)asn1_type__local_136,
  (intptr_t)"tpidReserved53",
  (intptr_t)asn1_type__local_137,
  (intptr_t)"tpidReserved54",
  (intptr_t)asn1_type__local_138,
  (intptr_t)"tpidReserved55",
  (intptr_t)asn1_type__local_139,
  (intptr_t)"tpidReserved56",
  (intptr_t)asn1_type__local_140,
  (intptr_t)"tpidReserved57",
  (intptr_t)asn1_type__local_141,
  (intptr_t)"tpidReserved58",
  (intptr_t)asn1_type__local_142,
  (intptr_t)"tpidReserved59",
  (intptr_t)asn1_type__local_143,
  (intptr_t)"tpidReserved60",
  (intptr_t)asn1_type__local_144,
  (intptr_t)"tpidReserved61",
  (intptr_t)asn1_type__local_145,
  (intptr_t)"tpidReserved62",
  (intptr_t)asn1_type__local_146,
  (intptr_t)"tpidReserved63",
  (intptr_t)asn1_type__local_147,
  (intptr_t)"tpidReserved64",
  (intptr_t)asn1_type__local_148,
  (intptr_t)"tpidReserved65",
  (intptr_t)asn1_type__local_149,
  (intptr_t)"tpidReserved66",
  (intptr_t)asn1_type__local_150,
  (intptr_t)"tpidReserved67",
  (intptr_t)asn1_type__local_151,
  (intptr_t)"tpidReserved68",
  (intptr_t)asn1_type__local_152,
  (intptr_t)"tpidReserved69",
  (intptr_t)asn1_type__local_153,
  (intptr_t)"tpidReserved70",
  (intptr_t)asn1_type__local_154,
  (intptr_t)"tpidReserved71",
  (intptr_t)asn1_type__local_155,
  (intptr_t)"tpidReserved72",
  (intptr_t)asn1_type__local_156,
  (intptr_t)"tpidReserved73",
  (intptr_t)asn1_type__local_157,
  (intptr_t)"tpidReserved74",
  (intptr_t)asn1_type__local_158,
  (intptr_t)"tpidReserved75",
  (intptr_t)asn1_type__local_159,
  (intptr_t)"tpidReserved76",
  (intptr_t)asn1_type__local_160,
  (intptr_t)"tpidReserved77",
  (intptr_t)asn1_type__local_161,
  (intptr_t)"tpidReserved78",
  (intptr_t)asn1_type__local_162,
  (intptr_t)"tpidReserved79",
  (intptr_t)asn1_type__local_163,
  (intptr_t)"tpidReserved80",
  (intptr_t)asn1_type__local_164,
  (intptr_t)"tpidReserved81",
  (intptr_t)asn1_type__local_165,
  (intptr_t)"tpidReserved82",
  (intptr_t)asn1_type__local_166,
  (intptr_t)"tpidReserved83",
  (intptr_t)asn1_type__local_167,
  (intptr_t)"tpidReserved84",
  (intptr_t)asn1_type__local_168,
  (intptr_t)"tpidReserved85",
  (intptr_t)asn1_type__local_169,
  (intptr_t)"tpidReserved86",
  (intptr_t)asn1_type__local_170,
  (intptr_t)"tpidReserved87",
  (intptr_t)asn1_type__local_171,
  (intptr_t)"tpidReserved88",
  (intptr_t)asn1_type__local_172,
  (intptr_t)"tpidReserved89",
  (intptr_t)asn1_type__local_173,
  (intptr_t)"tpidReserved90",
  (intptr_t)asn1_type__local_174,
  (intptr_t)"tpidReserved91",
  (intptr_t)asn1_type__local_175,
  (intptr_t)"tpidReserved92",
  (intptr_t)asn1_type__local_176,
  (intptr_t)"tpidReserved93",
  (intptr_t)asn1_type__local_177,
  (intptr_t)"tpidReserved94",
  (intptr_t)asn1_type__local_178,
  (intptr_t)"tpidReserved95",
  (intptr_t)asn1_type__local_179,
  (intptr_t)"tpidReserved96",
  (intptr_t)asn1_type__local_180,
  (intptr_t)"tpidReserved97",
  (intptr_t)asn1_type__local_181,
  (intptr_t)"tpidReserved98",
  (intptr_t)asn1_type__local_182,
  (intptr_t)"tpidReserved99",
  (intptr_t)asn1_type__local_183,
  (intptr_t)"tpidReserved100",
  (intptr_t)asn1_type__local_184,
  (intptr_t)"tpidReserved101",
  (intptr_t)asn1_type__local_185,
  (intptr_t)"tpidReserved102",
  (intptr_t)asn1_type__local_186,
  (intptr_t)"tpidReserved103",
  (intptr_t)asn1_type__local_187,
  (intptr_t)"tpidReserved104",
  (intptr_t)asn1_type__local_188,
  (intptr_t)"tpidReserved105",
  (intptr_t)asn1_type__local_189,
  (intptr_t)"tpidReserved106",
  (intptr_t)asn1_type__local_190,
  (intptr_t)"tpidReserved107",
  (intptr_t)asn1_type__local_191,
  (intptr_t)"tpidReserved108",
  (intptr_t)asn1_type__local_192,
  (intptr_t)"tpidReserved109",
  (intptr_t)asn1_type__local_193,
  (intptr_t)"tpidReserved110",
  (intptr_t)asn1_type__local_194,
  (intptr_t)"tpidReserved111",
  (intptr_t)asn1_type__local_195,
  (intptr_t)"tpidReserved112",
  (intptr_t)asn1_type__local_196,
  (intptr_t)"tpidReserved113",
  (intptr_t)asn1_type__local_197,
  (intptr_t)"tpidReserved114",
  (intptr_t)asn1_type__local_198,
  (intptr_t)"tpidReserved115",
  (intptr_t)asn1_type__local_199,
  (intptr_t)"tpidReserved116",
  (intptr_t)asn1_type__local_200,
  (intptr_t)"tpidReserved117",
  (intptr_t)asn1_type__local_201,
  (intptr_t)"tpidReserved118",
  (intptr_t)asn1_type__local_202,
  (intptr_t)"tpidReserved119",
  (intptr_t)asn1_type__local_203,
  (intptr_t)"tpidReserved120",
  (intptr_t)asn1_type__local_204,
  (intptr_t)"tpidReserved121",
  (intptr_t)asn1_type__local_205,
  (intptr_t)"tpidReserved122",
  (intptr_t)asn1_type__local_206,
  (intptr_t)"tpidReserved123",
  (intptr_t)asn1_type__local_207,
  (intptr_t)"tpidReserved124",
  (intptr_t)asn1_type__local_208,
  (intptr_t)"tpidReserved125",
  (intptr_t)asn1_type__local_209,
  (intptr_t)"tpidReserved126",
  (intptr_t)asn1_type__local_210,
  (intptr_t)"tpidReserved127",
};

const ASN1CType asn1_type_NoTpidProcessing[] = {
  (ASN1_CTYPE_BIT_STRING << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x3,
  0x1,
  0x1,
};

const ASN1CType asn1_type_ShortMsgBcPDU[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  2,
  sizeof(ShortMsgBcPDU),

  offsetof(ShortMsgBcPDU, destAddress) | 0x0,
  (intptr_t)asn1_type_VarLengthNumber,
  0,
  (intptr_t)"destAddress",

  offsetof(ShortMsgBcPDU, tExtensions) | 0x8000000,
  (intptr_t)asn1_type_ShortMsgTextensions,
  offsetof(ShortMsgBcPDU, tExtensions_option),
  (intptr_t)"tExtensions",

};

const ASN1CType asn1_type_ShortMsgData[] = {
  (ASN1_CTYPE_OCTET_STRING << ASN1_CTYPE_SHIFT) | 0x1000000 | 0x100002,
  0x0,
};

const ASN1CType asn1_type_ShortMsgTextensions[] = {
  (ASN1_CTYPE_SEQUENCE_OF << ASN1_CTYPE_SHIFT) | 0x1000000 | 0x100001,
  0x0,
  sizeof(ShortMsgTextension),
  (intptr_t)asn1_type_ShortMsgTextension,
  0,
};

static const ASN1CType asn1_type__local_211[] = {
  (ASN1_CTYPE_TAGGED << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  (intptr_t)asn1_type_RefExt,
};

static const RefExt asn1_value__local_215 = 0;

static const EXT_TYPE asn1_value__local_214[] = {
{
  (intptr_t)&asn1_value__local_215,
  (intptr_t)asn1_type_DummyExtension,
},
};

static const ASN1CType asn1_constraint__local_213[] = {
  ASN1_CCONSTRAINT_TABLE,
  (intptr_t)asn1_type_EXT_TYPE,
  1,
  (intptr_t)asn1_value__local_214,
  1,
  1,
  0,
  0x80000000,
  0,
};

static const ASN1CType asn1_type__local_212[] = {
  (ASN1_CTYPE_ANY << ASN1_CTYPE_SHIFT) | 0x200000 | 0x100001,
  (intptr_t)asn1_constraint__local_213,
};

const ASN1CType asn1_type_ShortMsgTextension[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x0 | 0x10,
  2,
  sizeof(ShortMsgTextension),

  offsetof(ShortMsgTextension, extensionId) | 0x0,
  (intptr_t)asn1_type__local_211,
  0,
  (intptr_t)"extensionId",

  offsetof(ShortMsgTextension, value) | 0x0,
  (intptr_t)asn1_type__local_212,
  0,
  (intptr_t)"value",

};

const ASN1CType asn1_type_DummyExtension[] = {
  (ASN1_CTYPE_NULL << ASN1_CTYPE_SHIFT) | 0x0 | 0x5,
};

static const ASN1CType asn1_type__local_216[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x0,
  0x7f,
};

const ASN1CType asn1_type_VarLengthNumber[] = {
  (ASN1_CTYPE_CHOICE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100000,
  2,
  sizeof(VarLengthNumber),
  offsetof(VarLengthNumber, choice),
  offsetof(VarLengthNumber, u),
  (intptr_t)asn1_type__local_216,
  (intptr_t)"content",
  (intptr_t)asn1_type_Ext1,
  (intptr_t)"extension",
};

static const ASN1CType asn1_type__local_217[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x80,
  0x407f,
};

const ASN1CType asn1_type_Ext1[] = {
  (ASN1_CTYPE_CHOICE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100001,
  2,
  sizeof(Ext1),
  offsetof(Ext1, choice),
  offsetof(Ext1, u),
  (intptr_t)asn1_type__local_217,
  (intptr_t)"content",
  (intptr_t)asn1_type_Ext2,
  (intptr_t)"extension",
};

static const ASN1CType asn1_type__local_218[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x4080,
  0x20407f,
};

const ASN1CType asn1_type_Ext2[] = {
  (ASN1_CTYPE_CHOICE << ASN1_CTYPE_SHIFT) | 0x0 | 0x100001,
  2,
  sizeof(Ext2),
  offsetof(Ext2, choice),
  offsetof(Ext2, u),
  (intptr_t)asn1_type__local_218,
  (intptr_t)"content",
  (intptr_t)asn1_type_Ext3,
  (intptr_t)"extension",
};

const ASN1CType asn1_type_Ext3[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x3800000 | 0x100001,
  0x204080,
  0x1020407f,
};

static const ASN1CType asn1_type__local_219[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x0,
  0x7f,
};

static const ASN1CType asn1_type__local_220[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100001,
  0x0,
  0x7fff,
};

const ASN1CType asn1_type_VarLengthNumber2[] = {
  (ASN1_CTYPE_CHOICE << ASN1_CTYPE_SHIFT) | 0x0 | 0x0,
  2,
  sizeof(VarLengthNumber2),
  offsetof(VarLengthNumber2, choice),
  offsetof(VarLengthNumber2, u),
  (intptr_t)asn1_type__local_219,
  (intptr_t)"shortNo",
  (intptr_t)asn1_type__local_220,
  (intptr_t)"longNo",
};

