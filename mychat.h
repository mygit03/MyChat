#ifndef MYCHAT_H
#define MYCHAT_H

#include <QWidget>
#include <QKeyEvent>

class QUdpSocket;
class QMenu;
class QAction;
class QActionGroup;

namespace Ui {
class MyChat;
}

class MyChat : public QWidget
{
    Q_OBJECT

public:
    explicit MyChat(QWidget *parent = 0);
    ~MyChat();

    void init();                            //初始化
    void createMenu();                      //创建发送消息菜单
    QString getUserName();                  //获取用户名
    QString getIP(QString localHost = "");  //获取IP地址
    QString getMessage();                   //获得要发送的消息
    void sendMsg();                         //发送消息

protected:
    virtual void keyPressEvent(QKeyEvent *e);

    virtual void keyReleaseEvent(QKeyEvent *e);

    virtual bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void on_tBtn_Bold_clicked(bool checked);

    void on_tBtn_Italic_clicked(bool checked);

    void on_tBtn_Underline_clicked(bool checked);

    void on_tBtn_color_clicked();

    void on_tBtn_sendFile_clicked();

    void on_tBtn_saveFile_clicked();

    void on_tBtn_clear_clicked();

    void on_comboBox_activated(const QString &arg1);

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_tBtn_sendMsg_clicked();

    void slot_recvMsg();

    void slot_sendWay(QAction *act);

private:
    Ui::MyChat *ui;

    QColor color;
    QUdpSocket *sender;     //发送方
    QUdpSocket *receiver;   //接收方
    qint16 port;            //端口号

    QMenu *menu;            //发送消息菜单
    QAction *act_enter;
    QAction *act_ctrl;
    QActionGroup *actGrp;   //动作组
    bool isEnter;           //按键发送消息方式
};

#endif // MYCHAT_H
