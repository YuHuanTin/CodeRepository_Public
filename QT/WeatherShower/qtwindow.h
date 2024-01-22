//
// Created by YuHuanTin on 2024/1/21.
//

#ifndef PRO_QTWINDOW_H
#define PRO_QTWINDOW_H

#include <QWidget>
#include <QGraphicsView>

#include <span>

QT_BEGIN_NAMESPACE
namespace Ui { class QTWindow; }
QT_END_NAMESPACE

class QTWindow : public QWidget {
Q_OBJECT

public:
    explicit QTWindow(QWidget *parent = nullptr);

    void drawCharts();

    ~QTWindow() override;

private:
    Ui::QTWindow *ui;
};


#endif //PRO_QTWINDOW_H
