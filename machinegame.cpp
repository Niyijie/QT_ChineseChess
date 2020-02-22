#include "machinegame.h"
#include "QHash"
#include "QDebug"
#include <QCryptographicHash>
#include "taskthread.h"

MachineGame::MachineGame(int _layers){
    InitValue();
    SetChessOffset();
    layers = _layers;
    count = 0;
}

void MachineGame::SetChessOffset(){
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

void MachineGame::copyBoard(int trueBoard[14][13]){
    memcpy(fakeBoard,trueBoard,14*13*sizeof (int));
}

bool MachineGame::isEnemy(int type,int tag){
    if((type>0&&tag>0)||(type<0&&tag<0))
        return false;          //同色是friend
    else
        return true;           //异色为enemy
}

bool MachineGame::CannonCanset(int type, int tag ,int &times){
    if(tag !=EMPTY && tag != INVALID)
        times++;
    if(times == 0 && tag == EMPTY)   //如果该位置为空
        return true;
    if(times == 2 && isEnemy(type,tag)){
        return true;
    }
    return false;
}

void MachineGame::InitValue(){
    //棋子位置分值数组
    int _posvalue[2][7][10*9] = {
        {
            //红棋
            {
                //红帅
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  1,  1,  1,  0,  0,  0,
                0,  0,  0,  10, 10, 10, 0,  0,  0,
                0,  0,  0,  15, 20, 15, 0,  0,  0
            },{
                //红车
                160, 170, 160, 150, 150, 150, 160, 170, 160,
                170, 180, 170, 190, 250, 190, 170, 180, 170,
                170, 190, 200, 220, 240, 220, 200, 190, 170,
                180, 220, 210, 240, 250, 240, 210, 220, 180,
                180, 220, 210, 240, 250, 240, 210, 220, 180,
                180, 220, 210, 240, 250, 240, 210, 220, 180,
                170, 190, 180, 220, 240, 220, 200, 190, 170,
                170, 180, 170, 170, 160, 170, 170, 180, 170,
                160, 170, 160, 160, 150, 160, 160, 170, 160,
                150, 160, 150, 160, 150, 160, 150, 160, 150
            },{
                //红马
                70,  80,  90,  80,  70,  80,  90,  80,  70,
                80,  110, 125, 90,  70,  90,  125, 110, 80,
                90,  100, 120, 125, 120, 125, 120, 100, 90,
                90,  100, 120, 130, 110, 130, 120, 100, 90,
                90,  110, 110, 120, 100, 120, 110, 110, 90,
                90,  100, 100, 110, 100, 110, 100, 100, 90,
                80,  90,  100, 100, 90,  100, 100, 90,  80,
                80,  80,  90,  90,  80,  90,  90,  80,  80,
                70,  75,  75,  70,  50,  70,  75,  75,  70,
                60,  70,  75,  70,  60,  70,  75,  70,  60
            },{
                //红相
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  25, 0,  0,  0,  25, 0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                20, 0,  0,  0,  35, 0,  0,  0,  20,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  30, 0,  0,  0,  30, 0,  0
            },{
                //红士
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  30, 0,  30, 0,  0,  0,
                0,  0,  0,  0,  22, 0,  0,  0,  0,
                0,  0,  0,  30, 0,  30, 0,  0,  0
            },{
                //红炮
                125, 130, 100, 70,  60,  70,  100, 130, 125,
                110, 125, 100, 70,  60,  70,  100, 125, 110,
                100, 120, 90,  80,  80,  80,  90,  120, 100,
                90,  110, 90,  110, 130, 110, 90,  110, 90,
                90,  110, 90,  110, 130, 110, 90,  110, 90,
                90,  100, 90,  110, 130, 110, 90,  100, 90,
                90,  100, 90,  90,  110, 90,  90,  100, 90,
                90,  100, 80,  80,  70,  80,  80,  100, 90,
                80,  90,  80,  70,  65,  70,  80,  90,  80,
                80,  90,  80,  70,  60,  70,  80,  90,  80
            },{
                //红兵
                10,  10,  10,  20,  25,  20,  10,  10,  10,
                25,  30,  40,  50,  60,  50,  40,  30,  25,
                25,  30,  30,  40,  40,  40,  30,  30,  25,
                20,  25,  25,  30,  30,  30,  25,  25,  20,
                15,  20,  20,  20,  20,  20,  20,  20,  15,
                10,  0,   15,  0,   15,  0,   15,  0,   10,
                10,  0,   10,  0,   15,  0,   10,  0,   10,
                0,   0,   0,   0,   0,   0,   0,   0,   0,
                0,   0,   0,   0,   0,   0,   0,   0,   0,
                0,   0,   0,   0,   0,   0,   0,   0,   0
            }
        },
        {
            //黑棋
            {
                //黑将
                0,  0,  0,  15, 20, 15, 0,  0,  0,
                0,  0,  0,  10, 10, 10, 0,  0,  0,
                0,  0,  0,  1,  1,  1,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0
            },{
                //黑车
                150, 160, 150, 160, 150, 160, 150, 160, 150,
                160, 170, 160, 160, 150, 160, 160, 170, 160,
                170, 180, 170, 170, 160, 170, 170, 180, 170,
                170, 190, 180, 220, 240, 220, 200, 190, 170,
                180, 220, 210, 240, 250, 240, 210, 220, 180,
                180, 220, 210, 240, 250, 240, 210, 220, 180,
                180, 220, 210, 240, 250, 240, 210, 220, 180,
                170, 190, 200, 220, 240, 220, 200, 190, 170,
                170, 180, 170, 190, 250, 190, 170, 180, 170,
                160, 170, 160, 150, 150, 150, 160, 170, 160
            },{
                //黑马
                60,  70,  75,  70,  60,  70,  75,  70,  60,
                70,  75,  75,  70,  50,  70,  75,  75,  70,
                80,  80,  90,  90,  80,  90,  90,  80,  80,
                80,  90,  100, 100, 90,  100, 100, 90,  80,
                90,  100, 100, 110, 100, 110, 100, 100, 90,
                90,  110, 110, 120, 100, 120, 110, 110, 90,
                90,  100, 120, 130, 110, 130, 120, 100, 90,
                90,  100, 120, 125, 120, 125, 120, 100, 90,
                80,  110, 125, 90,  70,  90,  125, 110, 80,
                70,  80,  90,  80,  70,  80,  90,  80,  70
            },{
                //黑象
                0,  0,  30, 0,  0,  0,  30, 0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                20, 0,  0,  0,  35, 0,  0,  0,  20,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  25, 0,  0,  0,  25, 0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
            },{
                //黑士
                0,  0,  0,  30, 0,  30, 0,  0,  0,
                0,  0,  0,  0,  22, 0,  0,  0,  0,
                0,  0,  0,  30, 0,  30, 0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0,
                0,  0,  0,  0,  0,  0,  0,  0,  0
            },{
                //黑炮
                80,  90,  80,  70,  60,  70,  80,  90,  80,
                80,  90,  80,  70,  65,  70,  80,  90,  80,
                90,  100, 80,  80,  70,  80,  80,  100, 90,
                90,  100, 90,  90,  110, 90,  90,  100, 90,
                90,  100, 90,  110, 130, 110, 90,  100, 90,
                90,  110, 90,  110, 130, 110, 90,  110, 90,
                90,  110, 90,  110, 130, 110, 90,  110, 90,
                100, 120, 90,  80,  80,  80,  90,  120, 100,
                110, 125, 100, 70,  60,  70,  100, 125, 110,
                125, 130, 100, 70,  60,  70,  100, 130, 125
            },{
                //黑卒
                0,   0,   0,   0,   0,   0,   0,   0,   0,
                0,   0,   0,   0,   0,   0,   0,   0,   0,
                0,   0,   0,   0,   0,   0,   0,   0,   0,
                10,  0,   10,  0,   15,  0,   10,  0,   10,
                10,  0,   15,  0,   15,  0,   15,  0,   10,
                15,  20,  20,  20,  20,  20,  20,  20,  15,
                20,  25,  25,  30,  30,  30,  25,  25,  20,
                25,  30,  30,  40,  40,  40,  30,  30,  25,
                25,  30,  40,  50,  60,  50,  40,  30,  25,
                10,  10,  10,  20,  25,  20,  10,  10,  10
            }
        }
    };
    memcpy(PositionValues,_posvalue,sizeof (_posvalue));
    //棋子基础分
    //依次为 [0]KING [1]ROOK [2]KNIGHT [3]ELEPHANT [4]MANDARIN [5]CANNON [6]PAWN
    int _basicValue[7] = {1000,90,40,20,20,45,10};
    //int _basicValue[7] = {5000,500,300,250,250,300,80};
    memcpy(ChessBasicValue,_basicValue,sizeof (_basicValue));
    //棋子的灵活性分值
    //依次为 [0]KING [1]ROOK [2]KNIGHT [3]ELEPHANT [4]MANDARIN [5]CANNON [6]PAWN
    int _activeValue[7] = {2,3,4,2,2,2,3};
    memcpy(ChessActiveValue,_activeValue,sizeof (_activeValue));
    //保护棋子分
    ProtectValue = 3;
    //威胁棋子分
    ThreatValue = 3;
}

void MachineGame::FindAllSteps(int side,QVector<step> &steps){
    for(int ii =0;ii<10;ii++){
        for(int jj=0;jj<9;jj++){
            //获取该位置标记值 若为空位置则continue
            int type = fakeBoard[BOARD_POS(ii)][BOARD_POS(jj)];
            if(type == EMPTY)
                continue;
            //若不是side方的棋子 则返回
            if((side == REDSIDE && type < 0) || (side == BLACKSIDE && type > 0))
                continue;
            //此位置坐标
            Cpos pos(ii,jj);
            switch(qAbs(type)){
                case KING:{
                    //将帅面对面时可吃对方
                    int cpx = -1;
                    if(type > 0){
                        //帅
                        for(int i=pos.px-1;i>=0;i--){
                            int taget = fakeBoard[BOARD_POS(i)][BOARD_POS(pos.py)];
                            if(taget != EMPTY && taget != BKING){
                                //王之间有棋子
                                break;
                            }else if(taget == BKING){
                                cpx = i;
                                break;
                            }
                        }
                    }
                    if(type < 0){
                        //将
                        for(int i=pos.px+1;i<=10;i++){
                            int taget = fakeBoard[BOARD_POS(i)][BOARD_POS(pos.py)];
                            if(taget != EMPTY && taget != RKING){
                                break;
                            }
                            else if(taget == RKING){
                                cpx = i;
                                break;
                            }
                        }
                    }
                    if(cpx != -1){
                        //该位置可下 将改步加到可走队列中
                        steps.push_back(step(pos,Cpos(cpx,pos.py),fakeBoard[BOARD_POS(cpx)][BOARD_POS(pos.py)]));
                    }
                    //寻找四个方向可走位置
                    for (auto p:KingDelta.plist) {
                        int px = pos.px + p.px;
                        int py = pos.py + p.py;
                        //限制将行走范围
                        if(py <= 2 || py >= 6||(type > 0&& (px <= 6||px > 9))||(type < 0 && (px >= 3||px<0)))
                            continue;
                        //如果目标位置可下
                        int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];
                        if(fakeBoard[BOARD_POS(px)][BOARD_POS(py)]==EMPTY||isEnemy(type,tag)){
                            steps.push_back(step(pos,Cpos(px,py),tag));
                        }
                    }
                    break;
                }
                case ROOK:{
                    //车   四个方向偏移量
                    int xl[4] = {-1,0,1,0};
                    int yl[4] = {0,1,0,-1};
                    for(int i=0;i<4;i++){
                        int px = pos.px + xl[i];
                        int py = pos.py + yl[i];
                        //寻找可下位置
                        while(fakeBoard[BOARD_POS(px)][BOARD_POS(py)]!=INVALID){
                            int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];
                            //如果该位置为空或者为enemy
                            if(tag == EMPTY || isEnemy(type,tag)){
                                //该位置为可走
                                steps.push_back(step(pos,Cpos(px,py),tag));
                                //如果遇到enemy
                                if(tag != EMPTY)
                                    break;
                                //下一个位置的坐标
                                px = px + xl[i];
                                py = py + yl[i];
                            }
                            else //如果是friend
                                break;

                        }//while
                    }//for
                    break;
                }
                case KNIGNT:{
                    //马  四个方向的偏移量（检查是否有绊马脚的子）
                    int xl[4] = {-1,0,1,0};
                    int yl[4] = {0,1,0,-1};
                    for(int i=0;i<4;i++){
                        if(fakeBoard[BOARD_POS(pos.px+xl[i])][BOARD_POS(pos.py+yl[i])] == EMPTY){
                            for (int j=2*i;j<2*i+2;j++) {
                                int px = pos.px + KnightDelta.plist[j].px;
                                int py = pos.py + KnightDelta.plist[j].py;
                                int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];
                                //如果目标位置可下
                                if(tag==EMPTY||isEnemy(type,tag)){
                                    steps.push_back(step(pos,Cpos(px,py),tag));
                                }
                            }
                        }
                    }
                    break;
                }
                case ELEPHANT:{
                    //相
                    int xl[4] = {-1,1,1,-1};
                    int yl[4] = {1,1,-1,-1};
                    for(int i=0;i<4;i++){
                        if(fakeBoard[BOARD_POS(pos.px+xl[i])][BOARD_POS(pos.py+yl[i])] == EMPTY){
                            int px = pos.px + ElephantDelta.plist[i].px;
                            int py = pos.py + ElephantDelta.plist[i].py;
                            //如果越界则返回
                            if((type > 0 && px <= 4) || (type < 0 && px >= 5))
                                continue;
                            //要下的位置的值
                            int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];
                            //如果目标位置可下
                            if(tag==EMPTY||isEnemy(type,tag)){
                                steps.push_back(step(pos,Cpos(px,py),tag));
                            }
                        }
                    }
                    break;
                }
                case MANDARIN:{
                    //士
                    for (auto p:MandarinDelta.plist) {
                        int px = pos.px + p.px;
                        int py = pos.py + p.py;
                        //要下的位置的值
                        int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];
                        //如果目标位置可下
                        if(tag==EMPTY||isEnemy(type,tag)){
                            if((px>=3&&px<=6)||py<=2||py>=6)
                                continue;
                            steps.push_back(step(pos,Cpos(px,py),tag));
                        }
                    }
                    break;
                }
                case CANNON:{
                    //炮   四个方向偏移量
                    int xl[4] = {-1,0,1,0};
                    int yl[4] = {0,1,0,-1};
                    for(int i=0;i<4;i++){
                        int px = pos.px + xl[i];
                        int py = pos.py + yl[i];
                        int times = 0;
                        //寻找可下位置
                        while(fakeBoard[BOARD_POS(px)][BOARD_POS(py)]!=INVALID){
                            int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];
                            //如果该位置为空或者为enemy
                            if(CannonCanset(type,tag,times)){
                                steps.push_back(step(pos,Cpos(px,py),tag));
                            }
                            if(times == 2)
                                break;
                            px = px + xl[i];
                            py = py + yl[i];
                        }//while
                    }//for
                    break;
                }
                case PAWN:{
                    //兵  四个方向偏移量
                    int xl[4] = {-1,0,1,0};
                    int yl[4] = {0,1,0,-1};
                    for(int i=0;i<4;i++){
                        int px = pos.px + xl[i];
                        int py = pos.py + yl[i];
                        //棋子不能后退
                        if((type > 0 && i==2) || (type < 0 && i==0))
                            continue;
                        //在自己边只能前进
                        if((type > 0 && px >= 5 && i!=0)||(type < 0 && px <= 4 && i!=2))
                            continue;
                        int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];
                        //如果目标位置可下
                        if(fakeBoard[BOARD_POS(px)][BOARD_POS(py)]==EMPTY||isEnemy(type,tag)){
                            steps.push_back(step(pos,Cpos(px,py),tag));
                        }
                   }
                   break;
                }
             }
        }
    }
}

void MachineGame::fakeMove(step _step){
    //如果可以把王吃了
    if(qAbs(fakeBoard[BOARD_POS(_step.to.px)][BOARD_POS(_step.to.py)]) == KING){
        gameOver = true;
    }
    //模拟移动棋子
    fakeBoard[BOARD_POS(_step.to.px)][BOARD_POS(_step.to.py)] = fakeBoard[BOARD_POS(_step.from.px)][BOARD_POS(_step.from.py)];
    //原位置置为empty
    fakeBoard[BOARD_POS(_step.from.px)][BOARD_POS(_step.from.py)] = EMPTY;
}

void MachineGame::unFakeMove(step _step){
    if(gameOver == true)
        gameOver = false;
    //恢复棋盘
    fakeBoard[BOARD_POS(_step.from.px)][BOARD_POS(_step.from.py)] = fakeBoard[BOARD_POS(_step.to.px)][BOARD_POS(_step.to.py)];
    //恢复终点位置的棋子类型
    fakeBoard[BOARD_POS(_step.to.px)][BOARD_POS(_step.to.py)] = _step.toType;
}


//获取最好的一步棋
step MachineGame::getRobotBestStep(){
    gameOver = false;
    step ret;
    /****极大极小算法**************************/
  //  GetMaxScore(layers,ret);
    /****负极大值算法**************************/
//    NegaMaxSearch(layers,BLACKSIDE,ret);
    /****Alpha-Beta算法***********************/
 //   AlphaBeta(layers,-INFINITY,INFINITY,BLACKSIDE,ret);
    /***********************/
//    PrincipalVariation(layers,-INFINITY,INFINITY,BLACKSIDE,ret);
    //qDebug() << "hint: " << count;
    /************************/
/*********四线程*****************/
//    TaskThread t1(1,*this);
//    TaskThread t2(2,*this);
//    TaskThread t3(3,*this);
//    TaskThread t4(4,*this);
//    QVector<step> steps;
//    int len = steps.size() / 4;
//    FindAllSteps(BLACKSIDE,steps);
//    t1.initial(steps,0,0+len);
//    t2.initial(steps,0+len+1,0+2*len);
//    t3.initial(steps,0+2*len+1,0+3*len);
//    t4.initial(steps,0+3*len+1,steps.size()-1);
//    t1.run();
//    t2.run();
//    t3.run();
//    t4.run();
//    t1.wait();
//    t2.wait();
//    t3.wait();
//    t4.wait();
//    int max1 = std::max(t1.value,t2.value);
//    int max2 = std::max(t3.value,t4.value);
//    int max3 = std::max(max1,max2);
//    if(max3 == t1.value)
//        return t1.ret;
//    else if(max3 == t2.value)
//        return t2.ret;
//    else if(max3 == t3.value)
//        return t3.ret;
//    else
//        return t4.ret;
/*********三线程*****************/
    TaskThread t1(1,*this);
    TaskThread t2(2,*this);
    TaskThread t3(3,*this);
    QVector<step> steps;
    int len = steps.size() / 3;
    FindAllSteps(BLACKSIDE,steps);
    t1.initial(steps,0,0+len);
    t2.initial(steps,0+len+1,0+2*len);
    t3.initial(steps,0+2*len+1,steps.size()-1);
    t1.run();
    t2.run();
    t3.run();
    t1.wait();
    t2.wait();
    t3.wait();
    int max1 = std::max(t1.value,t2.value);
    int max2 = std::max(t3.value,max1);
    if(max2 == t1.value)
        return t1.ret;
    else if(max2 == t2.value)
        return t2.ret;
    else if(max2 == t3.value)
        return t3.ret;
/************二线程********************/
//        TaskThread t1(1,*this);
//        TaskThread t2(2,*this);
//        QVector<step> steps;
//        int len = steps.size() / 2;
//        FindAllSteps(BLACKSIDE,steps);
//        t1.initial(steps,0,0+len);
//        t2.initial(steps,0+len+1,steps.size()-1);
//        t1.run();
//        t2.run();
//        t1.wait();
//        t2.wait();

//        int max = std::max(t1.value,t2.value);
//        if(max == t1.value)
//            return t1.ret;
//        else if(max == t2.value)
//            return t2.ret;

    return ret;
}

int MachineGame::GetMinScore(int _layers,step &s){
    if(_layers == 0 || gameOver == true)
        return calcscore(-1);
    //若置换表中已经存在则直接返回
//    QString TableKey = BoardArrayToString(fakeBoard,REDSIDE);
//    if(TTable.contains(TableKey)){
//        return TTable[TableKey];
//    }
    //找到红棋下的所有可走步
    QVector<step> steps;
    FindAllSteps(REDSIDE,steps);
    int min = INFINITY;
    for(int i=0;i<steps.size();i++){
        //试着走一下
        fakeMove(steps[i]);
        int score = GetMaxScore(_layers-1,s);
        unFakeMove(steps[i]);
        //当前局面存入哈希表中
        //TTable.insert(TableKey,score);
        //这一层为取最小值 上一层为取最大值
        if(score < min){
            min = score;
        }
    }
    return min;
}

int MachineGame::GetMaxScore(int _layers,step &s){
        if(_layers == 0 || gameOver == true)
            return calcscore(-1);
        //找到黑棋下的所有可走步
        QVector<step> steps;
        FindAllSteps(BLACKSIDE,steps);
        int max = -INFINITY;
        for(int i=0;i<steps.size();i++){
            //试着走一下
            fakeMove(steps[i]);
            int score = GetMinScore(_layers-1,s);
            unFakeMove(steps[i]);
            if(score > max){
                max = score;
                if(_layers == layers)
                    s = steps[i];
            }
        }
        return max;
}

int MachineGame::AlphaBeta(int depth, int alpha, int beta,int side,step &s){
    if(depth == 0 || gameOver)
        return calcscore(side);
    //查看hash表中是否有
//    QString TableKey = BoardArrayToString(fakeBoard,side);
//    if(TTable.contains(TableKey)&&TTable[TableKey].depth >= depth){
//        count ++;
//        //qDebug() << "depth: " << TTable[TableKey].depth << "value: " << TTable[TableKey].value <<"Tdepth: " << depth;
//        return TTable[TableKey].value;
//    }
    //找到所有可走步
    QVector<step> steps;
    FindAllSteps(side,steps);
    for(int i=0;i<steps.size();i++){
        //试着走一下
        fakeMove(steps[i]);
        int score = -AlphaBeta(depth-1,-beta,-alpha,1-side,s);
//        //当前局面存入哈希表中
//        if(!TTable.contains(TableKey)){
//            if(depth >= 2)
//                TTable.insert(TableKey,HTNode(depth,score));
//            //qDebug() << "insert:   depth: " << TTable[TableKey].depth << "value: " << TTable[TableKey].value <<"Tdepth: " << depth;
//        }
//        else if(TTable[TableKey].depth < depth){
//            TTable[TableKey].depth = depth;
//            TTable[TableKey].value = score;
//        }
        unFakeMove(steps[i]);
        if(score > alpha){
            alpha = score;
            if(depth == layers)
                s = steps[i];
        }
        if(alpha >= beta){
            break;
        }
    }
    return alpha;
}

int MachineGame::NegaMaxSearch(int depth, int side,step &s){
    if(depth == 0 || gameOver==true)
        return calcscore(side);
    QVector<step> steps;
    FindAllSteps(side,steps);
    int max = -INFINITY;
    for(int i=0;i<steps.size();i++){
        //试着走一下
        fakeMove(steps[i]);
        int score = -NegaMaxSearch(depth-1,1-side,s);
        unFakeMove(steps[i]);
        //这一层为取最大值 上一层为取最小值
        if(score > max){
            max = score;
            if(depth == layers)
                s = steps[i];
        }
    }
    return max;
}

int MachineGame::PrincipalVariation(int depth, int alpha, int beta, int side,step &s){
    if(depth == 0 || gameOver)
        return calcscore(side);
    //找到所有可走步
    QVector<step> steps;
    FindAllSteps(side,steps);
    //用全窗口搜索第一个节点
    fakeMove(steps[0]);
    int best = -PrincipalVariation(depth-1,-beta,-alpha,1-side,s);
    if(depth == layers)
        s = steps[0];
    unFakeMove(steps[0]);
    //从第二个节点起，对每一节点
    for(int i=1;i<steps.size();i++){
        if(best < beta){
            if(best > alpha)
                alpha = best;
            //空窗探测
            fakeMove(steps[i]);
            int score = -PrincipalVariation(depth-1,-alpha-1,-alpha,1-side,s);
            if(score > alpha && score < beta){
                best = -PrincipalVariation(depth-1,-beta,-score,1-side,s);
            }
            else if(score > best){
                best = score;
                if(depth == layers)
                    s = steps[i];
            }
            unFakeMove(steps[i]);
        }
    }
    return best;
}

int MachineGame::calcscore(int side){
    //评估当前棋面信息
    QVector<chessInfo> chesses;
    EveluateBoard(chesses);
    //计算当前棋面分
    int redSum = 0;
    int blackSum = 0;
    for(int i=0;i<chesses.size();i++){
        int tag = chesses[i].type;
        if(tag > 0){
            //计算棋子分值
             int basicScore = ChessBasicValue[qAbs(tag) - 1];
             int positionScore = PositionValues[REDSIDE][qAbs(tag)-1][chesses[i].pos.px*9+chesses[i].pos.py];
             int activeScore = chesses[i].ActiveNum * ChessActiveValue[qAbs(tag)-1];
             int protectScore = chesses[i].ProtectNum * ProtectValue;
             int threatScore = chesses[i].ThreatNum * ThreatValue;
             redSum += (basicScore + positionScore + activeScore + protectScore + threatScore);
        }
        else if(tag < 0){
            int basicScore = ChessBasicValue[qAbs(tag) - 1];
            int positionScore = PositionValues[BLACKSIDE][qAbs(tag)-1][chesses[i].pos.px*9+chesses[i].pos.py];
            int activeScore = chesses[i].ActiveNum * ChessActiveValue[qAbs(tag)-1];
            int protectScore = chesses[i].ProtectNum * ProtectValue;
            int threatScore = chesses[i].ThreatNum * ThreatValue;
            blackSum += (basicScore + positionScore + activeScore + protectScore + threatScore);
        }
    }
    //极大极小算法 || 负极大算法
    if(side == -1 || side == BLACKSIDE)
        return blackSum - redSum;
    else
        return redSum - blackSum;
}


void MachineGame::EveluateBoard(QVector<chessInfo> &chesses){
    for(int ii =0;ii<10;ii++){
        for(int jj=0;jj<9;jj++){
            //获取该位置标记值 若为空位置则continue
            int type = fakeBoard[BOARD_POS(ii)][BOARD_POS(jj)];
            if(type == EMPTY)
                continue;
            //此位置坐标
            Cpos pos(ii,jj);
            //评估每个棋子棋面信息
            chessInfo chess(type,pos);
            switch(qAbs(type)){
                case KING:{
                    //将帅面对面时可吃对方
                    int cpx = -1;
                    if(type > 0){
                        //帅
                        for(int i=pos.px-1;i>=0;i--){
                            int taget = fakeBoard[BOARD_POS(i)][BOARD_POS(pos.py)];
                            if(taget != EMPTY && taget != BKING){
                                //王之间有棋子
                                break;
                            }else if(taget == BKING){
                                cpx = i;
                                break;
                            }
                        }
                    }
                    if(type < 0){
                        //将
                        for(int i=pos.px+1;i<=10;i++){
                            int taget = fakeBoard[BOARD_POS(i)][BOARD_POS(pos.py)];
                            if(taget != EMPTY && taget != RKING){
                                break;
                            }
                            else if(taget == RKING){
                                cpx = i;
                                break;
                            }
                        }
                    }
                    if(cpx != -1){
                        //该棋子威胁数+1 可走位置数+1
                        chess.ThreatNum++;
                        chess.ActiveNum++;
                    }
                    //寻找四个方向可走位置
                    for (auto p:KingDelta.plist) {
                        int px = pos.px + p.px;
                        int py = pos.py + p.py;
                        //限制将行走范围
                        if(py <= 2 || py >= 6||(type > 0&& (px <= 6||px > 9))||(type < 0 && (px >= 3||px<0)))
                            continue;
                        //如果目标位置可下
                        int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];
                        if(fakeBoard[BOARD_POS(px)][BOARD_POS(py)]==EMPTY||isEnemy(type,tag)){
                            //若该位置是enemy 棋子威胁数+1
                            if(fakeBoard[BOARD_POS(px)][BOARD_POS(py)] != EMPTY)
                                chess.ThreatNum++;
                            //可走位置数+1
                            chess.ActiveNum++;
                        }
                        else
                            //该位置为friend 棋子保护数+1
                            chess.ProtectNum++;
                    }
                    break;
                }
                case ROOK:{
                    //车   四个方向偏移量
                    int xl[4] = {-1,0,1,0};
                    int yl[4] = {0,1,0,-1};
                    for(int i=0;i<4;i++){
                        int px = pos.px + xl[i];
                        int py = pos.py + yl[i];
                        //寻找可下位置
                        while(fakeBoard[BOARD_POS(px)][BOARD_POS(py)]!=INVALID){
                            int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];
                            //如果该位置为空或者为enemy
                            if(tag == EMPTY || isEnemy(type,tag)){
                                //可走位置数+1
                                chess.ActiveNum++;
                                //如果遇到enemy
                                if(tag != EMPTY){
                                    chess.ThreatNum++;
                                    break;
                                }
                                //下一个位置的坐标
                                px = px + xl[i];
                                py = py + yl[i];
                            }
                            else{ //如果是friend
                                chess.ProtectNum++;
                                break;
                            }
                        }//while
                    }//for
                    break;
                }
                case KNIGNT:{
                    //马  四个方向的偏移量（检查是否有绊马脚的子）
                    int xl[4] = {-1,0,1,0};
                    int yl[4] = {0,1,0,-1};
                    for(int i=0;i<4;i++){
                        if(fakeBoard[BOARD_POS(pos.px+xl[i])][BOARD_POS(pos.py+yl[i])] == EMPTY){
                            for (int j=2*i;j<2*i+2;j++) {
                                int px = pos.px + KnightDelta.plist[j].px;
                                int py = pos.py + KnightDelta.plist[j].py;
                                int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];
                                //如果目标位置可下
                                if(tag==EMPTY||isEnemy(type,tag)){
                                    //可走位置数+1
                                    chess.ActiveNum++;
                                    //若该位置是enemy 棋子威胁数+1
                                    if(fakeBoard[BOARD_POS(px)][BOARD_POS(py)] != EMPTY)
                                        chess.ThreatNum++;
                                }
                                else //如果是friend
                                    chess.ProtectNum++;
                            }
                        }
                    }
                    break;
                }
                case ELEPHANT:{
                    //相
                    int xl[4] = {-1,1,1,-1};
                    int yl[4] = {1,1,-1,-1};
                    for(int i=0;i<4;i++){
                        if(fakeBoard[BOARD_POS(pos.px+xl[i])][BOARD_POS(pos.py+yl[i])] == EMPTY){
                            int px = pos.px + ElephantDelta.plist[i].px;
                            int py = pos.py + ElephantDelta.plist[i].py;
                            //如果越界则返回
                            if((type > 0 && px <= 4) || (type < 0 && px >= 5))
                                continue;
                            //要下的位置的值
                            int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];
                            //如果目标位置可下
                            if(tag==EMPTY||isEnemy(type,tag)){
                                //可走位置数+1
                                chess.ActiveNum++;
                                //若该位置是enemy 棋子威胁数+1
                                if(fakeBoard[BOARD_POS(px)][BOARD_POS(py)] != EMPTY)
                                    chess.ThreatNum++;
                            }
                            else //如果是friend
                                chess.ProtectNum++;
                        }
                    }
                    break;
                }
                case MANDARIN:{
                    //士
                    for (auto p:MandarinDelta.plist) {
                        int px = pos.px + p.px;
                        int py = pos.py + p.py;
                        //
                        //要下的位置的值
                        int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];                   //如果目标位置可下
                        if(tag==EMPTY||isEnemy(type,tag)){
                            if((px>=3&&px<=6)||py<=2||py>=6)
                                continue;
                            //可走位置数+1
                            chess.ActiveNum++;
                            //若该位置是enemy 棋子威胁数+1
                            if(fakeBoard[BOARD_POS(px)][BOARD_POS(py)] != EMPTY)
                                chess.ThreatNum++;
                        }
                        else
                            chess.ProtectNum++;
                    }
                    break;
                }
                case CANNON:{
                    //炮   四个方向偏移量
                    int xl[4] = {-1,0,1,0};
                    int yl[4] = {0,1,0,-1};
                    for(int i=0;i<4;i++){
                        int px = pos.px + xl[i];
                        int py = pos.py + yl[i];
                        int times = 0;
                        //寻找可下位置
                        while(fakeBoard[BOARD_POS(px)][BOARD_POS(py)]!=INVALID){
                            int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];
                            //如果该位置为空或者为enemy
                            if(CannonCanset(type,tag,times)){
                                //可走位置数+1
                                chess.ActiveNum++;
                                if(times == 2){
                                    //若该位置是enemy 棋子威胁数+1
                                    chess.ThreatNum++;
                                    break;
                                }
                            }
                            else{
                                //如果炮保护的是friend
                                if(times==2){
                                    chess.ProtectNum++;
                                    break;
                                }
                            }
                            px = px + xl[i];
                            py = py + yl[i];
                        }//while
                    }//for
                    break;
                }
                case PAWN:{
                    //兵  四个方向偏移量
                    int xl[4] = {-1,0,1,0};
                    int yl[4] = {0,1,0,-1};
                    for(int i=0;i<4;i++){
                        int px = pos.px + xl[i];
                        int py = pos.py + yl[i];
                        //棋子不能后退
                        if((type > 0 && i==2) || (type < 0 && i==0))
                            continue;
                        //在自己边只能前进
                        if((type > 0 && px >= 5 && i!=0)||(type < 0 && px <= 4 && i!=2))
                            continue;
                        int tag = fakeBoard[BOARD_POS(px)][BOARD_POS(py)];
                        //如果目标位置可下
                        if(fakeBoard[BOARD_POS(px)][BOARD_POS(py)]==EMPTY||isEnemy(type,tag)){
                            //可走位置数+1
                            chess.ActiveNum++;
                            //若该位置是enemy 棋子威胁数+1
                            if(fakeBoard[BOARD_POS(px)][BOARD_POS(py)] != EMPTY)
                                chess.ThreatNum++;
                        }
                        else
                            chess.ProtectNum++;
                   }
                   break;
                }
             }
            chesses.push_back(chess);
//          qDebug() << "type: " << chess.type
//                     << "active: " << chess.ActiveNum << "threat: "
//                                  << chess.ThreatNum << "protect: " << chess.ProtectNum;
        }
    }
}

QString MachineGame::BoardArrayToString(int _fakeBoard[14][13],int side){
    //字符串为side+fakeBoard
    QString str = "";
    str += QString::number(side);
    for(int i=0;i<10;i++)
        for(int j=0;j<9;j++){
                str +=  QString::number(_fakeBoard[BOARD_POS(i)][BOARD_POS(j)]);
        }
    return str;
}
































