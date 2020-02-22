#include <QLabel>
#include <QListWidget>
#include <QMouseEvent>
#include <QMessageBox>
#include <QHostAddress>
#include <QMediaPlaylist>
#include <QtMultimedia/QMediaPlayer>
#include "widget.h"
#include "ui_widget.h"
#include "QTimer"
#include "QTime"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //设置偏移量
    SetChessOffset();
    //隐藏事件
    ui->blackWin->hide();
    ui->redWin->hide();
    ui->peaceWin->hide();
    //初始化音乐
    initMusic();
    //播放背景音乐
    Mback->play();
    //游戏未开始
    gameStart = false;
    clientSocket = nullptr;
}

Widget::~Widget(){
    delete ui;
}

//走棋函数
bool Widget::chessGo(Cpos from,Cpos to){
    //找到要走的棋子
    ChessObj *goChess = nullptr;
    for(auto chess:ChessLists){
        if(!chess->isEaten && chess->pos.px == from.px && chess->pos.py == from.py)
            goChess = chess;
    }

    //若没找到该棋子 则返回
    if(!goChess)
        return false;
    //移动棋子
    goChess->move(BX+to.py*STEP_LEN,BY+to.px*STEP_LEN);
    //走棋音效
    Mgo->play();
    //更新棋盘信息
    board[BOARD_POS(to.px)][BOARD_POS(to.py)] = goChess->type;
    board[BOARD_POS(from.px)][BOARD_POS(from.py)] = EMPTY;
    //悔棋信息对象
    RegretObj regretObj(goChess->pos.px,goChess->pos.py,goChess,to.px,to.py,nullptr);
    //如果该位置原来是enemy
    if(board[BOARD_POS(to.px)][BOARD_POS(to.py)] != EMPTY){
        //吃子
        for(QVector<ChessObj*>::iterator it = ChessLists.begin();it != ChessLists.end();++it){
            if(!(**it).isEaten&&(**it).pos.px==to.px && (**it).pos.py==to.py){
                //要吃掉的棋子
                ChessObj *deChess = *it;
                //将悔棋to对象设置好
                regretObj.toChess = deChess;
                deChess->hide();
                deChess->isEaten = true; //标记为已被吃
                //判断吃掉的子是否为帅将
                if((deChess->type==BKING&&myColor==RED)||(deChess->type==RKING&&myColor==BLACK)){
                    //红棋胜
                    ui->redWin->show();
                    Mwin->play();
                    gameStart = false;
                }
                else if((deChess->type==RKING&&myColor==RED)||(deChess->type==BKING&&myColor==BLACK)){
                    //黑棋胜
                    ui->blackWin->show();
                    Mlose->play();
                    gameStart = false;
                }
                break;
            }
        }//for
    }//if
    //更新goChess的坐标信息
    goChess->pos.px = to.px;
    goChess->pos.py = to.py;
    RegretLists.push_back(regretObj);
    return true;
}

//鼠标点击事件
void Widget::mousePressEvent(QMouseEvent * event)
{
    //如果游戏结束或未开始
    if(!gameStart)
        return;
    //若选中棋子
    if(this->ChooseChess){
        //计算坐标
        double x = event->pos().x();
        double y = event->pos().y();
        //数组坐标的xy与实际相反
        int py = (x-RBX+STEP_LEN/2)/STEP_LEN;
        int px = (y-RBY+STEP_LEN/2)/STEP_LEN;
        if (px < 0 || py < 0)
            return;
        //判断该位置是否可下
        if(px < 10 && py < 9 && CansetArray[px][py]){
            //起点坐标
            int bpx = ChooseChess->pos.px;
            int bpy = ChooseChess->pos.py;
            //走棋
            chessGo(Cpos(bpx,bpy),Cpos(px,py));
            initCansetInfo();
            //查看是否有将军
            for (auto chess:ChessLists) {
                if(!chess->isEaten){
                    chess->run(false);
                }
            }
            ChooseChess = nullptr;
            //如果当前为网络对战模式 则发送走棋信息给对方
            if(gameMode == NETBATTLE){
                //如果套接字为空
                if(!clientSocket){
                    QMessageBox::information(this,"提示","网络未连接...");
                    return;
                }
                //写消息头
                clientSocket->write(IntToByteArray(CLIENT_CHESSGO_MESSAGE));
                //写棋起点坐标
                clientSocket->write(IntToByteArray(bpx));
                clientSocket->write(IntToByteArray(bpy));
                //写棋目标坐标
                clientSocket->write(IntToByteArray(px));
                clientSocket->write(IntToByteArray(py));
                clientSocket->waitForBytesWritten();
            }
            else if(gameMode == SINGLEBATTLE){
                //单机游戏 切换回合
                turnFlag = CHANGETURN(turnFlag);
            }
            else if(gameMode == MACHINEBATTLE){
                //人机对战
                //复制当前棋盘
                robot->copyBoard(board);
                QTimer::singleShot(100,this,SLOT(robotMove()));
            }
        }
        else //点击空白位置清空虚框
            initCansetInfo();
    }
}

void Widget::robotMove(){
    QTime time;
    time.start();
    //获取机器人要走的棋
    step robotStep = robot->getRobotBestStep();
    //走棋
    chessGo(robotStep.from,robotStep.to);
    robot->CleanTable();
    qDebug()<<time.elapsed()/1000.0<<"s";
}

//初始化CansetArray数组
void Widget::initCansetInfo(){
    //删除虚框
    for (int i=0;i<boxTags.size();i++) {
        delete  boxTags[i];
    }
    boxTags.clear();
    //初始化数组
    for (int i=0;i<10;i++) {
        for (int j=0;j<9;j++) {
            CansetArray[i][j] = false;
        }
    }
}
//初始化棋盘
void Widget::initBoard(int color)
{
    //隐藏事件
    ui->blackWin->hide();
    ui->redWin->hide();
    ui->peaceWin->hide();
    //清空初始数据
    ChooseChess = nullptr;
    //清空虚框的信息
    initCansetInfo();
    //清空棋子
    if(ChessLists.size() != 0){
        for (auto chess:ChessLists){
            delete chess;
        }
        ChessLists.clear();
    }
    RegretLists.clear();
    //设置棋盘数据
    for (int i=0;i<14;i++)
        for (int j=0;j<13;j++)
            board[i][j] = INVALID;
    for (int i=BOARD_POS(0);i<BOARD_POS(10);i++)
        for (int j=BOARD_POS(0);j<BOARD_POS(9);j++)
            board[i][j] = EMPTY;
    //黑棋
    board[BOARD_POS(0)][BOARD_POS(0)] = BROOK;
    board[BOARD_POS(0)][BOARD_POS(1)] = BKNIGNT;
    board[BOARD_POS(0)][BOARD_POS(2)] = BELEPHANT;
    board[BOARD_POS(0)][BOARD_POS(3)] = BMANDARIN;
    board[BOARD_POS(0)][BOARD_POS(4)] = BKING;
    board[BOARD_POS(0)][BOARD_POS(5)] = BMANDARIN;
    board[BOARD_POS(0)][BOARD_POS(6)] = BELEPHANT;
    board[BOARD_POS(0)][BOARD_POS(7)] = BKNIGNT;
    board[BOARD_POS(0)][BOARD_POS(8)] = BROOK;
    board[BOARD_POS(2)][BOARD_POS(1)] = BCANNON;
    board[BOARD_POS(2)][BOARD_POS(7)] = BCANNON;
    board[BOARD_POS(3)][BOARD_POS(0)] = BPAWN;
    board[BOARD_POS(3)][BOARD_POS(2)] = BPAWN;
    board[BOARD_POS(3)][BOARD_POS(4)] = BPAWN;
    board[BOARD_POS(3)][BOARD_POS(6)] = BPAWN;
    board[BOARD_POS(3)][BOARD_POS(8)] = BPAWN;
    //红棋
    board[BOARD_POS(9)][BOARD_POS(0)] = RROOK;
    board[BOARD_POS(9)][BOARD_POS(1)] = RKNIGNT;
    board[BOARD_POS(9)][BOARD_POS(2)] = RELEPHANT;
    board[BOARD_POS(9)][BOARD_POS(3)] = RMANDARIN;
    board[BOARD_POS(9)][BOARD_POS(4)] = RKING;
    board[BOARD_POS(9)][BOARD_POS(5)] = RMANDARIN;
    board[BOARD_POS(9)][BOARD_POS(6)] = RELEPHANT;
    board[BOARD_POS(9)][BOARD_POS(7)] = RKNIGNT;
    board[BOARD_POS(9)][BOARD_POS(8)] = RROOK;
    board[BOARD_POS(7)][BOARD_POS(1)] = RCANNON;
    board[BOARD_POS(7)][BOARD_POS(7)] = RCANNON;
    board[BOARD_POS(6)][BOARD_POS(0)] = RPAWN;
    board[BOARD_POS(6)][BOARD_POS(2)] = RPAWN;
    board[BOARD_POS(6)][BOARD_POS(4)] = RPAWN;
    board[BOARD_POS(6)][BOARD_POS(6)] = RPAWN;
    board[BOARD_POS(6)][BOARD_POS(8)] = RPAWN;
    //显示棋子
    for (int i=0;i<10;i++) {
        for (int j=0;j<9;j++) {
            ChessObj *chess = nullptr;
            //根据本方颜色切换视角
            if(color == RED)
                chess = ChessObj::ChessFactory(board[BOARD_POS(i)][BOARD_POS(j)]);
            else if(color == BLACK)
                chess = ChessObj::ChessFactory(-board[BOARD_POS(i)][BOARD_POS(j)]);
            if(chess != nullptr){
                //横着为x 竖着为y
                chess->setParent(this);
                chess->move(BX+j*STEP_LEN,BY+i*STEP_LEN);
                chess->pos.px = i;
                chess->pos.py = j;
                chess->type = board[BOARD_POS(i)][BOARD_POS(j)];
                chess->father = this;
                chess->isEaten = false;  //false表示未被吃 true表示已被吃
                //设置棋子对应的偏移量
                if(qAbs(chess->type) == KING)
                    chess->delta = KingDelta;
                if(qAbs(chess->type) == KNIGNT)
                    chess->delta = KnightDelta;
                if(qAbs(chess->type) == ELEPHANT)
                    chess->delta = ElephantDelta;
                if(qAbs(chess->type) == MANDARIN)
                    chess->delta = MandarinDelta;
                //加入容器中
                ChessLists.push_back(chess);
                chess->show();
            }
        }
    }
}
//初始化音乐
void Widget::initMusic(){
    //背景音乐
    Mback = new QMediaPlayer(this);
    Mback->setMedia(QUrl::fromLocalFile("/Users/niyijie/Desktop/QT/ChineseChess/res/music/background.mp3"));
    //将军
    Mcheck = new QMediaPlayer(this);
    Mcheck->setMedia(QUrl::fromLocalFile("/Users/niyijie/Desktop/QT/ChineseChess/res/music/check.wav"));
    //选中
    Mselect = new QMediaPlayer(this);
    Mselect->setMedia(QUrl::fromLocalFile("/Users/niyijie/Desktop/QT/ChineseChess/res/music/select.mp3"));
    //胜利
    Mwin = new QMediaPlayer(this);
    Mwin->setMedia(QUrl::fromLocalFile("/Users/niyijie/Desktop/QT/ChineseChess/res/music/win.mp3"));
    //和棋
    Mpeace = new QMediaPlayer(this);
    Mpeace->setMedia(QUrl::fromLocalFile("/Users/niyijie/Desktop/QT/ChineseChess/res/music/peace.mp3"));
    //失败
    Mlose = new QMediaPlayer(this);
    Mlose->setMedia(QUrl::fromLocalFile("/Users/niyijie/Desktop/QT/ChineseChess/res/music/lose.mp3"));
    //吃
    Meat = new QMediaPlayer(this);
    Meat->setMedia(QUrl::fromLocalFile("/Users/niyijie/Desktop/QT/ChineseChess/res/music/eat.mp3"));
    //走棋
    Mgo = new QMediaPlayer(this);
    Mgo->setMedia(QUrl::fromLocalFile("/Users/niyijie/Desktop/QT/ChineseChess/res/music/go.mp3"));
}
//设置偏移量
void Widget::SetChessOffset()
{
    //将、帅偏移量  顺时针
    KingDelta.plist.push_back(Cpos(-1,0));
    KingDelta.plist.push_back(Cpos(0,1));
    KingDelta.plist.push_back(Cpos(1,0));
    KingDelta.plist.push_back(Cpos(0,-1));
    //马偏移量
    KnightDelta.plist.push_back(Cpos(-2,-1)); //上
    KnightDelta.plist.push_back(Cpos(-2,1));
    KnightDelta.plist.push_back(Cpos(-1,2));//右
    KnightDelta.plist.push_back(Cpos(1,2));
    KnightDelta.plist.push_back(Cpos(2,-1));//下
    KnightDelta.plist.push_back(Cpos(2,1));
    KnightDelta.plist.push_back(Cpos(-1,-2));//左
    KnightDelta.plist.push_back(Cpos(1,-2));
    //相偏移量   顺时针
    ElephantDelta.plist.push_back(Cpos(-2,2));
    ElephantDelta.plist.push_back(Cpos(2,2));
    ElephantDelta.plist.push_back(Cpos(2,-2));
    ElephantDelta.plist.push_back(Cpos(-2,-2));
    //士偏移量 顺时针
    MandarinDelta.plist.push_back(Cpos(-1,1));
    MandarinDelta.plist.push_back(Cpos(1,1));
    MandarinDelta.plist.push_back(Cpos(1,-1));
    MandarinDelta.plist.push_back(Cpos(-1,-1));
}
//显示虚框
void Widget::flushBoxs()
{
    for (auto box:boxTags) {
        box->setParent(this);
        box->show();
    }
}
//悔棋函数
void Widget::regret(){
    //取出栈顶元素
    RegretObj regret = RegretLists.back();
    RegretLists.pop_back();
    //恢复棋盘信息
    regret.fromChess->move(BX+regret.from.py*STEP_LEN,BY+regret.from.px*STEP_LEN);
    board[BOARD_POS(regret.from.px)][BOARD_POS(regret.from.py)] = regret.fromChess->type;
    board[BOARD_POS(regret.to.px)][BOARD_POS(regret.to.py)] = EMPTY;
    regret.fromChess->pos.px = regret.from.px;
    regret.fromChess->pos.py = regret.from.py;
    regret.fromChess->show();
    //清空虚框
    initCansetInfo();
    //切换回合
    turnFlag = CHANGETURN(turnFlag);
    //如果上一步吃了子
    if(regret.toChess != nullptr){
        //更新棋面信息
        regret.toChess->show();
        board[BOARD_POS(regret.to.px)][BOARD_POS(regret.to.py)] = regret.toChess->type;
        regret.toChess->isEaten = false;
    }
}


//客户端网络解析函数
void Widget::ClientParseMessage(int message){
    switch (message) {
    case CLIENT_CHESSGO_MESSAGE:{
        //读取对方走棋起点坐标
        QByteArray px1Array = clientSocket->read(sizeof(int));
        int px1 = ByteArrayToInt(px1Array);
        QByteArray py1Array = clientSocket->read(sizeof(int));
        int py1 = ByteArrayToInt(py1Array);
        QByteArray px2Array = clientSocket->read(sizeof(int));
        int px2 = ByteArrayToInt(px2Array);
        QByteArray py2Array = clientSocket->read(sizeof(int));
        int py2 = ByteArrayToInt(py2Array);
        //走棋
        chessGo(Cpos(px1,py1),Cpos(px2,py2));
        //切换回合
        turnFlag = CHANGETURN(turnFlag);
        //查看是否有将军
        for (auto chess:ChessLists) {
            if(!chess->isEaten){
                chess->run(false);
            }
        }
        initCansetInfo();
        break;
    }
    case PLAYER_LEAVE_MESSAGE:{
        //收到对方断开连接消息
        if( QMessageBox::question(this,tr("提示"),tr("对手已断开连接，是否继续等待?"),QMessageBox::Yes, QMessageBox::No )
                           == QMessageBox::No){
                    clientSocket->write(IntToByteArray(PLAYER_LEAVE_MESSAGE));
                    clientSocket->waitForBytesWritten();
                    clientSocket->disconnectFromHost();
                    clientSocket->close();
                    clientSocket = nullptr;
                    QMessageBox::information(this,"提示","成功断开连接...");
        }
        break;
    }
    case GAME_BEGIN_MESSAGE:{
        //获取本方颜色
        QByteArray colorArray = clientSocket->read(sizeof(int));
        int color = ByteArrayToInt(colorArray);
        if(color == RED){
            myColor = RED;
            turnFlag = REDTURN;
            gameStart = true;
        }else if(color == BLACK){
            myColor = BLACK;
            turnFlag = BLACKTURN;
            gameStart = true;
        }
        else{
            QMessageBox::information(this,"提示","消息错误...");
            return;
        }
        initBoard(myColor);
        break;
    }
    case PLAYER_PEACE_MESSAGE:{
        //获取具体内容
        QByteArray typeArray = clientSocket->read(sizeof(int));
        int type = ByteArrayToInt(typeArray);
        if(type == ASK){
            if( QMessageBox::question(this,tr("提示"),tr("对方请求和棋，是否同意?"),QMessageBox::Yes, QMessageBox::No )
                               == QMessageBox::Yes){
                //同意和棋
                ui->peaceWin->show();
                Mpeace->play();
                gameStart = false;
                //通知对方同意和棋
                clientSocket->write(IntToByteArray(PLAYER_PEACE_MESSAGE));
                clientSocket->write(IntToByteArray(YES));
                clientSocket->waitForBytesWritten();
            }
            else{
                //不同意和棋
                clientSocket->write(IntToByteArray(PLAYER_PEACE_MESSAGE));
                clientSocket->write(IntToByteArray(NO));
                clientSocket->waitForBytesWritten();
            }
        }else if(type == YES){
            //对方同意和棋
            ui->peaceWin->show();
            Mpeace->play();
            gameStart = false;
        }else if(type == NO){
            //对方不同意和棋
            QMessageBox::information(this,"提示","对方不同意和棋...");
        }
        break;
    }
    case PLAYER_GIVEUP_MESSAGE:{
        //对方认输 我方胜利
        if(myColor == RED)
            ui->redWin->show();
        if(myColor == BLACK)
            ui->blackWin->show();
        Mwin->play();
        gameStart = false;
        break;
    }
    case PLAYER_REGRET_MESSAGE:{
        //获取具体内容
        QByteArray typeArray = clientSocket->read(sizeof(int));
        int type = ByteArrayToInt(typeArray);
        if(type == ASK){
            if( QMessageBox::question(this,tr("提示"),tr("对方请求悔棋，是否同意?"),QMessageBox::Yes, QMessageBox::No )
                               == QMessageBox::Yes){
                //同意悔棋
                clientSocket->write(IntToByteArray(PLAYER_REGRET_MESSAGE));
                clientSocket->write(IntToByteArray(YES));
                clientSocket->waitForBytesWritten();
                regret();
            }
            else{
                //不同意悔棋
                clientSocket->write(IntToByteArray(PLAYER_REGRET_MESSAGE));
                clientSocket->write(IntToByteArray(NO));
                clientSocket->waitForBytesWritten();
            }
        }else if(type == YES){
            //对方同意悔棋
            regret();
        }else if(type == NO){
            //对方不同意和棋
            QMessageBox::information(this,"提示","对方不同意悔棋...");
        }
        break;
    }
    case PLAYER_RESTART_MESSAGE:{
        //获取具体内容
        QByteArray typeArray = clientSocket->read(sizeof(int));
        int type = ByteArrayToInt(typeArray);
        if(type == ASK){
            if( QMessageBox::question(this,tr("提示"),tr("对方请求重新开始，是否同意?"),QMessageBox::Yes, QMessageBox::No )
                               == QMessageBox::Yes){
                //同意重新开始
                clientSocket->write(IntToByteArray(PLAYER_RESTART_MESSAGE));
                clientSocket->write(IntToByteArray(YES));
                clientSocket->waitForBytesWritten();
                //重新开始
                if(myColor == RED)
                    turnFlag = REDTURN;
                else if(myColor == BLACK)
                    turnFlag = BLACKTURN;
                gameStart = true;
                initBoard(myColor);
            }
            else{
                //不同意重新开始
                clientSocket->write(IntToByteArray(PLAYER_RESTART_MESSAGE));
                clientSocket->write(IntToByteArray(NO));
                clientSocket->waitForBytesWritten();
            }
        }else if(type == YES){
            //对方同意重新开始
            if(myColor == RED)
                turnFlag = REDTURN;
            else if(myColor == BLACK)
                turnFlag = BLACKTURN;
            initBoard(myColor);
            gameStart = true;
        }else if(type == NO){
            //对方不同意和棋
            QMessageBox::information(this,"提示","对方不同意重新开始...");
        }
        break;
    }
    }
}

