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
    void broadcast(const Snapshot& snapshot);

    void stop_all();
    void join_all();
    void clear();
};

#endif
