#ifndef MYCHAT_H
#define MYCHAT_H

#include <QWidget>

namespace Ui {
class MyChat;
}

class MyChat : public QWidget
{
    Q_OBJECT

public:
    explicit MyChat(QWidget *parent = 0);
    ~MyChat();

    void init();
    QString getUserName();
    QString getIP();

private slots:
    void on_tBtn_Bold_clicked(bool checked);

    void on_tBtn_Italic_clicked(bool checked);

    void on_tBtn_Underline_clicked(bool checked);

    void on_tBtn_color_clicked();

    void on_tBtn_send_clicked();

    void on_tBtn_save_clicked();

    void on_tBtn_clear_clicked();

    void on_comboBox_activated(const QString &arg1);

    void on_fontComboBox_currentFontChanged(const QFont &f);

private:
    Ui::MyChat *ui;

    QColor color;
};

#endif // MYCHAT_H
