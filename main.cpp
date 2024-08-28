#include "hitokotowidget.h"
#include <QApplication>
#include <QWidget>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    auto w = new HitokotoWidget();
    w->show();

    return QApplication::exec();
}
