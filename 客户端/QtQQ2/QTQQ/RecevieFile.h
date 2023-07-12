#pragma once

#include <BasicWindow.h>
#include "ui_RecevieFile.h"

class RecevieFile : public BasicWindow
{
	Q_OBJECT

public:
	RecevieFile(QWidget *parent = Q_NULLPTR);
	~RecevieFile();

	// 信息，根据传入的字符串进行设置
	void setMsg(QString& msgLabel);



signals:
	void refuseFile();		// 拒绝

private slots:
	void on_okBtn_clicked();		// 确定
	void on_cancelBtn_clicked();	// 取消

private:
	Ui::RecevieFile ui;
};
