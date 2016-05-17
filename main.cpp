#include "janela.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Janela w;
    w.show();

    return a.exec();
}


