#include "blueterm.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BlueTerm w;
    w.show();
    return a.exec();
}
