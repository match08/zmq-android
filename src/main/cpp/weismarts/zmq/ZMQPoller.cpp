//
// Created by weismarts on 9/26/21.
//

#include "ZMQPoller.h"


ZMQPoller::ZMQPoller()
{
       poller = new zmq::active_poller_t();
};

void ZMQPoller::add(ZMQSocket *socket, zmq::event_flags eventFlags, handler_type handler)
{
       poller->add(*socket->getPtr(), eventFlags, handler);
}

void ZMQPoller::remove(ZMQSocket *socket)
{
       poller->remove(*socket->getPtr());
}

size_t ZMQPoller::poll(std::chrono::milliseconds timeout)
{
    return poller->wait(timeout);
}

zmq::poller_event<handler_type> ZMQPoller::pollin(size_t n)
{
    return poller->pollin(n);
}

