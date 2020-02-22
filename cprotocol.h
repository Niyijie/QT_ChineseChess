#ifndef PROTOCOL_H
#define PROTOCOL_H
#pragma once

#include "QByteArray"

#define CLINET_CONNECT_MESSAGE 1000  //客户端接入消息
#define GAME_BEGIN_MESSAGE 1001      //游戏准备就绪消息
#define CLIENT_CHESSGO_MESSAGE 1002  //玩家走棋消息
#define SERVER_CHESSGO_MESSAGE 1003  //服务器转发的走棋消息
#define PLAYER_LEAVE_MESSAGE 1004    //玩家退出消息

QByteArray IntToByteArray(int data){
    QByteArray array;
    int len_intVar = sizeof(data);
    array.resize(len_intVar);
    memcpy(array.data(), &data, len_intVar);
    return array;
}

int ByteArrayToInt(QByteArray array){
    int  IntVar;
    memcpy(&IntVar, array.data(), sizeof (int));
    return IntVar;
}



#endif // PROTOCOL_H
