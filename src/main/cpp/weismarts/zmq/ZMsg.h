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
    void Add(void* msg, size_t len);
    void Add(zmq::message_t &msg);
    void Add(std::string msg);
    void AddAll(std::vector<std::string> msgs);
    void Destroy();
    bool Send(ZMQSocket *socket, int flags=0);

    size_t Size();
    std::string Peekstr(size_t index);
    std::string Str();

    void *AtData(size_t n);

    static ZMsg RecvMsg(ZMQSocket *socket, bool wait= false);

private:
    zmq::multipart_t _multipart;
    size_t _size;



};


#endif //ZMQLIBRARY_ZMSG_H
