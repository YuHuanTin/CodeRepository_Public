#ifndef STUDENTDIG_H
#define STUDENTDIG_H

#include "QtSql/qsqldatabase.h"
#include "QtSql/qsqlquery.h"
#include "QtSql/qsqlquerymodel.h"
#include <QDialog>
#include <QtSql/QSqlDatabase> // ר�������ӡ��������ݿ�
#include <QtSql/QSqlQuery> // ר������ DML (���ݲ�������)��DDL (���ݶ�������)
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

    // ��������
    void on_pushButtonSort_clicked();

    // �������
    void on_pushButton_INSERT_clicked();

    // ɾ������
    void on_pushButton_DELETE_clicked();

    // ��������
    void on_pushButton_UPDATE_clicked();

    // ��ѯ����
    void on_pushButton_QUERY_clicked();

    // ��¼���޸Ĺ��ĸ���
    void on_tableWidget_itemChanged(QTableWidgetItem *);

    void on_tableWidget_itemPressed(QTableWidgetItem *);

private: // �Զ��庯��
    // ���� SQLite ���ݿ�
    void createDatabaseFunc();

    // ���� SQLite ���ݱ�
    void createTableFunc();

    // ִ�в�ѯ����
    QStringList queryTableFunc();

    // ��������
    void updateTableFunc(QTableWidgetItem *Item);

    // ɾ����¼
    bool deleteTableFunc(unsigned ID);

    QSqlDatabase sqlDatabase; // ���� SQL ����������
    QSqlQueryModel sqlQueryModel; // �洢�����

    int oldRow = -1; // ���޸ĵ�ѡ��������
    int oldID = -1; // ���޸ĵ�ѡ����id

private:
    Ui::StudentDig *ui;
};

#endif // STUDENTDIG_H
