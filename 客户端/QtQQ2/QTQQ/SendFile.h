#pragma once

#include "BasicWindow.h"
#include "ui_SendFile.h"

class SendFile : public BasicWindow
{
	Q_OBJECT

public:
	SendFile(QWidget *parent = Q_NULLPTR);
	~SendFile();

signals:	
	// �����ļ�������ź�
	void sendFileClicked(QString& strData, int& msgType, QString fileName);

private slots:
	/*
		�����"����"ʱ��Ҫ��tcpд������
		���ļ���"����"��"����"��Լ���ķ�������tcp��������

		���ԣ�����Ҫ����һ���źţ�
		����ź�Ҫ��������ʲô���ݣ�ʲô����

		����ź�Ҫ��ʲô��Ҫ˭�����ź�
		TalkWindowShell����һ���ۺ���������������Ҫ���͵�tcp����
	*/
	void on_openBtn_clicked();		// ��
	void on_sendBtn_clicked();		// ����


private:
	Ui::SendFile ui;

	QString m_filePath;		// �û�ѡ����ļ�
};
