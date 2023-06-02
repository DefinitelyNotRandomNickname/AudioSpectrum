#include "mainwidget.h"
#include <QTranslator>
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Audio Spectrum");

    MainWidget w;
    w.show();

    return app.exec();
}
