//
// Created by weismarts on 9/26/21.
//

#ifndef ZMQLIBRARY_ZMQPOLLER_H
#define ZMQLIBRARY_ZMQPOLLER_H

#include <string>
#include <cppzmq/zmq.hpp>
#include <cppzmq/zmq_addon.hpp>

#include <weismarts/zmq/ZMQSocket.h>

using handler_type = std::function<void(zmq::event_flags)>;

class ZMQPoller {

private:
    zmq::active_poller_t *poller;
public:
    ZMQPoller();

    void add(ZMQSocket *socket, zmq::event_flags eventFlags, handler_type handler);
    void modify(ZMQSocket *socket, zmq::event_flags eventFlags);
    void remove(ZMQSocket *socket);
    size_t poll(std::chrono::milliseconds timeout);
    zmq::poller_event<handler_type> pollin(size_t n);

};


#endif //ZMQLIBRARY_ZMQPOLLER_H
