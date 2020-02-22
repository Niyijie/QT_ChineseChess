#ifndef MACHINEGAME_H
#define MACHINEGAME_H
#define REDSIDE 0
#define BLACKSIDE 1
#define INFINITY 99999999

#include <QVector>
#include "chessobj.h"
#include "chessRule.h"
#include "QHash"

//可走的步
struct step{
    step(){}
    step(Cpos _from,Cpos _to,int _type){
        from = _from;
        to = _to;
        toType = _type;
    }
    Cpos from;
    Cpos to;
    //目标位置的棋子类型
    int toType;
};

struct HTNode{
    HTNode(int _depth,int _value){
        depth = _depth;
        value = _value;
    }
    HTNode(){}
    int depth;
    int value;
};

//棋子信息 用于计算当前棋盘的分数
struct chessInfo{
    chessInfo(int _type,Cpos _pos){
        type = _type;
        pos = _pos;
        ThreatNum = 0;
        ProtectNum = 0;
        ActiveNum = 0;
    }
    //棋子类型
    int type;
    //坐标
    Cpos pos;
    //威胁的棋子数
    int ThreatNum;
    //保护的棋子数
    int ProtectNum;
    //棋子可行走的位置数
    int ActiveNum;
};

class MachineGame
{
public:
    MachineGame(){}
    MachineGame(int _layers);
    //初始化各类价值数组
    void InitValue();
    //复制当前真实棋盘给机器人
    void copyBoard(int trueBoard[14][13]);
    //找到当前棋面下所有side方可走的棋
    void FindAllSteps(int side,QVector<step> &steps);
    int GetMinScore(int _layers,step &s);
    int GetMaxScore(int _layers,step &s);
    int AlphaBeta(int depth,int alpha,int beta,int side,step &s);
    int NegaMaxSearch(int depth,int side,step &s);
    int PrincipalVariation(int depth,int alpha,int beta,int side,step &s);
    //试着走
     void fakeMove(step _step);
     void unFakeMove(step _step);
    //获取机器人的棋走
    step getRobotBestStep();
    //计算当前棋面分
    int calcscore(int side);
    //评估当前棋盘
    void EveluateBoard(QVector<chessInfo> &chesses);
    //是否是敌人
    bool isEnemy(int type,int tag);
    //炮可走
    bool CannonCanset(int type,int tag,int &times);
    //
    //设置偏移量
    void SetChessOffset();
    //获取将棋盘数组转化成字符串
    QString BoardArrayToString(int _fakeBoard[14][13],int side);
    //清空哈希表
    void CleanTable(){
        TTable.clear();
    }

    int getLayers(){return layers;}

private:
    //棋子的基本价值数组
    //依次为 [0]KING [1]ROOK [2]KNIGHT [3]ELEPHANT [4]MANDARIN [5]CANNON [6]PAWN
    int ChessBasicValue[7];
    //棋子的灵活性分值
    //依次为 [0]KING [1]ROOK [2]KNIGHT [3]ELEPHANT [4]MANDARIN [5]CANNON [6]PAWN
    int ChessActiveValue[7];
    //保护棋子分
    int ProtectValue;
    //威胁棋子分
    int ThreatValue;
    //表示棋子的位置价值 第一个表示颜色，第二个表示棋子，第三为价值棋盘
    int PositionValues[2][7][10*9];
    //模拟棋盘
    int fakeBoard[14][13];
    //一些棋子偏移量
    ptOffset KingDelta;
    ptOffset KnightDelta;
    ptOffset ElephantDelta;
    ptOffset MandarinDelta;
    //最大最小算法的最大层次
    int layers;
    //标记是否游戏结束
    bool gameOver;
    //置换表
    QHash<QString,HTNode> TTable;
    long long count;
};
#endif // MACHINEGAME_H
