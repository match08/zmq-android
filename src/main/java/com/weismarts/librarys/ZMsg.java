package com.weismarts.librarys;

import java.nio.charset.Charset;
import java.util.ArrayList;

public class ZMsg {
    private ArrayList<ZFrame> _msgs;
    public ZMsg()
    {
        _msgs = new ArrayList<ZFrame>();
    }

    public void add(String msg)
    {
        _msgs.add(new ZFrame(msg));
    }

    public void add(byte[] msg)
    {
        _msgs.add(new ZFrame(msg));
    }

    public void destroy()
    {
        _msgs.clear();
    }

    public void send(ZMQSocket socket)
    {
        send(socket.getID(), _msgs);
    }

    public ZFrame getLast()
    {
        return _msgs.get(_msgs.size()-1);
    }


    static public ZMsg recvMsg(ZMQSocket socket)
    {
        return recvMsg(socket.getID());
    }

    private native void send(int socketID, ArrayList<ZFrame> msgs);
    static public native ZMsg recvMsg(int socketID);


    // weismarts.zmq::message_t
    public class ZFrame
    {
        private byte[] _data;
        public ZFrame(String data)
        {

            if(data!=null)
            {
                _data = data.getBytes(Charset.forName("UTF-8"));
            }
        }

        public ZFrame(byte[] data)
        {
            if(data!=null)
            {
                _data = data;
            }
        }

        public byte[] getData()
        {
            return _data;
        }
    }

}


