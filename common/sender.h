#ifndef COMMON_SENDER_H
#define COMMON_SENDER_H

#include "network/socket.h"
#include "queue.h"
#include "thread.h"

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
Sender<T>::Sender(Socket& socket, Queue<T>& queue):
    socket(socket), queue(queue) {}

template <typename T>
Sender<T>::~Sender() = default;

template <typename T>
void Sender<T>::run() {
    try {
        while (should_keep_running()) {
            T item = queue.pop();

            item.send(socket);
        }

    } catch (const ClosedQueue&) {
    } catch (const std::exception& ex) {
        std::cerr << "[Sender] " << ex.what() << "\n";
    }
}

#endif
