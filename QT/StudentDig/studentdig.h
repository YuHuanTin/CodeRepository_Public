#ifndef STUDENTDIG_H
#define STUDENTDIG_H

#include "QtSql/qsqldatabase.h"
#include "QtSql/qsqlquery.h"
#include "QtSql/qsqlquerymodel.h"
#include <QDialog>
#include <QtSql/QSqlDatabase> // 专用于连接、创建数据库
#include <QtSql/QSqlQuery> // 专门用于 DML (数据操纵语言)、DDL (数据定义语言)
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class StudentDig; }
QT_END_NAMESPACE

class StudentDig : public QDialog {
Q_OBJECT

public:
    StudentDig(QWidget *parent = nullptr);

    ~StudentDig();

private slots:

    // 排序数据
    void on_pushButtonSort_clicked();

    // 添加数据
    void on_pushButton_INSERT_clicked();

    // 删除数据
    void on_pushButton_DELETE_clicked();

    // 更新数据
    void on_pushButton_UPDATE_clicked();

    // 查询数据
    void on_pushButton_QUERY_clicked();

    // 记录被修改过的格子
    void on_tableWidget_itemChanged(QTableWidgetItem *);

    void on_tableWidget_itemPressed(QTableWidgetItem *);

private: // 自定义函数
    // 创建 SQLite 数据库
    void createDatabaseFunc();

    // 创建 SQLite 数据表
    void createTableFunc();

    // 执行查询操作
    QStringList queryTableFunc();

    // 更新数据
    void updateTableFunc(QTableWidgetItem *Item);

    // 删除记录
    bool deleteTableFunc(unsigned ID);

    QSqlDatabase sqlDatabase; // 创建 SQL 和数据链接
    QSqlQueryModel sqlQueryModel; // 存储结果集

    int oldRow = -1; // 被修改的选择框的行数
    int oldID = -1; // 被修改的选择框的id

private:
    Ui::StudentDig *ui;
};

#endif // STUDENTDIG_H
