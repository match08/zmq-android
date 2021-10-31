#include <jni.h>
#include <string>
#include <map>
#include <cppzmq/zmq.hpp>
#include <cppzmq/zmq_addon.hpp>

#include <weismarts/zmq/ZMQSocket.h>
#include <weismarts/zmq/ZMQPoller.h>
#include <weismarts/zmq/ZMsg.h>


std::vector<ZMQSocket*> sockets;
std::vector<ZMQPoller*> pollers;
std::unordered_map<ZMQSocket*, jobject> jobjectLists;

std::map<int, zmq::socket_type> SOCKET_TYPES
={
    {ZMQ_PAIR, zmq::socket_type::pair},
    {ZMQ_PUB, zmq::socket_type::pub},
    {ZMQ_SUB, zmq::socket_type::sub},
    {ZMQ_REQ, zmq::socket_type::req},
    {ZMQ_REP, zmq::socket_type::rep},
    {ZMQ_DEALER, zmq::socket_type::dealer},
    {ZMQ_ROUTER, zmq::socket_type::router},
    {ZMQ_PULL, zmq::socket_type::pull},
    {ZMQ_PUSH, zmq::socket_type::push},
    {ZMQ_XPUB, zmq::socket_type::xpub},
    {ZMQ_XSUB, zmq::socket_type::xsub},
    {ZMQ_STREAM, zmq::socket_type::stream}
};

std::map<int, zmq::event_flags> POLLER_EVENTS
={
        {0, zmq::event_flags::none},
        {ZMQ_POLLIN, zmq::event_flags::pollin},
        {ZMQ_POLLOUT, zmq::event_flags::pollout},
        {ZMQ_POLLPRI, zmq::event_flags::pollpri},
        {ZMQ_POLLERR, zmq::event_flags::pollerr}

};


void jArrayList2Cpp(JNIEnv *env,  jmethodID &java_util_ArrayList_get, jmethodID &java_util_ArrayList_size) {

    jclass java_util_ArrayList;
    java_util_ArrayList      = static_cast<jclass>(env->NewGlobalRef(env->FindClass("java/util/ArrayList")));
    java_util_ArrayList_size = env->GetMethodID (java_util_ArrayList, "size", "()I");
    java_util_ArrayList_get  = env->GetMethodID(java_util_ArrayList, "get", "(I)Ljava/lang/Object;");

}

std::vector<std::string> jZMsgs2Buffers(JNIEnv *env, jobject arrayList) {

    jclass java_util_ArrayList;
    jmethodID java_util_ArrayList_size;
    jmethodID java_util_ArrayList_get;
    java_util_ArrayList      = static_cast<jclass>(env->NewGlobalRef(env->FindClass("java/util/ArrayList")));
    java_util_ArrayList_size = env->GetMethodID (java_util_ArrayList, "size", "()I");
    java_util_ArrayList_get  = env->GetMethodID(java_util_ArrayList, "get", "(I)Ljava/lang/Object;");

    jint len = env->CallIntMethod(arrayList, java_util_ArrayList_size);
    std::vector<std::string> result;
    result.reserve(len);
    for (jint i=0; i<len; i++) {

        jobject element = env->CallObjectMethod(arrayList, java_util_ArrayList_get, i);
        jclass ZFrameClssz = env->GetObjectClass(element);
        //java ZFrame
        jmethodID dataMethodID =  env->GetMethodID(ZFrameClssz, "getData", "()[B");
        jbyteArray dataChrs = (jbyteArray)env->CallNonvirtualObjectMethod(element, ZFrameClssz, dataMethodID);

        jbyte * adrChars = env->GetByteArrayElements(dataChrs,JNI_FALSE);
        std::string addrstr = std::string((char*)adrChars,env->GetArrayLength(dataChrs));
        result.emplace_back(addrstr);
        env->ReleaseByteArrayElements(dataChrs, adrChars, 0);
        env->DeleteLocalRef(element);
    }
    return result;
}
std::string jZFrame2String(JNIEnv *env, jobject zFrame)
{
    jclass ZFrameClssz = env->GetObjectClass(zFrame);
    //java ZFrame
    jmethodID dataMethodID =  env->GetMethodID(ZFrameClssz, "getData", "()[B");
    jbyteArray dataChrs = (jbyteArray)env->CallNonvirtualObjectMethod(zFrame, ZFrameClssz, dataMethodID);

    jbyte * adrChars = env->GetByteArrayElements(dataChrs,JNI_FALSE);

    std::string msgDataStr = std::string((char*)adrChars,env->GetArrayLength(dataChrs));

    env->ReleaseByteArrayElements(dataChrs, adrChars, 0);
    env->DeleteLocalRef(zFrame);
    return msgDataStr;
}

jstring charToJString(JNIEnv *env, char *pat) {
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray(strlen(pat));
    env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte *) pat);
    jstring encoding = env->NewStringUTF("utf-8");
    return (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
}

jstring charToJStringN(JNIEnv *env, char *pat, size_t len) {
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray(len);
    env->SetByteArrayRegion(bytes, 0, len,(jbyte*)pat);
    jstring encoding = env->NewStringUTF("utf-8");
    return (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
}

jbyteArray charToJByteArray(JNIEnv *env, char *buf, size_t len) {
    jbyteArray array = env->NewByteArray(len);
    env->SetByteArrayRegion(array, 0, len, reinterpret_cast<jbyte *>(buf));
    return array;
}

char *jByteArrayToChar(JNIEnv *env, jbyteArray buf) {
    char *chars = NULL;
    jbyte *bytes;
    bytes = env->GetByteArrayElements(buf, 0);
    int chars_len = env->GetArrayLength(buf);
    chars = new char[chars_len + 1];
    memset(chars, 0, chars_len + 1);
    memcpy(chars, bytes, chars_len);
    chars[chars_len] = 0;
    env->ReleaseByteArrayElements(buf, bytes, 0);
    return chars;
}
char *jByteArrayToCharN(JNIEnv *env, jbyteArray buf, const size_t size) {
    const size_t TOTAL_SIZE = size+1;
    char chars[TOTAL_SIZE];
    jbyte *bytes;
    bytes = env->GetByteArrayElements(buf, 0);
    for (int i = 0; i < size; ++i) {
        chars[i] = bytes[i];
    }
    chars[size] = 0;
    env->ReleaseByteArrayElements(buf, bytes, 0);
    return chars;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// export to java
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" JNIEXPORT jint JNICALL
Java_com_weismarts_librarys_ZMQSocket_createSocket(
        JNIEnv* env,
        jobject obj,
        jint type) {


    ZMQSocket *mZMQSocket = new ZMQSocket( SOCKET_TYPES[type]);
    sockets.push_back(mZMQSocket);

    int id = sockets.size()-1;
//    jclass clazz =  env->GetObjectClass(obj);
//    jmethodID mid = env->GetMethodID(clazz,"getID", "()I");
//    jint id = env->CallIntMethod(obj, mid);

    return id;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_weismarts_librarys_ZMQSocket_connect(
        JNIEnv* env,
        jobject obj,
        jbyteArray adr)
{


    jclass clazz =  env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(clazz,"getID", "()I");
    jint id = env->CallIntMethod(obj, mid);

    jbyte * adrChars = env->GetByteArrayElements(adr,JNI_FALSE);
    std::string str = std::string((char*)adrChars,env->GetArrayLength(adr));
    env->ReleaseByteArrayElements(adr, adrChars, 0);
    sockets[id]->Connect(str);

    return JNI_TRUE;
}
extern "C" JNIEXPORT void JNICALL
Java_com_weismarts_librarys_ZMQSocket_setIdentity(
        JNIEnv* env,
        jobject obj,
        jbyteArray value)
{


    jclass clazz =  env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(clazz,"getID", "()I");
    jint id = env->CallIntMethod(obj, mid);

    jbyte * adrChars = env->GetByteArrayElements(value,JNI_FALSE);
    std::string str = std::string((char*)adrChars,env->GetArrayLength(value));
    env->ReleaseByteArrayElements(value, adrChars, 0);

    sockets[id]->SetIdentity(str);

}
extern "C" JNIEXPORT void JNICALL
Java_com_weismarts_librarys_ZMQSocket_setImmediate(
        JNIEnv* env,
        jobject obj,
        jboolean value)
{

    jclass clazz =  env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(clazz,"getID", "()I");
    jint id = env->CallIntMethod(obj, mid);

    sockets[id]->SetImmediate(value);

}
extern "C" JNIEXPORT void JNICALL
Java_com_weismarts_librarys_ZMQSocket_setLinger(
        JNIEnv* env,
        jobject obj,
        jint value)
{

    jclass clazz =  env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(clazz,"getID", "()I");
    jint id = env->CallIntMethod(obj, mid);

    sockets[id]->SetLinger(value);

}
extern "C" JNIEXPORT void JNICALL
Java_com_weismarts_librarys_ZMQSocket_close(
        JNIEnv* env,
        jobject obj/* this */)
{
    jclass clazz =  env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(clazz,"getID", "()I");
    jint id = env->CallIntMethod(obj, mid);
    sockets[id]->Close();
}


//extern "C" JNIEXPORT void JNICALL
//Java_com_weismarts_librarys_ZMQSocket_sendMultipart(
//        JNIEnv* env,
//        jobject obj, jobject multMsgs)
//{
//    jclass clazz =  env->GetObjectClass(obj);
//    jmethodID mid = env->GetMethodID(clazz,"getID", "()I");
//    jint id = env->CallIntMethod(obj, mid);
//
//    std::vector<std::string> sendMsgs = jZMsgs2Buffers(env, multMsgs);
//
////    sockets[id]->SendMultipart(sendMsgs);
//
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Poller
//////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" JNIEXPORT jint JNICALL
Java_com_weismarts_librarys_ZMQPoller_createPoller(
        JNIEnv* env,
        jobject obj) {


    ZMQPoller *poller = new ZMQPoller();
    pollers.push_back(poller);
    int id = pollers.size()-1;

    return id;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_weismarts_librarys_ZMQPoller_poll(
        JNIEnv* env,
        jobject obj,
        jlong timeout) {

    //id
    jclass clazz =  env->GetObjectClass(obj);
    jmethodID mId = env->GetMethodID(clazz,"getID", "()I");
    jint id = env->CallIntMethod(obj, mId);

    return pollers[id]->poll(std::chrono::milliseconds(timeout));

}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_weismarts_librarys_ZMQPoller_pollin(
        JNIEnv* env,
        jobject obj,
        jlong index) {

    //id
    jclass clazz =  env->GetObjectClass(obj);
    jmethodID mId = env->GetMethodID(clazz,"getID", "()I");
    jint id = env->CallIntMethod(obj, mId);

    zmq::poller_event<handler_type> event =  pollers[id]->pollin(index);

    if(event.user_data!= nullptr) return JNI_TRUE;

    return JNI_FALSE;

}

void __jcallback(JNIEnv* env, jobject callback, int eventType)
{
    jclass callbackClazz = env->GetObjectClass(callback);
    jmethodID listenerID = env->GetMethodID(callbackClazz,"listener", "(I)V");

    jvalue eventName[1];
    eventName[0].i = eventType;
    env->CallNonvirtualVoidMethodA(callback, callbackClazz, listenerID, eventName);
}


extern "C" JNIEXPORT void JNICALL
Java_com_weismarts_librarys_ZMQPoller_add(
        JNIEnv* env,
        jobject obj,
        jint socketID,
        jint eventType, jobject callback) {

    //id
    jclass clazz =  env->GetObjectClass(obj);
    jmethodID mId = env->GetMethodID(clazz,"getID", "()I");
    jint id = env->CallIntMethod(obj, mId);

    JavaVM *gVM;
    //JavaVM是虚拟机在JNI中的表示，等下再其他线程回调java层需要用到
    env->GetJavaVM(&gVM);
    //生成一个全局引用，回调的时候findclass才不会为null
    jobject jcallback = env->NewGlobalRef(callback);
    jobjectLists.emplace(sockets[socketID], jcallback);

    pollers[id]->add(sockets[socketID],
         POLLER_EVENTS[eventType], [gVM, jcallback](zmq::event_flags eventFlags)->void{
                JNIEnv *env;
                //获取当前native线程是否有没有被附加到jvm环境中
                int envStat = gVM->GetEnv((void **)   &env,JNI_VERSION_1_6);
                if(envStat == JNI_EDETACHED)
                {
                    //如果没有， 主动附加到jvm环境中，获取到env
                    if (gVM->AttachCurrentThread(&env, NULL) != 0) {
                        return;
                    }

                }

                jclass callbackClazz = env->GetObjectClass(jcallback);
                jmethodID listenerID = env->GetMethodID(callbackClazz,"listener", "(I)V");

                jvalue eventName[1];
                eventName[0].i = static_cast<int>(eventFlags);
                env->CallNonvirtualVoidMethodA(jcallback, callbackClazz, listenerID, eventName);

//                gVM->DestroyJavaVM();


            });

}
extern "C" JNIEXPORT void JNICALL
Java_com_weismarts_librarys_ZMQPoller_remove(
        JNIEnv* env,
        jobject obj,
        jint socketID) {

    //id
    jclass clazz =  env->GetObjectClass(obj);
    jmethodID mId = env->GetMethodID(clazz,"getID", "()I");
    jint id = env->CallIntMethod(obj, mId);

    ZMQSocket* zmqSocket = sockets[socketID];
    jobject gCallback = jobjectLists.at(zmqSocket);

    jobjectLists.erase(zmqSocket);
    pollers[id]->remove(zmqSocket);

    //释放你的全局引用的接口
    env->DeleteGlobalRef(gCallback);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// ZMsg
//////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" JNIEXPORT jboolean JNICALL
Java_com_weismarts_librarys_ZMsg_send(
        JNIEnv* env,
        jobject obj,
        jint socketID,
        jobject msg, jint flags) {

      return !!sockets[socketID]->getPtr()->send(zmq::buffer(jZFrame2String(env, msg)), flags==0? zmq::send_flags::none : zmq::send_flags::sndmore);
}



extern "C" JNIEXPORT jboolean JNICALL
Java_com_weismarts_librarys_ZMsg_sendMultipart(
        JNIEnv* env,
        jobject obj,
        jint socketID,
        jobject multMsgs, jint flags) {

    std::vector<std::string> msgs = jZMsgs2Buffers(env, multMsgs);
    ZMsg *zMsg  = new ZMsg();
    zMsg->AddAll(msgs);
    bool send = zMsg->Send(sockets[socketID], flags);
    zMsg->Destroy();
    zMsg = NULL;
    return send;
}


extern "C" JNIEXPORT jobject JNICALL
Java_com_weismarts_librarys_ZMsg_recvMsg(
        JNIEnv* env,
        jclass obj,
        jint socketID) {

    ZMsg zMsg =  ZMsg::RecvMsg(sockets[socketID]);

    jobject newMsg = env->NewObject(obj, env->GetMethodID(obj, "<init>", "()V"));
//    jmethodID addMethodID = env->GetMethodID(obj, "add", "(Ljava/lang/String;)V");
    jmethodID addMethodID = env->GetMethodID(obj, "add", "([B)V");
    //    ZFrame
    size_t len = zMsg.Size();
    for (size_t i = 0; i < len; ++i) {
        std::string str =  zMsg.Peekstr(i);
        const size_t len = str.length();
        const size_t TOTAL_SIZE  = len+1;
        char cstr[TOTAL_SIZE];
        for (int i = 0; i < len; ++i) {
            char c_str = str[i];
            cstr[i] = c_str;
        }
        cstr[len] = 0;
        env->CallVoidMethod(newMsg, addMethodID, charToJByteArray(env, cstr, len));
//        env->ReleaseStringUTFChars(jtstr, str);
    }
    return newMsg;
}