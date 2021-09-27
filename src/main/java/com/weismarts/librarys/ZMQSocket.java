package com.weismarts.librarys;

import android.util.Log;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

public class ZMQSocket {
    /*  Socket types.                                                             */
    final public static int ZMQ_PAIR = 0;
    final public static int ZMQ_PUB = 1;
    final public static int ZMQ_SUB = 2;
    final public static int ZMQ_REQ = 3;
    final public static int ZMQ_REP = 4;
    final public static int ZMQ_DEALER = 5;
    final public static int ZMQ_ROUTER = 6;
    final public static int ZMQ_PULL = 7;
    final public static int ZMQ_PUSH = 8;
    final public static int ZMQ_XPUB = 9;
    final public static int ZMQ_XSUB = 10;
    final public static int ZMQ_STREAM = 11;

    static {
        System.loadLibrary("ws-zmq");
    }

    public int _id=-1;

    public ZMQSocket(int type)
    {
        _id = createSocket(type);
    }

    public boolean connect(String addr)
    {

        try {
            return  connect(addr.getBytes("UTF-8"));
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            return false;
        }
    }

    public int getID()
    {
        return _id;
    }

    public void setIdentity(String value)
    {
        try {
            setIdentity(value.getBytes("UTF-8"));
        } catch (UnsupportedEncodingException e) {
            Log.d("ZMQSocket", "setIdentity: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private native int createSocket(int type);
    private native boolean connect(byte[] addr);


    public native void setIdentity(byte[] value);
    public native void setImmediate(boolean value);
    public native void setLinger(int value);
    public native void close();
    public native void sendMultipart(ArrayList<String> multMsgs);

}
