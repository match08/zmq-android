//
// Created by weismarts on 9/24/21.
//

#ifndef ZMQLIBRARY_ZMQSOCKET_H
#define ZMQLIBRARY_ZMQSOCKET_H

#include <string>
#include <cppzmq/zmq.hpp>
#include <cppzmq/zmq_addon.hpp>

class ZMQSocket {



private:
    zmq::context_t ctx;
    zmq::socket_t *sock;
public:
    ZMQSocket(zmq::socket_type type);
    void SetImmediate(bool value);
    void SetIdentity(std::string value);
    void SetLinger(int value);
    void Connect(const char* adr);
    void Connect(const std::string adr);
    void Close();
    void SendMultipart(std::vector<zmq::const_buffer> multMsgs);
    void SendMultipart(std::vector<zmq::message_t> multMsgs);

    zmq::socket_t *getPtr()
    {
        return sock;
    }
};


#endif //ZMQLIBRARY_ZMQSOCKET_H
