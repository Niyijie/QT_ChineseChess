#ifndef CHESSOBJ_H
#define CHESSOBJ_H

#include <QLabel>
#include <QDebug>
#include <QMetaType>
#include <QWidget>
#include <QVector>
#include <QtMultimedia/QMediaPlayer>
#include <QTcpSocket>
#include "chessRule.h"

class ChessObj;

//点坐标
struct  Cpos{
public:
    Cpos(){}
    Cpos(int _px,int _py){
        px = _px;
        py = _py;
    }
    int px;
    int py;
};

//偏移量结构体
struct ptOffset{
    //顺时针起的四个偏移量
    QVector<Cpos> plist;
};

//悔棋栈内存的结构体
struct RegretObj{
    RegretObj(int px1,int py1,ChessObj *cFrom,int px2,int py2,ChessObj *cTo){
        from = Cpos(px1,py1);
        fromChess = cFrom;
        to = Cpos(px2,py2);
        toChess = cTo;
    }
    //上一步起点位置
    Cpos from;
    ChessObj *fromChess;
    //上一步终点位置
    Cpos to;
    ChessObj *toChess;
};

//棋子类
class ChessObj:public QLabel{
public:
    ChessObj(){}
    //行走
    void run(bool mode);  //mode=false为检查是否将军，mode为true为走棋
    bool isEnemy(int tag);
    bool CannonCanset(int tag,int &times);
public:
    //棋子所在坐标
    Cpos pos;
    //棋子种类
    int type;
    //棋子偏移量
    ptOffset delta;
    //
    QWidget *father;
    //该棋子是否被吃了
    bool isEaten;

    static ChessObj* ChessFactory(int type);

private slots:
    void mousePressEvent(QMouseEvent *e);//鼠标点击事件
};




#endif // CHESSOBJ_H
