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
	// 发送文件发射的信号
	void sendFileClicked(QString& strData, int& msgType, QString fileName);

private slots:
	/*
		当点击"发送"时，要对tcp写入数据
		将文件的"名称"和"内容"以约定的方法，向tcp发送数据

		所以，这里要发射一个信号，
		这个信号要带参数，什么数据，什么类型

		这个信号要做什么，要谁来接信号
		TalkWindowShell中有一个槽函数，更新我们所要发送的tcp数据
	*/
	void on_openBtn_clicked();		// 打开
	void on_sendBtn_clicked();		// 发送


private:
	Ui::SendFile ui;

	QString m_filePath;		// 用户选择的文件
};
