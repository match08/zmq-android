package com.weismarts.librarys;


import java.util.ArrayList;

//定义回调接口

public class ZMQPoller {

    // polling events

    final public static int ZMQ_NONE = 0;
    final public static int ZMQ_POLLIN = 1;
    final public static int ZMQ_POLLOUT = 2;
    final public static int ZMQ_POLLERR = 4;
    final public static int ZMQ_POLLPRI = 8;

    public interface Callback{
        void listener(int eventType);
    }

    private int _id=-1;
    private Callback _callback;

    public ZMQPoller()
    {
        _id = createPoller();
    }

    public int getID()
    {
        return _id;
    }

    public void add(ZMQSocket socket, int eventType, Callback callback)
    {
       add(socket.getID(), eventType, callback);
    }
    public void remove(ZMQSocket socket)
    {
        remove(socket.getID());
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //native public
    //////////////////////////////////////////////////////////////////////////////////////////////////////


    public native long poll(long timeout);
    public native boolean pollin(long index);



    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //native private
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    private native int createPoller();
    private native void add(int socketID, int eventType, Callback callback);
    private native void remove(int socketID);
}
