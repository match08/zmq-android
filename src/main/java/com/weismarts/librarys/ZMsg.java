package com.weismarts.librarys;

import java.nio.charset.Charset;
import java.util.ArrayDeque;
import java.util.Iterator;

public class ZMsg {
    private ArrayDeque<ZFrame> _msgs;
    public ZMsg()
    {
        _msgs = new ArrayDeque<ZFrame>();
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
    public boolean send(ZMQSocket socket)
    {
        return  send(socket, true);
    }
    public boolean send(ZMQSocket socket, boolean destroy)
    {
        if (_msgs.size() == 0) {
            return true;
        }

        boolean ret = true;
        Iterator<ZFrame> i = _msgs.iterator();
        while (i.hasNext()) {
            ZFrame f = i.next();
            send(socket.getID(), f.getData(), f.size(),(i.hasNext()) ? 2 : 0);
            if (!ret) {
                break;
            }
        }
        if (destroy) {
            destroy();
        }
        return ret;
//
    }

    public ZFrame getLast()
    {
        return _msgs.peekLast();
    }


    static public ZMsg recvMsg(ZMQSocket socket)
    {
        return recvMsg(socket.getID());
    }

    private native boolean send(int socketID, byte[] msg, int len, int flags);//1 or 2 (1-none,2-more)
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

        public int size()
        {
            if (_data!=null) {
                return _data.length;
            }
            else {
                return 0;
            }
        }
    }

}


