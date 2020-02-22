#include "widget.h"

bool ChessObj::isEnemy(int tag){
    if(tag == INVALID)
        return false;
    if((type>0&&tag>0)||(type<0&&tag<0))
        return false;          //同色是friend
    else
        return true;           //异色为enemy
}

void ChessObj::mousePressEvent(QMouseEvent *e)//鼠标点击事件
{   
    //如果游戏结束或未开始
    if(!dynamic_cast<Widget*>(father)->gameStart)
        return;

    //在网络对战中不是本方回合则返回
    int turnFlag = dynamic_cast<Widget*>(father)->turnFlag;
    int myColor = dynamic_cast<Widget*>(father)->myColor;
    int gameMode = dynamic_cast<Widget*>(father)->gameMode;
    if(gameMode == NETBATTLE && ((myColor==RED&&turnFlag==BLACK) || (myColor==BLACK&&turnFlag==BLACK)))
        return;
    //
    int turn = dynamic_cast<Widget*>(father)->turnFlag;
    //只可控制本回合的棋子
    if((turn == REDTURN && isEnemy(REDTURN)) || (turn == BLACKTURN && isEnemy(BLACKTURN))){
        return;
    }
    //选中音效
    dynamic_cast<Widget*>(father)->Mselect->play();
    //清空上次选中棋子的信息
    if(dynamic_cast<Widget*>(father)->boxTags.size() != 0){
            dynamic_cast<Widget*>(father)->initCansetInfo();
    }
    //显示新框
    run(true);
    dynamic_cast<Widget*>(father)->flushBoxs();
    //更新此时选中棋子
    dynamic_cast<Widget*>(father)->ChooseChess = this;
}

bool ChessObj::CannonCanset(int tag, int &times){
    if(tag !=EMPTY && tag != INVALID)
        times++;
    if(times == 0 && tag == EMPTY)   //如果该位置为空
        return true;
    if(times == 2 && isEnemy(tag)){
        return true;
    }
    return false;
}

void ChessObj::run(bool mode)   //mode=false为检查是否将军，mode为true为走棋
{
    //更新选中棋子 若为false模式则无需更新
    if(mode)
        dynamic_cast<Widget*>(father)->ChooseChess = this;
    switch(qAbs(type)){
        //
        case KING:{
            //将帅面对面时可吃对方
            int cpx = -1;
            if(type > 0){
                //帅
                for(int i=pos.px-1;i>=0;i--){
                    int taget = dynamic_cast<Widget*>(father)->board[BOARD_POS(i)][BOARD_POS(pos.py)];
                    if(taget != EMPTY && taget != BKING){
                        break;
                    }else if(taget == BKING){
                        cpx = i;
                        dynamic_cast<Widget*>(father)->Mcheck->play();
                        break;
                    }
                }
            }
            if(type < 0){
                //将
                for(int i=pos.px+1;i<=10;i++){
                    int taget = dynamic_cast<Widget*>(father)->board[BOARD_POS(i)][BOARD_POS(pos.py)];
                    if(taget != EMPTY && taget != RKING){
                        break;
                    }
                    else if(taget == RKING){
                        dynamic_cast<Widget*>(father)->Mcheck->play();
                        cpx = i;
                        break;
                    }
                }
            }
            if(mode && cpx != -1){
                dynamic_cast<Widget*>(father)->Mcheck->play();
                QLabel *box = new QLabel();
                QPixmap *pixmax = new QPixmap();
                pixmax->load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/canset.png");
                box->setPixmap(*pixmax);
                box->move(BX+pos.py*STEP_LEN,BY+cpx*STEP_LEN);
                dynamic_cast<Widget*>(father)->boxTags.push_back(box);
                //棋盘对应位置置true
                dynamic_cast<Widget*>(father)->CansetArray[cpx][pos.py] = CANSET;
            }
            //寻找四个方向可走位置
            for (auto p:delta.plist) {
                int px = pos.px + p.px;
                int py = pos.py + p.py;
                //限制将行走范围
                if(py <= 2 || py >= 6||(type > 0&& (px <= 6||px > 9))||(type < 0 && (px >= 3||px<0)))
                    continue;
                //如果目标位置可下
                int tag = dynamic_cast<Widget*>(father)->board[BOARD_POS(px)][BOARD_POS(py)];
                //如果目标位置可下
                if(dynamic_cast<Widget*>(father)->board[BOARD_POS(px)][BOARD_POS(py)]==EMPTY||isEnemy(tag)){
                    //添加新的虚框
                    if(mode){
                        QLabel *box = new QLabel();
                        QPixmap *pixmax = new QPixmap();
                        pixmax->load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/canset.png");
                        box->setPixmap(*pixmax);
                        box->move(BX+py*STEP_LEN,BY+px*STEP_LEN);
                        dynamic_cast<Widget*>(father)->boxTags.push_back(box);
                        //棋盘对应位置置true
                        dynamic_cast<Widget*>(father)->CansetArray[px][py] = CANSET;
                    }
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
                while(dynamic_cast<Widget*>(father)->board[BOARD_POS(px)][BOARD_POS(py)]!=INVALID){
                    int tag = dynamic_cast<Widget*>(father)->board[BOARD_POS(px)][BOARD_POS(py)];
                    //如果该位置为空或者为enemy
                    if(tag == EMPTY || isEnemy(tag)){
                        if(mode){
                            QLabel *box = new QLabel();
                            QPixmap *pixmax = new QPixmap();
                            pixmax->load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/canset.png");
                            box->setPixmap(*pixmax);
                            box->move(BX+py*STEP_LEN,BY+px*STEP_LEN);
                            dynamic_cast<Widget*>(father)->boxTags.push_back(box);
                            dynamic_cast<Widget*>(father)->CansetArray[px][py] = CANSET;
                        }
                        //判断是否将军
                        if(qAbs(tag)==KING&&isEnemy(tag)){
                            //将军音效
                            dynamic_cast<Widget*>(father)->Mcheck->play();
                        }
                        //如果遇到enemy
                        if(tag != EMPTY)
                            break;
                        px = px + xl[i];
                        py = py + yl[i];
                    }
                    else       //如果是friend
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
                if(dynamic_cast<Widget*>(father)->board[BOARD_POS(pos.px+xl[i])][BOARD_POS(pos.py+yl[i])] == 0){
                    for (int j=2*i;j<2*i+2;j++) {
                        int px = pos.px + delta.plist[j].px;
                        int py = pos.py + delta.plist[j].py;
                        int tag = dynamic_cast<Widget*>(father)->board[BOARD_POS(px)][BOARD_POS(py)];
                        //如果目标位置可下
                        if(tag==EMPTY||isEnemy(tag)){
                            if(mode){
                                QLabel *box = new QLabel();
                                QPixmap *pixmax = new QPixmap();
                                pixmax->load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/canset.png");
                                box->setPixmap(*pixmax);
                                box->move(BX+py*STEP_LEN,BY+px*STEP_LEN);
                                dynamic_cast<Widget*>(father)->boxTags.push_back(box);
                                dynamic_cast<Widget*>(father)->CansetArray[px][py] = CANSET;
                            }
                            //判断是否将军
                            if(qAbs(tag)==KING && isEnemy(tag)){
                                //将军音效
                                dynamic_cast<Widget*>(father)->Mcheck->play();
                            }
                        }
                    }
                }
            }
            break;
        }
        case ELEPHANT:{
            //相
            if(!mode)
                return;
            int xl[4] = {-1,1,1,-1};
            int yl[4] = {1,1,-1,-1};
            for(int i=0;i<4;i++){
                if(dynamic_cast<Widget*>(father)->board[BOARD_POS(pos.px+xl[i])][BOARD_POS(pos.py+yl[i])] == EMPTY){
                    int px = pos.px + delta.plist[i].px;
                    int py = pos.py + delta.plist[i].py;
                    //如果越界则返回
                    if((type > 0 && px <= 4) || (type < 0 && px >= 5))
                        continue;
                    //要下的位置的值
                    int tag = dynamic_cast<Widget*>(father)->board[BOARD_POS(px)][BOARD_POS(py)];
                    //如果目标位置可下
                    if(tag==EMPTY||isEnemy(tag)){
                        QLabel *box = new QLabel();
                        QPixmap *pixmax = new QPixmap();
                        pixmax->load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/canset.png");
                        box->setPixmap(*pixmax);
                        box->move(BX+py*STEP_LEN,BY+px*STEP_LEN);
                        dynamic_cast<Widget*>(father)->boxTags.push_back(box);
                        dynamic_cast<Widget*>(father)->CansetArray[px][py] = CANSET;
                    }
                }
            }
            break;
        }
        case MANDARIN:{
            //士
            if(!mode)
                return;
            for (auto p:delta.plist) {
                int px = pos.px + p.px;
                int py = pos.py + p.py;
                //
                //要下的位置的值
                int tag = dynamic_cast<Widget*>(father)->board[BOARD_POS(px)][BOARD_POS(py)];                   //如果目标位置可下
                if(tag==EMPTY||isEnemy(tag)){
                    if((px>=3&&px<=6)||py<=2||py>=6)
                        continue;
                    QLabel *box = new QLabel();
                    QPixmap *pixmax = new QPixmap();
                    pixmax->load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/canset.png");
                    box->setPixmap(*pixmax);
                    box->move(BX+py*STEP_LEN,BY+px*STEP_LEN);
                    dynamic_cast<Widget*>(father)->boxTags.push_back(box);
                    dynamic_cast<Widget*>(father)->CansetArray[px][py] = CANSET;
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
                while(dynamic_cast<Widget*>(father)->board[BOARD_POS(px)][BOARD_POS(py)]!=INVALID){
                    int tag = dynamic_cast<Widget*>(father)->board[BOARD_POS(px)][BOARD_POS(py)];
                    //如果该位置为空或者为enemy
                    if(CannonCanset(tag,times)){
                        if(mode){
                            QLabel *box = new QLabel();
                            QPixmap *pixmax = new QPixmap();
                            pixmax->load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/canset.png");
                            box->setPixmap(*pixmax);
                            box->move(BX+py*STEP_LEN,BY+px*STEP_LEN);
                            dynamic_cast<Widget*>(father)->boxTags.push_back(box);
                            dynamic_cast<Widget*>(father)->CansetArray[px][py] = CANSET;
                        }
                        //判断是否将军
                        if(qAbs(tag)==KING && isEnemy(tag)){
                            //将军音效
                            dynamic_cast<Widget*>(father)->Mcheck->play();
                        }
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
                int tag = dynamic_cast<Widget*>(father)->board[BOARD_POS(px)][BOARD_POS(py)];
                //如果目标位置可下
                if(dynamic_cast<Widget*>(father)->board[BOARD_POS(px)][BOARD_POS(py)]==EMPTY||isEnemy(tag)){
                    if(mode){
                        //添加新的虚框
                        QLabel *box = new QLabel();
                        QPixmap *pixmax = new QPixmap();
                        pixmax->load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/canset.png");
                        box->setPixmap(*pixmax);
                        box->move(BX+py*STEP_LEN,BY+px*STEP_LEN);
                        dynamic_cast<Widget*>(father)->boxTags.push_back(box);
                        //棋盘对应位置置true
                        dynamic_cast<Widget*>(father)->CansetArray[px][py] = CANSET;
                    }
                    //判断是否将军
                    if(qAbs(tag)==KING && isEnemy(tag)){
                        //将军音效
                        dynamic_cast<Widget*>(father)->Mcheck->play();
                    }
                }
            }
        }

    }
}


//棋子制造工产
ChessObj* ChessObj::ChessFactory(int type)
{
    //空
    if(type == 0)
        return nullptr;
    //如果是黑子
    bool flag = true; //true为红棋
    if(type < 0)
    {
        type = -type;
        flag = false;
    }
    ChessObj *chess = new ChessObj();
    switch (type) {
        case KING:
        {
            if(flag){
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/rking.png");
                chess->setPixmap(pixmax);
            }
            else{
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/bking.png");
                chess->setPixmap(pixmax);
            }
            break;
        }
        case ROOK:
        {
            if(flag){
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/rrook.png");
                chess->setPixmap(pixmax);
            }
            else{
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/brook.png");
                chess->setPixmap(pixmax);
            }
            break;
        }
        case KNIGNT:
        {
            if(flag){
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/rknight.png");
                chess->setPixmap(pixmax);
            }
            else{
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/bknight.png");
                chess->setPixmap(pixmax);
            }
            break;
        }
        case ELEPHANT:
        {
            if(flag){
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/relephant.png");
                chess->setPixmap(pixmax);
            }
            else{
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/belephant.png");
                chess->setPixmap(pixmax);
            }
            break;
        }
        case MANDARIN:
        {
            if(flag){
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/rmandarin.png");
                chess->setPixmap(pixmax);
            }
            else{
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/bmandarin.png");
                chess->setPixmap(pixmax);
            }
            break;
        }
        case CANNON:
        {
            if(flag){
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/rcannon.png");
                chess->setPixmap(pixmax);
            }
            else{
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/bcannon.png");
                chess->setPixmap(pixmax);
            }
            break;
        }
        case PAWN:
        {
            if(flag){
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/rpawn.png");
                chess->setPixmap(pixmax);
            }
            else{
                QPixmap pixmax;
                pixmax.load("/Users/niyijie/Desktop/QT/ChineseChess/res/img/bpawn.png");
                chess->setPixmap(pixmax);
            }
            break;
        }
    }//switch
    return chess;
}











