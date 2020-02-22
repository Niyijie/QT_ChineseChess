#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>
#include <QTcpSocket>
#include "chessRule.h"
#include "chessobj.h"
#include "netProtocol.h"
#include "machinegame.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    //声明友元
    friend class ChessObj;
    friend class netControler;
    //初始
    void SetChessOffset();
    void initBoard(int color);
    void flushBoxs();
    void initCansetInfo();
    void initMusic();
    //走棋函数
    bool chessGo(Cpos from,Cpos to);
    void regret();

private slots:
    void on_singlebattle_clicked();
    void mousePressEvent(QMouseEvent *e);
    void on_leave_clicked();
    void on_again_clicked();
    void on_regret_clicked();
    void on_peace_clicked();
    void on_netbattle_clicked();
    void on_machinebattle_clicked();
    void ClientParseMessage(int message);
    void on_giveup_clicked();
    void robotMove();

private:
    Ui::Widget *ui;
    int board[14][13];   //棋盘 每边加上2行作为边界 真正起点在2   行为y 列为x
    //网络连接套接字
    QTcpSocket *clientSocket;
    //可下虚框
    QVector<QLabel*> boxTags;
    //标记可下位置的数组
    bool CansetArray[10][9];
    //此时选中的棋子
    ChessObj *ChooseChess;
    //棋子对象容器
    QVector<ChessObj*> ChessLists;
    //悔棋容器
    QVector<RegretObj> RegretLists;
    //谁的回合
    int turnFlag;
    //本方颜色
    int myColor;
    //游戏是否开始
    bool gameStart;
    //对战模式 1为单机 2为人机 3为网络
    int gameMode;
    //机器人
    MachineGame *robot;
    //音乐
    QMediaPlayer *Mback;        //背景音乐
    QMediaPlayer *Mcheck;       //将军
    QMediaPlayer *Mselect;      //选中
    QMediaPlayer *Mwin;         //胜利
    QMediaPlayer *Mpeace;       //和棋
    QMediaPlayer *Mlose;        //失败
    QMediaPlayer *Meat;         //吃
    QMediaPlayer *Mgo;          //走棋
    //棋子偏移量
    ptOffset KingDelta;
    ptOffset KnightDelta;
    ptOffset ElephantDelta;
    ptOffset MandarinDelta;


};
#endif // WIDGET_H
