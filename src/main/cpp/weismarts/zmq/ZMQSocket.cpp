//
// Created by weismarts on 9/24/21.
//

#include "ZMQSocket.h"


ZMQSocket::ZMQSocket(zmq::socket_type type)
{

    sock = new zmq::socket_t(ctx, type);

}

void ZMQSocket::SetImmediate(bool value)
{

    sock->set(zmq::sockopt::immediate, value);
}

void ZMQSocket::SetIdentity(std::string value)
{
    sock->set(zmq::sockopt::routing_id, value);
}

void ZMQSocket::SetLinger(int value)
{
    sock->set(zmq::sockopt::linger, value);
}

void ZMQSocket::Connect(const char* adr)
{

    sock->connect(adr);
}

void ZMQSocket::Connect(const std::string adr)
{

    sock->connect(adr);
}

void ZMQSocket::Close()
{
    sock->close();
    ctx.close();
}
void ZMQSocket::SendMultipart(std::vector<zmq::const_buffer> multMsgs)
{
    zmq::send_multipart(*sock, multMsgs);
}
void ZMQSocket::SendMultipart(std::vector<zmq::message_t> multMsgs)
{
    zmq::send_multipart(*sock, multMsgs);
}