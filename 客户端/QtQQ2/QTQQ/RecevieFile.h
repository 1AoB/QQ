#pragma once

#include <BasicWindow.h>
#include "ui_RecevieFile.h"

class RecevieFile : public BasicWindow
{
	Q_OBJECT

public:
	RecevieFile(QWidget *parent = Q_NULLPTR);
	~RecevieFile();

	// ��Ϣ�����ݴ�����ַ�����������
	void setMsg(QString& msgLabel);



signals:
	void refuseFile();		// �ܾ�

private slots:
	void on_okBtn_clicked();		// ȷ��
	void on_cancelBtn_clicked();	// ȡ��

private:
	Ui::RecevieFile ui;
};
