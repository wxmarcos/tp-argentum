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

void MonitorClients::send_to(uint16_t player_id, const Snapshot& snapshot) {
    std::lock_guard<std::mutex> lock(mutex);

    for (auto& client : clients) {
        if (client && client->get_id() == player_id) {
            client->push(snapshot);
            return;
        }
    }
}

void MonitorClients::send_to_many(const std::vector<uint16_t>& player_ids,
                                  const Snapshot& snapshot) {
    std::lock_guard<std::mutex> lock(mutex);

    for (auto& client : clients) {
        if (!client) continue;

        for (uint16_t id : player_ids) {
            if (client->get_id() == id) {
                client->push(snapshot);
                break;
            }
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
