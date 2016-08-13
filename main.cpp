#include "mychat.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyChat w;
    w.show();

    return a.exec();
}
