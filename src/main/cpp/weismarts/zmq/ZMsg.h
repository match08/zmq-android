//
// Created by weismarts on 9/26/21.
//

#ifndef ZMQLIBRARY_ZMSG_H
#define ZMQLIBRARY_ZMSG_H

#include <string>
#include <cppzmq/zmq.hpp>
#include <cppzmq/zmq_addon.hpp>

#include <weismarts/zmq/ZMQSocket.h>

class ZMsg {
public:
    ZMsg();

    void Add(char* msg);
    void Add(zmq::message_t &msg);
    void Add(std::string msg);
    void AddAll(std::vector<std::string> msgs);
    void Destroy();
    void Send(ZMQSocket *socket);

    size_t Size();
    std::string Peekstr(size_t index);

    static ZMsg RecvMsg(ZMQSocket *socket);

private:
    zmq::multipart_t _multipart;



};


#endif //ZMQLIBRARY_ZMSG_H
