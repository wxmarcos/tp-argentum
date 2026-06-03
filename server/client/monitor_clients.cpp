#include "monitor_clients.h"

void MonitorClients::add(std::unique_ptr<ClientHandler> client) {
    std::lock_guard<std::mutex> lock(mutex);
    clients.push_back(std::move(client));
}

void MonitorClients::broadcast(const Snapshot& snapshot) {
    std::lock_guard<std::mutex> lock(mutex);

    for (auto& client : clients) {
        if (client) {
            client->push(snapshot);
        }
    }
}

void MonitorClients::stop_all() {
    std::lock_guard<std::mutex> lock(mutex);

    for (auto& client : clients) {
        if (client) {
            client->stop();
        }
    }
}

void MonitorClients::join_all() {
    std::lock_guard<std::mutex> lock(mutex);

    for (auto& client : clients) {
        if (client) {
            client->join();
        }
    }
}

void MonitorClients::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    clients.clear();
}