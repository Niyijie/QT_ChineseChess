#include "widget.h"
#include "ui_widget.h"
#include "netProtocol.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    server = new QTcpServer(this);
    server->listen(QHostAddress::Any,8888);
    ui->textEdit->append("服务器开启成功 端口8888\n");
    clientRed = nullptr;
    clientBlack = nullptr;
    connect(server, &QTcpServer::newConnection,
                [=]() mutable {
                        if(clientRed&&clientBlack){
                            ui->textEdit->append("人数已达两个\n");
                            return;
                        }
                        QTcpSocket* conSocket = server->nextPendingConnection();
                        connect(conSocket, &QTcpSocket::readyRead,
                                [=](){
                                    //如果红方写消息来
                                    if(clientRed == conSocket){
                                        ui->textEdit->append("红方写消息\n");
                                        QByteArray msgType = conSocket->read(sizeof (int));
                                        ServerParseMessage(ByteArrayToInt(msgType),conSocket,clientBlack);
                                    }
                                    //如果黑方写消息来
                                    else if(clientBlack == conSocket){
                                        ui->textEdit->append("黑方写消息\n");
                                        QByteArray msgType = conSocket->read(sizeof (int));
                                        ServerParseMessage(ByteArrayToInt(msgType),conSocket,clientRed);
                                    }
                                });
                        if(!clientRed){
                            clientRed = conSocket;
                            ui->textEdit->append("红方连接成功\n");
                        }
                        else if(!clientBlack){
                            clientBlack = conSocket;
                            ui->textEdit->append("黑方连接成功\n");
                        }
                        //两人人数都已经到齐 游戏开始
                        if(clientRed && clientBlack){
                            //给红方写消息
                            clientRed->write(IntToByteArray(GAME_BEGIN_MESSAGE));
                            //规定其持红棋
                            clientRed->write(IntToByteArray(REDSIDE));
                            //给黑方写消息
                            clientBlack->write(IntToByteArray(GAME_BEGIN_MESSAGE));
                            //规定其持黑棋
                            clientBlack->write(IntToByteArray(BLACKSIDE));
                            ui->textEdit->append("游戏开始\n");                        }
                    });
}

Widget::~Widget()
{
    delete ui;
}


void Widget::ServerParseMessage(int message,QTcpSocket *conSocket,QTcpSocket *anotherSocket){
    switch (message) {
    case CLIENT_CHESSGO_MESSAGE:{
        if(!clientBlack || !clientRed){
            return;
        }
        //发送方走棋起点坐标
        int px1 = ByteArrayToInt(conSocket->read(sizeof (int)));
        int py1 = ByteArrayToInt(conSocket->read(sizeof (int)));
        //发送方走棋目标坐标
        int px2 = ByteArrayToInt(conSocket->read(sizeof (int)));
        int py2 = ByteArrayToInt(conSocket->read(sizeof (int)));
        //转换为对方视角下的坐标
        px1 = convertXPosition(px1);
        px2 = convertXPosition(px2);
        py1 = convertYPosition(py1);
        py2 = convertYPosition(py2);
        //转发给对手
        anotherSocket->write(IntToByteArray(CLIENT_CHESSGO_MESSAGE));
        //发送方走棋起点坐标
        anotherSocket->write(IntToByteArray(px1));
        anotherSocket->write(IntToByteArray(py1));
        //发送方走棋目标坐标
        anotherSocket->write(IntToByteArray(px2));
        anotherSocket->write(IntToByteArray(py2));
        anotherSocket->waitForBytesWritten();
        //
        if(conSocket == clientRed){
            ui->textEdit->append("红方走棋\n");
        }
        else if(conSocket == clientBlack){
            ui->textEdit->append("黑方走棋\n");
        }
        break;
    }
    case PLAYER_LEAVE_MESSAGE:{
        if(anotherSocket != nullptr){
            //告诉另一方其对手已经退出游戏
            ui->textEdit->append("通知另一方断开消息\n");
            anotherSocket->write(IntToByteArray(PLAYER_LEAVE_MESSAGE));
            anotherSocket->waitForBytesWritten();
        }
        //断开连接
        conSocket->disconnectFromHost();
        conSocket->close();
        //回收空间
        if(conSocket == clientRed){
            clientRed = nullptr;
            ui->textEdit->append("红方断开连接,等待新玩家接入\n");
        }
        else if(conSocket == clientBlack){
            clientBlack = nullptr;
            ui->textEdit->append("黑方断开连接，等待新玩家接入\n");
        }
        break;
    }
    case PLAYER_PEACE_MESSAGE:
    {
        QByteArray typeArray = conSocket->read(sizeof(int));
        anotherSocket->write(IntToByteArray(PLAYER_PEACE_MESSAGE));
        anotherSocket->write(typeArray);
        anotherSocket->waitForBytesWritten();
        break;
    }
    case PLAYER_GIVEUP_MESSAGE:{
        anotherSocket->write(IntToByteArray(PLAYER_GIVEUP_MESSAGE));
        anotherSocket->waitForBytesWritten();
        break;
    }
    case PLAYER_REGRET_MESSAGE:{
        QByteArray typeArray = conSocket->read(sizeof(int));
        anotherSocket->write(IntToByteArray(PLAYER_REGRET_MESSAGE));
        anotherSocket->write(typeArray);
        anotherSocket->waitForBytesWritten();
        break;
    }
    case PLAYER_RESTART_MESSAGE:{
        QByteArray typeArray = conSocket->read(sizeof(int));
        anotherSocket->write(IntToByteArray(PLAYER_RESTART_MESSAGE));
        anotherSocket->write(typeArray);
        anotherSocket->waitForBytesWritten();
        break;
    }
    }

}


int Widget::convertXPosition(int x){
    return 9 - x;
}

int Widget::convertYPosition(int y){
    return 8 - y;
}


