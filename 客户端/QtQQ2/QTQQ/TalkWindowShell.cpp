#include "TalkWindowShell.h"
#include "CommonUtils.h"
#include <QListWidget>
#include <QMessageBox>
#include <QtSql\qsqlquerymodel.h>
#include <QFile>
#include <QSqlQuery>
#include "WindowManager.h"
#include "RecevieFile.h"


// ��½�ߵ�QQ�ţ�Ա���ţ�
extern QString gLoginEmployeeID;

// ȫ�ֱ������ļ�����
QString gfileName;
// ȫ�ֱ������ļ�����
QString gfileData;


TalkWindowShell::TalkWindowShell(QWidget* parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);			// ���ڹر�ʱ��������Դ����
	initControl();
	initTcpSocket();
	initUdpSocket();

	// JS�ļ���û�����ݵ�ʱ�򣬲ſ�ʼд������
	QFile file(":/Resources/MainWindow/MsgHtml/msgtmpl.js");

	// ���û�����ݣ��ͽ�����д
	if (!file.size())
	{
		// ��ȡ����Ա��QQ������
		QStringList employeeIDList;

		getEmployeeID(employeeIDList);

		if (!creatJSFile(employeeIDList))
		{
			QMessageBox::information(this,
				QString::fromLocal8Bit("��ʾ"),
				QString::fromLocal8Bit("JS�ļ�д������ʧ��"));///////////////////!!!!!!!
		}
	}


}

TalkWindowShell::~TalkWindowShell()
{
	delete m_emotionWindow;
	m_emotionWindow = nullptr;
}

void TalkWindowShell::addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem, const QString& uid) // GroupType groupType
{
	ui.rightStackedWidget->addWidget(talkWindow);

	// �����ߣ����鴰�ڣ����� ���鴰������ �ź�
	// �����ߣ����촰�ڣ����ñ��鰴ť��״̬
	connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()),
		talkWindow, SLOT(onSetEmotionBtnStatus()));		// 

	// ������������
	QListWidgetItem* aItem = new QListWidgetItem(ui.listWidget);

	// �� ����������뵽ӳ��������
	// һ���Ӧ ��һ�����촰��
	m_talkwindowItemMap.insert(aItem, talkWindow);

	aItem->setSelected(true);		// ��ѡ��

	// �ж���Ⱥ�ģ����ǵ���
	QSqlQueryModel sqlDepModel;
	QString strQuery = QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(uid);
	sqlDepModel.setQuery(strQuery);
	int rows = sqlDepModel.rowCount();			// ��ȡ�к�

	// �ж��ǵ���
	if (rows == 0)
	{
		strQuery = QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(uid);
		sqlDepModel.setQuery(strQuery);
	}

	QModelIndex index;
	index = sqlDepModel.index(0, 0);		// 0�У�0��

	QImage img;
	// ��ȡͼƬ·��������
	img.load(sqlDepModel.data(index).toString());



	// ����ͷ��
	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));
	// �����
	ui.listWidget->addItem(aItem);
	// �����б���
	ui.listWidget->setItemWidget(aItem, talkWindowItem);

	// �����
	onTalkWindowItemClicked(aItem);

	// ��Ҫ������źŷǳ��࣬���ڹر�֮��Ҫ������Դ����
	// 1�����촰��Ҫ�رգ�2����ǰ������Ҫ�Ƴ���3��talkWindow ��ԴҪ�ͷ�
	// 4���ұ߶�ҳ�����ʾ��ҲҪɾ��
	connect(talkWindowItem, &TalkWindowItem::signalCloseClicked, [talkWindowItem, talkWindow, aItem, this]()
		{
			// �ȴ�����߲���
			m_talkwindowItemMap.remove(aItem);		// ɾ����
			talkWindow->close();									// ���촰�ڹر�
			// ͨ�� takeItem���� �Ƴ� ���������кţ�
			// ����row�������� aItem �����ȥ
			ui.listWidget->takeItem(ui.listWidget->row(aItem));
			delete talkWindowItem;

			// �ٴ����ұ߲���
			ui.rightStackedWidget->removeWidget(talkWindow);

			// ��һ���жϣ����ɾ���������һ������
			// �ͽ����ڹر�
			if (ui.rightStackedWidget->count() < 1)
			{
				close();
			}
		});


}

void TalkWindowShell::setCurrentWidget(QWidget* widget)
{
	ui.rightStackedWidget->setCurrentWidget(widget);
}

const QMap<QListWidgetItem*, QWidget*>& TalkWindowShell::getTalkWindowItemMap() const
{
	// ֱ�ӽ�ӳ�䣬���з���
	return m_talkwindowItemMap;
}

void TalkWindowShell::initControl()
{
	loadStyleSheet("TalkWindow");
	setWindowTitle(QString::fromLocal8Bit("���촰��"));

	m_emotionWindow = new EmotionWindow;
	m_emotionWindow->hide();				// ���ر��鴰��

	QList<int> leftWidgetSize;					// �ߴ�
	// ��߿�ȣ��ұ߿�ȣ�width() ������ܿ��
	leftWidgetSize << 154 << width() - 154;
	ui.splitter->setSizes(leftWidgetSize);			// ���������óߴ�

	ui.listWidget->setStyle(new CustomProxyStyle(this));		// �����Զ�����

	// ������ѡ��ʱ�򣬾ͽ��и���
	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
	connect(m_emotionWindow, SIGNAL(signalEmotionItemClicked(int)), this, SLOT(onEmotionItemClicked(int)));

}

void TalkWindowShell::initTcpSocket()
{
	m_tcpClientSocket = new QTcpSocket(this);
	// IP���˿�
	m_tcpClientSocket->connectToHost("127.0.0.1", gtcpPort);

}

void TalkWindowShell::initUdpSocket()
{
	m_udpRecevier = new QUdpSocket(this);
	for (quint16 port = gudpPort; port < gudpPort + 200; ++port)
	{
		// �󶨶˿ڣ�����ģʽ���ɹ�֮�� ֱ��break
		if (m_udpRecevier->bind(port, QUdpSocket::ShareAddress))
		{
			break;
		}
	}

	// ���������ˣ��ͷ����ź� ׼�� ��ʼ ������
	// �����ź�֮�󣬿�ʼ����δ����������
	connect(m_udpRecevier, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);


}

void TalkWindowShell::getEmployeeID(QStringList& employeesList)
{
	QSqlQueryModel queryModel;
	queryModel.setQuery("SELECT employeeID FROM tab_employees WHERE status = 1");

	// ���� ģ�͵�������
	// �������л�ȡ�������ݼ�¼ ��������������Ա����������
	int employeeNum = queryModel.rowCount();

	// ����һ�� ģ������
	QModelIndex index;
	for (int i = 0; i < employeeNum; i++)
	{
		// ����i�У�0�е�����ֵ
		index = queryModel.index(i, 0);
		// ͨ���������õ����ݣ���ת���ַ�������� ��ӵ�������
		employeesList << queryModel.data(index).toString();
	}

}

bool TalkWindowShell::creatJSFile(QStringList& employeeList)
{
	// ��ȡtxt�ļ�����
	QString strFileTxt = ":/Resources/MainWindow/MsgHtml/msgtmpl.txt";
	QFile fileRead(strFileTxt);

	QString strFile;			// �����ȡ��������

	// �ж���ֻ��ģʽ�򿪣��ܷ�򿪳ɹ�
	if (fileRead.open(QIODevice::ReadOnly))
	{
		// �򿪳ɹ�����ȡȫ��
		strFile = fileRead.readAll();
		fileRead.close();			// ��ȡ���˾͹ر�
	}
	else
	{
		QMessageBox::information(this,
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("��ȡ msgtmpl.txt ʧ��"));
		return false;
	}

	// �滻��external0��appendHtml0�������Լ�����Ϣʹ�ã�
	QFile fileWrite(":/Resources/MainWindow/MsgHtml/msgtmpl.js");

	// д��ģʽ ���� ����ģʽ
	if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		/*
			1����ʼ������Ϊ null
			2��ͨ���������ö���
			3���ڽ���html����д������
		*/

		// ���¿�ֵ
		// ԭʼ�ģ���ʼ���ģ���ֵ�Ĵ���
		QString strSourceInitNull = "var external = null;";

		// ���³�ʼ��
		// ԭʼ�ģ���ʼ���ģ�����Ĵ���
		QString strSourceInit = "external = channel.objects.external;";

		// ����newWebchannel
		// ����� external0 ȥ�����ȵ�ʱ���������
		// ԭʼ�Ķ��� external
		QString strSourceNew =
			"new QWebChannel(qt.webChannelTransport,\
			function(channel) {\
			external = channel.objects.external;\
		}\
		); \
		";

		// ����׷��recvHtml
		/*
			��һ��������˫���ţ��������ͻ
			���Բ�������������ֱ��д��˫�����

			��ˣ�Ҫͨ����ȡ�ַ����ķ��������� strSourceRecvHtml ����
			��������δ��룬�����ŵ�һ�� txt�ļ���ٽ��ж�ȡ

			function recvHtml(msg){
					$("#placeholder").append(external.msgLHtmlTmpl.format(msg));
					window.scrollTo(0,document.body.scrollHeight); ;
			};
		*/
		QString strSourceRecvHtml;
		QFile fileRecvHtml("Resources/MainWindow/MsgHtml/recvHtml.txt");
		if (fileRecvHtml.open(QIODevice::ReadOnly))
		{
			// �ȶ�ȡȫ�����ٸ�ֵ���ٹر�
			strSourceRecvHtml = fileRecvHtml.readAll();
			fileRecvHtml.close();
		}
		else
		{
			QMessageBox::information(this,
				QString::fromLocal8Bit("��ʾ"),
				QString::fromLocal8Bit("��ȡ recvHtml.txt ʧ��"));
			return false;
		}

		// �����滻��Ľű�����Ӧ�����4��
		QString strReplaceInitNull;
		QString strReplaceInit;
		QString strReplaceNew;
		QString strReplaceRecvHtml;

		for (int i = 0; i < employeeList.length(); i++)
		{
			// �༭�滻��� ��ֵ
			QString strInitNull = strSourceInitNull;
			strInitNull.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strReplaceInitNull += strInitNull;
			strReplaceInitNull += "\n";

			// �༭�滻��� ��ʼֵ
			QString strInit = strSourceInit;
			strInit.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strReplaceInit += strInit;
			strReplaceInit += "\n";

			// �༭�滻��� newWebchannel
			QString strNew = strSourceNew;
			strNew.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strReplaceNew += strNew;
			strReplaceNew += "\n";

			// replace���滻�޸ĵģ�ֱ�Ӿ��� strRecvHtml
			// �༭�滻��� recvHtml
			QString strRecvHtml = strSourceRecvHtml;
			strRecvHtml.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strRecvHtml.replace("recvHtml", QString("recvHtml_%1").arg(employeeList.at(i)));
			strReplaceRecvHtml += strRecvHtml;
			strReplaceRecvHtml += "\n";
		}

		// �����forѭ�����꣬�м���Ա�����ͻ���ּ���
		// Ȼ�� �ٽ��滻����ַ�����
		// ���� �滻�� ��ȡ���ݵġ�ԭʼ�ļ����� ��ԭ�ַ����� ��
		strFile.replace(strSourceInitNull, strReplaceInitNull);
		strFile.replace(strSourceInit, strReplaceInit);
		strFile.replace(strSourceNew, strReplaceNew);
		strFile.replace(strSourceRecvHtml, strReplaceRecvHtml);

		// strFile ��������������
		// ��һ���ļ�����д���ȥ
		QTextStream stream(&fileWrite);
		stream << strFile;
		fileWrite.close();

		return true;
	}
	else
	{
		QMessageBox::information(this,
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("д�� msgtmpl.js ʧ��"));

		return false;
	}


}

void TalkWindowShell::handleReceivedMsg(int senderEmployeeID, int msgType, QString strMsg)
{
	// QMsgTextEdit��������Ĭ�ϲ�������һ������ָ��
	// �� ����Ĭ��ֵ ��Ϊ nullptr �������������ʱ�򣬿��Բ�������
	QMsgTextEdit msgTextEdit;	// ��Ϣ�ı��༭��
	msgTextEdit.setText(strMsg);

	// ����ֻ���� �ı���Ϣ��������Ϣ��
	// �ļ����ͣ��������������
	if (msgType == 1)	// �ı���Ϣ
	{
		// ����Ϣ��ת��Ϊ html
		msgTextEdit.document()->toHtml();
	}
	else if (msgType == 0)	// ������Ϣ
	{
		// ÿ��������ռ���
		const int emotionWidth = 3;
		// ����������������ݳ��� ���� ÿ��������
		int emotionNum = strMsg.length() / emotionWidth;

		// ���������е� ���飬���html��ȥ
		for (int i = 0; i < emotionNum; i++)
		{
			// ��ȡ�����е��ַ�����
			// �������ȡ���������ʼ�ĵط���ȡ(�ڵ����������֮ǰ�������Ѿ��������ˣ��õ������ı������ݲ���)
			// ��ȡ��ȣ�һ�ν�ȡ3�����(����060��005��101)��ÿ������ռ3λ���
			// ��ת��int���ͣ����ؽ�ȥ
			msgTextEdit.addEmotionUrl(strMsg.mid(i * 3, emotionWidth).toInt());
		}
	}

	// �жϣ�׷��html���ı����棬��û�����壬��ɫ������
	// ������͵��Ǳ��飬����Ҫ�ж�
	QString hemlText = msgTextEdit.document()->toHtml();

	// �ı�HTML�����û�����壬���������
	// ����Ҫ��֤�� �ı�����
	// ����Ǳ���Ļ����ַ����� �϶��� .png
	if (!hemlText.contains(".png") && !hemlText.contains("</span>"))
	{
		QString fontHtml;
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly))
		{
			fontHtml = file.readAll();
			// ��html�ļ���� %1�����ַ��� text �滻
			fontHtml.replace("%1", strMsg);
			file.close();
		}
		else
		{
			// this����ǰ���첿������Ϊ������
			QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"),
				QString::fromLocal8Bit("�ļ� msgFont.txt �����ڣ�"));
			return;
		}

		// �ж�ת������û�а��� fontHtml
		if (!hemlText.contains(fontHtml))
		{
			hemlText.replace(strMsg, fontHtml);
		}
	}

	/*
		��������ӵ�"��ҳ"��
		TalkWindowShell�У��Ҳಿ��rightStackedWidget�������кܶ�ҳ��
		ÿ��ҳ�棬����һ�����촰��
		�����ڽ�������ӵ�"��ҳ"ʱ��Ҫ��ӵ���ǰ��"ҳ��"��
	*/

	// �Ȼ�ȡ ���ڵ�ַ
	// �Ҳಿ��rightStackedWidget����ȡ��ǰ�� �ҳ��
	// ���ص��� QWidget����ָ�룬Ҫת���� TalkWindow* ����ָ��
	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());

	// �������һ������ʼ ׷����Ϣ
	// 1����ת����������
	// appendMsg()������ԭ�ͣ��ڶ����������ַ����������� �����ߵ�ID
	// �ڵ�ǰ������������ �����ߵ�ID ��int���ͣ��������Ͳ�ƥ�䣬���Ҫת��һ��
	// 2�������ui�� TalkWindow ��˽�г�Ա���� TalkWindowShell ���޷�����
	// ��� ���ó���Ԫ��
	talkWindow->ui.msgWidget->appendMsg(hemlText, QString::number(senderEmployeeID));




}

void TalkWindowShell::onEmotionBtnClicked(bool)
{
	// ���鴰�ڣ����֮��Ҫ��������
	// ���óɡ����ɼ���
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());

	// ����ǰ�ؼ������λ�ã�ת��Ϊ��Ļ�ľ���λ��
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));

	emotionPoint.setX(emotionPoint.x() + 170);
	emotionPoint.setY(emotionPoint.y() + 220);
	m_emotionWindow->move(emotionPoint);			// �ƶ�

}

void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem* item)
{
	// ȡ�ô����ַ
	QWidget* talkWindowWidget = m_talkwindowItemMap.find(item).value();

	// ���ұߵĴ������
	ui.rightStackedWidget->setCurrentWidget(talkWindowWidget);
}

/*
	���ı������ݰ���ʽ��
	Ⱥ�ı�־��0˽�ģ�1Ⱥ�ģ� + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ�
		+ ��Ϣ���ͣ��ı��� + ���ݳ��� + �ı�����

	�����顿���ݰ���ʽ��
	Ⱥ�ı�־��0˽�ģ�1Ⱥ�ģ� + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ�
		+ ��Ϣ���ͣ����飩 + ������� + images + ��������

	���ļ������ݰ���ʽ��
	Ⱥ�ı�־��0˽�ģ�1Ⱥ�ģ� + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ�
		+ ��Ϣ���ͣ�2�ļ��� + �ļ����� + "bytes" + �ļ����� + "data_begin" + �ļ�����

	msgType����Ϣ���ͣ�0�Ǳ��飬1���ı���2���ļ�
*/
void TalkWindowShell::updateSendTcpMsg(QString& strData, int& msgType, QString fileName)
{
	/*
		�������ݰ�
		������Ҫ����һЩ�����ݰ��ĸ�ʽ
		���㣬�����ͻ��˽�������

		�����ݰ����͵�������֮�󣬷������ٽ������ݰ�
		���ɷ�������ͨ���㲥���͵������ͻ�����
	*/

	// ��ȡ��ǰ���촰�ڣ����� ��ǰ��Ĵ���
	// ui.rightStackedWidget->currentWidget() ����ȡ��ǰ�Ĳ���
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	// getTalkID()��Ϊ�˻�ȡ TalkWindow ˽�г�Ա m_talkId
	// ���� ���촰�ڵ�ID
	QString talkID = curTalkWindow->getTalkID();

	// Ⱥ�ı�־
	QString strGroupFlag;
	// ��������
	QString strSend;

	if (talkID.length() == 4)	// ȺQQ�ŵĳ��� 2001 2002
	{
		// Ⱥ�ı�־����Ϊ1
		strGroupFlag = "1";
	}
	else
	{
		strGroupFlag = "0";
	}

	// �������ݵĳ���
	int nstrDataLength = strData.length();

	// �����ȣ�ת���� ���
	int dataLength = QString::number(nstrDataLength).length();

	// ԭʼ���ݳ���
	// const int sourceDataLength = dataLength;		// û���ˣ���ע�͵���
	// ר���������������ݵĳ���
	QString strdataLength;

	// �ж��ı�����
	if (msgType == 1)
	{
		// �ı���Ϣ�ĳ���Լ��Ϊ5λ����֤���ݰ����ȣ�ʼ��Ϊ5λ��
		// ���緢�͡�hello����5�����ȵ����ݰ���"5"
		// ��ô ���ݳ���dataLength "5" ����� ���� ��1����
		// ��� ���ݳ���dataLength "50" ����� ���� ��2����
		// ��� ���ݳ���dataLength "500" ����� ���� ��3���ˣ��Դ�����
		if (dataLength == 1)
		{
			// ����� 1 ��ǰ��Ҫ��4��0���ټ��� ԭʼ���ݳ���
			strdataLength = "0000" + QString::number(nstrDataLength);
		}
		else if (dataLength == 2)
		{
			strdataLength = "000" + QString::number(nstrDataLength);
		}
		else if (dataLength == 3)
		{
			strdataLength = "00" + QString::number(nstrDataLength);
		}
		else if (dataLength == 4)
		{
			strdataLength = "0" + QString::number(nstrDataLength);
		}
		else if (dataLength == 5)
		{
			strdataLength = QString::number(nstrDataLength);
		}
		else
		{
			QMessageBox::information(this,
				QString::fromLocal8Bit("��ʾ"),
				QString::fromLocal8Bit("����������ݳ���"));
		}

		/*
			���ı������ݰ���ʽ��
			Ⱥ�ı�־��0˽�ģ�1Ⱥ�ģ� + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ�
				+ ��Ϣ���ͣ�1�ı��� + ���ݳ��� + �ı�����
		*/
		// ��ʼ���ͣ��ı�
		strSend = strGroupFlag + gLoginEmployeeID + talkID + "1" + strdataLength + strData;

	}
	else if (msgType == 0)	// ������Ϣ
	{
		/*
			�����顿���ݰ���ʽ��
			Ⱥ�ı�־��0˽�ģ�1Ⱥ�ģ� + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ�
				+ ��Ϣ���ͣ�0���飩 + ������� + images + ��������
		*/
		// ��ʼ���ͣ�����
		strSend = strGroupFlag + gLoginEmployeeID + talkID + "0" + strData;

	}
	else if (msgType == 2)	// �ļ���Ϣ
	{
		/*
			���ļ������ݰ���ʽ��
			Ⱥ�ı�־��0˽�ģ�1Ⱥ�ģ� + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ�
				+ ��Ϣ���ͣ�2�ļ��� + �ļ����� + "bytes" + �ļ����� + "data_begin" + �ļ�����
		*/

		// �Ƚ��ļ����ݣ�ת���� �ֽ����� QByteArray
		// ���� �ֽ��������� ����
		QByteArray bt = strData.toUtf8();
		// ��ȡ�ļ��ֽڳ���
		QString strLength = QString::number(bt.length());

		// ��ʼƴ�����ݰ�
		strSend = strGroupFlag + gLoginEmployeeID + talkID + "2" + strLength
			+ "bytes" + fileName + "data_begin" + strData;

	}

	// ��ʼ��ͻ��ˣ���������
	QByteArray dataBt;
	// ���ó��ȣ��� Ҫ�������ݵĳ��ȣ������� �ֽ�����ĳ���
	dataBt.resize(strSend.length());
	// �����ݽ��и�ֵ������ toUtf8() ת���� �ֽ���������
	dataBt = strSend.toUtf8();

	// ����һ��
	m_tcpClientSocket->write(dataBt);

}

void TalkWindowShell::onEmotionItemClicked(int emotionNum)
{
	// ��ȡ��ǰ�����촰��
	// ��ȡ�ұߵ�ǰ�Ĳ�����rightStackWidget ���� currentWidget��
	// ���ص�ָ�������� Widget�������ͣ���Ҫת���� TalkWindow* ָ������
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(
		ui.rightStackedWidget->currentWidget());
	// �ж�ָ�룬�Ƿ���Ч
	if (curTalkWindow)
	{
		// �����촰�ڣ����ͼƬ
		// ������Ŵ���ȥ�����м���
		curTalkWindow->addEmotionImage(emotionNum);

	}

}

/*
	���ݰ��ĸ�ʽ�����������Լ����еģ�����Լ����

	�ı����ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ�
		+ ��Ϣ����(1) + ���ݳ��� + ����

	�������ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ�
		+ ��Ϣ����(0) + ������� + images + ��������

	�ļ����ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ�
		+ ��Ϣ����(2) + �ļ��ֽ��� + bytes + �ļ��� + data_begin + �ļ�����

	Ⱥ�ı�־ռ1λ��0���ģ�1Ⱥ��
	��Ϣ����ռ1λ��0������Ϣ��1�ı���Ϣ��2�ļ���Ϣ

	QQ��ռ5λ��ȺQQ��ռ4λ��
	���ݳ���ռ5λ��
	��������ռ3λ������3λ��ǰ����0��ȫ
	�����ռλ���������ݿ�ȵ���˼����������ϸ���ͣ�

	ע�⣺
	��Ⱥ�ı�־Ϊ0ʱ�������ݰ��У��Ͳ��ǡ�����ϢȺQQ�š������ǡ�����ϢԱ��QQ�š�
	��Ⱥ�ı�־Ϊ1ʱ�������ݰ��У��Ͳ��ǡ�����ϢԱ��QQ�š������ǡ�����ϢȺQQ�š�

	Ⱥ���ı���Ϣ������
	��1100012001100005Hello��
	��1 10001 2001 1 00005 Hello��
	Ⱥ���ı���Ϣ��������Ⱥ�� ������Ϣ��Ա��QQ�� ������Ϣ��ȺQQ�� �ı����� ���ݿ��Ϊ5 �������ݡ�
	��ʾ��QQ��10001 ��Ⱥ2001�����ı���Ϣ��������5����������Ϊ Hello

	����ͼƬ��Ϣ������
	��0100011000201images060��
	��0 10001 10002 0 1images 060��
	����ͼƬ��Ϣ������������ ������Ϣ��Ա��QQ�� ������Ϣ��Ա��QQ�� ͼƬ���� ������� �������ơ�
	��ʾ��Ա��QQ��10001 ��Ա��QQ��10002������Ϣ ͼƬ���� 1������ ��������60(�������Ʋ���3λ��ǰ����0��ȫ)

	Ⱥ���ļ���Ϣ������
	��1100052001210bytestest.txtdata_beginhelloworld��
	��1 10005 2001 2 10bytes test.txt data_begin helloworld��
	Ⱥ���ļ���Ϣ��������Ⱥ�� ������Ϣ��Ա��QQ�� ������Ϣ��ȺQQ�� �ļ����� �ļ����ݳ��� �ļ��� ���ַ��� �ļ����� ��
	��ʾ��QQ��10001 ��Ⱥ2001 �����ļ���Ϣ���ļ����ݳ�����10���ļ���test.txt�����ַ���data_begin���ļ�����helloworld

*/
void TalkWindowShell::processPendingData()
{
	// �˿��У���δ���������
	// ���÷��� hasPendingDatagrams()������ֵ �ͻ�������ǣ��Ƿ���δ���������
	while (m_udpRecevier->hasPendingDatagrams())
	{
		// ����һЩ���������� ���ʱ��λ
		// ��Щλ���ǹ̶��ģ����ᷢ���ı䣬�����const static
		const static int groupFlagWidth = 1;	// Ⱥ�ı�־��ȣ�ռ1λ
		const static int groupWidth = 4;		// ȺQQ�ſ�ȣ�ռ4λ
		const static int employeeWidth = 5;		// Ա��QQ�ſ��
		const static int msgTypeWidth = 1;		// ��Ϣ���Ϳ��
		const static int msgLengthWidth = 5;	// �ı����ݿ�ȣ����ռ5λ
		const static int pictureWidth = 3;		// ����ͼƬ��ȣ�

		QByteArray btData;
		// ��ȡ ����Ҫ�������ݵĴ�С
		btData.resize(m_udpRecevier->pendingDatagramSize());
		// ��ȡUDP���ݣ����ݣ���С��
		m_udpRecevier->readDatagram(btData.data(), btData.size());

		// �����ݱ��浽 QString������
		QString strData = btData.data();
		// ���촰��ID��Ⱥ����Ⱥ�ţ�������Ա��QQ��
		QString strWindowID;
		// ���Ͷ� �� ���ն˵�QQ��
		QString strSendEmployeeID, strRecevieEmployeeID;

		QString strMsg;		// ����
		int msgLength;		// ���ݳ���
		int msgType;		// ��������

		// ͨ�� ���ݵ���.mid()��������ȡ ���Ͷ˵�QQ��
		// mid(��ȡλ�ã���ȡ�೤)���������ַ����н�ȡ ���ַ���
		strSendEmployeeID = strData.mid(groupFlagWidth, employeeWidth);

		/*
			�����Լ��� �ͻ�1 ���͸����ͻ�2���ͻ���3
			�������յ���Ϣ��Ҳ������ݷ��͸��Լ�
			���ԣ�������Լ����ͳ�ȥ�����ݣ����÷���������������ʡ��Դ
		*/
		// �Լ�������Ϣ���������ݴ���
		// ��� �����ߵ�ID���� QQ��½�ߵ�ID����������
		if (strSendEmployeeID == gLoginEmployeeID)
		{
			return;		// ֱ�ӷ���
		}

		// �жϱ�־λ�������1����ΪȺ��
		if (btData[0] == '1')
		{
			// ��ȡȺ��ID
			strWindowID = strData.mid(groupFlagWidth + employeeWidth, groupWidth);

			QChar cMsgType = btData[groupFlagWidth + employeeWidth + groupWidth];
			// �ж� �ǲ����ı���Ϣ
			if (cMsgType == '1')
			{
				msgType = 1;

				// ��ȡ�ı���Ϣ ����
				msgLength = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth
					, msgLengthWidth).toInt();

				// ��ȡ ���ݰ���� �ı�����
				strMsg = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth + msgLengthWidth
					, msgLength);

			}
			// �ж� �ǲ��Ǳ�����Ϣ
			else if (cMsgType == '0')
			{
				msgType = 0;

				// ����һ�� images �ַ�����λ��
				// indexOf()�����������ַ�����
				// ���� �ַ��� ��һ�γ��ֵ�λ��
				int posImages = strData.indexOf("images");

				// ��ȡ������Ϣ�ַ���
				// right()�������������� "λ��"����ȡ�� λ���Ҳ���������
				// �����õ��� ��һ�γ���images��λ�� ���±���ֵ
				// �� �����ܳ��� ��ȥ ��һ�γ���images��λ�� �ټ�ȥ images����ĳ���
				strMsg = strData.right(strData.length() - posImages - QString("images").length());

			}
			// �ж� �ǲ����ļ���Ϣ
			else if (cMsgType == '2')
			{
				msgType = 2;

				// ���� bytes �ĳ���
				int bytesWidth = QString("bytes").length();
				// bytes����һ�γ��ֵ�λ��
				int posBytes = strData.indexOf("bytes");
				// data_begin����һ�γ��ֵ�λ��
				int posData_begin = strData.indexOf("data_begin");


				// ��ȡ �ļ�����
				// �� bytes��һ�γ��ֵ�λ�� + bytes�ĳ��� ��ʼ��ȡ��
				// ��ȡ���ȣ� data_begin��һ�γ��ֵ�λ�� ��ȥ bytes��һ�γ��ֵ�λ�� 
				//					�ټ�ȥ bytes�ĳ���
				QString fileName = strData.mid(posBytes + bytesWidth
					, posData_begin - posBytes - bytesWidth);
				// ������������ �ļ����ƣ���ֵ��ȫ�ֱ���
				gfileName = fileName;

				// �ļ�����
				int dataLengthWidth;	// �ļ����ݵĿ��
				int posData = posData_begin + QString("data_begin").length();
				// ��ȡ���ݣ��ļ�����
				strMsg = strData.mid(posData);

				// ������������ �ļ����ݣ���ֵ��ȫ�ֱ���
				gfileData = strMsg;

				// ����employeeID��ȡ����������
				QString sender;
				int empID = strSendEmployeeID.toInt();	// ת��������
				QSqlQuery querySenderName(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1")
					.arg(empID));
				querySenderName.exec();		// ִ������SQL���

				// �жϣ����ݿ��� �Ƿ�������
				if (querySenderName.first())
				{
					sender = querySenderName.value(0).toString();
				}

				// �����ļ��ĺ�������...
				RecevieFile* recvFile = new RecevieFile(this);

				// ���˵���ȡ�������� �����ź�
				connect(recvFile, &RecevieFile::refuseFile, [this]()
					{
						return;
					});

				// �յ�xxx����Ϣ
				QString msgLabel = QString::fromLocal8Bit("�յ�����") + sender
					+ QString::fromLocal8Bit("�������ļ����Ƿ���գ�");
				// ���ı��ַ��������õ���ǩ��
				recvFile->setMsg(msgLabel);
				recvFile->show();


			}

		}
		else // ����
		{
			// ��ȡ�����ߵ�QQ��
			strRecevieEmployeeID = strData.mid(groupFlagWidth + employeeWidth, employeeWidth);
			// ��ȡ������ID�ǳƣ��ַ���
			strWindowID = strSendEmployeeID;

			// �����ҵ���Ϣ����������
			// �����ߵ�ID �� ��½�ߵ�ID ������һ���ģ���ֱ�ӷ���
			if (strRecevieEmployeeID != gLoginEmployeeID)
			{
				return;
			}


			// ��ȡ��Ϣ������
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + employeeWidth];

			// �ж���Ϣ����
			// �ı���Ϣ
			if (cMsgType == '1')
			{
				msgType = 1;

				// ��ȡ���ı���Ϣ�ĳ���
				msgLength = strData.mid(groupFlagWidth + employeeWidth + employeeWidth
					+ msgTypeWidth, msgLengthWidth).toInt();

				// �ı���Ϣ
				strMsg = strData.mid(groupFlagWidth + employeeWidth + employeeWidth
					+ msgTypeWidth + msgLengthWidth, msgLength);

			}
			// ������Ϣ
			else if (cMsgType == '0')
			{
				msgType = 0;
				int posImages = strData.indexOf("images");
				int imagesWidth = QString("images").length();

				// mid()���ڶ���������д����ָ�� ��ȡ���
				// ��˼�� �ӽ�ȡλ�õ�������е�����
				strMsg = strData.mid(posImages + imagesWidth);

			}
			// �ļ���Ϣ
			else if (cMsgType == '2')
			{
				msgType = 2;

				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int data_beginWidth = QString("data_begin").length();
				int posData_begin = strData.indexOf("data_begin");

				// �ļ�����
				QString fileName = strData.mid(posBytes + bytesWidth
					, posData_begin - posBytes - bytesWidth);
				gfileName = fileName;

				// �ļ�����
				strMsg = strData.mid(posData_begin + data_beginWidth);
				gfileData = strMsg;

				// ����employeeID��ȡ����������
				QString sender;
				int empID = strSendEmployeeID.toInt();	// ת��������
				QSqlQuery querySenderName(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1")
					.arg(empID));
				querySenderName.exec();		// ִ������SQL���

				// �жϣ����ݿ��� �Ƿ�������
				if (querySenderName.first())
				{
					sender = querySenderName.value(0).toString();
				}

				// �����ļ��ĺ�������...
				RecevieFile* recvFile = new RecevieFile(this);

				// ���˵���ȡ�������� �����ź�
				connect(recvFile, &RecevieFile::refuseFile, [this]()
					{
						return;
					});

				// �յ�xxx����Ϣ
				QString msgLabel = QString::fromLocal8Bit("�յ�����") + sender
					+ QString::fromLocal8Bit("�������ļ����Ƿ���գ�");
				// ���ı��ַ��������õ���ǩ��
				recvFile->setMsg(msgLabel);
				recvFile->show();
			}
		}

		/*
			���ݽ�������֮�󣬽�������ӵ� ������Ϣ����ҳ��ȥ
			���͵�����ˣ����й㲥�ַ���������Ϣ�����߲����յ�����

			���裬
			���ڴ� ��˾Ⱥ ���з�Ⱥ ��2�����촰�ڣ�
			��ǰ������� ��˾Ⱥ��
			��� ��ʱ �з�Ⱥ������Ϣ �������ǿ������ģ�
			���� ��Ҫ�� �з�Ⱥ���촰�ڣ�����Ϊ �����
			���� �з�Ⱥ���촰�����棬��ʾ���յ�����Ϣ

			��� ��ʱ ����Ⱥ������Ϣ���Ͳ���������
			��Ϊ��û�д� ����Ⱥ����

			�Ȼ�ȡ����QQ�ţ��������������û�д�
			������ˣ�����Ϊ����ڣ�������Ϣ
			���û�򿪣���ֱ�ӷ��أ�����������

		*/

		// �����촰�ڣ���Ϊ��Ĵ���
		QWidget* widget = WindowManager::getInstance()->findWindowName(strWindowID);

		// �жϴ����Ƿ��
		if (widget)
		{
			// �Ѵ��ڣ�����Ϊ�����
			this->setCurrentWidget(widget);

			// ����������б�ͬ������
			// ͨ��ӳ�䣬��ȡ�������촰�ڵ�ֵ��
			// ���浽 QListWidgetItem ����������
			QListWidgetItem* item = m_talkwindowItemMap.key(widget);
			item->setSelected(true);	// ��Ϊѡ�У��״̬
		}
		else
		{
			return;		// �����ڣ�ֱ�ӷ���
		}

		// ����Ϣ�������жϣ�������ļ����ͣ��򲻵��� handleReceivedMsg()
		if (msgType != 2)
		{
			int sendEmployeeID = strSendEmployeeID.toInt();
			// "��ҳ"��׷������
			handleReceivedMsg(sendEmployeeID, msgType, strMsg);
		}
	}
}


