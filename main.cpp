#include "mainwindow.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QScreen *screen=QGuiApplication::primaryScreen();
    QRect screenGeometry=screen->geometry();
    int height=screenGeometry.height();
    int width=screenGeometry.width();

    if(height==w.maximumHeight()&&width==w.maximumWidth())
    w.showFullScreen();
    else
    w.setGeometry(0,0,0,0);
    w.show();

    return a.exec();
}
