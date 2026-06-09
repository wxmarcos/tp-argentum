#include "server/client/monitor_clients.h"

size_t MonitorClients::size() const {
    std::lock_guard<std::mutex> lock(mutex);
    return clients.size();
}

void MonitorClients::add(std::unique_ptr<ClientHandler> client) {
    std::lock_guard<std::mutex> lock(mutex);
    clients.push_back(std::move(client));
}

void MonitorClients::remove_finished() {
    std::lock_guard<std::mutex> lock(mutex);

    auto it = clients.begin();

    while (it != clients.end()) {
        ClientHandler* client = it->get();

        if (client && !client->is_alive()) {
            client->join();
            it = clients.erase(it);
        } else {
            ++it;
        }
    }
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
