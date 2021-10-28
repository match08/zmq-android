//
// Created by weismarts on 9/26/21.
//

#include "ZMsg.h"


ZMsg::ZMsg(){

};

void ZMsg::Add(char* msg)
{
    _multipart.addstr(std::string(msg));
}

void ZMsg::Add(void* msg, size_t len)
{
    _multipart.addmem(msg, len);
}

void ZMsg::Add(std::string msg)
{
    _multipart.addstr(msg);
}
void ZMsg::Add(zmq::message_t &msg)
{
    _multipart.add(zmq::message_t(msg.data(), msg.size()));
}
void ZMsg::AddAll(std::vector<std::string> msgs)
{
    for (int i = 0; i < msgs.size(); ++i) {
        const std::string msg =  msgs[i];//std::string ((char*)msgs[i].data(), msgs[i].size());
        _multipart.addstr(msg);
    }
}

ZMsg ZMsg::RecvMsg(ZMQSocket *socket, bool wait)
{
    ZMsg zMsg;
    std::vector<zmq::message_t> recv_msgs;
    const auto ret = zmq::recv_multipart(
            *socket->getPtr(), std::back_inserter(recv_msgs), wait?  zmq::recv_flags::none : zmq::recv_flags::dontwait);

    if(!ret) return zMsg;

    const size_t len =  recv_msgs.size();
    for (int i = 0; i < len; ++i) {
        zMsg.Add(recv_msgs[i].data(), recv_msgs[i].size());
    }

    return zMsg;
}

void ZMsg::Send(ZMQSocket *socket, int flags)
{
    _multipart.send(*socket->getPtr(), flags);
}
size_t ZMsg::Size()
{
    return _multipart.size();
}
std::string ZMsg::Peekstr(size_t index)
{
    return _multipart.peekstr(index);
}

std::string ZMsg::Str()
{
    return _multipart.str();
}

void *ZMsg::AtData(size_t n) {
    return _multipart.at(n).data();
}

void ZMsg::Destroy()
{
    _multipart.clear();
}

