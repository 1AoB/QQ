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

	// �Ȼ�ȡ TalkWindowShell ����
	// ����Ƕ��� �� ����֮�����ϵ
	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowSheel();

	// �����ź�
	connect(this, &SendFile::sendFileClicked
		, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);

}

SendFile::~SendFile()
{
}

void SendFile::on_openBtn_clicked()
{
	// ��ȡ�ļ�·��
	m_filePath = QFileDialog::getOpenFileName(
		this,
		QString::fromLocal8Bit("ѡ���ļ�"),
		"/",
		QString::fromLocal8Bit("���͵��ļ�(*.txt *.doc);;�����ļ�(*.*);;"));

	ui.lineEdit->setText(m_filePath);
}

void SendFile::on_sendBtn_clicked()
{
	// ������·����Ϊ��
	if (!m_filePath.isEmpty())
	{
		// ��ȡ�ļ����ݣ��ٽ��ļ����ͳ�ȥ
		QFile file(m_filePath);
		if (file.open(QIODevice::ReadOnly))
		{
			// ��������
			int msgType = 2;

			// ȫ����ȡ
			QString str = file.readAll();

			// �ļ�����
			QFileInfo fileInfo(m_filePath);
			QString fileName = fileInfo.fileName();

			// �����ź�
			emit sendFileClicked(str, msgType, fileName);

			file.close();
		}
		else
		{
			QMessageBox::information(this,
				QStringLiteral("��ʾ"),
				QString::fromLocal8Bit("�����ļ���ʧ�ܣ�\n%1").arg(m_filePath));

			this->close();		// �Ի���ر�
			return;
		}

		m_filePath = "";
		this->close();
	}
}
