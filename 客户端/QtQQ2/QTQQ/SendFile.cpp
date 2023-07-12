#include "SendFile.h"
#include <WindowManager.h>
#include <QFileDialog>
#include <QMessageBox>

SendFile::SendFile(QWidget *parent)
	: BasicWindow(parent)
	,m_filePath("")
{
	ui.setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);

	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("SendFile");
	this->move(100, 400);

	// 先获取 TalkWindowShell 对象，
	// 这个是对象 与 对象之间的联系
	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowSheel();

	// 发送信号
	connect(this, &SendFile::sendFileClicked
		, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);

}

SendFile::~SendFile()
{
}

void SendFile::on_openBtn_clicked()
{
	// 获取文件路径
	m_filePath = QFileDialog::getOpenFileName(
		this,
		QString::fromLocal8Bit("选择文件"),
		"/",
		QString::fromLocal8Bit("发送的文件(*.txt *.doc);;所有文件(*.*);;"));

	ui.lineEdit->setText(m_filePath);
}

void SendFile::on_sendBtn_clicked()
{
	// 如果这个路径不为空
	if (!m_filePath.isEmpty())
	{
		// 读取文件内容，再将文件发送出去
		QFile file(m_filePath);
		if (file.open(QIODevice::ReadOnly))
		{
			// 数据类型
			int msgType = 2;

			// 全部读取
			QString str = file.readAll();

			// 文件名称
			QFileInfo fileInfo(m_filePath);
			QString fileName = fileInfo.fileName();

			// 发送信号
			emit sendFileClicked(str, msgType, fileName);

			file.close();
		}
		else
		{
			QMessageBox::information(this,
				QStringLiteral("提示"),
				QString::fromLocal8Bit("发送文件，失败！\n%1").arg(m_filePath));

			this->close();		// 对话框关闭
			return;
		}

		m_filePath = "";
		this->close();
	}
}
