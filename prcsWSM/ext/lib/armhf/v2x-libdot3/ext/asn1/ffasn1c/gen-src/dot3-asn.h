/* Automatically generated file - do not edit */
#ifndef _FFASN1_OUT_H
#define _FFASN1_OUT_H

#include "asn1defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef int RefExt;

extern const ASN1CType asn1_type_RefExt[];

typedef struct EXT_TYPE { /* object class definition */
  ASN1CType extRef;
  ASN1CType ExtValue;
} EXT_TYPE;


extern const ASN1CType asn1_type_EXT_TYPE[];

typedef int DataRate80211;

extern const ASN1CType asn1_type_DataRate80211[];

typedef int TXpower80211;

extern const ASN1CType asn1_type_TXpower80211[];

typedef int ChannelNumber80211;

extern const ASN1CType asn1_type_ChannelNumber80211[];

typedef int RepeatRate;

extern const ASN1CType asn1_type_RepeatRate[];

typedef struct Latitude {
  ASN1BitString fill;
  int lat;
} Latitude;


extern const ASN1CType asn1_type_Latitude[];

typedef int Longitude;

extern const ASN1CType asn1_type_Longitude[];

typedef struct TwoDLocation {
  Latitude latitude;
  Longitude longitude;
} TwoDLocation;


extern const ASN1CType asn1_type_TwoDLocation[];

typedef int Elevation;

extern const ASN1CType asn1_type_Elevation[];

typedef struct ThreeDLocation {
  Latitude latitude;
  Longitude longitude;
  Elevation elevation;
} ThreeDLocation;


extern const ASN1CType asn1_type_ThreeDLocation[];

typedef ASN1String AdvertiserIdentifier;

extern const ASN1CType asn1_type_AdvertiserIdentifier[];

typedef struct ProviderServiceContext {
  ASN1BitString fillBit;
  ASN1String psc;
} ProviderServiceContext;


extern const ASN1CType asn1_type_ProviderServiceContext[];

typedef ASN1String IPv6Address;

extern const ASN1CType asn1_type_IPv6Address[];

typedef int ServicePort;

extern const ASN1CType asn1_type_ServicePort[];

typedef ASN1String MACaddress;

extern const ASN1CType asn1_type_MACaddress[];

typedef MACaddress ProviderMacAddress;

#define asn1_type_ProviderMacAddress asn1_type_MACaddress

typedef int RcpiThreshold;

extern const ASN1CType asn1_type_RcpiThreshold[];

typedef int WsaCountThreshold;

extern const ASN1CType asn1_type_WsaCountThreshold[];

typedef int WsaCountThresholdInterval;

extern const ASN1CType asn1_type_WsaCountThresholdInterval[];

typedef struct EdcaParameterRecord {
  int res;
  int aci;
  int acm;
  int aifsn;
  int ecwMax;
  int ecwMin;
  int txopLimit;
} EdcaParameterRecord;


extern const ASN1CType asn1_type_EdcaParameterRecord[];

typedef struct EdcaParameterSet {
  EdcaParameterRecord acbeRecord;
  EdcaParameterRecord acbkRecord;
  EdcaParameterRecord acviRecord;
  EdcaParameterRecord acvoRecord;
} EdcaParameterSet;


extern const ASN1CType asn1_type_EdcaParameterSet[];

typedef int ChannelAccess80211;

enum {
  ChannelAccess80211_continuous = 0,
  ChannelAccess80211_alternatingSCH = 1,
  ChannelAccess80211_alternatingCCH = 2,
};

extern const ASN1CType asn1_type_ChannelAccess80211[];

typedef IPv6Address SecondaryDns;

#define asn1_type_SecondaryDns asn1_type_IPv6Address

typedef MACaddress GatewayMacAddress;

#define asn1_type_GatewayMacAddress asn1_type_MACaddress

typedef int SrvAdvMessageType;

enum {
  SrvAdvMessageType_saMessage = 0,
  SrvAdvMessageType_sarMessage = 1,
};

extern const ASN1CType asn1_type_SrvAdvMessageType[];

typedef int RsvAdvPrtVersion;

extern const ASN1CType asn1_type_RsvAdvPrtVersion[];

typedef struct SrvAdvPrtVersion {
  SrvAdvMessageType messageID;
  RsvAdvPrtVersion rsvAdvPrtVersion;
} SrvAdvPrtVersion;


extern const ASN1CType asn1_type_SrvAdvPrtVersion[];

typedef int SrvAdvID;

extern const ASN1CType asn1_type_SrvAdvID[];

typedef int SrvAdvContentCount;

extern const ASN1CType asn1_type_SrvAdvContentCount[];

typedef struct SrvAdvChangeCount {
  SrvAdvID saID;
  SrvAdvContentCount contentCount;
} SrvAdvChangeCount;


extern const ASN1CType asn1_type_SrvAdvChangeCount[];

typedef struct SrvAdvMsgHeaderExt {
  RefExt extensionId;
  ASN1OpenType value;
} SrvAdvMsgHeaderExt;


extern const ASN1CType asn1_type_SrvAdvMsgHeaderExt[];

typedef struct SrvAdvMsgHeaderExts {
  SrvAdvMsgHeaderExt *tab;
  size_t count;
} SrvAdvMsgHeaderExts;

extern const ASN1CType asn1_type_SrvAdvMsgHeaderExts[];

typedef int Ext3;

extern const ASN1CType asn1_type_Ext3[];

typedef enum {
  Ext2_content,
  Ext2_extension,
} Ext2_choice;

typedef struct Ext2 {
  Ext2_choice choice;
  union {
    int content;
    Ext3 extension;
  } u;
} Ext2;

extern const ASN1CType asn1_type_Ext2[];

typedef enum {
  Ext1_content,
  Ext1_extension,
} Ext1_choice;

typedef struct Ext1 {
  Ext1_choice choice;
  union {
    int content;
    Ext2 extension;
  } u;
} Ext1;

extern const ASN1CType asn1_type_Ext1[];

typedef enum {
  VarLengthNumber_content,
  VarLengthNumber_extension,
} VarLengthNumber_choice;

typedef struct VarLengthNumber {
  VarLengthNumber_choice choice;
  union {
    int content;
    Ext1 extension;
  } u;
} VarLengthNumber;

extern const ASN1CType asn1_type_VarLengthNumber[];

typedef int ChannelIndex;

enum {
  ChannelIndex_notUsed = 0,
  ChannelIndex_firstEntry = 1,
};

extern const ASN1CType asn1_type_ChannelIndex[];

typedef ASN1Null MandApp;

extern const ASN1CType asn1_type_MandApp[];

typedef ASN1Null ReplyAddress;

extern const ASN1CType asn1_type_ReplyAddress[];

typedef struct ServiceInfoExt {
  RefExt extensionId;
  ASN1OpenType value;
} ServiceInfoExt;


extern const ASN1CType asn1_type_ServiceInfoExt[];

typedef struct ServiceInfoExts {
  ServiceInfoExt *tab;
  size_t count;
} ServiceInfoExts;

extern const ASN1CType asn1_type_ServiceInfoExts[];

typedef struct ChannelOptions {
  BOOL mandApp_option;
  BOOL serviceProviderPort_option;
  BOOL extensions_option;
  ServiceInfoExts extensions;
} ChannelOptions;


extern const ASN1CType asn1_type_ChannelOptions[];

typedef struct ServiceInfo {
  VarLengthNumber serviceID;
  ChannelIndex channelIndex;
  ChannelOptions chOptions;
} ServiceInfo;


extern const ASN1CType asn1_type_ServiceInfo[];

typedef struct ServiceInfos {
  ServiceInfo *tab;
  size_t count;
} ServiceInfos;

extern const ASN1CType asn1_type_ServiceInfos[];

typedef int OperatingClass80211;

extern const ASN1CType asn1_type_OperatingClass80211[];

typedef struct WsaChInfoDataRate {
  ASN1BitString adaptable;
  int dataRate;
} WsaChInfoDataRate;


extern const ASN1CType asn1_type_WsaChInfoDataRate[];

typedef struct ChannelInfoExt {
  RefExt extensionId;
  ASN1OpenType value;
} ChannelInfoExt;


extern const ASN1CType asn1_type_ChannelInfoExt[];

typedef struct ChannelInfoExts {
  ChannelInfoExt *tab;
  size_t count;
} ChannelInfoExts;

extern const ASN1CType asn1_type_ChannelInfoExts[];

typedef struct ChInfoOptions {
  BOOL option1_option;
  BOOL option2_option;
  BOOL option3_option;
  BOOL option4_option;
  BOOL option5_option;
  BOOL option6_option;
  BOOL option7_option;
  BOOL extensions_option;
  ChannelInfoExts extensions;
} ChInfoOptions;


extern const ASN1CType asn1_type_ChInfoOptions[];

typedef struct ChannelInfo {
  OperatingClass80211 operatingClass;
  ChannelNumber80211 channelNumber;
  TXpower80211 powerLevel;
  WsaChInfoDataRate dataRate;
  ChInfoOptions extensions;
} ChannelInfo;


extern const ASN1CType asn1_type_ChannelInfo[];

typedef struct ChannelInfos {
  ChannelInfo *tab;
  size_t count;
} ChannelInfos;

extern const ASN1CType asn1_type_ChannelInfos[];

typedef int RouterLifetime;

extern const ASN1CType asn1_type_RouterLifetime[];

typedef ASN1String IpV6Prefix;

extern const ASN1CType asn1_type_IpV6Prefix[];

typedef int IpV6PrefixLength;

extern const ASN1CType asn1_type_IpV6PrefixLength[];

typedef struct RoutAdvertExt {
  RefExt extensionId;
  ASN1OpenType value;
} RoutAdvertExt;


extern const ASN1CType asn1_type_RoutAdvertExt[];

typedef struct RoutAdvertExts {
  RoutAdvertExt *tab;
  size_t count;
} RoutAdvertExts;

extern const ASN1CType asn1_type_RoutAdvertExts[];

typedef struct RoutingAdvertisement {
  RouterLifetime lifetime;
  IpV6Prefix ipPrefix;
  IpV6PrefixLength ipPrefixLength;
  IPv6Address defaultGateway;
  IPv6Address primaryDns;
  RoutAdvertExts extensions;
} RoutingAdvertisement;


extern const ASN1CType asn1_type_RoutingAdvertisement[];

typedef struct SrvAdvBody {
  SrvAdvChangeCount changeCount;
  BOOL extensions_option;
  SrvAdvMsgHeaderExts extensions;
  BOOL serviceInfos_option;
  ServiceInfos serviceInfos;
  BOOL channelInfos_option;
  ChannelInfos channelInfos;
  BOOL routingAdvertisement_option;
  RoutingAdvertisement routingAdvertisement;
} SrvAdvBody;


extern const ASN1CType asn1_type_SrvAdvBody[];

typedef struct SrvAdvMsg {
  SrvAdvPrtVersion version;
  SrvAdvBody body;
} SrvAdvMsg;


extern const ASN1CType asn1_type_SrvAdvMsg[];

typedef int ShortMsgVersion;

extern const ASN1CType asn1_type_ShortMsgVersion[];

typedef struct ShortMsgNextension {
  RefExt extensionId;
  ASN1OpenType value;
} ShortMsgNextension;


extern const ASN1CType asn1_type_ShortMsgNextension[];

typedef struct ShortMsgNextensions {
  ShortMsgNextension *tab;
  size_t count;
} ShortMsgNextensions;

extern const ASN1CType asn1_type_ShortMsgNextensions[];

typedef struct NullNetworking {
  ShortMsgVersion version;
  BOOL nExtensions_option;
  ShortMsgNextensions nExtensions;
} NullNetworking;


extern const ASN1CType asn1_type_NullNetworking[];

typedef struct NoSubtypeProcessing {
  ASN1BitString optBit;
  ShortMsgVersion version;
} NoSubtypeProcessing;


extern const ASN1CType asn1_type_NoSubtypeProcessing[];

typedef enum {
  ShortMsgSubtype_nullNetworking,
  ShortMsgSubtype_subTypeReserved1,
  ShortMsgSubtype_subTypeReserved2,
  ShortMsgSubtype_subTypeReserved3,
  ShortMsgSubtype_subTypeReserved4,
  ShortMsgSubtype_subTypeReserved5,
  ShortMsgSubtype_subTypeReserved6,
  ShortMsgSubtype_subTypeReserved7,
  ShortMsgSubtype_subTypeReserved8,
  ShortMsgSubtype_subTypeReserved9,
  ShortMsgSubtype_subTypeReserved19,
  ShortMsgSubtype_subTypeReserved11,
  ShortMsgSubtype_subTypeReserved12,
  ShortMsgSubtype_subTypeReserved13,
  ShortMsgSubtype_subTypeReserved14,
  ShortMsgSubtype_subTypeReserved15,
} ShortMsgSubtype_choice;

typedef struct ShortMsgSubtype {
  ShortMsgSubtype_choice choice;
  union {
    NullNetworking nullNetworking;
    NoSubtypeProcessing subTypeReserved1;
    NoSubtypeProcessing subTypeReserved2;
    NoSubtypeProcessing subTypeReserved3;
    NoSubtypeProcessing subTypeReserved4;
    NoSubtypeProcessing subTypeReserved5;
    NoSubtypeProcessing subTypeReserved6;
    NoSubtypeProcessing subTypeReserved7;
    NoSubtypeProcessing subTypeReserved8;
    NoSubtypeProcessing subTypeReserved9;
    NoSubtypeProcessing subTypeReserved19;
    NoSubtypeProcessing subTypeReserved11;
    NoSubtypeProcessing subTypeReserved12;
    NoSubtypeProcessing subTypeReserved13;
    NoSubtypeProcessing subTypeReserved14;
    NoSubtypeProcessing subTypeReserved15;
  } u;
} ShortMsgSubtype;

extern const ASN1CType asn1_type_ShortMsgSubtype[];

typedef struct ShortMsgTextension {
  RefExt extensionId;
  ASN1OpenType value;
} ShortMsgTextension;


extern const ASN1CType asn1_type_ShortMsgTextension[];

typedef struct ShortMsgTextensions {
  ShortMsgTextension *tab;
  size_t count;
} ShortMsgTextensions;

extern const ASN1CType asn1_type_ShortMsgTextensions[];

typedef struct ShortMsgBcPDU {
  VarLengthNumber destAddress;
  BOOL tExtensions_option;
  ShortMsgTextensions tExtensions;
} ShortMsgBcPDU;


extern const ASN1CType asn1_type_ShortMsgBcPDU[];

typedef ASN1BitString NoTpidProcessing;

extern const ASN1CType asn1_type_NoTpidProcessing[];

typedef enum {
  ShortMsgTpdus_bcMode,
  ShortMsgTpdus_tpidReserved1,
  ShortMsgTpdus_tpidReserved2,
  ShortMsgTpdus_tpidReserved3,
  ShortMsgTpdus_tpidReserved4,
  ShortMsgTpdus_tpidReserved5,
  ShortMsgTpdus_tpidReserved6,
  ShortMsgTpdus_tpidReserved7,
  ShortMsgTpdus_tpidReserved8,
  ShortMsgTpdus_tpidReserved9,
  ShortMsgTpdus_tpidReserved10,
  ShortMsgTpdus_tpidReserved11,
  ShortMsgTpdus_tpidReserved12,
  ShortMsgTpdus_tpidReserved13,
  ShortMsgTpdus_tpidReserved14,
  ShortMsgTpdus_tpidReserved15,
  ShortMsgTpdus_tpidReserved16,
  ShortMsgTpdus_tpidReserved17,
  ShortMsgTpdus_tpidReserved18,
  ShortMsgTpdus_tpidReserved19,
  ShortMsgTpdus_tpidReserved20,
  ShortMsgTpdus_tpidReserved21,
  ShortMsgTpdus_tpidReserved22,
  ShortMsgTpdus_tpidReserved23,
  ShortMsgTpdus_tpidReserved24,
  ShortMsgTpdus_tpidReserved25,
  ShortMsgTpdus_tpidReserved26,
  ShortMsgTpdus_tpidReserved27,
  ShortMsgTpdus_tpidReserved28,
  ShortMsgTpdus_tpidReserved29,
  ShortMsgTpdus_tpidReserved30,
  ShortMsgTpdus_tpidReserved31,
  ShortMsgTpdus_tpidReserved32,
  ShortMsgTpdus_tpidReserved33,
  ShortMsgTpdus_tpidReserved34,
  ShortMsgTpdus_tpidReserved35,
  ShortMsgTpdus_tpidReserved36,
  ShortMsgTpdus_tpidReserved37,
  ShortMsgTpdus_tpidReserved38,
  ShortMsgTpdus_tpidReserved39,
  ShortMsgTpdus_tpidReserved40,
  ShortMsgTpdus_tpidReserved41,
  ShortMsgTpdus_tpidReserved42,
  ShortMsgTpdus_tpidReserved43,
  ShortMsgTpdus_tpidReserved44,
  ShortMsgTpdus_tpidReserved45,
  ShortMsgTpdus_tpidReserved46,
  ShortMsgTpdus_tpidReserved47,
  ShortMsgTpdus_tpidReserved48,
  ShortMsgTpdus_tpidReserved49,
  ShortMsgTpdus_tpidReserved50,
  ShortMsgTpdus_tpidReserved51,
  ShortMsgTpdus_tpidReserved52,
  ShortMsgTpdus_tpidReserved53,
  ShortMsgTpdus_tpidReserved54,
  ShortMsgTpdus_tpidReserved55,
  ShortMsgTpdus_tpidReserved56,
  ShortMsgTpdus_tpidReserved57,
  ShortMsgTpdus_tpidReserved58,
  ShortMsgTpdus_tpidReserved59,
  ShortMsgTpdus_tpidReserved60,
  ShortMsgTpdus_tpidReserved61,
  ShortMsgTpdus_tpidReserved62,
  ShortMsgTpdus_tpidReserved63,
  ShortMsgTpdus_tpidReserved64,
  ShortMsgTpdus_tpidReserved65,
  ShortMsgTpdus_tpidReserved66,
  ShortMsgTpdus_tpidReserved67,
  ShortMsgTpdus_tpidReserved68,
  ShortMsgTpdus_tpidReserved69,
  ShortMsgTpdus_tpidReserved70,
  ShortMsgTpdus_tpidReserved71,
  ShortMsgTpdus_tpidReserved72,
  ShortMsgTpdus_tpidReserved73,
  ShortMsgTpdus_tpidReserved74,
  ShortMsgTpdus_tpidReserved75,
  ShortMsgTpdus_tpidReserved76,
  ShortMsgTpdus_tpidReserved77,
  ShortMsgTpdus_tpidReserved78,
  ShortMsgTpdus_tpidReserved79,
  ShortMsgTpdus_tpidReserved80,
  ShortMsgTpdus_tpidReserved81,
  ShortMsgTpdus_tpidReserved82,
  ShortMsgTpdus_tpidReserved83,
  ShortMsgTpdus_tpidReserved84,
  ShortMsgTpdus_tpidReserved85,
  ShortMsgTpdus_tpidReserved86,
  ShortMsgTpdus_tpidReserved87,
  ShortMsgTpdus_tpidReserved88,
  ShortMsgTpdus_tpidReserved89,
  ShortMsgTpdus_tpidReserved90,
  ShortMsgTpdus_tpidReserved91,
  ShortMsgTpdus_tpidReserved92,
  ShortMsgTpdus_tpidReserved93,
  ShortMsgTpdus_tpidReserved94,
  ShortMsgTpdus_tpidReserved95,
  ShortMsgTpdus_tpidReserved96,
  ShortMsgTpdus_tpidReserved97,
  ShortMsgTpdus_tpidReserved98,
  ShortMsgTpdus_tpidReserved99,
  ShortMsgTpdus_tpidReserved100,
  ShortMsgTpdus_tpidReserved101,
  ShortMsgTpdus_tpidReserved102,
  ShortMsgTpdus_tpidReserved103,
  ShortMsgTpdus_tpidReserved104,
  ShortMsgTpdus_tpidReserved105,
  ShortMsgTpdus_tpidReserved106,
  ShortMsgTpdus_tpidReserved107,
  ShortMsgTpdus_tpidReserved108,
  ShortMsgTpdus_tpidReserved109,
  ShortMsgTpdus_tpidReserved110,
  ShortMsgTpdus_tpidReserved111,
  ShortMsgTpdus_tpidReserved112,
  ShortMsgTpdus_tpidReserved113,
  ShortMsgTpdus_tpidReserved114,
  ShortMsgTpdus_tpidReserved115,
  ShortMsgTpdus_tpidReserved116,
  ShortMsgTpdus_tpidReserved117,
  ShortMsgTpdus_tpidReserved118,
  ShortMsgTpdus_tpidReserved119,
  ShortMsgTpdus_tpidReserved120,
  ShortMsgTpdus_tpidReserved121,
  ShortMsgTpdus_tpidReserved122,
  ShortMsgTpdus_tpidReserved123,
  ShortMsgTpdus_tpidReserved124,
  ShortMsgTpdus_tpidReserved125,
  ShortMsgTpdus_tpidReserved126,
  ShortMsgTpdus_tpidReserved127,
} ShortMsgTpdus_choice;

typedef struct ShortMsgTpdus {
  ShortMsgTpdus_choice choice;
  union {
    ShortMsgBcPDU bcMode;
    NoTpidProcessing tpidReserved1;
    NoTpidProcessing tpidReserved2;
    NoTpidProcessing tpidReserved3;
    NoTpidProcessing tpidReserved4;
    NoTpidProcessing tpidReserved5;
    NoTpidProcessing tpidReserved6;
    NoTpidProcessing tpidReserved7;
    NoTpidProcessing tpidReserved8;
    NoTpidProcessing tpidReserved9;
    NoTpidProcessing tpidReserved10;
    NoTpidProcessing tpidReserved11;
    NoTpidProcessing tpidReserved12;
    NoTpidProcessing tpidReserved13;
    NoTpidProcessing tpidReserved14;
    NoTpidProcessing tpidReserved15;
    NoTpidProcessing tpidReserved16;
    NoTpidProcessing tpidReserved17;
    NoTpidProcessing tpidReserved18;
    NoTpidProcessing tpidReserved19;
    NoTpidProcessing tpidReserved20;
    NoTpidProcessing tpidReserved21;
    NoTpidProcessing tpidReserved22;
    NoTpidProcessing tpidReserved23;
    NoTpidProcessing tpidReserved24;
    NoTpidProcessing tpidReserved25;
    NoTpidProcessing tpidReserved26;
    NoTpidProcessing tpidReserved27;
    NoTpidProcessing tpidReserved28;
    NoTpidProcessing tpidReserved29;
    NoTpidProcessing tpidReserved30;
    NoTpidProcessing tpidReserved31;
    NoTpidProcessing tpidReserved32;
    NoTpidProcessing tpidReserved33;
    NoTpidProcessing tpidReserved34;
    NoTpidProcessing tpidReserved35;
    NoTpidProcessing tpidReserved36;
    NoTpidProcessing tpidReserved37;
    NoTpidProcessing tpidReserved38;
    NoTpidProcessing tpidReserved39;
    NoTpidProcessing tpidReserved40;
    NoTpidProcessing tpidReserved41;
    NoTpidProcessing tpidReserved42;
    NoTpidProcessing tpidReserved43;
    NoTpidProcessing tpidReserved44;
    NoTpidProcessing tpidReserved45;
    NoTpidProcessing tpidReserved46;
    NoTpidProcessing tpidReserved47;
    NoTpidProcessing tpidReserved48;
    NoTpidProcessing tpidReserved49;
    NoTpidProcessing tpidReserved50;
    NoTpidProcessing tpidReserved51;
    NoTpidProcessing tpidReserved52;
    NoTpidProcessing tpidReserved53;
    NoTpidProcessing tpidReserved54;
    NoTpidProcessing tpidReserved55;
    NoTpidProcessing tpidReserved56;
    NoTpidProcessing tpidReserved57;
    NoTpidProcessing tpidReserved58;
    NoTpidProcessing tpidReserved59;
    NoTpidProcessing tpidReserved60;
    NoTpidProcessing tpidReserved61;
    NoTpidProcessing tpidReserved62;
    NoTpidProcessing tpidReserved63;
    NoTpidProcessing tpidReserved64;
    NoTpidProcessing tpidReserved65;
    NoTpidProcessing tpidReserved66;
    NoTpidProcessing tpidReserved67;
    NoTpidProcessing tpidReserved68;
    NoTpidProcessing tpidReserved69;
    NoTpidProcessing tpidReserved70;
    NoTpidProcessing tpidReserved71;
    NoTpidProcessing tpidReserved72;
    NoTpidProcessing tpidReserved73;
    NoTpidProcessing tpidReserved74;
    NoTpidProcessing tpidReserved75;
    NoTpidProcessing tpidReserved76;
    NoTpidProcessing tpidReserved77;
    NoTpidProcessing tpidReserved78;
    NoTpidProcessing tpidReserved79;
    NoTpidProcessing tpidReserved80;
    NoTpidProcessing tpidReserved81;
    NoTpidProcessing tpidReserved82;
    NoTpidProcessing tpidReserved83;
    NoTpidProcessing tpidReserved84;
    NoTpidProcessing tpidReserved85;
    NoTpidProcessing tpidReserved86;
    NoTpidProcessing tpidReserved87;
    NoTpidProcessing tpidReserved88;
    NoTpidProcessing tpidReserved89;
    NoTpidProcessing tpidReserved90;
    NoTpidProcessing tpidReserved91;
    NoTpidProcessing tpidReserved92;
    NoTpidProcessing tpidReserved93;
    NoTpidProcessing tpidReserved94;
    NoTpidProcessing tpidReserved95;
    NoTpidProcessing tpidReserved96;
    NoTpidProcessing tpidReserved97;
    NoTpidProcessing tpidReserved98;
    NoTpidProcessing tpidReserved99;
    NoTpidProcessing tpidReserved100;
    NoTpidProcessing tpidReserved101;
    NoTpidProcessing tpidReserved102;
    NoTpidProcessing tpidReserved103;
    NoTpidProcessing tpidReserved104;
    NoTpidProcessing tpidReserved105;
    NoTpidProcessing tpidReserved106;
    NoTpidProcessing tpidReserved107;
    NoTpidProcessing tpidReserved108;
    NoTpidProcessing tpidReserved109;
    NoTpidProcessing tpidReserved110;
    NoTpidProcessing tpidReserved111;
    NoTpidProcessing tpidReserved112;
    NoTpidProcessing tpidReserved113;
    NoTpidProcessing tpidReserved114;
    NoTpidProcessing tpidReserved115;
    NoTpidProcessing tpidReserved116;
    NoTpidProcessing tpidReserved117;
    NoTpidProcessing tpidReserved118;
    NoTpidProcessing tpidReserved119;
    NoTpidProcessing tpidReserved120;
    NoTpidProcessing tpidReserved121;
    NoTpidProcessing tpidReserved122;
    NoTpidProcessing tpidReserved123;
    NoTpidProcessing tpidReserved124;
    NoTpidProcessing tpidReserved125;
    NoTpidProcessing tpidReserved126;
    NoTpidProcessing tpidReserved127;
  } u;
} ShortMsgTpdus;

extern const ASN1CType asn1_type_ShortMsgTpdus[];

typedef ASN1String ShortMsgData;

extern const ASN1CType asn1_type_ShortMsgData[];

typedef struct ShortMsgNpdu {
  ShortMsgSubtype subtype;
  ShortMsgTpdus transport;
  ShortMsgData body;
} ShortMsgNpdu;


extern const ASN1CType asn1_type_ShortMsgNpdu[];

typedef ASN1Null DummyExtension;

extern const ASN1CType asn1_type_DummyExtension[];

typedef enum {
  VarLengthNumber2_shortNo,
  VarLengthNumber2_longNo,
} VarLengthNumber2_choice;

typedef struct VarLengthNumber2 {
  VarLengthNumber2_choice choice;
  union {
    int shortNo;
    int longNo;
  } u;
} VarLengthNumber2;

extern const ASN1CType asn1_type_VarLengthNumber2[];

#ifdef  __cplusplus
}
#endif

#endif /* _FFASN1_OUT_H */
