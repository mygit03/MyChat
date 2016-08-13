#include "mychat.h"
#include "ui_mychat.h"

#include <QHostAddress>
#include <QNetworkInterface>
#include <QProcess>
#include <QColorDialog>

MyChat::MyChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyChat)
{
    ui->setupUi(this);

    init();
}

MyChat::~MyChat()
{
    delete ui;
}

void MyChat::init()
{
    color = Qt::red;
    ui->textEdit->setTextColor(color);

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    QStringList hList;
    hList << tr("用户名") << tr("主机名") << tr("IP地址");
    ui->tableWidget->setHorizontalHeaderLabels(hList);

    QStringList sizeList;
    sizeList << tr("9") << tr("10") << tr("11") << tr("12") << tr("13") << tr("14") << tr("15")
             << tr("16") << tr("17") << tr("18") << tr("19") << tr("20") << tr("21") << tr("22");
    ui->comboBox->addItems(sizeList);
    ui->comboBox->setCurrentIndex(5);
    ui->textEdit->setFontPointSize(ui->comboBox->currentText().toDouble());

    connect(ui->btn_close, SIGNAL(clicked()), this, SLOT(close()));
}

QString MyChat::getUserName()
{
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
}

QString MyChat::getIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            return address.toString();
    }
    return 0;
}

void MyChat::on_tBtn_Bold_clicked(bool checked)
{
    if(checked){
        ui->textEdit->setFontWeight(QFont::Bold);
    }else{
        ui->textEdit->setFontWeight(QFont::Normal);
    }
}

void MyChat::on_tBtn_Italic_clicked(bool checked)
{
    ui->textEdit->setFontItalic(checked);
}

void MyChat::on_tBtn_Underline_clicked(bool checked)
{
    ui->textEdit->setFontUnderline(checked);
}

void MyChat::on_tBtn_color_clicked()
{
    color = QColorDialog::getColor(color, this);
    if(color.isValid()){
        ui->textEdit->setTextColor(color);
    }
}

void MyChat::on_tBtn_send_clicked()
{

}

void MyChat::on_tBtn_save_clicked()
{

}

void MyChat::on_tBtn_clear_clicked()
{
    ui->textBrowser->clear();
}

void MyChat::on_comboBox_activated(const QString &arg1)
{
    ui->textEdit->setFontPointSize(arg1.toDouble());
}

void MyChat::on_fontComboBox_currentFontChanged(const QFont &f)
{
    ui->textEdit->setFont(f);
}
