#ifndef MONITOR_CLIENTS_H_
#define MONITOR_CLIENTS_H_

#include <memory>
#include <mutex>
#include <vector>

#include "client/client_handler.h"
#include "common/snapshot/snapshot.h"

class MonitorClients {
private:
    std::vector<std::unique_ptr<ClientHandler>> clients;
    mutable std::mutex mutex;

public:
    size_t size() const;
    void add(std::unique_ptr<ClientHandler> client);
    void remove_finished();
    void send_to(uint16_t player_id, const Snapshot& snapshot);
    void send_to_many(const std::vector<uint16_t>& player_ids,const Snapshot& snapshot);
    void broadcast(const Snapshot& snapshot);

    void stop_all();
    void join_all();
    void clear();
};

#endif
