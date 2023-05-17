#include "studentdig.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    StudentDig w;
    w.show();

    return a.exec();
}
