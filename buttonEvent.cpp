#include "widget.h"
#include <QMessageBox>
#include <QHostAddress>
#include <QMouseEvent>
#include "ui_widget.h"

/*
*  按钮事件函数
*/

//离开按钮
void Widget::on_leave_clicked(){
    if(gameMode == NETBATTLE && clientSocket){
        if( QMessageBox::question(this,tr("提示"),tr("正在进行网络对战，是否断开连接?"),QMessageBox::Yes, QMessageBox::No )
                           == QMessageBox::Yes){
                //发送玩家退出消息
                clientSocket->write(IntToByteArray(PLAYER_LEAVE_MESSAGE));
                clientSocket->waitForBytesWritten();
                //主动和对方断开连接
                clientSocket->disconnectFromHost();
                clientSocket->close();
                //回收空间
                delete clientSocket;
                clientSocket = nullptr;
                QMessageBox::information(this,"提示","网络对战断开成功....");
           }
    }
    if( QMessageBox::question(this,tr("提示"),tr("是否要退出程序?"),QMessageBox::Yes, QMessageBox::No )
                       == QMessageBox::Yes){
            this->close();
       }
}
//重新开始按钮
void Widget::on_again_clicked(){
    if((gameMode == MACHINEBATTLE || gameMode == SINGLEBATTLE) && QMessageBox::question(this,tr("提示"),tr("是否要重新开始游戏?"),QMessageBox::Yes, QMessageBox::No )
                       == QMessageBox::Yes){
            initBoard(myColor);
            gameStart = true;
            turnFlag = myColor;
       }
    if(gameMode == NETBATTLE && QMessageBox::question(this,tr("提示"),tr("是否请求重新开始游戏?"),QMessageBox::Yes, QMessageBox::No )
                       == QMessageBox::Yes){
            clientSocket->write(IntToByteArray(PLAYER_RESTART_MESSAGE));
            clientSocket->write(IntToByteArray(ASK));
       }

}
//悔棋按钮
void Widget::on_regret_clicked(){
    if(!gameStart)
        return;
    if(RegretLists.size() == 0){
        QMessageBox::information(this,"提示","已经无法悔棋");
        return;
    }
    if((gameMode == MACHINEBATTLE||gameMode == SINGLEBATTLE) && QMessageBox::question(this,tr("提示"),tr("是否悔棋?"),QMessageBox::Yes, QMessageBox::No )
                       == QMessageBox::Yes){
            //悔棋
            regret();
            if(gameMode == MACHINEBATTLE)
                regret();
    }
    if(gameMode == NETBATTLE && QMessageBox::question(this,tr("提示"),tr("是否请求悔棋?"),QMessageBox::Yes, QMessageBox::No )
        == QMessageBox::Yes){
        clientSocket->write(IntToByteArray(PLAYER_REGRET_MESSAGE));
        clientSocket->write(IntToByteArray(ASK));
    }

}
//和棋按钮
void Widget::on_peace_clicked(){
    if(!gameStart)
        return;
    //单人模式情况下
    if((gameMode == MACHINEBATTLE || gameMode == SINGLEBATTLE) && QMessageBox::question(this,tr("提示"),tr("是否要和棋?"),QMessageBox::Yes, QMessageBox::No )
                       == QMessageBox::Yes){
            ui->peaceWin->show();
            Mpeace->play();
            gameStart = false;
       }
    //网络对战情况下
    if(gameMode == NETBATTLE && QMessageBox::question(this,tr("提示"),tr("是请求和棋?"),QMessageBox::Yes, QMessageBox::No )
            == QMessageBox::Yes){
            //发送请求和棋消息
            clientSocket->write(IntToByteArray(PLAYER_PEACE_MESSAGE));
            clientSocket->write(IntToByteArray(ASK));
    }
}


void Widget::on_giveup_clicked()
{
    if(!gameStart)
        return;
    //单人模式情况下
    if((gameMode == MACHINEBATTLE || gameMode == SINGLEBATTLE) && QMessageBox::question(this,tr("提示"),tr("是否要认输?"),QMessageBox::Yes, QMessageBox::No )
                       == QMessageBox::Yes){
            if(myColor == RED)
                ui->blackWin->show();
            if(myColor == BLACK)
                ui->redWin->show();
            Mlose->play();
            gameStart = false;
       }
    //网络对战情况下
    if( gameMode == NETBATTLE && QMessageBox::question(this,tr("提示"),tr("是否要认输?"),QMessageBox::Yes, QMessageBox::No )
                       == QMessageBox::Yes){
            //发送认输消息，认输无需对方同意
            clientSocket->write(IntToByteArray(PLAYER_GIVEUP_MESSAGE));
            clientSocket->waitForBytesWritten();
            if(myColor == RED)
                ui->blackWin->show();
            if(myColor == BLACK)
                ui->redWin->show();
            Mlose->play();
            gameStart = false;
       }
}


//网络对战按钮
void Widget::on_netbattle_clicked(){
        gameMode = NETBATTLE;
        if( QMessageBox::question(this,tr("提示"),tr("是否进行网络对战?"),QMessageBox::Yes, QMessageBox::No )
                           == QMessageBox::Yes){
                if(clientSocket){
                    QMessageBox::information(this,"提示","网络对战进行中。。。。");
                    return;
                }
                clientSocket = new QTcpSocket(this);
                clientSocket->connectToHost(QHostAddress("127.0.0.1"), 8888);
                connect(clientSocket, &QTcpSocket::readyRead,
                        [=](){
                            QByteArray msgType = clientSocket->read(sizeof(int));
                            ClientParseMessage(ByteArrayToInt(msgType));
                        });
        }
}
//单机对战按钮
void Widget::on_singlebattle_clicked(){
    //单机初始为红方回合
    turnFlag = REDTURN;
    myColor = RED;
    initBoard(myColor);
    gameStart = true;
    gameMode = SINGLEBATTLE;
}
//人机对战
void Widget::on_machinebattle_clicked(){
    //人机对战默认红方为玩家 黑方为机器人
    turnFlag = REDTURN;
    myColor = RED;
    initBoard(myColor);
    gameStart = true;
    robot = new MachineGame(4);
    qDebug() << "3线程";
    gameMode = MACHINEBATTLE;
}


