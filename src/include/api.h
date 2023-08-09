// Copyright (c) 2023 Arista Networks, Inc.  All rights reserved.
// Arista Networks, Inc. Confidential and Proprietary.

#ifndef CLB_API_H
#define CLB_API_H

#include <cstdint>

extern "C" {

// TODO: Remove these and use the official definitions in NCCL.
#define NCCL_MAX_TREE_ARITY 3
#define MAXCHANNELS 32
enum ncclResult_t {
   ncclSuccess = 0,
   ncclSystemError = 2,
};

// -------------------------------------------------------
// Plugin API

enum FlowDirection_v1 {
   SEND,
   RECEIVE,
};

enum FlowTopology_v1 {
   COLLECTIVE,
   P2P,
};

enum FlowTrafficType_v1 {
   UDP,
   TCP,
   ROCEv2,
};

struct Flow_v1 {
   uint64_t commHash;
   FlowDirection_v1 direction;
   uint32_t srcRank;
   uint32_t srcLocalRank;
   uint32_t dstRank;
   uint32_t channel;
   FlowTopology_v1 topology;
   FlowTrafficType_v1 trafficType;
   uint32_t srcIp;
   uint32_t dstIp;
   uint16_t srcPort;
   uint16_t dstPort;
   uint32_t qPair;
};

struct RingNode_v1 {
   uint32_t tpPrev;
   uint32_t tpNext;
};

struct TreeNode_v1 {
   uint32_t tpUp;
   uint32_t tpDown[ NCCL_MAX_TREE_ARITY ];
};

struct Comm_v1 {
   uint64_t commHash;

   uint32_t nRanks;
   uint32_t rank;
   uint32_t tpRank;

   uint32_t nLocalRanks;
   uint32_t localRank;
   uint32_t tpLocalRank;

   uint32_t nNodes;
   uint32_t node;

   uint32_t nChannels;
   RingNode_v1 ring[ MAXCHANNELS ];
   TreeNode_v1 tree[ MAXCHANNELS ];
};

// Plugin should define a global symbol: FlowExport_v1 flowExport_v1;
struct FlowExport_v1 {
   ncclResult_t ( *init )( const char hostname[], uint16_t port );
   ncclResult_t ( *exit )();
   ncclResult_t ( *exportFlow )( const Flow_v1 * flow );
   ncclResult_t ( *exportComm )( const Comm_v1 * comm );
   ncclResult_t ( *exportDone )( uint64_t commHash, uint32_t tpRank );
};

extern const FlowExport_v1 flowExport_v1;

} // extern "C"

#endif // CLB_API_H