#ifndef TASKTHREAD_H
#define TASKTHREAD_H
#include "QThread"
#include "QDebug"
#include "machinegame.h"
#include "QVector"

class TaskThread : public QThread{
    Q_OBJECT
public:
    friend class MachineGame;
    TaskThread(int _id,MachineGame &_robot);
    TaskThread(int _id);
    void setRobot(MachineGame &_robot){
        robot = _robot;
    }
    void run() override;
    void initial(QVector<step> &slist,int begin,int end);
    step getStep(){
        return ret;
    }
private:
    int id;
    int _fakeBoard[14][13];
    QVector<step> steps;
    MachineGame robot;
    step ret;
    int value;

};

#endif // TASKTHREAD_H
