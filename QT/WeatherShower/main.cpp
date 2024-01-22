#include <QApplication>
#include <QPushButton>

#include "qtwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    auto w = std::make_unique<QTWindow>();
    w->drawCharts();
    w->show();

    return a.exec();
}
