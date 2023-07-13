#pragma once

#include <QtWidgets/QDialog>
#include "ui_QtQQ_Server.h"
#include "TcpServer.h"
#include <QSqlQueryModel>
#include <QTimer>
#include <QUdpSocket>

class QtQQ_Server : public QDialog
{
    Q_OBJECT

public:
    QtQQ_Server(QWidget *parent = Q_NULLPTR);

private:
	void initComboBoxData();	//初始化组合框的数据
	void initTcpSocket();		//初始化TCP
	void initUdpSocket();		//初始化UDP
	bool connectMySql();
	void setDepNameMap();
	void setStatusMap();
	void setOnlineMap();
	int getCompDepID();			//获取公司群QQ号
	void updateTableData(int depID = 0,int employeeID = 0);

private slots:
	void onUDPbroadMsg(QByteArray& btData);
	void onRefresh();
	void on_queryDepartmentBtn_clicked();//根据群QQ号查找员工(点击信号与槽函数自动连接)
	void on_queryIDBtn_clicked();		//根据员工QQ号筛选
	void on_logoutBtn_clicked();		//注销员工QQ号
	void on_selectPictureBtn_clicked();	//选择图片（员工的寸照）
	void on_addBtn_clicked();	//新增员工
private:
    Ui::QtQQ_ServerClass ui;

	QTimer* m_timer;	//定时刷新数据
	int m_compDepID;	//公司群QQ号
	int m_depID;		//部门QQ号
	int m_employeeID;	//员工QQ号
	QString m_pixPath;	//头像路径
	QMap<QString, QString> m_statusMap;	//状态
	QMap<QString, QString>m_depNameMap;	//部门名称
	QMap<QString, QString>m_onlineMap;	//在线

	QSqlQueryModel m_queryInfoModel;//查询所有员工的信息模型

	TcpServer* m_tcpServer;	//tcp服务端
	QUdpSocket* m_udpSender;//udp广播
};
