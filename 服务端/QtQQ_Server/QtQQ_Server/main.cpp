#include "QtQQ_Server.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtQQ_Server w;
    w.show();
    return a.exec();
}

//市场部有问题
//F10进行调试