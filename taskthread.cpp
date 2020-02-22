#include "taskthread.h"

TaskThread::TaskThread(int _id,MachineGame &_robot){
    id = _id;
    robot = _robot;
}

void TaskThread::run() {
    int max = -INFINITY;
    int alpha = -INFINITY;
    int beta = INFINITY;
    for(int i=0;i<steps.size();i++){
        //试着走一下
        robot.fakeMove(steps[i]);
        //int score = robot.GetMinScore(robot.getLayers()-1,ret);
        int score = -robot.AlphaBeta(robot.getLayers()-1,-beta,-alpha,REDSIDE,ret);
        robot.unFakeMove(steps[i]);
//        if(score >= max){
//            max = score;
//            ret = steps[i];
//        }

        if(score > alpha){
            alpha = score;
            ret = steps[i];
        }
        if(alpha >= beta){
            break;
        }
    }
    value = alpha;
    //value = max;
}

void TaskThread::initial(QVector<step> &slist, int begin, int end){
    value = -INFINITY;
    steps.clear();
    for(int i=begin;i<=end;i++)
        steps.push_back(slist[i]);
}
