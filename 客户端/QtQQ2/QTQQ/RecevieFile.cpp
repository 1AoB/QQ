#include "RecevieFile.h"
#include <QFileDialog>
#include <QMessageBox>




// ȫ�ֱ������ļ�����
extern QString gfileName;
// ȫ�ֱ���������
extern QString gfileData;


RecevieFile::RecevieFile(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);

	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("ReceiveFile");
	this->move(100, 400);

	// ����û���û�е�ȷ����Ҳ����ȡ�������ǵ����Ͻǵ� X��Ҳ�Ǿܾ�����Ч��
	connect(m_titleBar, &TitleBar::signalButtonCloseClicked
		, this, &RecevieFile::refuseFile);

}

RecevieFile::~RecevieFile()
{
}

// ��Ϣ
void RecevieFile::setMsg(QString& msgLabel)
{
	ui.label->setText(msgLabel);
}

// ȷ��
void RecevieFile::on_okBtn_clicked()
{
	this->close();

	// ��ȡ��Ҫ�����ļ���·��
	QString fileDirPath = QFileDialog::getExistingDirectory(
		nullptr,		// ���������ֵ����Ϊ˵�����Ѿ�����һ��close��
		QStringLiteral("�ļ�����·��"),
		"/");

	// ��ȡ�ļ�����·���������ļ���һ��
	// �ļ�����
	// ֮ǰ�ڽ�������ʱ�򣬾ͽ������ļ������ˣ�
	// ��һ��ȫ�ֱ������ù���
	QString filePath = fileDirPath + "/" + gfileName;

	QFile file(filePath);		// ������һ�� �ļ�����

	// �ж��ļ��Ƿ�д��ɹ�
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::information(nullptr,
			QStringLiteral("��ʾ"),
			QStringLiteral("�ļ�����ʧ��"));
	}
	else
	{
		// ���ճɹ�����ʼд������
		file.write(gfileData.toUtf8());
		file.close();

		QMessageBox::information(nullptr,
			QStringLiteral("��ʾ"),
			QStringLiteral("�ļ����ճɹ�"));
	}
}

// ȡ��
void RecevieFile::on_cancelBtn_clicked()
{
	emit refuseFile();
	this->close();
}
