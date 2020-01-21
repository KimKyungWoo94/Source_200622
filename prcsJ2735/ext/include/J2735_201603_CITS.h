/* Automatically generated file - do not edit */
#ifndef _FFASN1_J2735_201603_CITS_H
#define _FFASN1_J2735_201603_CITS_H

#include "asn1defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef int DSRCmsgID;

extern const ASN1CType asn1_type_DSRCmsgID[];

typedef struct MessageFrame {
  DSRCmsgID messageId;
  ASN1OpenType value;
} MessageFrame;


extern const ASN1CType asn1_type_MessageFrame[];

typedef struct MESSAGE_ID_AND_TYPE { /* object class definition */
  ASN1CType id;
  ASN1CType Type;
} MESSAGE_ID_AND_TYPE;


extern const ASN1CType asn1_type_MESSAGE_ID_AND_TYPE[];

typedef int MsgCount;

extern const ASN1CType asn1_type_MsgCount[];

typedef ASN1String TemporaryID;

extern const ASN1CType asn1_type_TemporaryID[];

typedef int DSecond;

extern const ASN1CType asn1_type_DSecond[];

typedef int Latitude;

extern const ASN1CType asn1_type_Latitude[];

typedef int Longitude;

extern const ASN1CType asn1_type_Longitude[];

typedef int Elevation;

extern const ASN1CType asn1_type_Elevation[];

typedef int SemiMajorAxisAccuracy;

extern const ASN1CType asn1_type_SemiMajorAxisAccuracy[];

typedef int SemiMinorAxisAccuracy;

extern const ASN1CType asn1_type_SemiMinorAxisAccuracy[];

typedef int SemiMajorAxisOrientation;

extern const ASN1CType asn1_type_SemiMajorAxisOrientation[];

typedef struct PositionalAccuracy {
  SemiMajorAxisAccuracy semiMajor;
  SemiMinorAxisAccuracy semiMinor;
  SemiMajorAxisOrientation orientation;
} PositionalAccuracy;


extern const ASN1CType asn1_type_PositionalAccuracy[];

typedef enum TransmissionState {
  TransmissionState_neutral,
  TransmissionState_park,
  TransmissionState_forwardGears,
  TransmissionState_reverseGears,
  TransmissionState_reserved1,
  TransmissionState_reserved2,
  TransmissionState_reserved3,
  TransmissionState_unavailable,
} TransmissionState;

extern const ASN1CType asn1_type_TransmissionState[];

typedef int Speed;

extern const ASN1CType asn1_type_Speed[];

typedef int Heading;

extern const ASN1CType asn1_type_Heading[];

typedef int SteeringWheelAngle;

extern const ASN1CType asn1_type_SteeringWheelAngle[];

typedef int Acceleration;

extern const ASN1CType asn1_type_Acceleration[];

typedef int VerticalAcceleration;

extern const ASN1CType asn1_type_VerticalAcceleration[];

typedef int YawRate;

extern const ASN1CType asn1_type_YawRate[];

typedef struct AccelerationSet4Way {
  Acceleration Long;
  Acceleration lat;
  VerticalAcceleration vert;
  YawRate yaw;
} AccelerationSet4Way;


extern const ASN1CType asn1_type_AccelerationSet4Way[];

typedef ASN1BitString BrakeAppliedStatus;

extern const ASN1CType asn1_type_BrakeAppliedStatus[];

typedef enum TractionControlStatus {
  TractionControlStatus_unavailable,
  TractionControlStatus_off,
  TractionControlStatus_on,
  TractionControlStatus_engaged,
} TractionControlStatus;

extern const ASN1CType asn1_type_TractionControlStatus[];

typedef enum AntiLockBrakeStatus {
  AntiLockBrakeStatus_unavailable,
  AntiLockBrakeStatus_off,
  AntiLockBrakeStatus_on,
  AntiLockBrakeStatus_engaged,
} AntiLockBrakeStatus;

extern const ASN1CType asn1_type_AntiLockBrakeStatus[];

typedef enum StabilityControlStatus {
  StabilityControlStatus_unavailable,
  StabilityControlStatus_off,
  StabilityControlStatus_on,
  StabilityControlStatus_engaged,
} StabilityControlStatus;

extern const ASN1CType asn1_type_StabilityControlStatus[];

typedef enum BrakeBoostApplied {
  BrakeBoostApplied_unavailable,
  BrakeBoostApplied_off,
  BrakeBoostApplied_on,
} BrakeBoostApplied;

extern const ASN1CType asn1_type_BrakeBoostApplied[];

typedef enum AuxiliaryBrakeStatus {
  AuxiliaryBrakeStatus_unavailable,
  AuxiliaryBrakeStatus_off,
  AuxiliaryBrakeStatus_on,
  AuxiliaryBrakeStatus_reserved,
} AuxiliaryBrakeStatus;

extern const ASN1CType asn1_type_AuxiliaryBrakeStatus[];

typedef struct BrakeSystemStatus {
  BrakeAppliedStatus wheelBrakes;
  TractionControlStatus traction;
  AntiLockBrakeStatus albs;
  StabilityControlStatus scs;
  BrakeBoostApplied brakeBoost;
  AuxiliaryBrakeStatus auxBrakes;
} BrakeSystemStatus;


extern const ASN1CType asn1_type_BrakeSystemStatus[];

typedef int VehicleWidth;

extern const ASN1CType asn1_type_VehicleWidth[];

typedef int VehicleLength;

extern const ASN1CType asn1_type_VehicleLength[];

typedef struct VehicleSize {
  VehicleWidth width;
  VehicleLength length;
} VehicleSize;


extern const ASN1CType asn1_type_VehicleSize[];

typedef struct BSMcoreData {
  MsgCount msgCnt;
  TemporaryID id;
  DSecond secMark;
  Latitude lat;
  Longitude Long;
  Elevation elev;
  PositionalAccuracy accuracy;
  TransmissionState transmission;
  Speed speed;
  Heading heading;
  SteeringWheelAngle angle;
  AccelerationSet4Way accelSet;
  BrakeSystemStatus brakes;
  VehicleSize size;
} BSMcoreData;


extern const ASN1CType asn1_type_BSMcoreData[];

typedef int PartII_Id;

extern const ASN1CType asn1_type_PartII_Id[];

typedef struct PartIIcontent_1 {
  PartII_Id partII_Id;
  ASN1OpenType partII_Value;
} PartIIcontent_1;


extern const ASN1CType asn1_type_PartIIcontent_1[];

typedef struct BasicSafetyMessage_1 {
  PartIIcontent_1 *tab;
  size_t count;
} BasicSafetyMessage_1;

extern const ASN1CType asn1_type_BasicSafetyMessage_1[];

typedef int RegionId;

extern const ASN1CType asn1_type_RegionId[];

typedef struct RegionalExtension_1 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_1;


extern const ASN1CType asn1_type_RegionalExtension_1[];

typedef struct BasicSafetyMessage_2 {
  RegionalExtension_1 *tab;
  size_t count;
} BasicSafetyMessage_2;

extern const ASN1CType asn1_type_BasicSafetyMessage_2[];

typedef struct BasicSafetyMessage {
  BSMcoreData coreData;
  BOOL partII_option;
  BasicSafetyMessage_1 partII;
  BOOL regional_option;
  BasicSafetyMessage_2 regional;
} BasicSafetyMessage;


extern const ASN1CType asn1_type_BasicSafetyMessage[];

typedef int MinuteOfTheYear;

extern const ASN1CType asn1_type_MinuteOfTheYear[];

typedef enum LayerType {
  LayerType_none,
  LayerType_mixedContent,
  LayerType_generalMapData,
  LayerType_intersectionData,
  LayerType_curveData,
  LayerType_roadwaySectionData,
  LayerType_parkingAreaData,
  LayerType_sharedLaneData,
} LayerType;

extern const ASN1CType asn1_type_LayerType[];

typedef int LayerID;

extern const ASN1CType asn1_type_LayerID[];

typedef ASN1String DescriptiveName;

extern const ASN1CType asn1_type_DescriptiveName[];

typedef int RoadRegulatorID;

extern const ASN1CType asn1_type_RoadRegulatorID[];

typedef int IntersectionID;

extern const ASN1CType asn1_type_IntersectionID[];

typedef struct IntersectionReferenceID {
  BOOL region_option;
  RoadRegulatorID region;
  IntersectionID id;
} IntersectionReferenceID;


extern const ASN1CType asn1_type_IntersectionReferenceID[];

typedef struct RegionalExtension_49 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_49;


extern const ASN1CType asn1_type_RegionalExtension_49[];

typedef struct Position3D_1 {
  RegionalExtension_49 *tab;
  size_t count;
} Position3D_1;

extern const ASN1CType asn1_type_Position3D_1[];

typedef struct Position3D {
  Latitude lat;
  Longitude Long;
  BOOL elevation_option;
  Elevation elevation;
  BOOL regional_option;
  Position3D_1 regional;
} Position3D;


extern const ASN1CType asn1_type_Position3D[];

typedef int LaneWidth;

extern const ASN1CType asn1_type_LaneWidth[];

typedef enum SpeedLimitType {
  SpeedLimitType_unknown,
  SpeedLimitType_maxSpeedInSchoolZone,
  SpeedLimitType_maxSpeedInSchoolZoneWhenChildrenArePresent,
  SpeedLimitType_maxSpeedInConstructionZone,
  SpeedLimitType_vehicleMinSpeed,
  SpeedLimitType_vehicleMaxSpeed,
  SpeedLimitType_vehicleNightMaxSpeed,
  SpeedLimitType_truckMinSpeed,
  SpeedLimitType_truckMaxSpeed,
  SpeedLimitType_truckNightMaxSpeed,
  SpeedLimitType_vehiclesWithTrailersMinSpeed,
  SpeedLimitType_vehiclesWithTrailersMaxSpeed,
  SpeedLimitType_vehiclesWithTrailersNightMaxSpeed,
} SpeedLimitType;

extern const ASN1CType asn1_type_SpeedLimitType[];

typedef int Velocity;

extern const ASN1CType asn1_type_Velocity[];

typedef struct RegulatorySpeedLimit {
  SpeedLimitType type;
  Velocity speed;
} RegulatorySpeedLimit;


extern const ASN1CType asn1_type_RegulatorySpeedLimit[];

typedef struct SpeedLimitList {
  RegulatorySpeedLimit *tab;
  size_t count;
} SpeedLimitList;

extern const ASN1CType asn1_type_SpeedLimitList[];

typedef int LaneID;

extern const ASN1CType asn1_type_LaneID[];

typedef int ApproachID;

extern const ASN1CType asn1_type_ApproachID[];

typedef ASN1BitString LaneDirection;

extern const ASN1CType asn1_type_LaneDirection[];

typedef ASN1BitString LaneSharing;

extern const ASN1CType asn1_type_LaneSharing[];

typedef ASN1BitString LaneAttributes_Vehicle;

extern const ASN1CType asn1_type_LaneAttributes_Vehicle[];

typedef ASN1BitString LaneAttributes_Crosswalk;

extern const ASN1CType asn1_type_LaneAttributes_Crosswalk[];

typedef ASN1BitString LaneAttributes_Bike;

extern const ASN1CType asn1_type_LaneAttributes_Bike[];

typedef ASN1BitString LaneAttributes_Sidewalk;

extern const ASN1CType asn1_type_LaneAttributes_Sidewalk[];

typedef ASN1BitString LaneAttributes_Barrier;

extern const ASN1CType asn1_type_LaneAttributes_Barrier[];

typedef ASN1BitString LaneAttributes_Striping;

extern const ASN1CType asn1_type_LaneAttributes_Striping[];

typedef ASN1BitString LaneAttributes_TrackedVehicle;

extern const ASN1CType asn1_type_LaneAttributes_TrackedVehicle[];

typedef ASN1BitString LaneAttributes_Parking;

extern const ASN1CType asn1_type_LaneAttributes_Parking[];

typedef enum {
  LaneTypeAttributes_vehicle,
  LaneTypeAttributes_crosswalk,
  LaneTypeAttributes_bikeLane,
  LaneTypeAttributes_sidewalk,
  LaneTypeAttributes_median,
  LaneTypeAttributes_striping,
  LaneTypeAttributes_trackedVehicle,
  LaneTypeAttributes_parking,
} LaneTypeAttributes_choice;

typedef struct LaneTypeAttributes {
  LaneTypeAttributes_choice choice;
  union {
    LaneAttributes_Vehicle vehicle;
    LaneAttributes_Crosswalk crosswalk;
    LaneAttributes_Bike bikeLane;
    LaneAttributes_Sidewalk sidewalk;
    LaneAttributes_Barrier median;
    LaneAttributes_Striping striping;
    LaneAttributes_TrackedVehicle trackedVehicle;
    LaneAttributes_Parking parking;
  } u;
} LaneTypeAttributes;

extern const ASN1CType asn1_type_LaneTypeAttributes[];

typedef struct RegionalExtension_41 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_41;


extern const ASN1CType asn1_type_RegionalExtension_41[];

typedef struct LaneAttributes {
  LaneDirection directionalUse;
  LaneSharing sharedWith;
  LaneTypeAttributes laneType;
  BOOL regional_option;
  RegionalExtension_41 regional;
} LaneAttributes;


extern const ASN1CType asn1_type_LaneAttributes[];

typedef ASN1BitString AllowedManeuvers;

extern const ASN1CType asn1_type_AllowedManeuvers[];

typedef int Offset_B10;

extern const ASN1CType asn1_type_Offset_B10[];

typedef struct Node_XY_20b {
  Offset_B10 x;
  Offset_B10 y;
} Node_XY_20b;


extern const ASN1CType asn1_type_Node_XY_20b[];

typedef int Offset_B11;

extern const ASN1CType asn1_type_Offset_B11[];

typedef struct Node_XY_22b {
  Offset_B11 x;
  Offset_B11 y;
} Node_XY_22b;


extern const ASN1CType asn1_type_Node_XY_22b[];

typedef int Offset_B12;

extern const ASN1CType asn1_type_Offset_B12[];

typedef struct Node_XY_24b {
  Offset_B12 x;
  Offset_B12 y;
} Node_XY_24b;


extern const ASN1CType asn1_type_Node_XY_24b[];

typedef int Offset_B13;

extern const ASN1CType asn1_type_Offset_B13[];

typedef struct Node_XY_26b {
  Offset_B13 x;
  Offset_B13 y;
} Node_XY_26b;


extern const ASN1CType asn1_type_Node_XY_26b[];

typedef int Offset_B14;

extern const ASN1CType asn1_type_Offset_B14[];

typedef struct Node_XY_28b {
  Offset_B14 x;
  Offset_B14 y;
} Node_XY_28b;


extern const ASN1CType asn1_type_Node_XY_28b[];

typedef int Offset_B16;

extern const ASN1CType asn1_type_Offset_B16[];

typedef struct Node_XY_32b {
  Offset_B16 x;
  Offset_B16 y;
} Node_XY_32b;


extern const ASN1CType asn1_type_Node_XY_32b[];

typedef struct Node_LLmD_64b {
  Longitude lon;
  Latitude lat;
} Node_LLmD_64b;


extern const ASN1CType asn1_type_Node_LLmD_64b[];

typedef struct RegionalExtension_48 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_48;


extern const ASN1CType asn1_type_RegionalExtension_48[];

typedef enum {
  NodeOffsetPointXY_node_XY1,
  NodeOffsetPointXY_node_XY2,
  NodeOffsetPointXY_node_XY3,
  NodeOffsetPointXY_node_XY4,
  NodeOffsetPointXY_node_XY5,
  NodeOffsetPointXY_node_XY6,
  NodeOffsetPointXY_node_LatLon,
  NodeOffsetPointXY_regional,
} NodeOffsetPointXY_choice;

typedef struct NodeOffsetPointXY {
  NodeOffsetPointXY_choice choice;
  union {
    Node_XY_20b node_XY1;
    Node_XY_22b node_XY2;
    Node_XY_24b node_XY3;
    Node_XY_26b node_XY4;
    Node_XY_28b node_XY5;
    Node_XY_32b node_XY6;
    Node_LLmD_64b node_LatLon;
    RegionalExtension_48 regional;
  } u;
} NodeOffsetPointXY;

extern const ASN1CType asn1_type_NodeOffsetPointXY[];

typedef enum NodeAttributeXY {
  NodeAttributeXY_reserved,
  NodeAttributeXY_stopLine,
  NodeAttributeXY_roundedCapStyleA,
  NodeAttributeXY_roundedCapStyleB,
  NodeAttributeXY_mergePoint,
  NodeAttributeXY_divergePoint,
  NodeAttributeXY_downstreamStopLine,
  NodeAttributeXY_downstreamStartNode,
  NodeAttributeXY_closedToTraffic,
  NodeAttributeXY_safeIsland,
  NodeAttributeXY_curbPresentAtStepOff,
  NodeAttributeXY_hydrantPresent,
} NodeAttributeXY;

extern const ASN1CType asn1_type_NodeAttributeXY[];

typedef struct NodeAttributeXYList {
  NodeAttributeXY *tab;
  size_t count;
} NodeAttributeXYList;

extern const ASN1CType asn1_type_NodeAttributeXYList[];

typedef enum SegmentAttributeXY {
  SegmentAttributeXY_reserved,
  SegmentAttributeXY_doNotBlock,
  SegmentAttributeXY_whiteLine,
  SegmentAttributeXY_mergingLaneLeft,
  SegmentAttributeXY_mergingLaneRight,
  SegmentAttributeXY_curbOnLeft,
  SegmentAttributeXY_curbOnRight,
  SegmentAttributeXY_loadingzoneOnLeft,
  SegmentAttributeXY_loadingzoneOnRight,
  SegmentAttributeXY_turnOutPointOnLeft,
  SegmentAttributeXY_turnOutPointOnRight,
  SegmentAttributeXY_adjacentParkingOnLeft,
  SegmentAttributeXY_adjacentParkingOnRight,
  SegmentAttributeXY_adjacentBikeLaneOnLeft,
  SegmentAttributeXY_adjacentBikeLaneOnRight,
  SegmentAttributeXY_sharedBikeLane,
  SegmentAttributeXY_bikeBoxInFront,
  SegmentAttributeXY_transitStopOnLeft,
  SegmentAttributeXY_transitStopOnRight,
  SegmentAttributeXY_transitStopInLane,
  SegmentAttributeXY_sharedWithTrackedVehicle,
  SegmentAttributeXY_safeIsland,
  SegmentAttributeXY_lowCurbsPresent,
  SegmentAttributeXY_rumbleStripPresent,
  SegmentAttributeXY_audibleSignalingPresent,
  SegmentAttributeXY_adaptiveTimingPresent,
  SegmentAttributeXY_rfSignalRequestPresent,
  SegmentAttributeXY_partialCurbIntrusion,
  SegmentAttributeXY_taperToLeft,
  SegmentAttributeXY_taperToRight,
  SegmentAttributeXY_taperToCenterLine,
  SegmentAttributeXY_parallelParking,
  SegmentAttributeXY_headInParking,
  SegmentAttributeXY_freeParking,
  SegmentAttributeXY_timeRestrictionsOnParking,
  SegmentAttributeXY_costToPark,
  SegmentAttributeXY_midBlockCurbPresent,
  SegmentAttributeXY_unEvenPavementPresent,
} SegmentAttributeXY;

extern const ASN1CType asn1_type_SegmentAttributeXY[];

typedef struct SegmentAttributeXYList {
  SegmentAttributeXY *tab;
  size_t count;
} SegmentAttributeXYList;

extern const ASN1CType asn1_type_SegmentAttributeXYList[];

typedef int DeltaAngle;

extern const ASN1CType asn1_type_DeltaAngle[];

typedef int RoadwayCrownAngle;

extern const ASN1CType asn1_type_RoadwayCrownAngle[];

typedef int MergeDivergeNodeAngle;

extern const ASN1CType asn1_type_MergeDivergeNodeAngle[];

typedef struct RegionalExtension_42 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_42;


extern const ASN1CType asn1_type_RegionalExtension_42[];

typedef struct LaneDataAttribute_1 {
  RegionalExtension_42 *tab;
  size_t count;
} LaneDataAttribute_1;

extern const ASN1CType asn1_type_LaneDataAttribute_1[];

typedef enum {
  LaneDataAttribute_pathEndPointAngle,
  LaneDataAttribute_laneCrownPointCenter,
  LaneDataAttribute_laneCrownPointLeft,
  LaneDataAttribute_laneCrownPointRight,
  LaneDataAttribute_laneAngle,
  LaneDataAttribute_speedLimits,
  LaneDataAttribute_regional,
} LaneDataAttribute_choice;

typedef struct LaneDataAttribute {
  LaneDataAttribute_choice choice;
  union {
    DeltaAngle pathEndPointAngle;
    RoadwayCrownAngle laneCrownPointCenter;
    RoadwayCrownAngle laneCrownPointLeft;
    RoadwayCrownAngle laneCrownPointRight;
    MergeDivergeNodeAngle laneAngle;
    SpeedLimitList speedLimits;
    LaneDataAttribute_1 regional;
  } u;
} LaneDataAttribute;

extern const ASN1CType asn1_type_LaneDataAttribute[];

typedef struct LaneDataAttributeList {
  LaneDataAttribute *tab;
  size_t count;
} LaneDataAttributeList;

extern const ASN1CType asn1_type_LaneDataAttributeList[];

typedef struct RegionalExtension_46 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_46;


extern const ASN1CType asn1_type_RegionalExtension_46[];

typedef struct NodeAttributeSetXY_1 {
  RegionalExtension_46 *tab;
  size_t count;
} NodeAttributeSetXY_1;

extern const ASN1CType asn1_type_NodeAttributeSetXY_1[];

typedef struct NodeAttributeSetXY {
  BOOL localNode_option;
  NodeAttributeXYList localNode;
  BOOL disabled_option;
  SegmentAttributeXYList disabled;
  BOOL enabled_option;
  SegmentAttributeXYList enabled;
  BOOL data_option;
  LaneDataAttributeList data;
  BOOL dWidth_option;
  Offset_B10 dWidth;
  BOOL dElevation_option;
  Offset_B10 dElevation;
  BOOL regional_option;
  NodeAttributeSetXY_1 regional;
} NodeAttributeSetXY;


extern const ASN1CType asn1_type_NodeAttributeSetXY[];

typedef struct NodeXY {
  NodeOffsetPointXY delta;
  BOOL attributes_option;
  NodeAttributeSetXY attributes;
} NodeXY;


extern const ASN1CType asn1_type_NodeXY[];

typedef struct NodeSetXY {
  NodeXY *tab;
  size_t count;
} NodeSetXY;

extern const ASN1CType asn1_type_NodeSetXY[];

typedef int DrivenLineOffsetSm;

extern const ASN1CType asn1_type_DrivenLineOffsetSm[];

typedef int DrivenLineOffsetLg;

extern const ASN1CType asn1_type_DrivenLineOffsetLg[];

typedef enum {
  ComputedLane_1_small,
  ComputedLane_1_large,
} ComputedLane_1_choice;

typedef struct ComputedLane_1 {
  ComputedLane_1_choice choice;
  union {
    DrivenLineOffsetSm small;
    DrivenLineOffsetLg large;
  } u;
} ComputedLane_1;

extern const ASN1CType asn1_type_ComputedLane_1[];

typedef enum {
  ComputedLane_2_small,
  ComputedLane_2_large,
} ComputedLane_2_choice;

typedef struct ComputedLane_2 {
  ComputedLane_2_choice choice;
  union {
    DrivenLineOffsetSm small;
    DrivenLineOffsetLg large;
  } u;
} ComputedLane_2;

extern const ASN1CType asn1_type_ComputedLane_2[];

typedef int Angle;

extern const ASN1CType asn1_type_Angle[];

typedef int Scale_B12;

extern const ASN1CType asn1_type_Scale_B12[];

typedef struct RegionalExtension_33 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_33;


extern const ASN1CType asn1_type_RegionalExtension_33[];

typedef struct ComputedLane_3 {
  RegionalExtension_33 *tab;
  size_t count;
} ComputedLane_3;

extern const ASN1CType asn1_type_ComputedLane_3[];

typedef struct ComputedLane {
  LaneID referenceLaneId;
  ComputedLane_1 offsetXaxis;
  ComputedLane_2 offsetYaxis;
  BOOL rotateXY_option;
  Angle rotateXY;
  BOOL scaleXaxis_option;
  Scale_B12 scaleXaxis;
  BOOL scaleYaxis_option;
  Scale_B12 scaleYaxis;
  BOOL regional_option;
  ComputedLane_3 regional;
} ComputedLane;


extern const ASN1CType asn1_type_ComputedLane[];

typedef enum {
  NodeListXY_nodes,
  NodeListXY_computed,
} NodeListXY_choice;

typedef struct NodeListXY {
  NodeListXY_choice choice;
  union {
    NodeSetXY nodes;
    ComputedLane computed;
  } u;
} NodeListXY;

extern const ASN1CType asn1_type_NodeListXY[];

typedef struct ConnectingLane {
  LaneID lane;
  BOOL maneuver_option;
  AllowedManeuvers maneuver;
} ConnectingLane;


extern const ASN1CType asn1_type_ConnectingLane[];

typedef int SignalGroupID;

extern const ASN1CType asn1_type_SignalGroupID[];

typedef int RestrictionClassID;

extern const ASN1CType asn1_type_RestrictionClassID[];

typedef int LaneConnectionID;

extern const ASN1CType asn1_type_LaneConnectionID[];

typedef struct Connection {
  ConnectingLane connectingLane;
  BOOL remoteIntersection_option;
  IntersectionReferenceID remoteIntersection;
  BOOL signalGroup_option;
  SignalGroupID signalGroup;
  BOOL userClass_option;
  RestrictionClassID userClass;
  BOOL connectionID_option;
  LaneConnectionID connectionID;
} Connection;


extern const ASN1CType asn1_type_Connection[];

typedef struct ConnectsToList {
  Connection *tab;
  size_t count;
} ConnectsToList;

extern const ASN1CType asn1_type_ConnectsToList[];

typedef struct OverlayLaneList {
  LaneID *tab;
  size_t count;
} OverlayLaneList;

extern const ASN1CType asn1_type_OverlayLaneList[];

typedef struct RegionalExtension_36 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_36;


extern const ASN1CType asn1_type_RegionalExtension_36[];

typedef struct GenericLane_1 {
  RegionalExtension_36 *tab;
  size_t count;
} GenericLane_1;

extern const ASN1CType asn1_type_GenericLane_1[];

typedef struct GenericLane {
  LaneID laneID;
  BOOL name_option;
  DescriptiveName name;
  BOOL ingressApproach_option;
  ApproachID ingressApproach;
  BOOL egressApproach_option;
  ApproachID egressApproach;
  LaneAttributes laneAttributes;
  BOOL maneuvers_option;
  AllowedManeuvers maneuvers;
  NodeListXY nodeList;
  BOOL connectsTo_option;
  ConnectsToList connectsTo;
  BOOL overlays_option;
  OverlayLaneList overlays;
  BOOL regional_option;
  GenericLane_1 regional;
} GenericLane;


extern const ASN1CType asn1_type_GenericLane[];

typedef struct LaneList {
  GenericLane *tab;
  size_t count;
} LaneList;

extern const ASN1CType asn1_type_LaneList[];

typedef struct RegionalExtension_50 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_50;


extern const ASN1CType asn1_type_RegionalExtension_50[];

typedef struct SignalControlZone {
  RegionalExtension_50 zone;
} SignalControlZone;


extern const ASN1CType asn1_type_SignalControlZone[];

typedef struct PreemptPriorityList {
  SignalControlZone *tab;
  size_t count;
} PreemptPriorityList;

extern const ASN1CType asn1_type_PreemptPriorityList[];

typedef struct RegionalExtension_39 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_39;


extern const ASN1CType asn1_type_RegionalExtension_39[];

typedef struct IntersectionGeometry_1 {
  RegionalExtension_39 *tab;
  size_t count;
} IntersectionGeometry_1;

extern const ASN1CType asn1_type_IntersectionGeometry_1[];

typedef struct IntersectionGeometry {
  BOOL name_option;
  DescriptiveName name;
  IntersectionReferenceID id;
  MsgCount revision;
  Position3D refPoint;
  BOOL laneWidth_option;
  LaneWidth laneWidth;
  BOOL speedLimits_option;
  SpeedLimitList speedLimits;
  LaneList laneSet;
  BOOL preemptPriorityData_option;
  PreemptPriorityList preemptPriorityData;
  BOOL regional_option;
  IntersectionGeometry_1 regional;
} IntersectionGeometry;


extern const ASN1CType asn1_type_IntersectionGeometry[];

typedef struct IntersectionGeometryList {
  IntersectionGeometry *tab;
  size_t count;
} IntersectionGeometryList;

extern const ASN1CType asn1_type_IntersectionGeometryList[];

typedef int RoadSegmentID;

extern const ASN1CType asn1_type_RoadSegmentID[];

typedef struct RoadSegmentReferenceID {
  BOOL region_option;
  RoadRegulatorID region;
  RoadSegmentID id;
} RoadSegmentReferenceID;


extern const ASN1CType asn1_type_RoadSegmentReferenceID[];

typedef struct RoadLaneSetList {
  GenericLane *tab;
  size_t count;
} RoadLaneSetList;

extern const ASN1CType asn1_type_RoadLaneSetList[];

typedef struct RegionalExtension_54 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_54;


extern const ASN1CType asn1_type_RegionalExtension_54[];

typedef struct RoadSegment_1 {
  RegionalExtension_54 *tab;
  size_t count;
} RoadSegment_1;

extern const ASN1CType asn1_type_RoadSegment_1[];

typedef struct RoadSegment {
  BOOL name_option;
  DescriptiveName name;
  RoadSegmentReferenceID id;
  MsgCount revision;
  Position3D refPoint;
  BOOL laneWidth_option;
  LaneWidth laneWidth;
  BOOL speedLimits_option;
  SpeedLimitList speedLimits;
  RoadLaneSetList roadLaneSet;
  BOOL regional_option;
  RoadSegment_1 regional;
} RoadSegment;


extern const ASN1CType asn1_type_RoadSegment[];

typedef struct RoadSegmentList {
  RoadSegment *tab;
  size_t count;
} RoadSegmentList;

extern const ASN1CType asn1_type_RoadSegmentList[];

typedef struct DataParameters {
  BOOL processMethod_option;
  ASN1String processMethod;
  BOOL processAgency_option;
  ASN1String processAgency;
  BOOL lastCheckedDate_option;
  ASN1String lastCheckedDate;
  BOOL geoidUsed_option;
  ASN1String geoidUsed;
} DataParameters;


extern const ASN1CType asn1_type_DataParameters[];

typedef enum RestrictionAppliesTo {
  RestrictionAppliesTo_none,
  RestrictionAppliesTo_equippedTransit,
  RestrictionAppliesTo_equippedTaxis,
  RestrictionAppliesTo_equippedOther,
  RestrictionAppliesTo_emissionCompliant,
  RestrictionAppliesTo_equippedBicycle,
  RestrictionAppliesTo_weightCompliant,
  RestrictionAppliesTo_heightCompliant,
  RestrictionAppliesTo_pedestrians,
  RestrictionAppliesTo_slowMovingPersons,
  RestrictionAppliesTo_wheelchairUsers,
  RestrictionAppliesTo_visualDisabilities,
  RestrictionAppliesTo_audioDisabilities,
  RestrictionAppliesTo_otherUnknownDisabilities,
} RestrictionAppliesTo;

extern const ASN1CType asn1_type_RestrictionAppliesTo[];

typedef struct RegionalExtension_53 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_53;


extern const ASN1CType asn1_type_RegionalExtension_53[];

typedef struct RestrictionUserType_1 {
  RegionalExtension_53 *tab;
  size_t count;
} RestrictionUserType_1;

extern const ASN1CType asn1_type_RestrictionUserType_1[];

typedef enum {
  RestrictionUserType_basicType,
  RestrictionUserType_regional,
} RestrictionUserType_choice;

typedef struct RestrictionUserType {
  RestrictionUserType_choice choice;
  union {
    RestrictionAppliesTo basicType;
    RestrictionUserType_1 regional;
  } u;
} RestrictionUserType;

extern const ASN1CType asn1_type_RestrictionUserType[];

typedef struct RestrictionUserTypeList {
  RestrictionUserType *tab;
  size_t count;
} RestrictionUserTypeList;

extern const ASN1CType asn1_type_RestrictionUserTypeList[];

typedef struct RestrictionClassAssignment {
  RestrictionClassID id;
  RestrictionUserTypeList users;
} RestrictionClassAssignment;


extern const ASN1CType asn1_type_RestrictionClassAssignment[];

typedef struct RestrictionClassList {
  RestrictionClassAssignment *tab;
  size_t count;
} RestrictionClassList;

extern const ASN1CType asn1_type_RestrictionClassList[];

typedef struct RegionalExtension_5 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_5;


extern const ASN1CType asn1_type_RegionalExtension_5[];

typedef struct MapData_1 {
  RegionalExtension_5 *tab;
  size_t count;
} MapData_1;

extern const ASN1CType asn1_type_MapData_1[];

typedef struct MapData {
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  MsgCount msgIssueRevision;
  BOOL layerType_option;
  LayerType layerType;
  BOOL layerID_option;
  LayerID layerID;
  BOOL intersections_option;
  IntersectionGeometryList intersections;
  BOOL roadSegments_option;
  RoadSegmentList roadSegments;
  BOOL dataParameters_option;
  DataParameters dataParameters;
  BOOL restrictionList_option;
  RestrictionClassList restrictionList;
  BOOL regional_option;
  MapData_1 regional;
} MapData;


extern const ASN1CType asn1_type_MapData[];

typedef ASN1BitString IntersectionStatusObject;

extern const ASN1CType asn1_type_IntersectionStatusObject[];

typedef struct EnabledLaneList {
  LaneID *tab;
  size_t count;
} EnabledLaneList;

extern const ASN1CType asn1_type_EnabledLaneList[];

typedef enum MovementPhaseState {
  MovementPhaseState_unavailable,
  MovementPhaseState_dark,
  MovementPhaseState_stop_Then_Proceed,
  MovementPhaseState_stop_And_Remain,
  MovementPhaseState_pre_Movement,
  MovementPhaseState_permissive_Movement_Allowed,
  MovementPhaseState_protected_Movement_Allowed,
  MovementPhaseState_permissive_clearance,
  MovementPhaseState_protected_clearance,
  MovementPhaseState_caution_Conflicting_Traffic,
} MovementPhaseState;

extern const ASN1CType asn1_type_MovementPhaseState[];

typedef int TimeMark;

extern const ASN1CType asn1_type_TimeMark[];

typedef int TimeIntervalConfidence;

extern const ASN1CType asn1_type_TimeIntervalConfidence[];

typedef struct TimeChangeDetails {
  BOOL startTime_option;
  TimeMark startTime;
  TimeMark minEndTime;
  BOOL maxEndTime_option;
  TimeMark maxEndTime;
  BOOL likelyTime_option;
  TimeMark likelyTime;
  BOOL confidence_option;
  TimeIntervalConfidence confidence;
  BOOL nextTime_option;
  TimeMark nextTime;
} TimeChangeDetails;


extern const ASN1CType asn1_type_TimeChangeDetails[];

typedef enum AdvisorySpeedType {
  AdvisorySpeedType_none,
  AdvisorySpeedType_greenwave,
  AdvisorySpeedType_ecoDrive,
  AdvisorySpeedType_transit,
} AdvisorySpeedType;

extern const ASN1CType asn1_type_AdvisorySpeedType[];

typedef int SpeedAdvice;

extern const ASN1CType asn1_type_SpeedAdvice[];

typedef enum SpeedConfidence {
  SpeedConfidence_unavailable,
  SpeedConfidence_prec100ms,
  SpeedConfidence_prec10ms,
  SpeedConfidence_prec5ms,
  SpeedConfidence_prec1ms,
  SpeedConfidence_prec0_1ms,
  SpeedConfidence_prec0_05ms,
  SpeedConfidence_prec0_01ms,
} SpeedConfidence;

extern const ASN1CType asn1_type_SpeedConfidence[];

typedef int ZoneLength;

extern const ASN1CType asn1_type_ZoneLength[];

typedef struct RegionalExtension_32 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_32;


extern const ASN1CType asn1_type_RegionalExtension_32[];

typedef struct AdvisorySpeed_1 {
  RegionalExtension_32 *tab;
  size_t count;
} AdvisorySpeed_1;

extern const ASN1CType asn1_type_AdvisorySpeed_1[];

typedef struct AdvisorySpeed {
  AdvisorySpeedType type;
  BOOL speed_option;
  SpeedAdvice speed;
  BOOL confidence_option;
  SpeedConfidence confidence;
  BOOL distance_option;
  ZoneLength distance;
  BOOL Class_option;
  RestrictionClassID Class;
  BOOL regional_option;
  AdvisorySpeed_1 regional;
} AdvisorySpeed;


extern const ASN1CType asn1_type_AdvisorySpeed[];

typedef struct AdvisorySpeedList {
  AdvisorySpeed *tab;
  size_t count;
} AdvisorySpeedList;

extern const ASN1CType asn1_type_AdvisorySpeedList[];

typedef struct RegionalExtension_43 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_43;


extern const ASN1CType asn1_type_RegionalExtension_43[];

typedef struct MovementEvent_1 {
  RegionalExtension_43 *tab;
  size_t count;
} MovementEvent_1;

extern const ASN1CType asn1_type_MovementEvent_1[];

typedef struct MovementEvent {
  MovementPhaseState eventState;
  BOOL timing_option;
  TimeChangeDetails timing;
  BOOL speeds_option;
  AdvisorySpeedList speeds;
  BOOL regional_option;
  MovementEvent_1 regional;
} MovementEvent;


extern const ASN1CType asn1_type_MovementEvent[];

typedef struct MovementEventList {
  MovementEvent *tab;
  size_t count;
} MovementEventList;

extern const ASN1CType asn1_type_MovementEventList[];

typedef BOOL WaitOnStopline;

extern const ASN1CType asn1_type_WaitOnStopline[];

typedef BOOL PedestrianBicycleDetect;

extern const ASN1CType asn1_type_PedestrianBicycleDetect[];

typedef struct RegionalExtension_34 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_34;


extern const ASN1CType asn1_type_RegionalExtension_34[];

typedef struct ConnectionManeuverAssist_1 {
  RegionalExtension_34 *tab;
  size_t count;
} ConnectionManeuverAssist_1;

extern const ASN1CType asn1_type_ConnectionManeuverAssist_1[];

typedef struct ConnectionManeuverAssist {
  LaneConnectionID connectionID;
  BOOL queueLength_option;
  ZoneLength queueLength;
  BOOL availableStorageLength_option;
  ZoneLength availableStorageLength;
  BOOL waitOnStop_option;
  WaitOnStopline waitOnStop;
  BOOL pedBicycleDetect_option;
  PedestrianBicycleDetect pedBicycleDetect;
  BOOL regional_option;
  ConnectionManeuverAssist_1 regional;
} ConnectionManeuverAssist;


extern const ASN1CType asn1_type_ConnectionManeuverAssist[];

typedef struct ManeuverAssistList {
  ConnectionManeuverAssist *tab;
  size_t count;
} ManeuverAssistList;

extern const ASN1CType asn1_type_ManeuverAssistList[];

typedef struct RegionalExtension_44 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_44;


extern const ASN1CType asn1_type_RegionalExtension_44[];

typedef struct MovementState_1 {
  RegionalExtension_44 *tab;
  size_t count;
} MovementState_1;

extern const ASN1CType asn1_type_MovementState_1[];

typedef struct MovementState {
  BOOL movementName_option;
  DescriptiveName movementName;
  SignalGroupID signalGroup;
  MovementEventList state_time_speed;
  BOOL maneuverAssistList_option;
  ManeuverAssistList maneuverAssistList;
  BOOL regional_option;
  MovementState_1 regional;
} MovementState;


extern const ASN1CType asn1_type_MovementState[];

typedef struct MovementList {
  MovementState *tab;
  size_t count;
} MovementList;

extern const ASN1CType asn1_type_MovementList[];

typedef struct RegionalExtension_40 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_40;


extern const ASN1CType asn1_type_RegionalExtension_40[];

typedef struct IntersectionState_1 {
  RegionalExtension_40 *tab;
  size_t count;
} IntersectionState_1;

extern const ASN1CType asn1_type_IntersectionState_1[];

typedef struct IntersectionState {
  BOOL name_option;
  DescriptiveName name;
  IntersectionReferenceID id;
  MsgCount revision;
  IntersectionStatusObject status;
  BOOL moy_option;
  MinuteOfTheYear moy;
  BOOL timeStamp_option;
  DSecond timeStamp;
  BOOL enabledLanes_option;
  EnabledLaneList enabledLanes;
  MovementList states;
  BOOL maneuverAssistList_option;
  ManeuverAssistList maneuverAssistList;
  BOOL regional_option;
  IntersectionState_1 regional;
} IntersectionState;


extern const ASN1CType asn1_type_IntersectionState[];

typedef struct IntersectionStateList {
  IntersectionState *tab;
  size_t count;
} IntersectionStateList;

extern const ASN1CType asn1_type_IntersectionStateList[];

typedef struct RegionalExtension_12 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_12;


extern const ASN1CType asn1_type_RegionalExtension_12[];

typedef struct SPAT_1 {
  RegionalExtension_12 *tab;
  size_t count;
} SPAT_1;

extern const ASN1CType asn1_type_SPAT_1[];

typedef struct SPAT {
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  BOOL name_option;
  DescriptiveName name;
  IntersectionStateList intersections;
  BOOL regional_option;
  SPAT_1 regional;
} SPAT;


extern const ASN1CType asn1_type_SPAT[];

typedef enum RequestedItem {
  RequestedItem_reserved,
  RequestedItem_itemA,
  RequestedItem_itemB,
  RequestedItem_itemC,
  RequestedItem_itemD,
  RequestedItem_itemE,
  RequestedItem_itemF,
  RequestedItem_itemG,
  RequestedItem_itemI,
  RequestedItem_itemJ,
  RequestedItem_itemK,
  RequestedItem_itemL,
  RequestedItem_itemM,
  RequestedItem_itemN,
  RequestedItem_itemO,
  RequestedItem_itemP,
  RequestedItem_itemQ,
} RequestedItem;

extern const ASN1CType asn1_type_RequestedItem[];

typedef struct RequestedItemList {
  RequestedItem *tab;
  size_t count;
} RequestedItemList;

extern const ASN1CType asn1_type_RequestedItemList[];

typedef struct RegionalExtension_2 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_2;


extern const ASN1CType asn1_type_RegionalExtension_2[];

typedef struct CommonSafetyRequest_1 {
  RegionalExtension_2 *tab;
  size_t count;
} CommonSafetyRequest_1;

extern const ASN1CType asn1_type_CommonSafetyRequest_1[];

typedef struct CommonSafetyRequest {
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  BOOL msgCnt_option;
  MsgCount msgCnt;
  BOOL id_option;
  TemporaryID id;
  RequestedItemList requests;
  BOOL regional_option;
  CommonSafetyRequest_1 regional;
} CommonSafetyRequest;


extern const ASN1CType asn1_type_CommonSafetyRequest[];

typedef int ITIScodes;

extern const ASN1CType asn1_type_ITIScodes[];

typedef struct RoadSideAlert_1 {
  ITIScodes *tab;
  size_t count;
} RoadSideAlert_1;

extern const ASN1CType asn1_type_RoadSideAlert_1[];

typedef ASN1String Priority;

extern const ASN1CType asn1_type_Priority[];

typedef ASN1BitString HeadingSlice;

extern const ASN1CType asn1_type_HeadingSlice[];

typedef enum Extent {
  Extent_useInstantlyOnly,
  Extent_useFor3meters,
  Extent_useFor10meters,
  Extent_useFor50meters,
  Extent_useFor100meters,
  Extent_useFor500meters,
  Extent_useFor1000meters,
  Extent_useFor5000meters,
  Extent_useFor10000meters,
  Extent_useFor50000meters,
  Extent_useFor100000meters,
  Extent_useFor500000meters,
  Extent_useFor1000000meters,
  Extent_useFor5000000meters,
  Extent_useFor10000000meters,
  Extent_forever,
} Extent;

extern const ASN1CType asn1_type_Extent[];

typedef int DYear;

extern const ASN1CType asn1_type_DYear[];

typedef int DMonth;

extern const ASN1CType asn1_type_DMonth[];

typedef int DDay;

extern const ASN1CType asn1_type_DDay[];

typedef int DHour;

extern const ASN1CType asn1_type_DHour[];

typedef int DMinute;

extern const ASN1CType asn1_type_DMinute[];

typedef int DOffset;

extern const ASN1CType asn1_type_DOffset[];

typedef struct DDateTime {
  BOOL year_option;
  DYear year;
  BOOL month_option;
  DMonth month;
  BOOL day_option;
  DDay day;
  BOOL hour_option;
  DHour hour;
  BOOL minute_option;
  DMinute minute;
  BOOL second_option;
  DSecond second;
  BOOL offset_option;
  DOffset offset;
} DDateTime;


extern const ASN1CType asn1_type_DDateTime[];

typedef struct TransmissionAndSpeed {
  TransmissionState transmisson;
  Velocity speed;
} TransmissionAndSpeed;


extern const ASN1CType asn1_type_TransmissionAndSpeed[];

typedef enum TimeConfidence {
  TimeConfidence_unavailable,
  TimeConfidence_time_100_000,
  TimeConfidence_time_050_000,
  TimeConfidence_time_020_000,
  TimeConfidence_time_010_000,
  TimeConfidence_time_002_000,
  TimeConfidence_time_001_000,
  TimeConfidence_time_000_500,
  TimeConfidence_time_000_200,
  TimeConfidence_time_000_100,
  TimeConfidence_time_000_050,
  TimeConfidence_time_000_020,
  TimeConfidence_time_000_010,
  TimeConfidence_time_000_005,
  TimeConfidence_time_000_002,
  TimeConfidence_time_000_001,
  TimeConfidence_time_000_000_5,
  TimeConfidence_time_000_000_2,
  TimeConfidence_time_000_000_1,
  TimeConfidence_time_000_000_05,
  TimeConfidence_time_000_000_02,
  TimeConfidence_time_000_000_01,
  TimeConfidence_time_000_000_005,
  TimeConfidence_time_000_000_002,
  TimeConfidence_time_000_000_001,
  TimeConfidence_time_000_000_000_5,
  TimeConfidence_time_000_000_000_2,
  TimeConfidence_time_000_000_000_1,
  TimeConfidence_time_000_000_000_05,
  TimeConfidence_time_000_000_000_02,
  TimeConfidence_time_000_000_000_01,
  TimeConfidence_time_000_000_000_005,
  TimeConfidence_time_000_000_000_002,
  TimeConfidence_time_000_000_000_001,
  TimeConfidence_time_000_000_000_000_5,
  TimeConfidence_time_000_000_000_000_2,
  TimeConfidence_time_000_000_000_000_1,
  TimeConfidence_time_000_000_000_000_05,
  TimeConfidence_time_000_000_000_000_02,
  TimeConfidence_time_000_000_000_000_01,
} TimeConfidence;

extern const ASN1CType asn1_type_TimeConfidence[];

typedef enum PositionConfidence {
  PositionConfidence_unavailable,
  PositionConfidence_a500m,
  PositionConfidence_a200m,
  PositionConfidence_a100m,
  PositionConfidence_a50m,
  PositionConfidence_a20m,
  PositionConfidence_a10m,
  PositionConfidence_a5m,
  PositionConfidence_a2m,
  PositionConfidence_a1m,
  PositionConfidence_a50cm,
  PositionConfidence_a20cm,
  PositionConfidence_a10cm,
  PositionConfidence_a5cm,
  PositionConfidence_a2cm,
  PositionConfidence_a1cm,
} PositionConfidence;

extern const ASN1CType asn1_type_PositionConfidence[];

typedef enum ElevationConfidence {
  ElevationConfidence_unavailable,
  ElevationConfidence_elev_500_00,
  ElevationConfidence_elev_200_00,
  ElevationConfidence_elev_100_00,
  ElevationConfidence_elev_050_00,
  ElevationConfidence_elev_020_00,
  ElevationConfidence_elev_010_00,
  ElevationConfidence_elev_005_00,
  ElevationConfidence_elev_002_00,
  ElevationConfidence_elev_001_00,
  ElevationConfidence_elev_000_50,
  ElevationConfidence_elev_000_20,
  ElevationConfidence_elev_000_10,
  ElevationConfidence_elev_000_05,
  ElevationConfidence_elev_000_02,
  ElevationConfidence_elev_000_01,
} ElevationConfidence;

extern const ASN1CType asn1_type_ElevationConfidence[];

typedef struct PositionConfidenceSet {
  PositionConfidence pos;
  ElevationConfidence elevation;
} PositionConfidenceSet;


extern const ASN1CType asn1_type_PositionConfidenceSet[];

typedef enum HeadingConfidence {
  HeadingConfidence_unavailable,
  HeadingConfidence_prec10deg,
  HeadingConfidence_prec05deg,
  HeadingConfidence_prec01deg,
  HeadingConfidence_prec0_1deg,
  HeadingConfidence_prec0_05deg,
  HeadingConfidence_prec0_01deg,
  HeadingConfidence_prec0_0125deg,
} HeadingConfidence;

extern const ASN1CType asn1_type_HeadingConfidence[];

typedef enum ThrottleConfidence {
  ThrottleConfidence_unavailable,
  ThrottleConfidence_prec10percent,
  ThrottleConfidence_prec1percent,
  ThrottleConfidence_prec0_5percent,
} ThrottleConfidence;

extern const ASN1CType asn1_type_ThrottleConfidence[];

typedef struct SpeedandHeadingandThrottleConfidence {
  HeadingConfidence heading;
  SpeedConfidence speed;
  ThrottleConfidence throttle;
} SpeedandHeadingandThrottleConfidence;


extern const ASN1CType asn1_type_SpeedandHeadingandThrottleConfidence[];

typedef struct FullPositionVector {
  BOOL utcTime_option;
  DDateTime utcTime;
  Longitude Long;
  Latitude lat;
  BOOL elevation_option;
  Elevation elevation;
  BOOL heading_option;
  Heading heading;
  BOOL speed_option;
  TransmissionAndSpeed speed;
  BOOL posAccuracy_option;
  PositionalAccuracy posAccuracy;
  BOOL timeConfidence_option;
  TimeConfidence timeConfidence;
  BOOL posConfidence_option;
  PositionConfidenceSet posConfidence;
  BOOL speedConfidence_option;
  SpeedandHeadingandThrottleConfidence speedConfidence;
} FullPositionVector;


extern const ASN1CType asn1_type_FullPositionVector[];

typedef ASN1String FurtherInfoID;

extern const ASN1CType asn1_type_FurtherInfoID[];

typedef struct RegionalExtension_10 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_10;


extern const ASN1CType asn1_type_RegionalExtension_10[];

typedef struct RoadSideAlert_2 {
  RegionalExtension_10 *tab;
  size_t count;
} RoadSideAlert_2;

extern const ASN1CType asn1_type_RoadSideAlert_2[];

typedef struct RoadSideAlert {
  MsgCount msgCnt;
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  ITIScodes typeEvent;
  BOOL description_option;
  RoadSideAlert_1 description;
  BOOL priority_option;
  Priority priority;
  BOOL heading_option;
  HeadingSlice heading;
  BOOL extent_option;
  Extent extent;
  BOOL position_option;
  FullPositionVector position;
  BOOL furtherInfoID_option;
  FurtherInfoID furtherInfoID;
  BOOL regional_option;
  RoadSideAlert_2 regional;
} RoadSideAlert;


extern const ASN1CType asn1_type_RoadSideAlert[];

typedef enum ResponseType {
  ResponseType_notInUseOrNotEquipped,
  ResponseType_emergency,
  ResponseType_nonEmergency,
  ResponseType_pursuit,
  ResponseType_stationary,
  ResponseType_slowMoving,
  ResponseType_stopAndGoMovement,
} ResponseType;

extern const ASN1CType asn1_type_ResponseType[];

typedef int SSPindex;

extern const ASN1CType asn1_type_SSPindex[];

typedef enum SirenInUse {
  SirenInUse_unavailable,
  SirenInUse_notInUse,
  SirenInUse_inUse,
  SirenInUse_reserved,
} SirenInUse;

extern const ASN1CType asn1_type_SirenInUse[];

typedef enum LightbarInUse {
  LightbarInUse_unavailable,
  LightbarInUse_notInUse,
  LightbarInUse_inUse,
  LightbarInUse_yellowCautionLights,
  LightbarInUse_schooldBusLights,
  LightbarInUse_arrowSignsActive,
  LightbarInUse_slowMovingVehicle,
  LightbarInUse_freqStops,
} LightbarInUse;

extern const ASN1CType asn1_type_LightbarInUse[];

typedef enum MultiVehicleResponse {
  MultiVehicleResponse_unavailable,
  MultiVehicleResponse_singleVehicle,
  MultiVehicleResponse_multiVehicle,
  MultiVehicleResponse_reserved,
} MultiVehicleResponse;

extern const ASN1CType asn1_type_MultiVehicleResponse[];

typedef ASN1BitString PrivilegedEventFlags;

extern const ASN1CType asn1_type_PrivilegedEventFlags[];

typedef struct PrivilegedEvents {
  SSPindex sspRights;
  PrivilegedEventFlags event;
} PrivilegedEvents;


extern const ASN1CType asn1_type_PrivilegedEvents[];

typedef struct EmergencyDetails {
  SSPindex sspRights;
  SirenInUse sirenUse;
  LightbarInUse lightsUse;
  MultiVehicleResponse multi;
  BOOL events_option;
  PrivilegedEvents events;
  BOOL responseType_option;
  ResponseType responseType;
} EmergencyDetails;


extern const ASN1CType asn1_type_EmergencyDetails[];

typedef int VehicleMass;

extern const ASN1CType asn1_type_VehicleMass[];

typedef enum VehicleType {
  VehicleType_none,
  VehicleType_unknown,
  VehicleType_special,
  VehicleType_moto,
  VehicleType_car,
  VehicleType_carOther,
  VehicleType_bus,
  VehicleType_axleCnt2,
  VehicleType_axleCnt3,
  VehicleType_axleCnt4,
  VehicleType_axleCnt4Trailer,
  VehicleType_axleCnt5Trailer,
  VehicleType_axleCnt6Trailer,
  VehicleType_axleCnt5MultiTrailer,
  VehicleType_axleCnt6MultiTrailer,
  VehicleType_axleCnt7MultiTrailer,
} VehicleType;

extern const ASN1CType asn1_type_VehicleType[];

typedef enum VehicleGroupAffected {
  VehicleGroupAffected_all_vehicles,
  VehicleGroupAffected_bicycles,
  VehicleGroupAffected_motorcycles,
  VehicleGroupAffected_cars,
  VehicleGroupAffected_light_vehicles,
  VehicleGroupAffected_cars_and_light_vehicles,
  VehicleGroupAffected_cars_with_trailers,
  VehicleGroupAffected_cars_with_recreational_trailers,
  VehicleGroupAffected_vehicles_with_trailers,
  VehicleGroupAffected_heavy_vehicles,
  VehicleGroupAffected_trucks,
  VehicleGroupAffected_buses,
  VehicleGroupAffected_articulated_buses,
  VehicleGroupAffected_school_buses,
  VehicleGroupAffected_vehicles_with_semi_trailers,
  VehicleGroupAffected_vehicles_with_double_trailers,
  VehicleGroupAffected_high_profile_vehicles,
  VehicleGroupAffected_wide_vehicles,
  VehicleGroupAffected_long_vehicles,
  VehicleGroupAffected_hazardous_loads,
  VehicleGroupAffected_exceptional_loads,
  VehicleGroupAffected_abnormal_loads,
  VehicleGroupAffected_convoys,
  VehicleGroupAffected_maintenance_vehicles,
  VehicleGroupAffected_delivery_vehicles,
  VehicleGroupAffected_vehicles_with_even_numbered_license_plates,
  VehicleGroupAffected_vehicles_with_odd_numbered_license_plates,
  VehicleGroupAffected_vehicles_with_parking_permits,
  VehicleGroupAffected_vehicles_with_catalytic_converters,
  VehicleGroupAffected_vehicles_without_catalytic_converters,
  VehicleGroupAffected_gas_powered_vehicles,
  VehicleGroupAffected_diesel_powered_vehicles,
  VehicleGroupAffected_lPG_vehicles,
  VehicleGroupAffected_military_convoys,
  VehicleGroupAffected_military_vehicles,
} VehicleGroupAffected;

extern const ASN1CType asn1_type_VehicleGroupAffected[];

typedef enum IncidentResponseEquipment {
  IncidentResponseEquipment_ground_fire_suppression,
  IncidentResponseEquipment_heavy_ground_equipment,
  IncidentResponseEquipment_aircraft,
  IncidentResponseEquipment_marine_equipment,
  IncidentResponseEquipment_support_equipment,
  IncidentResponseEquipment_medical_rescue_unit,
  IncidentResponseEquipment_other,
  IncidentResponseEquipment_ground_fire_suppression_other,
  IncidentResponseEquipment_engine,
  IncidentResponseEquipment_truck_or_aerial,
  IncidentResponseEquipment_quint,
  IncidentResponseEquipment_tanker_pumper_combination,
  IncidentResponseEquipment_brush_truck,
  IncidentResponseEquipment_aircraft_rescue_firefighting,
  IncidentResponseEquipment_heavy_ground_equipment_other,
  IncidentResponseEquipment_dozer_or_plow,
  IncidentResponseEquipment_tractor,
  IncidentResponseEquipment_tanker_or_tender,
  IncidentResponseEquipment_aircraft_other,
  IncidentResponseEquipment_aircraft_fixed_wing_tanker,
  IncidentResponseEquipment_helitanker,
  IncidentResponseEquipment_helicopter,
  IncidentResponseEquipment_marine_equipment_other,
  IncidentResponseEquipment_fire_boat_with_pump,
  IncidentResponseEquipment_boat_no_pump,
  IncidentResponseEquipment_support_apparatus_other,
  IncidentResponseEquipment_breathing_apparatus_support,
  IncidentResponseEquipment_light_and_air_unit,
  IncidentResponseEquipment_medical_rescue_unit_other,
  IncidentResponseEquipment_rescue_unit,
  IncidentResponseEquipment_urban_search_rescue_unit,
  IncidentResponseEquipment_high_angle_rescue,
  IncidentResponseEquipment_crash_fire_rescue,
  IncidentResponseEquipment_bLS_unit,
  IncidentResponseEquipment_aLS_unit,
  IncidentResponseEquipment_mobile_command_post,
  IncidentResponseEquipment_chief_officer_car,
  IncidentResponseEquipment_hAZMAT_unit,
  IncidentResponseEquipment_type_i_hand_crew,
  IncidentResponseEquipment_type_ii_hand_crew,
  IncidentResponseEquipment_privately_owned_vehicle,
  IncidentResponseEquipment_other_apparatus_resource,
  IncidentResponseEquipment_ambulance,
  IncidentResponseEquipment_bomb_squad_van,
  IncidentResponseEquipment_combine_harvester,
  IncidentResponseEquipment_construction_vehicle,
  IncidentResponseEquipment_farm_tractor,
  IncidentResponseEquipment_grass_cutting_machines,
  IncidentResponseEquipment_hAZMAT_containment_tow,
  IncidentResponseEquipment_heavy_tow,
  IncidentResponseEquipment_light_tow,
  IncidentResponseEquipment_flatbed_tow,
  IncidentResponseEquipment_hedge_cutting_machines,
  IncidentResponseEquipment_mobile_crane,
  IncidentResponseEquipment_refuse_collection_vehicle,
  IncidentResponseEquipment_resurfacing_vehicle,
  IncidentResponseEquipment_road_sweeper,
  IncidentResponseEquipment_roadside_litter_collection_crews,
  IncidentResponseEquipment_salvage_vehicle,
  IncidentResponseEquipment_sand_truck,
  IncidentResponseEquipment_snowplow,
  IncidentResponseEquipment_steam_roller,
  IncidentResponseEquipment_swat_team_van,
  IncidentResponseEquipment_track_laying_vehicle,
  IncidentResponseEquipment_unknown_vehicle,
  IncidentResponseEquipment_white_lining_vehicle,
  IncidentResponseEquipment_dump_truck,
  IncidentResponseEquipment_supervisor_vehicle,
  IncidentResponseEquipment_snow_blower,
  IncidentResponseEquipment_rotary_snow_blower,
  IncidentResponseEquipment_road_grader,
  IncidentResponseEquipment_steam_truck,
} IncidentResponseEquipment;

extern const ASN1CType asn1_type_IncidentResponseEquipment[];

typedef enum ResponderGroupAffected {
  ResponderGroupAffected_emergency_vehicle_units,
  ResponderGroupAffected_federal_law_enforcement_units,
  ResponderGroupAffected_state_police_units,
  ResponderGroupAffected_county_police_units,
  ResponderGroupAffected_local_police_units,
  ResponderGroupAffected_ambulance_units,
  ResponderGroupAffected_rescue_units,
  ResponderGroupAffected_fire_units,
  ResponderGroupAffected_hAZMAT_units,
  ResponderGroupAffected_light_tow_unit,
  ResponderGroupAffected_heavy_tow_unit,
  ResponderGroupAffected_freeway_service_patrols,
  ResponderGroupAffected_transportation_response_units,
  ResponderGroupAffected_private_contractor_response_units,
} ResponderGroupAffected;

extern const ASN1CType asn1_type_ResponderGroupAffected[];

typedef struct RegionalExtension_3 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_3;


extern const ASN1CType asn1_type_RegionalExtension_3[];

typedef struct EmergencyVehicleAlert_1 {
  RegionalExtension_3 *tab;
  size_t count;
} EmergencyVehicleAlert_1;

extern const ASN1CType asn1_type_EmergencyVehicleAlert_1[];

typedef struct EmergencyVehicleAlert {
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  BOOL id_option;
  TemporaryID id;
  RoadSideAlert rsaMsg;
  BOOL responseType_option;
  ResponseType responseType;
  BOOL details_option;
  EmergencyDetails details;
  BOOL mass_option;
  VehicleMass mass;
  BOOL basicType_option;
  VehicleType basicType;
  BOOL vehicleType_option;
  VehicleGroupAffected vehicleType;
  BOOL responseEquip_option;
  IncidentResponseEquipment responseEquip;
  BOOL responderType_option;
  ResponderGroupAffected responderType;
  BOOL regional_option;
  EmergencyVehicleAlert_1 regional;
} EmergencyVehicleAlert;


extern const ASN1CType asn1_type_EmergencyVehicleAlert[];

typedef ASN1BitString GNSSstatus;

extern const ASN1CType asn1_type_GNSSstatus[];

typedef int OffsetLL_B18;

extern const ASN1CType asn1_type_OffsetLL_B18[];

typedef int VertOffset_B12;

extern const ASN1CType asn1_type_VertOffset_B12[];

typedef int TimeOffset;

extern const ASN1CType asn1_type_TimeOffset[];

typedef int CoarseHeading;

extern const ASN1CType asn1_type_CoarseHeading[];

typedef struct PathHistoryPoint {
  OffsetLL_B18 latOffset;
  OffsetLL_B18 lonOffset;
  VertOffset_B12 elevationOffset;
  TimeOffset timeOffset;
  BOOL speed_option;
  Speed speed;
  BOOL posAccuracy_option;
  PositionalAccuracy posAccuracy;
  BOOL heading_option;
  CoarseHeading heading;
} PathHistoryPoint;


extern const ASN1CType asn1_type_PathHistoryPoint[];

typedef struct PathHistoryPointList {
  PathHistoryPoint *tab;
  size_t count;
} PathHistoryPointList;

extern const ASN1CType asn1_type_PathHistoryPointList[];

typedef struct PathHistory {
  BOOL initialPosition_option;
  FullPositionVector initialPosition;
  BOOL currGNSSstatus_option;
  GNSSstatus currGNSSstatus;
  PathHistoryPointList crumbData;
} PathHistory;


extern const ASN1CType asn1_type_PathHistory[];

typedef int RadiusOfCurvature;

extern const ASN1CType asn1_type_RadiusOfCurvature[];

typedef int Confidence;

extern const ASN1CType asn1_type_Confidence[];

typedef struct PathPrediction {
  RadiusOfCurvature radiusOfCurve;
  Confidence confidence;
} PathPrediction;


extern const ASN1CType asn1_type_PathPrediction[];

typedef enum {
  ApproachOrLane_approach,
  ApproachOrLane_lane,
} ApproachOrLane_choice;

typedef struct ApproachOrLane {
  ApproachOrLane_choice choice;
  union {
    ApproachID approach;
    LaneID lane;
  } u;
} ApproachOrLane;

extern const ASN1CType asn1_type_ApproachOrLane[];

typedef ASN1BitString VehicleEventFlags;

extern const ASN1CType asn1_type_VehicleEventFlags[];

typedef struct RegionalExtension_4 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_4;


extern const ASN1CType asn1_type_RegionalExtension_4[];

typedef struct IntersectionCollision_1 {
  RegionalExtension_4 *tab;
  size_t count;
} IntersectionCollision_1;

extern const ASN1CType asn1_type_IntersectionCollision_1[];

typedef struct IntersectionCollision {
  MsgCount msgCnt;
  TemporaryID id;
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  BOOL partOne_option;
  BSMcoreData partOne;
  BOOL path_option;
  PathHistory path;
  BOOL pathPrediction_option;
  PathPrediction pathPrediction;
  IntersectionReferenceID intersectionID;
  ApproachOrLane laneNumber;
  VehicleEventFlags eventFlag;
  BOOL regional_option;
  IntersectionCollision_1 regional;
} IntersectionCollision;


extern const ASN1CType asn1_type_IntersectionCollision[];

typedef enum NMEA_Revision {
  NMEA_Revision_unknown,
  NMEA_Revision_reserved,
  NMEA_Revision_rev1,
  NMEA_Revision_rev2,
  NMEA_Revision_rev3,
  NMEA_Revision_rev4,
  NMEA_Revision_rev5,
} NMEA_Revision;

extern const ASN1CType asn1_type_NMEA_Revision[];

typedef int NMEA_MsgType;

extern const ASN1CType asn1_type_NMEA_MsgType[];

typedef int ObjectCount;

extern const ASN1CType asn1_type_ObjectCount[];

typedef ASN1String NMEA_Payload;

extern const ASN1CType asn1_type_NMEA_Payload[];

typedef struct RegionalExtension_6 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_6;


extern const ASN1CType asn1_type_RegionalExtension_6[];

typedef struct NMEAcorrections_1 {
  RegionalExtension_6 *tab;
  size_t count;
} NMEAcorrections_1;

extern const ASN1CType asn1_type_NMEAcorrections_1[];

typedef struct NMEAcorrections {
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  BOOL rev_option;
  NMEA_Revision rev;
  BOOL msg_option;
  NMEA_MsgType msg;
  BOOL wdCount_option;
  ObjectCount wdCount;
  NMEA_Payload payload;
  BOOL regional_option;
  NMEAcorrections_1 regional;
} NMEAcorrections;


extern const ASN1CType asn1_type_NMEAcorrections[];

typedef struct Sample {
  int sampleStart;
  int sampleEnd;
} Sample;


extern const ASN1CType asn1_type_Sample[];

typedef int TermTime;

extern const ASN1CType asn1_type_TermTime[];

typedef int TermDistance;

extern const ASN1CType asn1_type_TermDistance[];

typedef enum {
  ProbeDataManagement_1_termtime,
  ProbeDataManagement_1_termDistance,
} ProbeDataManagement_1_choice;

typedef struct ProbeDataManagement_1 {
  ProbeDataManagement_1_choice choice;
  union {
    TermTime termtime;
    TermDistance termDistance;
  } u;
} ProbeDataManagement_1;

extern const ASN1CType asn1_type_ProbeDataManagement_1[];

typedef int GrossSpeed;

extern const ASN1CType asn1_type_GrossSpeed[];

typedef int SecondOfTime;

extern const ASN1CType asn1_type_SecondOfTime[];

typedef struct SnapshotTime {
  GrossSpeed speed1;
  SecondOfTime time1;
  GrossSpeed speed2;
  SecondOfTime time2;
} SnapshotTime;


extern const ASN1CType asn1_type_SnapshotTime[];

typedef int GrossDistance;

extern const ASN1CType asn1_type_GrossDistance[];

typedef struct SnapshotDistance {
  GrossDistance distance1;
  GrossSpeed speed1;
  GrossDistance distance2;
  GrossSpeed speed2;
} SnapshotDistance;


extern const ASN1CType asn1_type_SnapshotDistance[];

typedef enum {
  ProbeDataManagement_2_snapshotTime,
  ProbeDataManagement_2_snapshotDistance,
} ProbeDataManagement_2_choice;

typedef struct ProbeDataManagement_2 {
  ProbeDataManagement_2_choice choice;
  union {
    SnapshotTime snapshotTime;
    SnapshotDistance snapshotDistance;
  } u;
} ProbeDataManagement_2;

extern const ASN1CType asn1_type_ProbeDataManagement_2[];

typedef enum VehicleStatusDeviceTypeTag {
  VehicleStatusDeviceTypeTag_unknown,
  VehicleStatusDeviceTypeTag_lights,
  VehicleStatusDeviceTypeTag_wipers,
  VehicleStatusDeviceTypeTag_brakes,
  VehicleStatusDeviceTypeTag_stab,
  VehicleStatusDeviceTypeTag_trac,
  VehicleStatusDeviceTypeTag_albs,
  VehicleStatusDeviceTypeTag_sunS,
  VehicleStatusDeviceTypeTag_rainS,
  VehicleStatusDeviceTypeTag_airTemp,
  VehicleStatusDeviceTypeTag_steering,
  VehicleStatusDeviceTypeTag_vertAccelThres,
  VehicleStatusDeviceTypeTag_vertAccel,
  VehicleStatusDeviceTypeTag_hozAccelLong,
  VehicleStatusDeviceTypeTag_hozAccelLat,
  VehicleStatusDeviceTypeTag_hozAccelCon,
  VehicleStatusDeviceTypeTag_accel4way,
  VehicleStatusDeviceTypeTag_confidenceSet,
  VehicleStatusDeviceTypeTag_obDist,
  VehicleStatusDeviceTypeTag_obDirect,
  VehicleStatusDeviceTypeTag_yaw,
  VehicleStatusDeviceTypeTag_yawRateCon,
  VehicleStatusDeviceTypeTag_dateTime,
  VehicleStatusDeviceTypeTag_fullPos,
  VehicleStatusDeviceTypeTag_position2D,
  VehicleStatusDeviceTypeTag_position3D,
  VehicleStatusDeviceTypeTag_vehicle,
  VehicleStatusDeviceTypeTag_speedHeadC,
  VehicleStatusDeviceTypeTag_speedC,
} VehicleStatusDeviceTypeTag;

extern const ASN1CType asn1_type_VehicleStatusDeviceTypeTag[];

typedef struct VehicleStatusRequest {
  VehicleStatusDeviceTypeTag dataType;
  BOOL subType_option;
  int subType;
  BOOL sendOnLessThenValue_option;
  int sendOnLessThenValue;
  BOOL sendOnMoreThenValue_option;
  int sendOnMoreThenValue;
  BOOL sendAll_option;
  BOOL sendAll;
} VehicleStatusRequest;


extern const ASN1CType asn1_type_VehicleStatusRequest[];

typedef struct VehicleStatusRequestList {
  VehicleStatusRequest *tab;
  size_t count;
} VehicleStatusRequestList;

extern const ASN1CType asn1_type_VehicleStatusRequestList[];

typedef struct RegionalExtension_8 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_8;


extern const ASN1CType asn1_type_RegionalExtension_8[];

typedef struct ProbeDataManagement_3 {
  RegionalExtension_8 *tab;
  size_t count;
} ProbeDataManagement_3;

extern const ASN1CType asn1_type_ProbeDataManagement_3[];

typedef struct ProbeDataManagement {
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  Sample sample;
  HeadingSlice directions;
  ProbeDataManagement_1 term;
  ProbeDataManagement_2 snapshot;
  SecondOfTime txInterval;
  BOOL dataElements_option;
  VehicleStatusRequestList dataElements;
  BOOL regional_option;
  ProbeDataManagement_3 regional;
} ProbeDataManagement;


extern const ASN1CType asn1_type_ProbeDataManagement[];

typedef int ProbeSegmentNumber;

extern const ASN1CType asn1_type_ProbeSegmentNumber[];

typedef ASN1String VINstring;

extern const ASN1CType asn1_type_VINstring[];

typedef unsigned int StationID;

extern const ASN1CType asn1_type_StationID[];

typedef enum {
  VehicleID_entityID,
  VehicleID_stationID,
} VehicleID_choice;

typedef struct VehicleID {
  VehicleID_choice choice;
  union {
    TemporaryID entityID;
    StationID stationID;
  } u;
} VehicleID;

extern const ASN1CType asn1_type_VehicleID[];

typedef enum {
  VehicleIdent_1_vGroup,
  VehicleIdent_1_rGroup,
  VehicleIdent_1_rEquip,
} VehicleIdent_1_choice;

typedef struct VehicleIdent_1 {
  VehicleIdent_1_choice choice;
  union {
    VehicleGroupAffected vGroup;
    ResponderGroupAffected rGroup;
    IncidentResponseEquipment rEquip;
  } u;
} VehicleIdent_1;

extern const ASN1CType asn1_type_VehicleIdent_1[];

typedef struct VehicleIdent {
  BOOL name_option;
  DescriptiveName name;
  BOOL vin_option;
  VINstring vin;
  BOOL ownerCode_option;
  ASN1String ownerCode;
  BOOL id_option;
  VehicleID id;
  BOOL vehicleType_option;
  VehicleType vehicleType;
  BOOL vehicleClass_option;
  VehicleIdent_1 vehicleClass;
} VehicleIdent;


extern const ASN1CType asn1_type_VehicleIdent[];

typedef int BasicVehicleClass;

extern const ASN1CType asn1_type_BasicVehicleClass[];

typedef enum BasicVehicleRole {
  BasicVehicleRole_basicVehicle,
  BasicVehicleRole_publicTransport,
  BasicVehicleRole_specialTransport,
  BasicVehicleRole_dangerousGoods,
  BasicVehicleRole_roadWork,
  BasicVehicleRole_roadRescue,
  BasicVehicleRole_emergency,
  BasicVehicleRole_safetyCar,
  BasicVehicleRole_none_unknown,
  BasicVehicleRole_truck,
  BasicVehicleRole_motorcycle,
  BasicVehicleRole_roadSideSource,
  BasicVehicleRole_police,
  BasicVehicleRole_fire,
  BasicVehicleRole_ambulance,
  BasicVehicleRole_dot,
  BasicVehicleRole_transit,
  BasicVehicleRole_slowMoving,
  BasicVehicleRole_stopNgo,
  BasicVehicleRole_cyclist,
  BasicVehicleRole_pedestrian,
  BasicVehicleRole_nonMotorized,
  BasicVehicleRole_military,
} BasicVehicleRole;

extern const ASN1CType asn1_type_BasicVehicleRole[];

typedef int Iso3833VehicleType;

extern const ASN1CType asn1_type_Iso3833VehicleType[];

typedef int FuelType;

extern const ASN1CType asn1_type_FuelType[];

typedef struct RegionalExtension_60 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_60;


extern const ASN1CType asn1_type_RegionalExtension_60[];

typedef struct VehicleClassification_1 {
  RegionalExtension_60 *tab;
  size_t count;
} VehicleClassification_1;

extern const ASN1CType asn1_type_VehicleClassification_1[];

typedef struct VehicleClassification {
  BOOL keyType_option;
  BasicVehicleClass keyType;
  BOOL role_option;
  BasicVehicleRole role;
  BOOL iso3883_option;
  Iso3833VehicleType iso3883;
  BOOL hpmsType_option;
  VehicleType hpmsType;
  BOOL vehicleType_option;
  VehicleGroupAffected vehicleType;
  BOOL responseEquip_option;
  IncidentResponseEquipment responseEquip;
  BOOL responderType_option;
  ResponderGroupAffected responderType;
  BOOL fuelType_option;
  FuelType fuelType;
  BOOL regional_option;
  VehicleClassification_1 regional;
} VehicleClassification;


extern const ASN1CType asn1_type_VehicleClassification[];

typedef ASN1BitString ExteriorLights;

extern const ASN1CType asn1_type_ExteriorLights[];

typedef struct VehicleSafetyExtensions {
  BOOL events_option;
  VehicleEventFlags events;
  BOOL pathHistory_option;
  PathHistory pathHistory;
  BOOL pathPrediction_option;
  PathPrediction pathPrediction;
  BOOL lights_option;
  ExteriorLights lights;
} VehicleSafetyExtensions;


extern const ASN1CType asn1_type_VehicleSafetyExtensions[];

typedef enum WiperStatus {
  WiperStatus_unavailable,
  WiperStatus_off,
  WiperStatus_intermittent,
  WiperStatus_low,
  WiperStatus_high,
  WiperStatus_washerInUse,
  WiperStatus_automaticPresent,
} WiperStatus;

extern const ASN1CType asn1_type_WiperStatus[];

typedef int WiperRate;

extern const ASN1CType asn1_type_WiperRate[];

typedef struct WiperSet {
  WiperStatus statusFront;
  WiperRate rateFront;
  BOOL statusRear_option;
  WiperStatus statusRear;
  BOOL rateRear_option;
  WiperRate rateRear;
} WiperSet;


extern const ASN1CType asn1_type_WiperSet[];

typedef enum BrakeAppliedPressure {
  BrakeAppliedPressure_unavailable,
  BrakeAppliedPressure_minPressure,
  BrakeAppliedPressure_bkLvl_2,
  BrakeAppliedPressure_bkLvl_3,
  BrakeAppliedPressure_bkLvl_4,
  BrakeAppliedPressure_bkLvl_5,
  BrakeAppliedPressure_bkLvl_6,
  BrakeAppliedPressure_bkLvl_7,
  BrakeAppliedPressure_bkLvl_8,
  BrakeAppliedPressure_bkLvl_9,
  BrakeAppliedPressure_bkLvl_10,
  BrakeAppliedPressure_bkLvl_11,
  BrakeAppliedPressure_bkLvl_12,
  BrakeAppliedPressure_bkLvl_13,
  BrakeAppliedPressure_bkLvl_14,
  BrakeAppliedPressure_maxPressure,
} BrakeAppliedPressure;

extern const ASN1CType asn1_type_BrakeAppliedPressure[];

typedef int CoefficientOfFriction;

extern const ASN1CType asn1_type_CoefficientOfFriction[];

typedef int SunSensor;

extern const ASN1CType asn1_type_SunSensor[];

typedef enum RainSensor {
  RainSensor_none,
  RainSensor_lightMist,
  RainSensor_heavyMist,
  RainSensor_lightRainOrDrizzle,
  RainSensor_rain,
  RainSensor_moderateRain,
  RainSensor_heavyRain,
  RainSensor_heavyDownpour,
} RainSensor;

extern const ASN1CType asn1_type_RainSensor[];

typedef int AmbientAirTemperature;

extern const ASN1CType asn1_type_AmbientAirTemperature[];

typedef int AmbientAirPressure;

extern const ASN1CType asn1_type_AmbientAirPressure[];

typedef enum SteeringWheelAngleConfidence {
  SteeringWheelAngleConfidence_unavailable,
  SteeringWheelAngleConfidence_prec2deg,
  SteeringWheelAngleConfidence_prec1deg,
  SteeringWheelAngleConfidence_prec0_02deg,
} SteeringWheelAngleConfidence;

extern const ASN1CType asn1_type_SteeringWheelAngleConfidence[];

typedef int SteeringWheelAngleRateOfChange;

extern const ASN1CType asn1_type_SteeringWheelAngleRateOfChange[];

typedef int DrivingWheelAngle;

extern const ASN1CType asn1_type_DrivingWheelAngle[];

typedef struct VehicleStatus_1 {
  SteeringWheelAngle angle;
  BOOL confidence_option;
  SteeringWheelAngleConfidence confidence;
  BOOL rate_option;
  SteeringWheelAngleRateOfChange rate;
  BOOL wheels_option;
  DrivingWheelAngle wheels;
} VehicleStatus_1;


extern const ASN1CType asn1_type_VehicleStatus_1[];

typedef ASN1BitString VerticalAccelerationThreshold;

extern const ASN1CType asn1_type_VerticalAccelerationThreshold[];

typedef enum YawRateConfidence {
  YawRateConfidence_unavailable,
  YawRateConfidence_degSec_100_00,
  YawRateConfidence_degSec_010_00,
  YawRateConfidence_degSec_005_00,
  YawRateConfidence_degSec_001_00,
  YawRateConfidence_degSec_000_10,
  YawRateConfidence_degSec_000_05,
  YawRateConfidence_degSec_000_01,
} YawRateConfidence;

extern const ASN1CType asn1_type_YawRateConfidence[];

typedef enum AccelerationConfidence {
  AccelerationConfidence_unavailable,
  AccelerationConfidence_accl_100_00,
  AccelerationConfidence_accl_010_00,
  AccelerationConfidence_accl_005_00,
  AccelerationConfidence_accl_001_00,
  AccelerationConfidence_accl_000_10,
  AccelerationConfidence_accl_000_05,
  AccelerationConfidence_accl_000_01,
} AccelerationConfidence;

extern const ASN1CType asn1_type_AccelerationConfidence[];

typedef struct AccelSteerYawRateConfidence {
  YawRateConfidence yawRate;
  AccelerationConfidence acceleration;
  SteeringWheelAngleConfidence steeringWheelAngle;
} AccelSteerYawRateConfidence;


extern const ASN1CType asn1_type_AccelSteerYawRateConfidence[];

typedef struct ConfidenceSet {
  BOOL accelConfidence_option;
  AccelSteerYawRateConfidence accelConfidence;
  BOOL speedConfidence_option;
  SpeedandHeadingandThrottleConfidence speedConfidence;
  BOOL timeConfidence_option;
  TimeConfidence timeConfidence;
  BOOL posConfidence_option;
  PositionConfidenceSet posConfidence;
  BOOL steerConfidence_option;
  SteeringWheelAngleConfidence steerConfidence;
  BOOL headingConfidence_option;
  HeadingConfidence headingConfidence;
  BOOL throttleConfidence_option;
  ThrottleConfidence throttleConfidence;
} ConfidenceSet;


extern const ASN1CType asn1_type_ConfidenceSet[];

typedef struct VehicleStatus_2 {
  BOOL accel4way_option;
  AccelerationSet4Way accel4way;
  BOOL vertAccelThres_option;
  VerticalAccelerationThreshold vertAccelThres;
  BOOL yawRateCon_option;
  YawRateConfidence yawRateCon;
  BOOL hozAccelCon_option;
  AccelerationConfidence hozAccelCon;
  BOOL confidenceSet_option;
  ConfidenceSet confidenceSet;
} VehicleStatus_2;


extern const ASN1CType asn1_type_VehicleStatus_2[];

typedef int ObstacleDistance;

extern const ASN1CType asn1_type_ObstacleDistance[];

typedef struct VehicleStatus_3 {
  ObstacleDistance obDist;
  Angle obDirect;
  DDateTime dateTime;
} VehicleStatus_3;


extern const ASN1CType asn1_type_VehicleStatus_3[];

typedef int ThrottlePosition;

extern const ASN1CType asn1_type_ThrottlePosition[];

typedef int VehicleHeight;

extern const ASN1CType asn1_type_VehicleHeight[];

typedef int BumperHeight;

extern const ASN1CType asn1_type_BumperHeight[];

typedef struct BumperHeights {
  BumperHeight front;
  BumperHeight rear;
} BumperHeights;


extern const ASN1CType asn1_type_BumperHeights[];

typedef int TrailerWeight;

extern const ASN1CType asn1_type_TrailerWeight[];

typedef struct VehicleStatus_4 {
  VehicleHeight height;
  BumperHeights bumpers;
  VehicleMass mass;
  TrailerWeight trailerWeight;
  VehicleType type;
} VehicleStatus_4;


extern const ASN1CType asn1_type_VehicleStatus_4[];

typedef int TireLocation;

extern const ASN1CType asn1_type_TireLocation[];

typedef int TirePressure;

extern const ASN1CType asn1_type_TirePressure[];

typedef int TireTemp;

extern const ASN1CType asn1_type_TireTemp[];

typedef enum WheelSensorStatus {
  WheelSensorStatus_off,
  WheelSensorStatus_on,
  WheelSensorStatus_notDefined,
  WheelSensorStatus_notSupported,
} WheelSensorStatus;

extern const ASN1CType asn1_type_WheelSensorStatus[];

typedef enum WheelEndElectFault {
  WheelEndElectFault_isOk,
  WheelEndElectFault_isNotDefined,
  WheelEndElectFault_isError,
  WheelEndElectFault_isNotSupported,
} WheelEndElectFault;

extern const ASN1CType asn1_type_WheelEndElectFault[];

typedef int TireLeakageRate;

extern const ASN1CType asn1_type_TireLeakageRate[];

typedef enum TirePressureThresholdDetection {
  TirePressureThresholdDetection_noData,
  TirePressureThresholdDetection_overPressure,
  TirePressureThresholdDetection_noWarningPressure,
  TirePressureThresholdDetection_underPressure,
  TirePressureThresholdDetection_extremeUnderPressure,
  TirePressureThresholdDetection_undefined,
  TirePressureThresholdDetection_errorIndicator,
  TirePressureThresholdDetection_notAvailable,
} TirePressureThresholdDetection;

extern const ASN1CType asn1_type_TirePressureThresholdDetection[];

typedef struct TireData {
  BOOL location_option;
  TireLocation location;
  BOOL pressure_option;
  TirePressure pressure;
  BOOL temp_option;
  TireTemp temp;
  BOOL wheelSensorStatus_option;
  WheelSensorStatus wheelSensorStatus;
  BOOL wheelEndElectFault_option;
  WheelEndElectFault wheelEndElectFault;
  BOOL leakageRate_option;
  TireLeakageRate leakageRate;
  BOOL detection_option;
  TirePressureThresholdDetection detection;
} TireData;


extern const ASN1CType asn1_type_TireData[];

typedef struct TireDataList {
  TireData *tab;
  size_t count;
} TireDataList;

extern const ASN1CType asn1_type_TireDataList[];

typedef int AxleLocation;

extern const ASN1CType asn1_type_AxleLocation[];

typedef int AxleWeight;

extern const ASN1CType asn1_type_AxleWeight[];

typedef struct AxleWeightSet {
  BOOL location_option;
  AxleLocation location;
  BOOL weight_option;
  AxleWeight weight;
} AxleWeightSet;


extern const ASN1CType asn1_type_AxleWeightSet[];

typedef struct AxleWeightList {
  AxleWeightSet *tab;
  size_t count;
} AxleWeightList;

extern const ASN1CType asn1_type_AxleWeightList[];

typedef int CargoWeight;

extern const ASN1CType asn1_type_CargoWeight[];

typedef int SteeringAxleTemperature;

extern const ASN1CType asn1_type_SteeringAxleTemperature[];

typedef int DriveAxleLocation;

extern const ASN1CType asn1_type_DriveAxleLocation[];

typedef int DriveAxleLiftAirPressure;

extern const ASN1CType asn1_type_DriveAxleLiftAirPressure[];

typedef int DriveAxleTemperature;

extern const ASN1CType asn1_type_DriveAxleTemperature[];

typedef int DriveAxleLubePressure;

extern const ASN1CType asn1_type_DriveAxleLubePressure[];

typedef int SteeringAxleLubePressure;

extern const ASN1CType asn1_type_SteeringAxleLubePressure[];

typedef struct J1939data {
  BOOL tires_option;
  TireDataList tires;
  BOOL axles_option;
  AxleWeightList axles;
  BOOL trailerWeight_option;
  TrailerWeight trailerWeight;
  BOOL cargoWeight_option;
  CargoWeight cargoWeight;
  BOOL steeringAxleTemperature_option;
  SteeringAxleTemperature steeringAxleTemperature;
  BOOL driveAxleLocation_option;
  DriveAxleLocation driveAxleLocation;
  BOOL driveAxleLiftAirPressure_option;
  DriveAxleLiftAirPressure driveAxleLiftAirPressure;
  BOOL driveAxleTemperature_option;
  DriveAxleTemperature driveAxleTemperature;
  BOOL driveAxleLubePressure_option;
  DriveAxleLubePressure driveAxleLubePressure;
  BOOL steeringAxleLubePressure_option;
  SteeringAxleLubePressure steeringAxleLubePressure;
} J1939data;


extern const ASN1CType asn1_type_J1939data[];

typedef enum EssPrecipYesNo {
  EssPrecipYesNo_precip,
  EssPrecipYesNo_noPrecip,
  EssPrecipYesNo_error,
} EssPrecipYesNo;

extern const ASN1CType asn1_type_EssPrecipYesNo[];

typedef int EssPrecipRate;

extern const ASN1CType asn1_type_EssPrecipRate[];

typedef enum EssPrecipSituation {
  EssPrecipSituation_other,
  EssPrecipSituation_unknown,
  EssPrecipSituation_noPrecipitation,
  EssPrecipSituation_unidentifiedSlight,
  EssPrecipSituation_unidentifiedModerate,
  EssPrecipSituation_unidentifiedHeavy,
  EssPrecipSituation_snowSlight,
  EssPrecipSituation_snowModerate,
  EssPrecipSituation_snowHeavy,
  EssPrecipSituation_rainSlight,
  EssPrecipSituation_rainModerate,
  EssPrecipSituation_rainHeavy,
  EssPrecipSituation_frozenPrecipitationSlight,
  EssPrecipSituation_frozenPrecipitationModerate,
  EssPrecipSituation_frozenPrecipitationHeavy,
} EssPrecipSituation;

extern const ASN1CType asn1_type_EssPrecipSituation[];

typedef int EssSolarRadiation;

extern const ASN1CType asn1_type_EssSolarRadiation[];

typedef int EssMobileFriction;

extern const ASN1CType asn1_type_EssMobileFriction[];

typedef struct VehicleStatus_5 {
  EssPrecipYesNo isRaining;
  BOOL rainRate_option;
  EssPrecipRate rainRate;
  BOOL precipSituation_option;
  EssPrecipSituation precipSituation;
  BOOL solarRadiation_option;
  EssSolarRadiation solarRadiation;
  BOOL friction_option;
  EssMobileFriction friction;
} VehicleStatus_5;


extern const ASN1CType asn1_type_VehicleStatus_5[];

typedef struct VehicleStatus {
  BOOL lights_option;
  ExteriorLights lights;
  BOOL lightBar_option;
  LightbarInUse lightBar;
  BOOL wipers_option;
  WiperSet wipers;
  BOOL brakeStatus_option;
  BrakeSystemStatus brakeStatus;
  BOOL brakePressure_option;
  BrakeAppliedPressure brakePressure;
  BOOL roadFriction_option;
  CoefficientOfFriction roadFriction;
  BOOL sunData_option;
  SunSensor sunData;
  BOOL rainData_option;
  RainSensor rainData;
  BOOL airTemp_option;
  AmbientAirTemperature airTemp;
  BOOL airPres_option;
  AmbientAirPressure airPres;
  BOOL steering_option;
  VehicleStatus_1 steering;
  BOOL accelSets_option;
  VehicleStatus_2 accelSets;
  BOOL object_option;
  VehicleStatus_3 object;
  BOOL fullPos_option;
  FullPositionVector fullPos;
  BOOL throttlePos_option;
  ThrottlePosition throttlePos;
  BOOL speedHeadC_option;
  SpeedandHeadingandThrottleConfidence speedHeadC;
  BOOL speedC_option;
  SpeedConfidence speedC;
  BOOL vehicleData_option;
  VehicleStatus_4 vehicleData;
  BOOL vehicleIdent_option;
  VehicleIdent vehicleIdent;
  BOOL j1939data_option;
  J1939data j1939data;
  BOOL weatherReport_option;
  VehicleStatus_5 weatherReport;
  BOOL gnssStatus_option;
  GNSSstatus gnssStatus;
} VehicleStatus;


extern const ASN1CType asn1_type_VehicleStatus[];

typedef struct Snapshot {
  FullPositionVector thePosition;
  BOOL safetyExt_option;
  VehicleSafetyExtensions safetyExt;
  BOOL dataSet_option;
  VehicleStatus dataSet;
} Snapshot;


extern const ASN1CType asn1_type_Snapshot[];

typedef struct ProbeVehicleData_1 {
  Snapshot *tab;
  size_t count;
} ProbeVehicleData_1;

extern const ASN1CType asn1_type_ProbeVehicleData_1[];

typedef struct RegionalExtension_9 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_9;


extern const ASN1CType asn1_type_RegionalExtension_9[];

typedef struct ProbeVehicleData_2 {
  RegionalExtension_9 *tab;
  size_t count;
} ProbeVehicleData_2;

extern const ASN1CType asn1_type_ProbeVehicleData_2[];

typedef struct ProbeVehicleData {
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  BOOL segNum_option;
  ProbeSegmentNumber segNum;
  BOOL probeID_option;
  VehicleIdent probeID;
  FullPositionVector startVector;
  VehicleClassification vehicleType;
  ProbeVehicleData_1 snapshots;
  BOOL regional_option;
  ProbeVehicleData_2 regional;
} ProbeVehicleData;


extern const ASN1CType asn1_type_ProbeVehicleData[];

typedef enum RTCM_Revision {
  RTCM_Revision_unknown,
  RTCM_Revision_rtcmRev2,
  RTCM_Revision_rtcmRev3,
  RTCM_Revision_reserved,
} RTCM_Revision;

extern const ASN1CType asn1_type_RTCM_Revision[];

typedef int Offset_B09;

extern const ASN1CType asn1_type_Offset_B09[];

typedef struct AntennaOffsetSet {
  Offset_B12 antOffsetX;
  Offset_B09 antOffsetY;
  Offset_B10 antOffsetZ;
} AntennaOffsetSet;


extern const ASN1CType asn1_type_AntennaOffsetSet[];

typedef struct RTCMheader {
  GNSSstatus status;
  AntennaOffsetSet offsetSet;
} RTCMheader;


extern const ASN1CType asn1_type_RTCMheader[];

typedef ASN1String RTCMmessage;

extern const ASN1CType asn1_type_RTCMmessage[];

typedef struct RTCMmessageList {
  RTCMmessage *tab;
  size_t count;
} RTCMmessageList;

extern const ASN1CType asn1_type_RTCMmessageList[];

typedef struct RegionalExtension_11 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_11;


extern const ASN1CType asn1_type_RegionalExtension_11[];

typedef struct RTCMcorrections_1 {
  RegionalExtension_11 *tab;
  size_t count;
} RTCMcorrections_1;

extern const ASN1CType asn1_type_RTCMcorrections_1[];

typedef struct RTCMcorrections {
  MsgCount msgCnt;
  RTCM_Revision rev;
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  BOOL anchorPoint_option;
  FullPositionVector anchorPoint;
  BOOL rtcmHeader_option;
  RTCMheader rtcmHeader;
  RTCMmessageList msgs;
  BOOL regional_option;
  RTCMcorrections_1 regional;
} RTCMcorrections;


extern const ASN1CType asn1_type_RTCMcorrections[];

typedef int RequestID;

extern const ASN1CType asn1_type_RequestID[];

typedef enum PriorityRequestType {
  PriorityRequestType_priorityRequestTypeReserved,
  PriorityRequestType_priorityRequest,
  PriorityRequestType_priorityRequestUpdate,
  PriorityRequestType_priorityCancellation,
} PriorityRequestType;

extern const ASN1CType asn1_type_PriorityRequestType[];

typedef enum {
  IntersectionAccessPoint_lane,
  IntersectionAccessPoint_approach,
  IntersectionAccessPoint_connection,
} IntersectionAccessPoint_choice;

typedef struct IntersectionAccessPoint {
  IntersectionAccessPoint_choice choice;
  union {
    LaneID lane;
    ApproachID approach;
    LaneConnectionID connection;
  } u;
} IntersectionAccessPoint;

extern const ASN1CType asn1_type_IntersectionAccessPoint[];

typedef struct RegionalExtension_56 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_56;


extern const ASN1CType asn1_type_RegionalExtension_56[];

typedef struct SignalRequest_1 {
  RegionalExtension_56 *tab;
  size_t count;
} SignalRequest_1;

extern const ASN1CType asn1_type_SignalRequest_1[];

typedef struct SignalRequest {
  IntersectionReferenceID id;
  RequestID requestID;
  PriorityRequestType requestType;
  IntersectionAccessPoint inBoundLane;
  BOOL outBoundLane_option;
  IntersectionAccessPoint outBoundLane;
  BOOL regional_option;
  SignalRequest_1 regional;
} SignalRequest;


extern const ASN1CType asn1_type_SignalRequest[];

typedef struct RegionalExtension_55 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_55;


extern const ASN1CType asn1_type_RegionalExtension_55[];

typedef struct SignalRequestPackage_1 {
  RegionalExtension_55 *tab;
  size_t count;
} SignalRequestPackage_1;

extern const ASN1CType asn1_type_SignalRequestPackage_1[];

typedef struct SignalRequestPackage {
  SignalRequest request;
  BOOL minute_option;
  MinuteOfTheYear minute;
  BOOL second_option;
  DSecond second;
  BOOL duration_option;
  DSecond duration;
  BOOL regional_option;
  SignalRequestPackage_1 regional;
} SignalRequestPackage;


extern const ASN1CType asn1_type_SignalRequestPackage[];

typedef struct SignalRequestList {
  SignalRequestPackage *tab;
  size_t count;
} SignalRequestList;

extern const ASN1CType asn1_type_SignalRequestList[];

typedef enum RequestSubRole {
  RequestSubRole_requestSubRoleUnKnown,
  RequestSubRole_requestSubRole1,
  RequestSubRole_requestSubRole2,
  RequestSubRole_requestSubRole3,
  RequestSubRole_requestSubRole4,
  RequestSubRole_requestSubRole5,
  RequestSubRole_requestSubRole6,
  RequestSubRole_requestSubRole7,
  RequestSubRole_requestSubRole8,
  RequestSubRole_requestSubRole9,
  RequestSubRole_requestSubRole10,
  RequestSubRole_requestSubRole11,
  RequestSubRole_requestSubRole12,
  RequestSubRole_requestSubRole13,
  RequestSubRole_requestSubRole14,
  RequestSubRole_requestSubRoleReserved,
} RequestSubRole;

extern const ASN1CType asn1_type_RequestSubRole[];

typedef enum RequestImportanceLevel {
  RequestImportanceLevel_requestImportanceLevelUnKnown,
  RequestImportanceLevel_requestImportanceLevel1,
  RequestImportanceLevel_requestImportanceLevel2,
  RequestImportanceLevel_requestImportanceLevel3,
  RequestImportanceLevel_requestImportanceLevel4,
  RequestImportanceLevel_requestImportanceLevel5,
  RequestImportanceLevel_requestImportanceLevel6,
  RequestImportanceLevel_requestImportanceLevel7,
  RequestImportanceLevel_requestImportanceLevel8,
  RequestImportanceLevel_requestImportanceLevel9,
  RequestImportanceLevel_requestImportanceLevel10,
  RequestImportanceLevel_requestImportanceLevel11,
  RequestImportanceLevel_requestImportanceLevel12,
  RequestImportanceLevel_requestImportanceLevel13,
  RequestImportanceLevel_requestImportanceLevel14,
  RequestImportanceLevel_requestImportanceReserved,
} RequestImportanceLevel;

extern const ASN1CType asn1_type_RequestImportanceLevel[];

typedef struct RegionalExtension_52 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_52;


extern const ASN1CType asn1_type_RegionalExtension_52[];

typedef struct RequestorType {
  BasicVehicleRole role;
  BOOL subrole_option;
  RequestSubRole subrole;
  BOOL request_option;
  RequestImportanceLevel request;
  BOOL iso3883_option;
  Iso3833VehicleType iso3883;
  BOOL hpmsType_option;
  VehicleType hpmsType;
  BOOL regional_option;
  RegionalExtension_52 regional;
} RequestorType;


extern const ASN1CType asn1_type_RequestorType[];

typedef struct RequestorPositionVector {
  Position3D position;
  BOOL heading_option;
  Angle heading;
  BOOL speed_option;
  TransmissionAndSpeed speed;
} RequestorPositionVector;


extern const ASN1CType asn1_type_RequestorPositionVector[];

typedef ASN1BitString TransitVehicleStatus;

extern const ASN1CType asn1_type_TransitVehicleStatus[];

typedef enum TransitVehicleOccupancy {
  TransitVehicleOccupancy_occupancyUnknown,
  TransitVehicleOccupancy_occupancyEmpty,
  TransitVehicleOccupancy_occupancyVeryLow,
  TransitVehicleOccupancy_occupancyLow,
  TransitVehicleOccupancy_occupancyMed,
  TransitVehicleOccupancy_occupancyHigh,
  TransitVehicleOccupancy_occupancyNearlyFull,
  TransitVehicleOccupancy_occupancyFull,
} TransitVehicleOccupancy;

extern const ASN1CType asn1_type_TransitVehicleOccupancy[];

typedef int DeltaTime;

extern const ASN1CType asn1_type_DeltaTime[];

typedef struct RegionalExtension_51 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_51;


extern const ASN1CType asn1_type_RegionalExtension_51[];

typedef struct RequestorDescription_1 {
  RegionalExtension_51 *tab;
  size_t count;
} RequestorDescription_1;

extern const ASN1CType asn1_type_RequestorDescription_1[];

typedef struct RequestorDescription {
  VehicleID id;
  BOOL type_option;
  RequestorType type;
  BOOL position_option;
  RequestorPositionVector position;
  BOOL name_option;
  DescriptiveName name;
  BOOL routeName_option;
  DescriptiveName routeName;
  BOOL transitStatus_option;
  TransitVehicleStatus transitStatus;
  BOOL transitOccupancy_option;
  TransitVehicleOccupancy transitOccupancy;
  BOOL transitSchedule_option;
  DeltaTime transitSchedule;
  BOOL regional_option;
  RequestorDescription_1 regional;
} RequestorDescription;


extern const ASN1CType asn1_type_RequestorDescription[];

typedef struct RegionalExtension_13 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_13;


extern const ASN1CType asn1_type_RegionalExtension_13[];

typedef struct SignalRequestMessage_1 {
  RegionalExtension_13 *tab;
  size_t count;
} SignalRequestMessage_1;

extern const ASN1CType asn1_type_SignalRequestMessage_1[];

typedef struct SignalRequestMessage {
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  DSecond second;
  BOOL sequenceNumber_option;
  MsgCount sequenceNumber;
  BOOL requests_option;
  SignalRequestList requests;
  RequestorDescription requestor;
  BOOL regional_option;
  SignalRequestMessage_1 regional;
} SignalRequestMessage;


extern const ASN1CType asn1_type_SignalRequestMessage[];

typedef struct SignalRequesterInfo {
  VehicleID id;
  RequestID request;
  MsgCount sequenceNumber;
  BOOL role_option;
  BasicVehicleRole role;
  BOOL typeData_option;
  RequestorType typeData;
} SignalRequesterInfo;


extern const ASN1CType asn1_type_SignalRequesterInfo[];

typedef enum PrioritizationResponseStatus {
  PrioritizationResponseStatus_unknown,
  PrioritizationResponseStatus_requested,
  PrioritizationResponseStatus_processing,
  PrioritizationResponseStatus_watchOtherTraffic,
  PrioritizationResponseStatus_granted,
  PrioritizationResponseStatus_rejected,
  PrioritizationResponseStatus_maxPresence,
  PrioritizationResponseStatus_reserviceLocked,
} PrioritizationResponseStatus;

extern const ASN1CType asn1_type_PrioritizationResponseStatus[];

typedef struct RegionalExtension_57 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_57;


extern const ASN1CType asn1_type_RegionalExtension_57[];

typedef struct SignalStatusPackage_1 {
  RegionalExtension_57 *tab;
  size_t count;
} SignalStatusPackage_1;

extern const ASN1CType asn1_type_SignalStatusPackage_1[];

typedef struct SignalStatusPackage {
  BOOL requester_option;
  SignalRequesterInfo requester;
  IntersectionAccessPoint inboundOn;
  BOOL outboundOn_option;
  IntersectionAccessPoint outboundOn;
  BOOL minute_option;
  MinuteOfTheYear minute;
  BOOL second_option;
  DSecond second;
  BOOL duration_option;
  DSecond duration;
  PrioritizationResponseStatus status;
  BOOL regional_option;
  SignalStatusPackage_1 regional;
} SignalStatusPackage;


extern const ASN1CType asn1_type_SignalStatusPackage[];

typedef struct SignalStatusPackageList {
  SignalStatusPackage *tab;
  size_t count;
} SignalStatusPackageList;

extern const ASN1CType asn1_type_SignalStatusPackageList[];

typedef struct RegionalExtension_58 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_58;


extern const ASN1CType asn1_type_RegionalExtension_58[];

typedef struct SignalStatus_1 {
  RegionalExtension_58 *tab;
  size_t count;
} SignalStatus_1;

extern const ASN1CType asn1_type_SignalStatus_1[];

typedef struct SignalStatus {
  MsgCount sequenceNumber;
  IntersectionReferenceID id;
  SignalStatusPackageList sigStatus;
  BOOL regional_option;
  SignalStatus_1 regional;
} SignalStatus;


extern const ASN1CType asn1_type_SignalStatus[];

typedef struct SignalStatusList {
  SignalStatus *tab;
  size_t count;
} SignalStatusList;

extern const ASN1CType asn1_type_SignalStatusList[];

typedef struct RegionalExtension_14 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_14;


extern const ASN1CType asn1_type_RegionalExtension_14[];

typedef struct SignalStatusMessage_1 {
  RegionalExtension_14 *tab;
  size_t count;
} SignalStatusMessage_1;

extern const ASN1CType asn1_type_SignalStatusMessage_1[];

typedef struct SignalStatusMessage {
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  DSecond second;
  BOOL sequenceNumber_option;
  MsgCount sequenceNumber;
  SignalStatusList status;
  BOOL regional_option;
  SignalStatusMessage_1 regional;
} SignalStatusMessage;


extern const ASN1CType asn1_type_SignalStatusMessage[];

typedef ASN1String UniqueMSGID;

extern const ASN1CType asn1_type_UniqueMSGID[];

typedef ASN1String URL_Base;

extern const ASN1CType asn1_type_URL_Base[];

typedef enum TravelerInfoType {
  TravelerInfoType_unknown,
  TravelerInfoType_advisory,
  TravelerInfoType_roadSignage,
  TravelerInfoType_commercialSignage,
} TravelerInfoType;

extern const ASN1CType asn1_type_TravelerInfoType[];

typedef enum MUTCDCode {
  MUTCDCode_none,
  MUTCDCode_regulatory,
  MUTCDCode_warning,
  MUTCDCode_maintenance,
  MUTCDCode_motoristService,
  MUTCDCode_guide,
  MUTCDCode_rec,
} MUTCDCode;

extern const ASN1CType asn1_type_MUTCDCode[];

typedef ASN1String MsgCRC;

extern const ASN1CType asn1_type_MsgCRC[];

typedef struct RoadSignID {
  Position3D position;
  HeadingSlice viewAngle;
  BOOL mutcdCode_option;
  MUTCDCode mutcdCode;
  BOOL crc_option;
  MsgCRC crc;
} RoadSignID;


extern const ASN1CType asn1_type_RoadSignID[];

typedef enum {
  TravelerDataFrame_1_furtherInfoID,
  TravelerDataFrame_1_roadSignID,
} TravelerDataFrame_1_choice;

typedef struct TravelerDataFrame_1 {
  TravelerDataFrame_1_choice choice;
  union {
    FurtherInfoID furtherInfoID;
    RoadSignID roadSignID;
  } u;
} TravelerDataFrame_1;

extern const ASN1CType asn1_type_TravelerDataFrame_1[];

typedef int MinutesDuration;

extern const ASN1CType asn1_type_MinutesDuration[];

typedef int SignPrority;

extern const ASN1CType asn1_type_SignPrority[];

typedef enum DirectionOfUse {
  DirectionOfUse_unavailable,
  DirectionOfUse_forward,
  DirectionOfUse_reverse,
  DirectionOfUse_both,
} DirectionOfUse;

extern const ASN1CType asn1_type_DirectionOfUse[];

typedef int Zoom;

extern const ASN1CType asn1_type_Zoom[];

typedef int OffsetLL_B12;

extern const ASN1CType asn1_type_OffsetLL_B12[];

typedef struct Node_LL_24B {
  OffsetLL_B12 lon;
  OffsetLL_B12 lat;
} Node_LL_24B;


extern const ASN1CType asn1_type_Node_LL_24B[];

typedef int OffsetLL_B14;

extern const ASN1CType asn1_type_OffsetLL_B14[];

typedef struct Node_LL_28B {
  OffsetLL_B14 lon;
  OffsetLL_B14 lat;
} Node_LL_28B;


extern const ASN1CType asn1_type_Node_LL_28B[];

typedef int OffsetLL_B16;

extern const ASN1CType asn1_type_OffsetLL_B16[];

typedef struct Node_LL_32B {
  OffsetLL_B16 lon;
  OffsetLL_B16 lat;
} Node_LL_32B;


extern const ASN1CType asn1_type_Node_LL_32B[];

typedef struct Node_LL_36B {
  OffsetLL_B18 lon;
  OffsetLL_B18 lat;
} Node_LL_36B;


extern const ASN1CType asn1_type_Node_LL_36B[];

typedef int OffsetLL_B22;

extern const ASN1CType asn1_type_OffsetLL_B22[];

typedef struct Node_LL_44B {
  OffsetLL_B22 lon;
  OffsetLL_B22 lat;
} Node_LL_44B;


extern const ASN1CType asn1_type_Node_LL_44B[];

typedef int OffsetLL_B24;

extern const ASN1CType asn1_type_OffsetLL_B24[];

typedef struct Node_LL_48B {
  OffsetLL_B24 lon;
  OffsetLL_B24 lat;
} Node_LL_48B;


extern const ASN1CType asn1_type_Node_LL_48B[];

typedef struct RegionalExtension_47 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_47;


extern const ASN1CType asn1_type_RegionalExtension_47[];

typedef enum {
  NodeOffsetPointLL_node_LL1,
  NodeOffsetPointLL_node_LL2,
  NodeOffsetPointLL_node_LL3,
  NodeOffsetPointLL_node_LL4,
  NodeOffsetPointLL_node_LL5,
  NodeOffsetPointLL_node_LL6,
  NodeOffsetPointLL_node_LatLon,
  NodeOffsetPointLL_regional,
} NodeOffsetPointLL_choice;

typedef struct NodeOffsetPointLL {
  NodeOffsetPointLL_choice choice;
  union {
    Node_LL_24B node_LL1;
    Node_LL_28B node_LL2;
    Node_LL_32B node_LL3;
    Node_LL_36B node_LL4;
    Node_LL_44B node_LL5;
    Node_LL_48B node_LL6;
    Node_LLmD_64b node_LatLon;
    RegionalExtension_47 regional;
  } u;
} NodeOffsetPointLL;

extern const ASN1CType asn1_type_NodeOffsetPointLL[];

typedef enum NodeAttributeLL {
  NodeAttributeLL_reserved,
  NodeAttributeLL_stopLine,
  NodeAttributeLL_roundedCapStyleA,
  NodeAttributeLL_roundedCapStyleB,
  NodeAttributeLL_mergePoint,
  NodeAttributeLL_divergePoint,
  NodeAttributeLL_downstreamStopLine,
  NodeAttributeLL_downstreamStartNode,
  NodeAttributeLL_closedToTraffic,
  NodeAttributeLL_safeIsland,
  NodeAttributeLL_curbPresentAtStepOff,
  NodeAttributeLL_hydrantPresent,
} NodeAttributeLL;

extern const ASN1CType asn1_type_NodeAttributeLL[];

typedef struct NodeAttributeLLList {
  NodeAttributeLL *tab;
  size_t count;
} NodeAttributeLLList;

extern const ASN1CType asn1_type_NodeAttributeLLList[];

typedef enum SegmentAttributeLL {
  SegmentAttributeLL_reserved,
  SegmentAttributeLL_doNotBlock,
  SegmentAttributeLL_whiteLine,
  SegmentAttributeLL_mergingLaneLeft,
  SegmentAttributeLL_mergingLaneRight,
  SegmentAttributeLL_curbOnLeft,
  SegmentAttributeLL_curbOnRight,
  SegmentAttributeLL_loadingzoneOnLeft,
  SegmentAttributeLL_loadingzoneOnRight,
  SegmentAttributeLL_turnOutPointOnLeft,
  SegmentAttributeLL_turnOutPointOnRight,
  SegmentAttributeLL_adjacentParkingOnLeft,
  SegmentAttributeLL_adjacentParkingOnRight,
  SegmentAttributeLL_adjacentBikeLaneOnLeft,
  SegmentAttributeLL_adjacentBikeLaneOnRight,
  SegmentAttributeLL_sharedBikeLane,
  SegmentAttributeLL_bikeBoxInFront,
  SegmentAttributeLL_transitStopOnLeft,
  SegmentAttributeLL_transitStopOnRight,
  SegmentAttributeLL_transitStopInLane,
  SegmentAttributeLL_sharedWithTrackedVehicle,
  SegmentAttributeLL_safeIsland,
  SegmentAttributeLL_lowCurbsPresent,
  SegmentAttributeLL_rumbleStripPresent,
  SegmentAttributeLL_audibleSignalingPresent,
  SegmentAttributeLL_adaptiveTimingPresent,
  SegmentAttributeLL_rfSignalRequestPresent,
  SegmentAttributeLL_partialCurbIntrusion,
  SegmentAttributeLL_taperToLeft,
  SegmentAttributeLL_taperToRight,
  SegmentAttributeLL_taperToCenterLine,
  SegmentAttributeLL_parallelParking,
  SegmentAttributeLL_headInParking,
  SegmentAttributeLL_freeParking,
  SegmentAttributeLL_timeRestrictionsOnParking,
  SegmentAttributeLL_costToPark,
  SegmentAttributeLL_midBlockCurbPresent,
  SegmentAttributeLL_unEvenPavementPresent,
} SegmentAttributeLL;

extern const ASN1CType asn1_type_SegmentAttributeLL[];

typedef struct SegmentAttributeLLList {
  SegmentAttributeLL *tab;
  size_t count;
} SegmentAttributeLLList;

extern const ASN1CType asn1_type_SegmentAttributeLLList[];

typedef struct RegionalExtension_45 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_45;


extern const ASN1CType asn1_type_RegionalExtension_45[];

typedef struct NodeAttributeSetLL_1 {
  RegionalExtension_45 *tab;
  size_t count;
} NodeAttributeSetLL_1;

extern const ASN1CType asn1_type_NodeAttributeSetLL_1[];

typedef struct NodeAttributeSetLL {
  BOOL localNode_option;
  NodeAttributeLLList localNode;
  BOOL disabled_option;
  SegmentAttributeLLList disabled;
  BOOL enabled_option;
  SegmentAttributeLLList enabled;
  BOOL data_option;
  LaneDataAttributeList data;
  BOOL dWidth_option;
  Offset_B10 dWidth;
  BOOL dElevation_option;
  Offset_B10 dElevation;
  BOOL regional_option;
  NodeAttributeSetLL_1 regional;
} NodeAttributeSetLL;


extern const ASN1CType asn1_type_NodeAttributeSetLL[];

typedef struct NodeLL {
  NodeOffsetPointLL delta;
  BOOL attributes_option;
  NodeAttributeSetLL attributes;
} NodeLL;


extern const ASN1CType asn1_type_NodeLL[];

typedef struct NodeSetLL {
  NodeLL *tab;
  size_t count;
} NodeSetLL;

extern const ASN1CType asn1_type_NodeSetLL[];

typedef enum {
  NodeListLL_nodes,
} NodeListLL_choice;

typedef struct NodeListLL {
  NodeListLL_choice choice;
  union {
    NodeSetLL nodes;
  } u;
} NodeListLL;

extern const ASN1CType asn1_type_NodeListLL[];

typedef enum {
  OffsetSystem_1_xy,
  OffsetSystem_1_ll,
} OffsetSystem_1_choice;

typedef struct OffsetSystem_1 {
  OffsetSystem_1_choice choice;
  union {
    NodeListXY xy;
    NodeListLL ll;
  } u;
} OffsetSystem_1;

extern const ASN1CType asn1_type_OffsetSystem_1[];

typedef struct OffsetSystem {
  BOOL scale_option;
  Zoom scale;
  OffsetSystem_1 offset;
} OffsetSystem;


extern const ASN1CType asn1_type_OffsetSystem[];

typedef int Radius_B12;

extern const ASN1CType asn1_type_Radius_B12[];

typedef enum DistanceUnits {
  DistanceUnits_centimeter,
  DistanceUnits_cm2_5,
  DistanceUnits_decimeter,
  DistanceUnits_meter,
  DistanceUnits_kilometer,
  DistanceUnits_foot,
  DistanceUnits_yard,
  DistanceUnits_mile,
} DistanceUnits;

extern const ASN1CType asn1_type_DistanceUnits[];

typedef struct Circle {
  Position3D center;
  Radius_B12 radius;
  DistanceUnits units;
} Circle;


extern const ASN1CType asn1_type_Circle[];

typedef struct RegionalExtension_38 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_38;


extern const ASN1CType asn1_type_RegionalExtension_38[];

typedef struct GeometricProjection_1 {
  RegionalExtension_38 *tab;
  size_t count;
} GeometricProjection_1;

extern const ASN1CType asn1_type_GeometricProjection_1[];

typedef struct GeometricProjection {
  HeadingSlice direction;
  BOOL extent_option;
  Extent extent;
  BOOL laneWidth_option;
  LaneWidth laneWidth;
  Circle circle;
  BOOL regional_option;
  GeometricProjection_1 regional;
} GeometricProjection;


extern const ASN1CType asn1_type_GeometricProjection[];

typedef struct ShapePointSet {
  BOOL anchor_option;
  Position3D anchor;
  BOOL laneWidth_option;
  LaneWidth laneWidth;
  BOOL directionality_option;
  DirectionOfUse directionality;
  NodeListXY nodeList;
} ShapePointSet;


extern const ASN1CType asn1_type_ShapePointSet[];

typedef struct RegionOffsets {
  OffsetLL_B16 xOffset;
  OffsetLL_B16 yOffset;
  BOOL zOffset_option;
  OffsetLL_B16 zOffset;
} RegionOffsets;


extern const ASN1CType asn1_type_RegionOffsets[];

typedef struct RegionList {
  RegionOffsets *tab;
  size_t count;
} RegionList;

extern const ASN1CType asn1_type_RegionList[];

typedef struct RegionPointSet {
  BOOL anchor_option;
  Position3D anchor;
  BOOL scale_option;
  Zoom scale;
  RegionList nodeList;
} RegionPointSet;


extern const ASN1CType asn1_type_RegionPointSet[];

typedef enum {
  ValidRegion_1_shapePointSet,
  ValidRegion_1_circle,
  ValidRegion_1_regionPointSet,
} ValidRegion_1_choice;

typedef struct ValidRegion_1 {
  ValidRegion_1_choice choice;
  union {
    ShapePointSet shapePointSet;
    Circle circle;
    RegionPointSet regionPointSet;
  } u;
} ValidRegion_1;

extern const ASN1CType asn1_type_ValidRegion_1[];

typedef struct ValidRegion {
  HeadingSlice direction;
  BOOL extent_option;
  Extent extent;
  ValidRegion_1 area;
} ValidRegion;


extern const ASN1CType asn1_type_ValidRegion[];

typedef enum {
  GeographicalPath_1_path,
  GeographicalPath_1_geometry,
  GeographicalPath_1_oldRegion,
} GeographicalPath_1_choice;

typedef struct GeographicalPath_1 {
  GeographicalPath_1_choice choice;
  union {
    OffsetSystem path;
    GeometricProjection geometry;
    ValidRegion oldRegion;
  } u;
} GeographicalPath_1;

extern const ASN1CType asn1_type_GeographicalPath_1[];

typedef struct RegionalExtension_37 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_37;


extern const ASN1CType asn1_type_RegionalExtension_37[];

typedef struct GeographicalPath_2 {
  RegionalExtension_37 *tab;
  size_t count;
} GeographicalPath_2;

extern const ASN1CType asn1_type_GeographicalPath_2[];

typedef struct GeographicalPath {
  BOOL name_option;
  DescriptiveName name;
  BOOL id_option;
  RoadSegmentReferenceID id;
  BOOL anchor_option;
  Position3D anchor;
  BOOL laneWidth_option;
  LaneWidth laneWidth;
  BOOL directionality_option;
  DirectionOfUse directionality;
  BOOL closedPath_option;
  BOOL closedPath;
  BOOL direction_option;
  HeadingSlice direction;
  BOOL description_option;
  GeographicalPath_1 description;
  BOOL regional_option;
  GeographicalPath_2 regional;
} GeographicalPath;


extern const ASN1CType asn1_type_GeographicalPath[];

typedef struct TravelerDataFrame_2 {
  GeographicalPath *tab;
  size_t count;
} TravelerDataFrame_2;

extern const ASN1CType asn1_type_TravelerDataFrame_2[];

typedef ASN1String ITIStext;

extern const ASN1CType asn1_type_ITIStext[];

typedef enum {
  ITIScodesAndText_1_itis,
  ITIScodesAndText_1_text,
} ITIScodesAndText_1_choice;

typedef struct ITIScodesAndText_1 {
  ITIScodesAndText_1_choice choice;
  union {
    ITIScodes itis;
    ITIStext text;
  } u;
} ITIScodesAndText_1;

extern const ASN1CType asn1_type_ITIScodesAndText_1[];

typedef struct ITIScodesAndText_2 {
  ITIScodesAndText_1 item;
} ITIScodesAndText_2;


extern const ASN1CType asn1_type_ITIScodesAndText_2[];

typedef struct ITIScodesAndText {
  ITIScodesAndText_2 *tab;
  size_t count;
} ITIScodesAndText;

extern const ASN1CType asn1_type_ITIScodesAndText[];

typedef ASN1String ITIStextPhrase;

extern const ASN1CType asn1_type_ITIStextPhrase[];

typedef enum {
  WorkZone_1_itis,
  WorkZone_1_text,
} WorkZone_1_choice;

typedef struct WorkZone_1 {
  WorkZone_1_choice choice;
  union {
    ITIScodes itis;
    ITIStextPhrase text;
  } u;
} WorkZone_1;

extern const ASN1CType asn1_type_WorkZone_1[];

typedef struct WorkZone_2 {
  WorkZone_1 item;
} WorkZone_2;


extern const ASN1CType asn1_type_WorkZone_2[];

typedef struct WorkZone {
  WorkZone_2 *tab;
  size_t count;
} WorkZone;

extern const ASN1CType asn1_type_WorkZone[];

typedef enum {
  GenericSignage_1_itis,
  GenericSignage_1_text,
} GenericSignage_1_choice;

typedef struct GenericSignage_1 {
  GenericSignage_1_choice choice;
  union {
    ITIScodes itis;
    ITIStextPhrase text;
  } u;
} GenericSignage_1;

extern const ASN1CType asn1_type_GenericSignage_1[];

typedef struct GenericSignage_2 {
  GenericSignage_1 item;
} GenericSignage_2;


extern const ASN1CType asn1_type_GenericSignage_2[];

typedef struct GenericSignage {
  GenericSignage_2 *tab;
  size_t count;
} GenericSignage;

extern const ASN1CType asn1_type_GenericSignage[];

typedef enum {
  SpeedLimit_1_itis,
  SpeedLimit_1_text,
} SpeedLimit_1_choice;

typedef struct SpeedLimit_1 {
  SpeedLimit_1_choice choice;
  union {
    ITIScodes itis;
    ITIStextPhrase text;
  } u;
} SpeedLimit_1;

extern const ASN1CType asn1_type_SpeedLimit_1[];

typedef struct SpeedLimit_2 {
  SpeedLimit_1 item;
} SpeedLimit_2;


extern const ASN1CType asn1_type_SpeedLimit_2[];

typedef struct SpeedLimit {
  SpeedLimit_2 *tab;
  size_t count;
} SpeedLimit;

extern const ASN1CType asn1_type_SpeedLimit[];

typedef enum {
  ExitService_1_itis,
  ExitService_1_text,
} ExitService_1_choice;

typedef struct ExitService_1 {
  ExitService_1_choice choice;
  union {
    ITIScodes itis;
    ITIStextPhrase text;
  } u;
} ExitService_1;

extern const ASN1CType asn1_type_ExitService_1[];

typedef struct ExitService_2 {
  ExitService_1 item;
} ExitService_2;


extern const ASN1CType asn1_type_ExitService_2[];

typedef struct ExitService {
  ExitService_2 *tab;
  size_t count;
} ExitService;

extern const ASN1CType asn1_type_ExitService[];

typedef enum {
  TravelerDataFrame_3_advisory,
  TravelerDataFrame_3_workZone,
  TravelerDataFrame_3_genericSign,
  TravelerDataFrame_3_speedLimit,
  TravelerDataFrame_3_exitService,
} TravelerDataFrame_3_choice;

typedef struct TravelerDataFrame_3 {
  TravelerDataFrame_3_choice choice;
  union {
    ITIScodesAndText advisory;
    WorkZone workZone;
    GenericSignage genericSign;
    SpeedLimit speedLimit;
    ExitService exitService;
  } u;
} TravelerDataFrame_3;

extern const ASN1CType asn1_type_TravelerDataFrame_3[];

typedef ASN1String URL_Short;

extern const ASN1CType asn1_type_URL_Short[];

typedef struct TravelerDataFrame {
  SSPindex sspTimRights;
  TravelerInfoType frameType;
  TravelerDataFrame_1 msgId;
  BOOL startYear_option;
  DYear startYear;
  MinuteOfTheYear startTime;
  MinutesDuration duratonTime;
  SignPrority priority;
  SSPindex sspLocationRights;
  TravelerDataFrame_2 regions;
  SSPindex sspMsgRights1;
  SSPindex sspMsgRights2;
  TravelerDataFrame_3 content;
  BOOL url_option;
  URL_Short url;
} TravelerDataFrame;


extern const ASN1CType asn1_type_TravelerDataFrame[];

typedef struct TravelerDataFrameList {
  TravelerDataFrame *tab;
  size_t count;
} TravelerDataFrameList;

extern const ASN1CType asn1_type_TravelerDataFrameList[];

typedef struct RegionalExtension_15 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_15;


extern const ASN1CType asn1_type_RegionalExtension_15[];

typedef struct TravelerInformation_1 {
  RegionalExtension_15 *tab;
  size_t count;
} TravelerInformation_1;

extern const ASN1CType asn1_type_TravelerInformation_1[];

typedef struct TravelerInformation {
  MsgCount msgCnt;
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  BOOL packetID_option;
  UniqueMSGID packetID;
  BOOL urlB_option;
  URL_Base urlB;
  TravelerDataFrameList dataFrames;
  BOOL regional_option;
  TravelerInformation_1 regional;
} TravelerInformation;


extern const ASN1CType asn1_type_TravelerInformation[];

typedef enum PersonalDeviceUserType {
  PersonalDeviceUserType_unavailable,
  PersonalDeviceUserType_aPEDESTRIAN,
  PersonalDeviceUserType_aPEDALCYCLIST,
  PersonalDeviceUserType_aPUBLICSAFETYWORKER,
  PersonalDeviceUserType_anANIMAL,
} PersonalDeviceUserType;

extern const ASN1CType asn1_type_PersonalDeviceUserType[];

typedef enum HumanPropelledType {
  HumanPropelledType_unavailable,
  HumanPropelledType_otherTypes,
  HumanPropelledType_onFoot,
  HumanPropelledType_skateboard,
  HumanPropelledType_pushOrKickScooter,
  HumanPropelledType_wheelchair,
} HumanPropelledType;

extern const ASN1CType asn1_type_HumanPropelledType[];

typedef enum AnimalPropelledType {
  AnimalPropelledType_unavailable,
  AnimalPropelledType_otherTypes,
  AnimalPropelledType_animalMounted,
  AnimalPropelledType_animalDrawnCarriage,
} AnimalPropelledType;

extern const ASN1CType asn1_type_AnimalPropelledType[];

typedef enum MotorizedPropelledType {
  MotorizedPropelledType_unavailable,
  MotorizedPropelledType_otherTypes,
  MotorizedPropelledType_wheelChair,
  MotorizedPropelledType_bicycle,
  MotorizedPropelledType_scooter,
  MotorizedPropelledType_selfBalancingDevice,
} MotorizedPropelledType;

extern const ASN1CType asn1_type_MotorizedPropelledType[];

typedef enum {
  PropelledInformation_human,
  PropelledInformation_animal,
  PropelledInformation_motor,
} PropelledInformation_choice;

typedef struct PropelledInformation {
  PropelledInformation_choice choice;
  union {
    HumanPropelledType human;
    AnimalPropelledType animal;
    MotorizedPropelledType motor;
  } u;
} PropelledInformation;

extern const ASN1CType asn1_type_PropelledInformation[];

typedef ASN1BitString PersonalDeviceUsageState;

extern const ASN1CType asn1_type_PersonalDeviceUsageState[];

typedef BOOL PersonalCrossingRequest;

extern const ASN1CType asn1_type_PersonalCrossingRequest[];

typedef BOOL PersonalCrossingInProgress;

extern const ASN1CType asn1_type_PersonalCrossingInProgress[];

typedef enum NumberOfParticipantsInCluster {
  NumberOfParticipantsInCluster_unavailable,
  NumberOfParticipantsInCluster_small,
  NumberOfParticipantsInCluster_medium,
  NumberOfParticipantsInCluster_large,
} NumberOfParticipantsInCluster;

extern const ASN1CType asn1_type_NumberOfParticipantsInCluster[];

typedef int PersonalClusterRadius;

extern const ASN1CType asn1_type_PersonalClusterRadius[];

typedef enum PublicSafetyEventResponderWorkerType {
  PublicSafetyEventResponderWorkerType_unavailable,
  PublicSafetyEventResponderWorkerType_towOperater,
  PublicSafetyEventResponderWorkerType_fireAndEMSWorker,
  PublicSafetyEventResponderWorkerType_aDOTWorker,
  PublicSafetyEventResponderWorkerType_lawEnforcement,
  PublicSafetyEventResponderWorkerType_hazmatResponder,
  PublicSafetyEventResponderWorkerType_animalControlWorker,
  PublicSafetyEventResponderWorkerType_otherPersonnel,
} PublicSafetyEventResponderWorkerType;

extern const ASN1CType asn1_type_PublicSafetyEventResponderWorkerType[];

typedef ASN1BitString PublicSafetyAndRoadWorkerActivity;

extern const ASN1CType asn1_type_PublicSafetyAndRoadWorkerActivity[];

typedef ASN1BitString PublicSafetyDirectingTrafficSubType;

extern const ASN1CType asn1_type_PublicSafetyDirectingTrafficSubType[];

typedef ASN1BitString PersonalAssistive;

extern const ASN1CType asn1_type_PersonalAssistive[];

typedef ASN1BitString UserSizeAndBehaviour;

extern const ASN1CType asn1_type_UserSizeAndBehaviour[];

typedef enum Attachment {
  Attachment_unavailable,
  Attachment_stroller,
  Attachment_bicycleTrailer,
  Attachment_cart,
  Attachment_wheelchair,
  Attachment_otherWalkAssistAttachments,
  Attachment_pet,
} Attachment;

extern const ASN1CType asn1_type_Attachment[];

typedef int AttachmentRadius;

extern const ASN1CType asn1_type_AttachmentRadius[];

typedef enum AnimalType {
  AnimalType_unavailable,
  AnimalType_serviceUse,
  AnimalType_pet,
  AnimalType_farm,
} AnimalType;

extern const ASN1CType asn1_type_AnimalType[];

typedef struct RegionalExtension_7 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_7;


extern const ASN1CType asn1_type_RegionalExtension_7[];

typedef struct PersonalSafetyMessage_1 {
  RegionalExtension_7 *tab;
  size_t count;
} PersonalSafetyMessage_1;

extern const ASN1CType asn1_type_PersonalSafetyMessage_1[];

typedef struct PersonalSafetyMessage {
  PersonalDeviceUserType basicType;
  DSecond secMark;
  MsgCount msgCnt;
  TemporaryID id;
  Position3D position;
  PositionalAccuracy accuracy;
  Velocity speed;
  Heading heading;
  BOOL accelSet_option;
  AccelerationSet4Way accelSet;
  BOOL pathHistory_option;
  PathHistory pathHistory;
  BOOL pathPrediction_option;
  PathPrediction pathPrediction;
  BOOL propulsion_option;
  PropelledInformation propulsion;
  BOOL useState_option;
  PersonalDeviceUsageState useState;
  BOOL crossRequest_option;
  PersonalCrossingRequest crossRequest;
  BOOL crossState_option;
  PersonalCrossingInProgress crossState;
  BOOL clusterSize_option;
  NumberOfParticipantsInCluster clusterSize;
  BOOL clusterRadius_option;
  PersonalClusterRadius clusterRadius;
  BOOL eventResponderType_option;
  PublicSafetyEventResponderWorkerType eventResponderType;
  BOOL activityType_option;
  PublicSafetyAndRoadWorkerActivity activityType;
  BOOL activitySubType_option;
  PublicSafetyDirectingTrafficSubType activitySubType;
  BOOL assistType_option;
  PersonalAssistive assistType;
  BOOL sizing_option;
  UserSizeAndBehaviour sizing;
  BOOL attachment_option;
  Attachment attachment;
  BOOL attachmentRadius_option;
  AttachmentRadius attachmentRadius;
  BOOL animalType_option;
  AnimalType animalType;
  BOOL regional_option;
  PersonalSafetyMessage_1 regional;
} PersonalSafetyMessage;


extern const ASN1CType asn1_type_PersonalSafetyMessage[];

typedef struct Header {
  BOOL year_option;
  DYear year;
  BOOL timeStamp_option;
  MinuteOfTheYear timeStamp;
  BOOL secMark_option;
  DSecond secMark;
  BOOL msgIssueRevision_option;
  MsgCount msgIssueRevision;
} Header;


extern const ASN1CType asn1_type_Header[];

typedef struct RegionalExtension_16 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_16;


extern const ASN1CType asn1_type_RegionalExtension_16[];

typedef struct TestMessage00 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_16 regional;
} TestMessage00;


extern const ASN1CType asn1_type_TestMessage00[];

typedef struct RegionalExtension_17 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_17;


extern const ASN1CType asn1_type_RegionalExtension_17[];

typedef struct TestMessage01 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_17 regional;
} TestMessage01;


extern const ASN1CType asn1_type_TestMessage01[];

typedef struct RegionalExtension_18 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_18;


extern const ASN1CType asn1_type_RegionalExtension_18[];

typedef struct TestMessage02 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_18 regional;
} TestMessage02;


extern const ASN1CType asn1_type_TestMessage02[];

typedef struct RegionalExtension_19 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_19;


extern const ASN1CType asn1_type_RegionalExtension_19[];

typedef struct TestMessage03 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_19 regional;
} TestMessage03;


extern const ASN1CType asn1_type_TestMessage03[];

typedef struct RegionalExtension_20 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_20;


extern const ASN1CType asn1_type_RegionalExtension_20[];

typedef struct TestMessage04 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_20 regional;
} TestMessage04;


extern const ASN1CType asn1_type_TestMessage04[];

typedef struct RegionalExtension_21 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_21;


extern const ASN1CType asn1_type_RegionalExtension_21[];

typedef struct TestMessage05 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_21 regional;
} TestMessage05;


extern const ASN1CType asn1_type_TestMessage05[];

typedef struct RegionalExtension_22 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_22;


extern const ASN1CType asn1_type_RegionalExtension_22[];

typedef struct TestMessage06 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_22 regional;
} TestMessage06;


extern const ASN1CType asn1_type_TestMessage06[];

typedef struct RegionalExtension_23 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_23;


extern const ASN1CType asn1_type_RegionalExtension_23[];

typedef struct TestMessage07 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_23 regional;
} TestMessage07;


extern const ASN1CType asn1_type_TestMessage07[];

typedef struct RegionalExtension_24 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_24;


extern const ASN1CType asn1_type_RegionalExtension_24[];

typedef struct TestMessage08 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_24 regional;
} TestMessage08;


extern const ASN1CType asn1_type_TestMessage08[];

typedef struct RegionalExtension_25 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_25;


extern const ASN1CType asn1_type_RegionalExtension_25[];

typedef struct TestMessage09 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_25 regional;
} TestMessage09;


extern const ASN1CType asn1_type_TestMessage09[];

typedef struct RegionalExtension_26 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_26;


extern const ASN1CType asn1_type_RegionalExtension_26[];

typedef struct TestMessage10 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_26 regional;
} TestMessage10;


extern const ASN1CType asn1_type_TestMessage10[];

typedef struct RegionalExtension_27 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_27;


extern const ASN1CType asn1_type_RegionalExtension_27[];

typedef struct TestMessage11 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_27 regional;
} TestMessage11;


extern const ASN1CType asn1_type_TestMessage11[];

typedef struct RegionalExtension_28 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_28;


extern const ASN1CType asn1_type_RegionalExtension_28[];

typedef struct TestMessage12 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_28 regional;
} TestMessage12;


extern const ASN1CType asn1_type_TestMessage12[];

typedef struct RegionalExtension_29 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_29;


extern const ASN1CType asn1_type_RegionalExtension_29[];

typedef struct TestMessage13 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_29 regional;
} TestMessage13;


extern const ASN1CType asn1_type_TestMessage13[];

typedef struct RegionalExtension_30 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_30;


extern const ASN1CType asn1_type_RegionalExtension_30[];

typedef struct TestMessage14 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_30 regional;
} TestMessage14;


extern const ASN1CType asn1_type_TestMessage14[];

typedef struct RegionalExtension_31 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_31;


extern const ASN1CType asn1_type_RegionalExtension_31[];

typedef struct TestMessage15 {
  BOOL header_option;
  Header header;
  BOOL regional_option;
  RegionalExtension_31 regional;
} TestMessage15;


extern const ASN1CType asn1_type_TestMessage15[];

typedef struct REG_EXT_ID_AND_TYPE { /* object class definition */
  ASN1CType id;
  ASN1CType Type;
} REG_EXT_ID_AND_TYPE;


extern const ASN1CType asn1_type_REG_EXT_ID_AND_TYPE[];

typedef struct PARTII_EXT_ID_AND_TYPE { /* object class definition */
  ASN1CType id;
  ASN1CType Type;
} PARTII_EXT_ID_AND_TYPE;


extern const ASN1CType asn1_type_PARTII_EXT_ID_AND_TYPE[];

typedef struct EventDescription_1 {
  ITIScodes *tab;
  size_t count;
} EventDescription_1;

extern const ASN1CType asn1_type_EventDescription_1[];

typedef struct RegionalExtension_35 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_35;


extern const ASN1CType asn1_type_RegionalExtension_35[];

typedef struct EventDescription_2 {
  RegionalExtension_35 *tab;
  size_t count;
} EventDescription_2;

extern const ASN1CType asn1_type_EventDescription_2[];

typedef struct EventDescription {
  ITIScodes typeEvent;
  BOOL description_option;
  EventDescription_1 description;
  BOOL priority_option;
  Priority priority;
  BOOL heading_option;
  HeadingSlice heading;
  BOOL extent_option;
  Extent extent;
  BOOL regional_option;
  EventDescription_2 regional;
} EventDescription;


extern const ASN1CType asn1_type_EventDescription[];

typedef BOOL PivotingAllowed;

extern const ASN1CType asn1_type_PivotingAllowed[];

typedef struct PivotPointDescription {
  Offset_B11 pivotOffset;
  Angle pivotAngle;
  PivotingAllowed pivots;
} PivotPointDescription;


extern const ASN1CType asn1_type_PivotPointDescription[];

typedef BOOL IsDolly;

extern const ASN1CType asn1_type_IsDolly[];

typedef int TrailerMass;

extern const ASN1CType asn1_type_TrailerMass[];

typedef int VertOffset_B07;

extern const ASN1CType asn1_type_VertOffset_B07[];

typedef struct TrailerHistoryPoint {
  Angle pivotAngle;
  TimeOffset timeOffset;
  Node_XY_24b positionOffset;
  BOOL elevationOffset_option;
  VertOffset_B07 elevationOffset;
  BOOL heading_option;
  CoarseHeading heading;
} TrailerHistoryPoint;


extern const ASN1CType asn1_type_TrailerHistoryPoint[];

typedef struct TrailerHistoryPointList {
  TrailerHistoryPoint *tab;
  size_t count;
} TrailerHistoryPointList;

extern const ASN1CType asn1_type_TrailerHistoryPointList[];

typedef struct TrailerUnitDescription {
  IsDolly isDolly;
  VehicleWidth width;
  VehicleLength length;
  BOOL height_option;
  VehicleHeight height;
  BOOL mass_option;
  TrailerMass mass;
  BOOL bumperHeights_option;
  BumperHeights bumperHeights;
  BOOL centerOfGravity_option;
  VehicleHeight centerOfGravity;
  PivotPointDescription frontPivot;
  BOOL rearPivot_option;
  PivotPointDescription rearPivot;
  BOOL rearWheelOffset_option;
  Offset_B12 rearWheelOffset;
  Node_XY_24b positionOffset;
  BOOL elevationOffset_option;
  VertOffset_B07 elevationOffset;
  BOOL crumbData_option;
  TrailerHistoryPointList crumbData;
} TrailerUnitDescription;


extern const ASN1CType asn1_type_TrailerUnitDescription[];

typedef struct TrailerUnitDescriptionList {
  TrailerUnitDescription *tab;
  size_t count;
} TrailerUnitDescriptionList;

extern const ASN1CType asn1_type_TrailerUnitDescriptionList[];

typedef struct TrailerData {
  SSPindex sspRights;
  PivotPointDescription connection;
  TrailerUnitDescriptionList units;
} TrailerData;


extern const ASN1CType asn1_type_TrailerData[];

typedef struct SpecialVehicleExtensions {
  BOOL vehicleAlerts_option;
  EmergencyDetails vehicleAlerts;
  BOOL description_option;
  EventDescription description;
  BOOL trailers_option;
  TrailerData trailers;
} SpecialVehicleExtensions;


extern const ASN1CType asn1_type_SpecialVehicleExtensions[];

typedef struct VehicleData {
  BOOL height_option;
  VehicleHeight height;
  BOOL bumpers_option;
  BumperHeights bumpers;
  BOOL mass_option;
  VehicleMass mass;
  BOOL trailerWeight_option;
  TrailerWeight trailerWeight;
} VehicleData;


extern const ASN1CType asn1_type_VehicleData[];

typedef struct WeatherReport {
  EssPrecipYesNo isRaining;
  BOOL rainRate_option;
  EssPrecipRate rainRate;
  BOOL precipSituation_option;
  EssPrecipSituation precipSituation;
  BOOL solarRadiation_option;
  EssSolarRadiation solarRadiation;
  BOOL friction_option;
  EssMobileFriction friction;
  BOOL roadFriction_option;
  CoefficientOfFriction roadFriction;
} WeatherReport;


extern const ASN1CType asn1_type_WeatherReport[];

typedef struct WeatherProbe {
  BOOL airTemp_option;
  AmbientAirTemperature airTemp;
  BOOL airPressure_option;
  AmbientAirPressure airPressure;
  BOOL rainRates_option;
  WiperSet rainRates;
} WeatherProbe;


extern const ASN1CType asn1_type_WeatherProbe[];

typedef Angle ObstacleDirection;

extern const ASN1CType asn1_type_ObstacleDirection[];

typedef int ITIScodes_2;

extern const ASN1CType asn1_type_ITIScodes_2[];

typedef enum GenericLocations {
  GenericLocations_on_bridges,
  GenericLocations_in_tunnels,
  GenericLocations_entering_or_leaving_tunnels,
  GenericLocations_on_ramps,
  GenericLocations_in_road_construction_area,
  GenericLocations_around_a_curve,
  GenericLocations_on_curve,
  GenericLocations_on_tracks,
  GenericLocations_in_street,
  GenericLocations_shoulder,
  GenericLocations_on_minor_roads,
  GenericLocations_in_the_opposing_lanes,
  GenericLocations_adjacent_to_roadway,
  GenericLocations_across_tracks,
  GenericLocations_on_bend,
  GenericLocations_intersection,
  GenericLocations_entire_intersection,
  GenericLocations_in_the_median,
  GenericLocations_moved_to_side_of_road,
  GenericLocations_moved_to_shoulder,
  GenericLocations_on_the_roadway,
  GenericLocations_dip,
  GenericLocations_traffic_circle,
  GenericLocations_crossover,
  GenericLocations_cross_road,
  GenericLocations_side_road,
  GenericLocations_to,
  GenericLocations_by,
  GenericLocations_through,
  GenericLocations_area_of,
  GenericLocations_under,
  GenericLocations_over,
  GenericLocations_from,
  GenericLocations_approaching,
  GenericLocations_entering_at,
  GenericLocations_exiting_at,
  GenericLocations_in_shaded_areas,
  GenericLocations_in_low_lying_areas,
  GenericLocations_in_the_downtown_area,
  GenericLocations_in_the_inner_city_area,
  GenericLocations_in_parts,
  GenericLocations_in_some_places,
  GenericLocations_in_the_ditch,
  GenericLocations_in_the_valley,
  GenericLocations_on_hill_top,
  GenericLocations_near_the_foothills,
  GenericLocations_at_high_altitudes,
  GenericLocations_near_the_lake,
  GenericLocations_near_the_shore,
  GenericLocations_nearby_basin,
  GenericLocations_over_the_crest_of_a_hill,
  GenericLocations_other_than_on_the_roadway,
  GenericLocations_near_the_beach,
  GenericLocations_near_beach_access_point,
  GenericLocations_mountain_pass,
  GenericLocations_lower_level,
  GenericLocations_upper_level,
  GenericLocations_airport,
  GenericLocations_concourse,
  GenericLocations_gate,
  GenericLocations_baggage_claim,
  GenericLocations_customs_point,
  GenericLocations_reservation_center,
  GenericLocations_station,
  GenericLocations_platform,
  GenericLocations_dock,
  GenericLocations_depot,
  GenericLocations_ev_charging_point,
  GenericLocations_information_welcome_point,
  GenericLocations_at_rest_area,
  GenericLocations_at_service_area,
  GenericLocations_at_weigh_station,
  GenericLocations_roadside_park,
  GenericLocations_picnic_areas,
  GenericLocations_rest_area,
  GenericLocations_service_stations,
  GenericLocations_toilets,
  GenericLocations_bus_stop,
  GenericLocations_park_and_ride_lot,
  GenericLocations_on_the_right,
  GenericLocations_on_the_left,
  GenericLocations_in_the_center,
  GenericLocations_in_the_opposite_direction,
  GenericLocations_cross_traffic,
  GenericLocations_northbound_traffic,
  GenericLocations_eastbound_traffic,
  GenericLocations_southbound_traffic,
  GenericLocations_westbound_traffic,
  GenericLocations_north,
  GenericLocations_south,
  GenericLocations_east,
  GenericLocations_west,
  GenericLocations_northeast,
  GenericLocations_northwest,
  GenericLocations_southeast,
  GenericLocations_southwest,
} GenericLocations;

extern const ASN1CType asn1_type_GenericLocations[];

typedef struct ObstacleDetection {
  ObstacleDistance obDist;
  ObstacleDirection obDirect;
  BOOL description_option;
  ITIScodes_2 description;
  BOOL locationDetails_option;
  GenericLocations locationDetails;
  DDateTime dateTime;
  BOOL vertEvent_option;
  VerticalAccelerationThreshold vertEvent;
} ObstacleDetection;


extern const ASN1CType asn1_type_ObstacleDetection[];

typedef int ITIScodes_1;

extern const ASN1CType asn1_type_ITIScodes_1[];

typedef struct DisabledVehicle {
  ITIScodes_1 statusDetails;
  BOOL locationDetails_option;
  GenericLocations locationDetails;
} DisabledVehicle;


extern const ASN1CType asn1_type_DisabledVehicle[];

typedef GrossSpeed SpeedProfileMeasurement;

#define asn1_type_SpeedProfileMeasurement asn1_type_GrossSpeed

typedef struct SpeedProfileMeasurementList {
  SpeedProfileMeasurement *tab;
  size_t count;
} SpeedProfileMeasurementList;

extern const ASN1CType asn1_type_SpeedProfileMeasurementList[];

typedef struct SpeedProfile {
  SpeedProfileMeasurementList speedReports;
} SpeedProfile;


extern const ASN1CType asn1_type_SpeedProfile[];

typedef struct RTCMPackage {
  BOOL rtcmHeader_option;
  RTCMheader rtcmHeader;
  RTCMmessageList msgs;
} RTCMPackage;


extern const ASN1CType asn1_type_RTCMPackage[];

typedef struct RegionalExtension_59 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_59;


extern const ASN1CType asn1_type_RegionalExtension_59[];

typedef struct SupplementalVehicleExtensions_1 {
  RegionalExtension_59 *tab;
  size_t count;
} SupplementalVehicleExtensions_1;

extern const ASN1CType asn1_type_SupplementalVehicleExtensions_1[];

typedef struct SupplementalVehicleExtensions {
  BOOL classification_option;
  BasicVehicleClass classification;
  BOOL classDetails_option;
  VehicleClassification classDetails;
  BOOL vehicleData_option;
  VehicleData vehicleData;
  BOOL weatherReport_option;
  WeatherReport weatherReport;
  BOOL weatherProbe_option;
  WeatherProbe weatherProbe;
  BOOL obstacle_option;
  ObstacleDetection obstacle;
  BOOL status_option;
  DisabledVehicle status;
  BOOL speedProfile_option;
  SpeedProfile speedProfile;
  BOOL theRTCM_option;
  RTCMPackage theRTCM;
  BOOL regional_option;
  SupplementalVehicleExtensions_1 regional;
} SupplementalVehicleExtensions;


extern const ASN1CType asn1_type_SupplementalVehicleExtensions[];

typedef struct DDate {
  DYear year;
  DMonth month;
  DDay day;
} DDate;


extern const ASN1CType asn1_type_DDate[];

typedef struct DFullTime {
  DYear year;
  DMonth month;
  DDay day;
  DHour hour;
  DMinute minute;
} DFullTime;


extern const ASN1CType asn1_type_DFullTime[];

typedef struct DMonthDay {
  DMonth month;
  DDay day;
} DMonthDay;


extern const ASN1CType asn1_type_DMonthDay[];

typedef struct DTime {
  DHour hour;
  DMinute minute;
  DSecond second;
  BOOL offset_option;
  DOffset offset;
} DTime;


extern const ASN1CType asn1_type_DTime[];

typedef struct DYearMonth {
  DYear year;
  DMonth month;
} DYearMonth;


extern const ASN1CType asn1_type_DYearMonth[];

typedef int VertOffset_B08;

extern const ASN1CType asn1_type_VertOffset_B08[];

typedef int VertOffset_B09;

extern const ASN1CType asn1_type_VertOffset_B09[];

typedef int VertOffset_B10;

extern const ASN1CType asn1_type_VertOffset_B10[];

typedef int VertOffset_B11;

extern const ASN1CType asn1_type_VertOffset_B11[];

typedef struct RegionalExtension_61 {
  RegionId regionId;
  ASN1OpenType regExtValue;
} RegionalExtension_61;


extern const ASN1CType asn1_type_RegionalExtension_61[];

typedef enum {
  VerticalOffset_offset1,
  VerticalOffset_offset2,
  VerticalOffset_offset3,
  VerticalOffset_offset4,
  VerticalOffset_offset5,
  VerticalOffset_offset6,
  VerticalOffset_elevation,
  VerticalOffset_regional,
} VerticalOffset_choice;

typedef struct VerticalOffset {
  VerticalOffset_choice choice;
  union {
    VertOffset_B07 offset1;
    VertOffset_B08 offset2;
    VertOffset_B09 offset3;
    VertOffset_B10 offset4;
    VertOffset_B11 offset5;
    VertOffset_B12 offset6;
    Elevation elevation;
    RegionalExtension_61 regional;
  } u;
} VerticalOffset;

extern const ASN1CType asn1_type_VerticalOffset[];

typedef ASN1String CodeWord;

extern const ASN1CType asn1_type_CodeWord[];

typedef int Count;

extern const ASN1CType asn1_type_Count[];

typedef int Duration;

extern const ASN1CType asn1_type_Duration[];

typedef enum Location_quality {
  Location_quality_loc_qual_bt1m,
  Location_quality_loc_qual_bt5m,
  Location_quality_loc_qual_bt12m,
  Location_quality_loc_qual_bt50m,
  Location_quality_loc_qual_bt125m,
  Location_quality_loc_qual_bt500m,
  Location_quality_loc_qual_bt1250m,
  Location_quality_loc_qual_unknown,
} Location_quality;

extern const ASN1CType asn1_type_Location_quality[];

typedef enum Location_tech {
  Location_tech_loc_tech_unknown,
  Location_tech_loc_tech_GNSS,
  Location_tech_loc_tech_DGPS,
  Location_tech_loc_tech_RTK,
  Location_tech_loc_tech_PPP,
  Location_tech_loc_tech_drGPS,
  Location_tech_loc_tech_drDGPS,
  Location_tech_loc_tech_dr,
  Location_tech_loc_tech_nav,
  Location_tech_loc_tech_fault,
} Location_tech;

extern const ASN1CType asn1_type_Location_tech[];

typedef ASN1String MessageBLOB;

extern const ASN1CType asn1_type_MessageBLOB[];

typedef ASN1String PayloadData;

extern const ASN1CType asn1_type_PayloadData[];

typedef ASN1String SignalReqScheme;

extern const ASN1CType asn1_type_SignalReqScheme[];

typedef ASN1BitString TransitStatus;

extern const ASN1CType asn1_type_TransitStatus[];

typedef ASN1String URL_Link;

extern const ASN1CType asn1_type_URL_Link[];

typedef struct VehicleToLanePosition {
  StationID stationID;
  LaneID laneID;
} VehicleToLanePosition;


extern const ASN1CType asn1_type_VehicleToLanePosition[];

typedef struct VehicleToLanePositionList {
  VehicleToLanePosition *tab;
  size_t count;
} VehicleToLanePositionList;

extern const ASN1CType asn1_type_VehicleToLanePositionList[];

typedef struct ConnectionManeuverAssist_addGrpC {
  VehicleToLanePositionList vehicleToLanePositions;
  BOOL rsuDistanceFromAnchor_option;
  NodeOffsetPointXY rsuDistanceFromAnchor;
} ConnectionManeuverAssist_addGrpC;


extern const ASN1CType asn1_type_ConnectionManeuverAssist_addGrpC[];

typedef struct PrioritizationResponse {
  StationID stationID;
  PrioritizationResponseStatus priorState;
  SignalGroupID signalGroup;
} PrioritizationResponse;


extern const ASN1CType asn1_type_PrioritizationResponse[];

typedef struct PrioritizationResponseList {
  PrioritizationResponse *tab;
  size_t count;
} PrioritizationResponseList;

extern const ASN1CType asn1_type_PrioritizationResponseList[];

typedef struct IntersectionState_addGrpC {
  BOOL activePrioritizations_option;
  PrioritizationResponseList activePrioritizations;
} IntersectionState_addGrpC;


extern const ASN1CType asn1_type_IntersectionState_addGrpC[];

typedef struct LaneDataAttribute_addGrpB {
  uint8_t dummy_field;
} LaneDataAttribute_addGrpB;


extern const ASN1CType asn1_type_LaneDataAttribute_addGrpB[];

typedef int TimeRemaining;

extern const ASN1CType asn1_type_TimeRemaining[];

typedef int MinTimetoChange;

extern const ASN1CType asn1_type_MinTimetoChange[];

typedef int MaxTimetoChange;

extern const ASN1CType asn1_type_MaxTimetoChange[];

typedef struct MovementEvent_addGrpB {
  BOOL startTime_option;
  TimeRemaining startTime;
  MinTimetoChange minEndTime;
  BOOL maxEndTime_option;
  MaxTimetoChange maxEndTime;
  BOOL likelyTime_option;
  TimeRemaining likelyTime;
  BOOL confidence_option;
  TimeIntervalConfidence confidence;
  BOOL nextTime_option;
  TimeRemaining nextTime;
} MovementEvent_addGrpB;


extern const ASN1CType asn1_type_MovementEvent_addGrpB[];

typedef int LongitudeDMS;

extern const ASN1CType asn1_type_LongitudeDMS[];

typedef int LatitudeDMS;

extern const ASN1CType asn1_type_LatitudeDMS[];

typedef struct Node_LLdms_48b {
  LongitudeDMS lon;
  LatitudeDMS lat;
} Node_LLdms_48b;


extern const ASN1CType asn1_type_Node_LLdms_48b[];

typedef int DegreesLong;

extern const ASN1CType asn1_type_DegreesLong[];

typedef int MinutesAngle;

extern const ASN1CType asn1_type_MinutesAngle[];

typedef int SecondsAngle;

extern const ASN1CType asn1_type_SecondsAngle[];

typedef struct LongitudeDMS2 {
  DegreesLong d;
  MinutesAngle m;
  SecondsAngle s;
} LongitudeDMS2;


extern const ASN1CType asn1_type_LongitudeDMS2[];

typedef int DegreesLat;

extern const ASN1CType asn1_type_DegreesLat[];

typedef struct LatitudeDMS2 {
  DegreesLat d;
  MinutesAngle m;
  SecondsAngle s;
} LatitudeDMS2;


extern const ASN1CType asn1_type_LatitudeDMS2[];

typedef struct Node_LLdms_80b {
  LongitudeDMS2 lon;
  LatitudeDMS2 lat;
} Node_LLdms_80b;


extern const ASN1CType asn1_type_Node_LLdms_80b[];

typedef enum {
  NodeOffsetPointXY_addGrpB_posA,
  NodeOffsetPointXY_addGrpB_posB,
} NodeOffsetPointXY_addGrpB_choice;

typedef struct NodeOffsetPointXY_addGrpB {
  NodeOffsetPointXY_addGrpB_choice choice;
  union {
    Node_LLdms_48b posA;
    Node_LLdms_80b posB;
  } u;
} NodeOffsetPointXY_addGrpB;

extern const ASN1CType asn1_type_NodeOffsetPointXY_addGrpB[];

typedef struct Position3D_addGrpB {
  LatitudeDMS2 latitude;
  LongitudeDMS2 longitude;
  Elevation elevation;
} Position3D_addGrpB;


extern const ASN1CType asn1_type_Position3D_addGrpB[];

typedef int AltitudeValue;

extern const ASN1CType asn1_type_AltitudeValue[];

typedef enum AltitudeConfidence {
  AltitudeConfidence_alt_000_01,
  AltitudeConfidence_alt_000_02,
  AltitudeConfidence_alt_000_05,
  AltitudeConfidence_alt_000_10,
  AltitudeConfidence_alt_000_20,
  AltitudeConfidence_alt_000_50,
  AltitudeConfidence_alt_001_00,
  AltitudeConfidence_alt_002_00,
  AltitudeConfidence_alt_005_00,
  AltitudeConfidence_alt_010_00,
  AltitudeConfidence_alt_020_00,
  AltitudeConfidence_alt_050_00,
  AltitudeConfidence_alt_100_00,
  AltitudeConfidence_alt_200_00,
  AltitudeConfidence_outOfRange,
  AltitudeConfidence_unavailable,
} AltitudeConfidence;

extern const ASN1CType asn1_type_AltitudeConfidence[];

typedef struct Altitude {
  AltitudeValue value;
  AltitudeConfidence confidence;
} Altitude;


extern const ASN1CType asn1_type_Altitude[];

typedef struct Position3D_addGrpC {
  Altitude altitude;
} Position3D_addGrpC;


extern const ASN1CType asn1_type_Position3D_addGrpC[];

typedef enum EmissionType {
  EmissionType_typeA,
  EmissionType_typeB,
  EmissionType_typeC,
  EmissionType_typeD,
  EmissionType_typeE,
} EmissionType;

extern const ASN1CType asn1_type_EmissionType[];

typedef struct RestrictionUserType_addGrpC {
  BOOL emission_option;
  EmissionType emission;
} RestrictionUserType_addGrpC;


extern const ASN1CType asn1_type_RestrictionUserType_addGrpC[];

typedef struct CITSPVD_1 {
  LaneID *tab;
  size_t count;
} CITSPVD_1;

extern const ASN1CType asn1_type_CITSPVD_1[];

typedef struct CITSLog {
  int id;
  unsigned int msgid;
  int posid;
  unsigned int event;
  int display;
  int hmistate;
  DDateTime time;
} CITSLog;


extern const ASN1CType asn1_type_CITSLog[];

typedef struct CITSPVD {
  BOOL transitStatus_option;
  TransitStatus transitStatus;
  BOOL acceleration_option;
  Acceleration acceleration;
  BOOL worklanes_option;
  CITSPVD_1 worklanes;
  BOOL curlane_option;
  LaneID curlane;
  BOOL vehiclecnt_option;
  int vehiclecnt;
  BOOL servicelog_option;
  CITSLog servicelog;
} CITSPVD;


extern const ASN1CType asn1_type_CITSPVD[];

typedef struct PVD_addGrpD_1 {
  CITSPVD *tab;
  size_t count;
} PVD_addGrpD_1;

extern const ASN1CType asn1_type_PVD_addGrpD_1[];

typedef struct PVD_addGrpD {
  BOOL cits_option;
  PVD_addGrpD_1 cits;
} PVD_addGrpD;


extern const ASN1CType asn1_type_PVD_addGrpD[];

typedef struct CITSRSA_1 {
  LaneID *tab;
  size_t count;
} CITSRSA_1;

extern const ASN1CType asn1_type_CITSRSA_1[];

typedef struct CITSRSA {
  BOOL temprature_option;
  int temprature;
  BOOL humidity_option;
  int humidity;
  BOOL percent_option;
  int percent;
  BOOL limitspeed_option;
  int limitspeed;
  BOOL lanewidth_option;
  int lanewidth;
  BOOL laneset_option;
  CITSRSA_1 laneset;
} CITSRSA;


extern const ASN1CType asn1_type_CITSRSA[];

typedef struct RSA_addGrpD {
  BOOL cits_option;
  CITSRSA cits;
} RSA_addGrpD;


extern const ASN1CType asn1_type_RSA_addGrpD[];

typedef struct SignalHeadLocation {
  NodeOffsetPointXY node;
  SignalGroupID signalGroupID;
} SignalHeadLocation;


extern const ASN1CType asn1_type_SignalHeadLocation[];

typedef struct SignalHeadLocationList {
  SignalHeadLocation *tab;
  size_t count;
} SignalHeadLocationList;

extern const ASN1CType asn1_type_SignalHeadLocationList[];

typedef struct MapData_addGrpC {
  BOOL signalHeadLocations_option;
  SignalHeadLocationList signalHeadLocations;
} MapData_addGrpC;


extern const ASN1CType asn1_type_MapData_addGrpC[];

typedef int Angle_addGrpB;

extern const ASN1CType asn1_type_Angle_addGrpB[];

typedef int Day;

extern const ASN1CType asn1_type_Day[];

typedef enum DayOfWeek {
  DayOfWeek_unknown,
  DayOfWeek_monday,
  DayOfWeek_tuesday,
  DayOfWeek_wednesday,
  DayOfWeek_thursday,
  DayOfWeek_friday,
  DayOfWeek_saturday,
  DayOfWeek_sunday,
} DayOfWeek;

extern const ASN1CType asn1_type_DayOfWeek[];

typedef int Elevation_addGrpB;

extern const ASN1CType asn1_type_Elevation_addGrpB[];

typedef enum Holiday {
  Holiday_weekday,
  Holiday_holiday,
} Holiday;

extern const ASN1CType asn1_type_Holiday[];

typedef int Hour;

extern const ASN1CType asn1_type_Hour[];

typedef int Minute;

extern const ASN1CType asn1_type_Minute[];

typedef int Month;

extern const ASN1CType asn1_type_Month[];

typedef int MsgCount_addGrpB;

extern const ASN1CType asn1_type_MsgCount_addGrpB[];

typedef int Second;

extern const ASN1CType asn1_type_Second[];

typedef enum SummerTime {
  SummerTime_notInSummerTime,
  SummerTime_inSummerTime,
} SummerTime;

extern const ASN1CType asn1_type_SummerTime[];

typedef int TenthSecond;

extern const ASN1CType asn1_type_TenthSecond[];

typedef int Year;

extern const ASN1CType asn1_type_Year[];

typedef struct TimeMark_addGrpB {
  Year year;
  Month month;
  Day day;
  SummerTime summerTime;
  Holiday holiday;
  DayOfWeek dayofWeek;
  Hour hour;
  Minute minute;
  Second second;
  TenthSecond tenthSecond;
} TimeMark_addGrpB;


extern const ASN1CType asn1_type_TimeMark_addGrpB[];

#ifdef  __cplusplus
}
#endif

#endif /* _FFASN1_J2735_201603_CITS_H */
