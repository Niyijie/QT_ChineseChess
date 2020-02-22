#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

/*
只支持两人网络对战
*/
#define REDSIDE 10       //红色
#define BLACKSIDE -10    //黑色

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    //服务器主循环
    void run();
private slots:
    //网络消息解析函数
    void ServerParseMessage(int message,QTcpSocket *conSocket,QTcpSocket *anotherSocket);
    //坐标转换函数
    int convertXPosition(int x);
    int convertYPosition(int y);

private:
    Ui::Widget *ui;
    //服务器
    QTcpServer *server;
    //客户端套接字
    QTcpSocket *clientRed;
    QTcpSocket *clientBlack;
};
#endif // WIDGET_H
