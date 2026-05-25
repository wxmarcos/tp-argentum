#ifndef COMMON_SENDER_H
#define COMMON_SENDER_H

#include "thread.h"
#include "network/socket.h"
#include "queue.h"
#include <string>

template <typename T>
class Sender: public Thread {
    private:
    Socket& socket;
    Queue<T>& queue;

    public:
    Sender(Socket& socket, Queue<T>& queue);
    ~Sender() override;

    void run() override;
};

template <typename T>
Sender<T>::Sender(Socket& socket, Queue<T>& queue)
    : socket(socket), queue(queue) {}

template <typename T>
Sender<T>::~Sender() = default;

template <typename T>
void Sender<T>::run() {
    try {
        T item;
        while (should_keep_running()) {
            try {
                item = queue.pop();
                if (item.is_disconnect()) {
                    break;
                }
                std::string response = item.text();
                socket.sendall(response.c_str(), response.size());
            } catch (const ClosedQueue&) {
                break;
            }
        }
    } catch (const std::exception&) {
    }
}

#endif
