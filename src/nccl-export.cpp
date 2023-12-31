// Copyright (c) 2023 Arista Networks, Inc.
// Use of this source code is governed by the Apache License 2.0
// that can be found in the COPYING file.

#include "api.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>

char hostname[ 64 ];

ncclResult_t
init_v1() {
   if ( gethostname( hostname, sizeof( hostname ) ) ) {
      perror( "gethostname() failed" );
      strcpy( hostname, "gpu???" );
   } else {
      *strchrnul( hostname, '.' ) = 0;
   }
   return ncclSuccess;
}

ncclResult_t
exit_v1() {
   return ncclSuccess;
}

std::string
debug( FlowDirection_v1 dir ) {
   switch ( dir ) {
   case SEND:
      return "SEND";
   case RECEIVE:
      return "RECV";
   }
   return "????";
}

std::string
debug( FlowTopology_v1 topo ) {
   switch ( topo ) {
   case COLLECTIVE:
      return "COLL";
   case P2P:
      return "PTOP";
   }
   return "????";
}

std::string
debug( FlowTrafficType_v1 type ) {
   switch ( type ) {
   case UDP:
      return "UDP";
   case TCP:
      return "TCP";
   case ROCEv2:
      return "RCE";
   }
   return "???";
}

std::string
debugIp( const struct IpGenAddr_v1 & genAddr ) {
   int addressFamily = genAddr.isIpv4 ? AF_INET : AF_INET6;
   char myAddr[ 48 ];
   if ( inet_ntop( addressFamily, &genAddr.addr, myAddr, 48 ) ) {
      return std::string( myAddr );
   }
   return "???";
}

std::string
rank( uint32_t rank ) {
   return rank == UINT32_MAX ? "-" : std::to_string( rank );
}

ncclResult_t
exportFlow_v1( const Flow_v1 * flow ) {
   std::cout << hostname << ": FLOW 0x" << std::hex << flow->commHash << std::dec
             << " " << debug( flow->direction ) << " src=" << flow->srcRank
             << " dst=" << flow->dstRank << " channel=" << flow->channel << " "
             << debug( flow->topology ) << " " << debug( flow->trafficType ) << " "
             << debugIp( flow->srcIp ) << ":" << flow->srcPort << " <-> "
             << debugIp( flow->dstIp ) << ":" << flow->dstPort
             << " srcQPair=" << flow->srcQPair << " dstQPair=" << flow->dstQPair
             << "\n";
   return ncclSuccess;
}

ncclResult_t
exportComm_v1( const Comm_v1 * comm ) {
   std::cout << hostname << ": COMM 0x" << std::hex << comm->commHash << std::dec
             << " rank=" << comm->tpRank << " node=" << comm->node
             << " channels=" << comm->nChannels << "\n";
   std::cout << hostname << ": RING 0x" << std::hex << comm->commHash << std::dec;
   for ( int i = 0; i < comm->nChannels; ++i ) {
      std::cout << " (" << rank( comm->ring[ i ].tpPrev ) << " -> " << comm->rank
                << " -> " << rank( comm->ring[ i ].tpNext ) << ")";
   }
   std::cout << "\n"
             << hostname << ": TREE 0x" << std::hex << comm->commHash << std::dec;
   for ( int i = 0; i < comm->nChannels; ++i ) {
      std::cout << " (" << rank( comm->tree[ i ].tpUp ) << " -> " << comm->rank
                << " -> " << rank( comm->tree[ i ].tpDown[ 0 ] ) << ","
                << rank( comm->tree[ i ].tpDown[ 1 ] ) << ","
                << rank( comm->tree[ i ].tpDown[ 2 ] ) << ")";
   }
   std::cout << "\n";
   return ncclSuccess;
}

ncclResult_t
exportDone_v1( uint64_t commHash, uint32_t tpRank ) {
   std::cout << hostname << ": DONE 0x" << std::hex << commHash << std::dec
             << " rank=" << tpRank << "\n";
   return ncclSuccess;
}

const FlowExport_v1 flowExport_v1{
   .init = init_v1,
   .exit = exit_v1,
   .exportFlow = exportFlow_v1,
   .exportComm = exportComm_v1,
   .exportDone = exportDone_v1,
};
