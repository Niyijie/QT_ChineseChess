#ifndef NETPROTOCOL_H
#define NETPROTOCOL_H

#include <QByteArray>

//网络消息协议
#define CLINET_CONNECT_MESSAGE 1000  //客户端接入消息
#define GAME_BEGIN_MESSAGE 1001      //游戏准备就绪消息
#define CLIENT_CHESSGO_MESSAGE 1002  //玩家走棋消息
#define PLAYER_LEAVE_MESSAGE 1003    //玩家退出消息
#define PLAYER_REGRET_MESSAGE 1004   //玩家请求悔棋消息
#define PLAYER_PEACE_MESSAGE 1005    //玩家请求和棋消息
#define PLAYER_GIVEUP_MESSAGE 1006   //玩家认输消息
#define PLAYER_RESTART_MESSAGE 1007  //玩家请求重新开始消息

static QByteArray IntToByteArray(int data){
    QByteArray array;
    int len_intVar = sizeof(data);
    array.resize(len_intVar);
    memcpy(array.data(), &data, len_intVar);
    return array;
}

static int ByteArrayToInt(QByteArray array){
    int  IntVar;
    memcpy(&IntVar, array.data(), sizeof (int));
    return IntVar;
}

#endif // NETPROTOCOL_H
