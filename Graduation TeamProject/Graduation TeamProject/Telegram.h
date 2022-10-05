#pragma once
#include "Object.h"

class CTelegram
{
    CGameObject* Sender;

    CGameObject* Receiver;

    MESSAGE_TYPE          Msg;

    double       DispatchTime;

    void* ExtraInfo;

public:
    CTelegram() :DispatchTime(-1),
        Sender(nullptr),
        Receiver(nullptr),
        Msg(MESSAGE_TYPE::Msg_None)
    {}


    CTelegram(double time,
        CGameObject* sender,
        CGameObject* receiver,
        MESSAGE_TYPE    msg,
        void* info = NULL) : DispatchTime(time),
        Sender(sender),
        Receiver(receiver),
        Msg(msg),
        ExtraInfo(info)
    {}

    void SetMsg(MESSAGE_TYPE msg) { Msg = msg; };
};

