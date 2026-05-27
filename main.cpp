#include "constructor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Constructor w;
    w.show();
    return QApplication::exec();
}
