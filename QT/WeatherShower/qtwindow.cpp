//
// Created by YuHuanTin on 2024/1/21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_QTWindow.h" resolved

#include "qtwindow.h"
#include "ui_QTWindow.h"

#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <qdatetime.h>

QTWindow::QTWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::QTWindow) {
    ui->setupUi(this);

}

QTWindow::~QTWindow() {
    delete ui;
}

void QTWindow::drawCharts() {
    // define another name replace ui->graphicsView
    auto &&cv = ui->graphicsView;

    auto chart = new QChart();
    cv->setChart(chart);


    // 创建线条
    auto h_s = new QLineSeries();
    auto l_s = new QLineSeries();
    h_s->setName("最高气温");
    l_s->setName("最低气温");

    h_s->setColor(Qt::red);
    l_s->setColor(Qt::blue);


    // 添加数据到 series
    std::vector h = {2, 1, 5, 7, 10, 11, 11};
    std::vector l = {-4, -3, -2, 0, 0, 4, 5};

    auto min = std::min_element(l.begin(), l.end()).operator*();
    auto max = std::max_element(h.begin(), h.end()).operator*();

    for (std::size_t i = 0; i < h.size(); ++i) {
        h_s->append(QDate::currentDate().addDays(i).startOfDay().toMSecsSinceEpoch(), h.at(i));
        l_s->append(QDate::currentDate().addDays(i).startOfDay().toMSecsSinceEpoch(), l.at(i));
    }

    // 两条线条的数据一定是相同长的
    assert(h.size() == l.size() && h_s->count() == l_s->count());

    // 绑定线条到 chart
    chart->addSeries(h_s);
    chart->addSeries(l_s);


    // 创建坐标轴
    auto axisX = new QDateTimeAxis();
    auto axisY = new QValueAxis();
    axisX->setTitleText("日期");
    axisY->setTitleText("温度");

    axisX->setFormat("dddd");
    axisY->setLabelFormat("%.1f");

    axisX->setTickCount(h_s->count() + 2);
    axisY->setTickCount(h_s->count());


    // 设置 x 轴范围
    axisX->setRange(QDate::currentDate().addDays(-1).startOfDay(), QDate::currentDate().addDays(7).startOfDay());
    // 设置 y 轴范围
    axisY->setRange(min - 5, max + 5);


    // 绑定坐标轴
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    // 设置坐标轴
    h_s->attachAxis(axisX);
    h_s->attachAxis(axisY);
    l_s->attachAxis(axisX);
    l_s->attachAxis(axisY);

    // 显示坐标轴
    axisX->setLabelsVisible(true);
    axisY->setLabelsVisible(true);

    // 显示标签
    h_s->setPointLabelsVisible(true);
    l_s->setPointLabelsVisible(true);

    // 设置标签格式
    h_s->setPointLabelsFormat(QStringLiteral("@yPoint°C"));
    l_s->setPointLabelsFormat(QStringLiteral("@yPoint°C"));
}
