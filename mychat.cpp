#include "mychat.h"
#include "ui_mychat.h"

#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QProcess>
#include <QColorDialog>
#include <QStandardPaths>
#include <QUdpSocket>
#include <QDateTime>
#include <QMessageBox>
#include <QScrollBar>
#include <QFileDialog>
#include <QStandardPaths>
#include <QPrinter>
#include <QPainter>
#include <QTextBlock>
#include <QMenu>
#include <QAction>

MyChat::MyChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyChat)
{
    ui->setupUi(this);

    init();
    createMenu();

    ui->textEdit->installEventFilter(this);     //安装事件过滤器
}

MyChat::~MyChat()
{
    delete ui;
}

//初始化
void MyChat::init()
{
    color = Qt::darkGreen;
    ui->textEdit->setTextColor(color);

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);        //不可编辑
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);       //选中整行
    QStringList hList;
    hList << tr("用户名") << tr("主机名") << tr("IP地址");
    ui->tableWidget->setHorizontalHeaderLabels(hList);

    QStringList sizeList;
    sizeList << tr("9") << tr("10") << tr("11") << tr("12") << tr("13") << tr("14") << tr("15")
             << tr("16") << tr("17") << tr("18") << tr("19") << tr("20") << tr("21") << tr("22");
    ui->comboBox->addItems(sizeList);
    ui->comboBox->setCurrentIndex(3);
    ui->textEdit->setFontPointSize(ui->comboBox->currentText().toDouble());

    connect(ui->tBtn_close, SIGNAL(clicked()), this, SLOT(close()));

    //初始化用户名、主机名、IP地址
    QString userName = getUserName();
    QString localHost = QHostInfo::localHostName();
    QString ipAddr = getIP(localHost);
    QStringList ownerList;
    ownerList << userName << localHost << ipAddr;
    ui->tableWidget->setRowCount(1);
    for(int i = 0; i < 3; i++){
        QTableWidgetItem *item = new QTableWidgetItem(ownerList.at(i));
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(0, i, item);
    }

    sender = new QUdpSocket(this);
    receiver = new QUdpSocket(this);
    port = 6665;
    receiver->bind(port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(receiver, SIGNAL(readyRead()), this, SLOT(slot_recvMsg()));
}

//创建发送消息菜单
void MyChat::createMenu()
{
    act_enter = new QAction(tr("按Enter键发送消息"), this);
    act_enter->setCheckable(true);
    act_enter->setChecked(true);
    isEnter = true;
    act_ctrl = new QAction(tr("按Ctrl+Enter键发送消息"), this);
    act_ctrl->setCheckable(true);

    actGrp = new QActionGroup(this);
    actGrp->addAction(act_enter);
    actGrp->addAction(act_ctrl);
    connect(actGrp, SIGNAL(triggered(QAction*)), this, SLOT(slot_sendWay(QAction*)));

    menu = new QMenu(ui->tBtn_sendMsg);
    menu->addAction(act_enter);
    menu->addAction(act_ctrl);
    ui->tBtn_sendMsg->setMenu(menu);
}

//获取用户名
QString MyChat::getUserName()
{
#if 1
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();
    foreach (QString string, envVariables) {
        int index = environment.indexOf(QRegExp(string));
        if (index != -1) {
            QStringList stringList = environment.at(index).split('=');
            if (stringList.size() == 2) {
                return stringList.at(1);
                break;
            }
        }
    }
    return "unknown";
#else
    QString userName = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    userName = userName.section("/", -1, -1);
    return userName;
#endif
}

//获取本机IP
QString MyChat::getIP(QString localHost)
{
    QString ipAddr;
#if 0
    QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
    foreach(QHostAddress address, AddressList){
        if(address.protocol() == QAbstractSocket::IPv4Protocol &&
                address != QHostAddress::Null &&
                address != QHostAddress::LocalHost){
            if (address.toString().contains("127.0.")){
                continue;
            }
            ipAddr = address.toString();
            break;
        }
    }
#else
    QHostInfo info = QHostInfo::fromName(localHost);
    info.addresses();//QHostInfo的address函数获取本机ip地址
    //如果存在多条ip地址ipv4和ipv6：
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol()==QAbstractSocket::IPv4Protocol){//只取ipv4协议的地址
            qDebug()<<address.toString();
            ipAddr = address.toString();
        }
    }
#endif
    return ipAddr;
}

//获取消息
QString MyChat::getMessage()
{
    QString msg = ui->textEdit->toHtml();

    ui->textEdit->clear();
    ui->textEdit->setFocus();
    return msg;
}

//发送消息
void MyChat::sendMsg()
{
    if (ui->textEdit->toPlainText() == "") {
        QMessageBox::warning(0,tr("警告"),tr("发送内容不能为空"),QMessageBox::Ok);
        return;
    }

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QString localHostName = QHostInfo::localHostName();
    QString address = getIP(localHostName);
    qDebug() << "send:" << address;
    QString msg = getMessage();
    out << getUserName() << localHostName;
    out << address << msg;
    ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
    //UDP广播
//    sender->writeDatagram(data,data.length(),QHostAddress::Broadcast, port);
    //向特定IP发送
    QHostAddress serverAddress = QHostAddress(address);
    sender->writeDatagram(data.data(), data.size(),serverAddress, port);

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textBrowser->setTextColor(Qt::darkCyan);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",12));
    ui->textBrowser->append("[ " +getUserName()+" ] "+ time);
    ui->textBrowser->setAlignment(Qt::AlignRight);
    ui->textBrowser->append(msg);
}

//键盘按下事件
void MyChat::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return){
        qDebug() << "key Enter~";
    }else if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return && (e->modifiers() & Qt::ControlModifier)){
        qDebug() << "key Ctrl+Enter~";
    }
}

//键盘释放事件
void MyChat::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return){
        qDebug() << "key release Enter~";
        if(isEnter){
            sendMsg();
        }
    }else if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return && (e->modifiers() & Qt::ControlModifier)){
        qDebug() << "key release Ctrl+Enter~";
    }
}

//事件过滤器
bool MyChat::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->textEdit){
        if(event->type() == QEvent::KeyPress){
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return){
                qDebug() << "event Enter~";
                if(isEnter){
                    ui->textEdit->setLineWrapMode(QTextEdit::NoWrap);
                    return true;
                }else{
                    ui->textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
                    return false;
                }
            }else if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return &&
                    (keyEvent->modifiers() & Qt::ControlModifier)){
                qDebug() << "event Ctrl+Enter~";
//                if(!isEnter){
//                    sendMsg();
//                }
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

//调用软键盘:需将项目中的软键盘拷贝到应用程序所在目录
void MyChat::on_tBtn_keyBoard_clicked()
{
    QString path = QCoreApplication::applicationDirPath();
    path.append("/osk.exe");
    QProcess::startDetached(path);
}

//粗体
void MyChat::on_tBtn_Bold_clicked(bool checked)
{
    if(checked){
        ui->textEdit->setFontWeight(QFont::Bold);
    }else{
        ui->textEdit->setFontWeight(QFont::Normal);
    }
}

//斜体
void MyChat::on_tBtn_Italic_clicked(bool checked)
{
    ui->textEdit->setFontItalic(checked);
}

//下划线
void MyChat::on_tBtn_Underline_clicked(bool checked)
{
    ui->textEdit->setFontUnderline(checked);
}

//设置颜色
void MyChat::on_tBtn_color_clicked()
{
    color = QColorDialog::getColor(color, this);
    if(color.isValid()){
        ui->textEdit->setTextColor(color);
    }
}

//发送文件
void MyChat::on_tBtn_sendFile_clicked()
{
}

//保存聊天记录
void MyChat::on_tBtn_saveFile_clicked()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), path,
                                                    tr("Word文档(*.doc)"));
    if(fileName.isEmpty()){
        return;
    }
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream in(&file);
        QString text = ui->textBrowser->toHtml();
        in << text;
        file.close();
        QMessageBox::information(this, tr("提示"), tr("保存成功!"));
    }
}

//清空聊天记录
void MyChat::on_tBtn_clear_clicked()
{
    ui->textBrowser->clear();
}

//设置字体大小
void MyChat::on_comboBox_activated(const QString &arg1)
{
    ui->textEdit->setFontPointSize(arg1.toDouble());
}

//设置字体
void MyChat::on_fontComboBox_currentFontChanged(const QFont &f)
{
    ui->textEdit->setFont(f);
}

//接收消息
void MyChat::slot_recvMsg()
{
    while(receiver->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(receiver->pendingDatagramSize());
        receiver->readDatagram(datagram.data(), datagram.size());
        QDataStream in(&datagram, QIODevice::ReadOnly);

        QString userName,localHostName,ipAddress,message;
        QString time = QDateTime::currentDateTime()
                .toString("yyyy-MM-dd hh:mm:ss");

        in >> userName >> localHostName >> ipAddress >> message;
        ui->textBrowser->setTextColor(Qt::blue);
        ui->textBrowser->setCurrentFont(QFont("Times New Roman",12));
        ui->textBrowser->append("[ " +userName+" ] "+ time);
        ui->textBrowser->setAlignment(Qt::AlignLeft);
        ui->textBrowser->append(message);
        break;
    }
}

//选择发送方式
void MyChat::slot_sendWay(QAction *act)
{
    if(act == act_enter){
        qDebug() << "Enter!";
        isEnter = true;
    }else{
        qDebug() << "Ctrl+Enter!";
        isEnter = false;
    }
}

//发送消息
void MyChat::on_tBtn_sendMsg_clicked()
{
    sendMsg();
}
