#ifndef COMMON_SNAPSHOT_CODEC_H
#define COMMON_SNAPSHOT_CODEC_H

#include "common/protocol_defs.h"
#include "common/snapshot/snapshot.h"

class Socket;

uint16_t snapshot_payload_size(const Snapshot& snapshot);
void send_snapshot_payload(Socket& socket, const Snapshot& snapshot);
Snapshot recv_snapshot_payload(Socket& socket, protocol::ServerOpcode opcode,
                               uint16_t payload_size);

#endif  // COMMON_SNAPSHOT_CODEC_H
