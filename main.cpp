#include "widget.h"
#include <QApplication>
#include "QHash"
#include "taskthread.h"

int main(int argc, char *argv[])
{
       QApplication a(argc, argv);
       Widget w;
       w.setWindowTitle("中国象棋");
       w.show();
       return a.exec();
}
