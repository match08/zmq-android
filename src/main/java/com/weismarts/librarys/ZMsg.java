package com.weismarts.librarys;

import java.nio.charset.Charset;
import java.util.ArrayDeque;
import java.util.ArrayList;
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
        return send(socket, true);
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
            send(socket.getID(), f ,(i.hasNext()) ? 2 : 0);
            if (!ret) {
                break;
            }
        }
        if (destroy) {
            destroy();
        }
        return ret;
    }
    //1 or 2 (1-none,2-more)
    public boolean sendMultipart(ZMQSocket socket)
    {
        Iterator<ZFrame> i = _msgs.iterator();
        ArrayList<ZFrame> msgs = new ArrayList<>();
        while (i.hasNext()) {
            ZFrame f = i.next();
            msgs.add(f);
        }
        return sendMultipart(socket.getID(),msgs, 2);
    }

    public ZFrame getLast()
    {
        return _msgs.peekLast();
    }


    static public ZMsg recvMsg(ZMQSocket socket)
    {
        return recvMsg(socket.getID());
    }

    private native boolean send(int socketID, ZFrame msg, int flags);//1 or 2 (1-none,2-more)

    private native boolean sendMultipart(int socketID, ArrayList<ZFrame> msgs, int flags);//1 or 2 (1-none,2-more)
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

        public String toString()
        {
            return new String(_data, Charset.forName("UTF-8"));
        }
    }

}


