#include "studentdig.h"
#include "QtSql/qsqldatabase.h"
#include "qdebug.h"
#include "qglobal.h"
#include "qlineedit.h"
#include "qlist.h"
#include "qmessagebox.h"
#include "qnamespace.h"
#include "qsqlquery.h"
#include "qsqlrecord.h"
#include "ui_studentdig.h"
#include <iostream>


StudentDig::StudentDig(QWidget *parent)
        : QDialog(parent), ui(new Ui::StudentDig) {
    ui->setupUi(this);

    createDatabaseFunc();

    createTableFunc();
}

StudentDig::~StudentDig() {
    sqlDatabase.close();

    delete ui;
}

void StudentDig::on_pushButtonSort_clicked() {
    QMessageBox::information(nullptr, "msg", __FUNCTION__);
}

void StudentDig::on_pushButton_INSERT_clicked() {
    QSqlQuery sqlQuery;

    unsigned id = ui->lineEdit_ID->text().toUInt();
    if (id == 0) {
        QMessageBox::critical(nullptr, "failed", "student == 0", QMessageBox::Ok);
        return;
    }

    QString name = ui->lineEdit_NAME->text();
    if (name.isEmpty()) {
        QMessageBox::critical(nullptr, "failed", "name is empty", QMessageBox::Ok);
        return;
    }

    double score = ui->lineEdit_SCORE->text().toDouble();
    if (score < 0 || score > 100) {
        QMessageBox::critical(nullptr, "failed", "score must range on 0-100", QMessageBox::Ok);
        return;
    }

    QString sqlStr = QString("insert into student "
                             "values(%1,'%2',%3);").arg(id).arg(name).arg(score);

    if (!sqlQuery.exec(sqlStr)) {
        QMessageBox::critical(nullptr, "failed", "insert data failed!", QMessageBox::Ok);
        return;
    }

    on_pushButton_QUERY_clicked();
}

void StudentDig::on_pushButton_DELETE_clicked() {
    QList<QTableWidgetItem *> list = ui->tableWidget->selectedItems();
    QList<int> deleteRows;
    for (int i = 0; i < list.count(); ++i) {
        if (!deleteRows.contains(list.at(i)->row())) {
            deleteRows.append(list.at(i)->row());
        }
    }
    // 从下往上删除
    for (auto i = deleteRows.rbegin(); i != deleteRows.rend(); ++i) {
        // 根据 ID 删除数据库记录
        // 如果删除成功则 ui 也删除该条记录
        // 如果删除失败则不删除ui记录和sql记录
        unsigned id = ui->tableWidget->item(*i, 0)->text().toUInt();
        if (deleteTableFunc(id)) {
            ui->tableWidget->removeRow(*i);
        }
    }
}

void StudentDig::on_pushButton_UPDATE_clicked() {
    auto lists = ui->tableWidget->selectedItems();
    for (int i = 0; i < lists.size(); ++i) {
        QMessageBox::information(nullptr, QString::number(lists.at(i)->row()), "", QMessageBox::Ok);
    }
}

void StudentDig::on_pushButton_QUERY_clicked() {
    oldRow = -1;
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    QStringList result = queryTableFunc();

    // 设置 QTableWidget 的行数和表头
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "id" << "name" << "score");

    for (int row = 0; row < result.size() / 3; row++) {
        ui->tableWidget->insertRow(row);
        QTableWidgetItem *idItem = new QTableWidgetItem(result.at(row * 3));
        QTableWidgetItem *nameItem = new QTableWidgetItem(result.at(row * 3 + 1));
        QTableWidgetItem *scoreItem = new QTableWidgetItem(result.at(row * 3 + 2));

        ui->tableWidget->setItem(row, 0, idItem);
        ui->tableWidget->setItem(row, 1, nameItem);
        ui->tableWidget->setItem(row, 2, scoreItem);
    }
}

void StudentDig::on_tableWidget_itemChanged(QTableWidgetItem *Item) {
    if (Item != nullptr && oldRow == Item->row() && oldRow != -1) {
        updateTableFunc(Item);
        oldRow = -1;
    }
}
void StudentDig::on_tableWidget_itemPressed(QTableWidgetItem *Item) {
    oldRow = Item->row();
    oldID = ui->tableWidget->item(Item->row(), 0)->text().toUInt();
}

void StudentDig::createDatabaseFunc() {
    // 添加数据库驱动
    sqlDatabase = QSqlDatabase::addDatabase("QSQLITE");

    // 设置数据库名称
    sqlDatabase.setDatabaseName("studentmis.db");

    // 打开数据库是否成功
    if (!sqlDatabase.open()) {
        QMessageBox::critical(nullptr, "failed", "open database failed!", QMessageBox::Ok);
    }
}

void StudentDig::createTableFunc() {
    QSqlQuery createQuery;
    // 创建 SQL 语句
    QString sqlStr = QString("CREATE TABLE IF NOT EXISTS student("
                             "id int primary key not null,"
                             "name text not null,"
                             "score real not null)"
    );
    // 执行 SQL 语句
    if (!createQuery.exec(sqlStr)) {
        QMessageBox::critical(nullptr, "failed", "create table failed!", QMessageBox::Ok);
    }
}

QStringList StudentDig::queryTableFunc() {
    QSqlQuery sqlQuery;

    sqlQuery.exec("select * from student");

    // 获取列数
//    auto colNum = sqlQuery.record().count();
    QStringList result;
    while (sqlQuery.next()) {
        unsigned id = sqlQuery.value(0).toUInt();
        QString name = sqlQuery.value(1).toString();
        double score = sqlQuery.value(2).toDouble();

        result << QString::number(id) << name << QString::number(score, 'f', 2);
    }
    return result;
}

void StudentDig::updateTableFunc(QTableWidgetItem *Item) {
    QSqlQuery sqlQuery;
    unsigned newID = ui->tableWidget->item(Item->row(), 0)->text().toUInt();
    QString newName = ui->tableWidget->item(Item->row(), 1)->text();
    double newScore = ui->tableWidget->item(Item->row(), 2)->text().toDouble();

    QString sqlStr = QString("UPDATE student SET id = %1,name = '%2' ,score = %3 WHERE id = %4")
            .arg(newID).arg(newName).arg(newScore).arg(oldID);
    if (!sqlQuery.exec(sqlStr)) {
        QMessageBox::critical(nullptr, "failed", QString("updata data failed!"), QMessageBox::Ok);
        return;
    }
}

bool StudentDig::deleteTableFunc(unsigned ID) {
    QSqlQuery sqlQuery;
    QString sqlStr = "DELETE FROM student WHERE id = " + QString::number(ID);
    if (!sqlQuery.exec(sqlStr)) {
        QMessageBox::critical(nullptr, "failed", QString("delete data failed!\nID: %1").arg(ID), QMessageBox::Ok);
        return false;
    }
    return true;
}

