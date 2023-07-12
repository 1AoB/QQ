#include "TalkWindow.h"
#include "RootContactItem.h"
#include "ContactItem.h"
#include "CommonUtils.h"
#include "WindowManager.h"

#include <QToolTip>
#include <QFile>
#include <QMessageBox>
#include <QtSql\qsqlquerymodel.h>
#include <QSqlQuery>
#include <SendFile.h>
#include <QTimer>

extern QString gLoginEmployeeID;

TalkWindow::TalkWindow(QWidget* parent, const QString& uid)/*GroupType groupType)*/
	: QWidget(parent)
	, m_talkId(uid)
	// , m_groupType(groupType)
{
	ui.setupUi(this);

	// �ڴ������촰�ڵ�ʱ�򣬰�ֵ��ӽ�ӳ�����棬���д�����Դ����
	// ���򣬴��ڿ����������ӣ����ǻ���ڶ������˾Ⱥ�����������
	WindowManager::getInstance()->addWindowName(m_talkId, this);

	setAttribute(Qt::WA_DeleteOnClose);


	// �ڳ�ʼ���ؼ�֮ǰ����ʼ��״̬������Ⱥ��״̬��Զ����false��
	initGroupTalkStatus();
	initControl();

}

TalkWindow::~TalkWindow()
{
	// �� TalkWindow() ���캯���У����������� uid
	// ���� ֱ�Ӵ��� m_talkId �Ϳ�����
	WindowManager::getInstance()->deleteWindowName(m_talkId);
}


void TalkWindow::onFileOpenBtnClicked(bool)
{
	// ���췢���ļ��Ի���Ķ���ֱ����ʾ
	SendFile* sendFile = new SendFile(this);
	sendFile->show();
}

void TalkWindow::addEmotionImage(int emotionNum)
{
	// �Ȼ�ȡ����
	ui.textEdit->setFocus();
	// ���ã���ӱ���ͼƬ
	ui.textEdit->addEmotionUrl(emotionNum);
}

void TalkWindow::onSendBtnClicked(bool)
{
	// ������Ϣ����Ϊ�գ����Ϊ�� ������ʾ
	// ת���� Ⱥ�ı� toPlainText()
	if (ui.textEdit->toPlainText().isEmpty())
	{
		// ��ʾ�ı���showText(���꣬�ַ����ı������ڣ����ƾ��Σ���ʾ2000����)
		// �ȸ������꣬��һ�����λ�ã�Ҫת�� ȫ������λ��
		QToolTip::showText(
			this->mapToGlobal(QPoint(630, 660)),
			QString::fromLocal8Bit("���͵���Ϣ����Ϊ��"),
			this,
			QRect(0, 0, 120, 100),
			2000);
		return;
	}

	// ���ı� ת�� html����
	//QString& html = ui.textEdit->document()->toHtml();
	QString& html = ui.textEdit->document()->toHtml();

	// �ı�html�����û�����壬���������
	if (!html.contains(".png") && !html.contains("</span>"))
	{
		QString fontHtml;
		QString text = ui.textEdit->toPlainText();
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly))
		{
			fontHtml = file.readAll();
			// ��html�ļ���� %1�����ַ��� text �滻
			fontHtml.replace("%1", text);
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
		if (!html.contains(fontHtml))
		{
			html.replace(text, fontHtml);
		}
	}

	// ���
	ui.textEdit->clear();
	ui.textEdit->deleteAllEmotionImage();

	// ����Ϣ���ڣ�׷����Ϣ
	ui.msgWidget->appendMsg(html);

}

void TalkWindow::setWindowName(const QString& name)
{
	ui.nameLabel->setText(name);
}

QString TalkWindow::getTalkID()
{
	return m_talkId;
}

void TalkWindow::initControl()
{
	// �ȸ��ұ߷����������óߴ�
	QList<int> rightWidgetSize;
	rightWidgetSize << 600 << 138;

	// ������һ������
	// ����� ���ҵķ����������ÿ���������� ���
	// ����� ���µķ����������ÿ���������� �߶�
	ui.bodySplitter->setSizes(rightWidgetSize);

	ui.textEdit->setFontPointSize(10);
	ui.textEdit->setFocus();						// ��ȡ���㣬�򿪴���ʱ��ֱ�ӾͿ������룬����Ҫ��������һ��

	// ��С�����������Ͻǹرգ��رգ������Աߵģ�
	connect(ui.sysmin, SIGNAL(clicked(bool)), parent(), SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.closeBtn, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));

	// ���鰴ť�����
	connect(ui.faceBtn, SIGNAL(clicked(bool)), parent(), SLOT(onEmotionBtnClicked(bool)));

	// ����
	connect(ui.sendBtn, SIGNAL(clicked(bool)), this, SLOT(onSendBtnClicked(bool)));

	// �����ļ��ź�
	connect(ui.fileopenBtn, SIGNAL(clicked(bool)), this, SLOT(onFileOpenBtnClicked(bool)));

	// ������˫����ʱ��
	// Ҫ��������˫��������һ�����һ��ĵ�ַ
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
		this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	// �ж� �Ƿ�Ϊ Ⱥ��
	if (m_isGroupTalk)
	{
		initTalkWindow();		// Ⱥ��
	}
	else
	{
		initPtoPTalk();				// ����
	}

}

void TalkWindow::initGroupTalkStatus()
{
	QSqlQueryModel sqlDepModel;
	// SELECT * FROM tab_department WHERE departmentID = 2001
	QString strSql = QString("SELECT * FROM tab_department WHERE departmentID = %1").arg(m_talkId);

	sqlDepModel.setQuery(strSql);

	int rows = sqlDepModel.rowCount();			// �����ܹ���ģ���к�

	// ==0 Ϊ����
	if (rows == 0)
	{
		m_isGroupTalk = false;
	}
	else
	{
		m_isGroupTalk = true;
	}


}

int TalkWindow::getCompDepID()
{
	QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
		.arg(QString::fromLocal8Bit("��˾Ⱥ")));
	queryDepID.exec();
	queryDepID.next();				// ���� queryDepID.first(); Ҳ��

	return queryDepID.value(0).toInt();
}



void TalkWindow::initTalkWindow()
{
	// �ȹ������
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();

	// �����������ú��ӣ�ָʾ���Ĳ���
	// ָʾ�����õĲ��ԣ��������������û�д��ڣ�
	// ���ָʾ��������ڣ���������������
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	// ��ʼ��������
	pRootItem->setData(0, Qt::UserRole, 0);		// ��������0�У��û�����������Ϊ0��

	// �������
	// ��һ���������Ƿ���Ҫ��ͷ������Ҫ��дfalse
	// �ڶ����������� ������Ϊ���ĸ�����
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);					// ���� Shell�߶� ��ȥ shellͷ�ߣ�talkwindow titleWidget��

	// ��ȡȺ������
	QString strGroupName;
	// ��ѯ��������
	QSqlQuery queryGroupName(QString("SELECT department_name FROM tab_department WHERE departmentID = %1").arg(m_talkId));
	queryGroupName.exec();

	if (queryGroupName.next())
	{
		strGroupName = queryGroupName.value(0).toString();
	}

	/*
		������ǹ�˾Ⱥ����Ҫ����ɸѡ
		��Ա�������棬���һ�����²����м���Ա����
		ֻҪ����departmentID�������²���ID������ɸѡ

		�����2000��ʱ�򣬾�Ҫ�������⴦��
		�Ǿͽ�Ա������������ݣ�ȫ���ù�����ֱ�� select * ȫ��ѡ��
		�ڸ��� departmentID ������ɸѡ
	*/
	// �Ȼ�ȡȺ�ţ����� m_talkID ���бȶ�
	QSqlQueryModel queryEmployeeModel;

	// �����һ���ģ��Ǿ��ǹ�˾Ⱥ
	if (getCompDepID() == m_talkId.toInt())
	{
		// ��ѯ��״̬Ϊ1��
		queryEmployeeModel.setQuery("SELECT employeeID FROM tab_employees WHERE status = 1");
	}
	else
	{	// ������ǹ�˾Ⱥ��ͨ����˾Ⱥ�ţ����й���
		queryEmployeeModel.setQuery(QString("SELECT employeeID FROM tab_employees WHERE status = 1 AND departmentID = %1")
			.arg(m_talkId));
	}

	// ��ȡ����ǰԱ��������
	int nEmployeeNum = queryEmployeeModel.rowCount();

	// Ⱥ�����֣����������Ƕ��٣�Ա��������
	QString qsGroupName = QString::fromLocal8Bit("%1 %2/%3")
		.arg(strGroupName).arg(0).arg(nEmployeeNum);
	//QMessageBox::information(this, "Tips", qsGroupName);
	// �� qsGroupName ����ı�������Ϊ��ǰ�� pItemName ���ı�
	pItemName->setText(qsGroupName);

	//��ʱˢ������////////////////////////ˢ����������/////////////////////////
	m_timer = new QTimer(this);
	m_timer->setInterval(150);//150�������һ��
	m_timer->start();
	connect(m_timer, &QTimer::timeout, this, [=]() {
		QSqlQuery OnlineNum;
		OnlineNum.exec("SELECT count(employeeID) FROM tab_employees WHERE online = 2");
		int OnlineNum_;
		while (OnlineNum.next()) {
			OnlineNum_ = OnlineNum.value(0).toInt();
		}
		QString qsGroupName_ = QString::fromLocal8Bit("%1 %2/%3")
			.arg(strGroupName).arg(OnlineNum_).arg(nEmployeeNum);
		pItemName->setText(qsGroupName_);
		});////////////////////////////////////////////////////////////////////////////

		// �������ڵ�
	ui.treeWidget->addTopLevelItem(pRootItem);			// ����������һ������һ��
	// ������Ŀ�������Ը��� pRootItem ���ã���0�У��������Ĳ����� pItemName��
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	// �����е����Ĭ��չ��
	pRootItem->setExpanded(true);

	// �������
	for (int i = 0; i < nEmployeeNum; i++)
	{
		// ��ѯԱ��ģ����������ȡԱ��QQ��
		QModelIndex modelindex = queryEmployeeModel.index(i, 0);
		int employeeID = queryEmployeeModel.data(modelindex).toInt();

		// ����Ա��ID������ÿ����ӵ�Ա�����Ƕ�һ�޶���
		addPeopInfo(pRootItem, employeeID);
	}
}

void TalkWindow::initPtoPTalk()
{
	// ����˽�ĵ�ʱ�����Ҳ��һ��ͼƬ
	QPixmap pixSkin;
	pixSkin.load(":/Resources/MainWindow/skin.png");

	ui.widget->setFixedSize(pixSkin.size());			// ���óߴ�

	// ��widget������ͼƬ
	QLabel* skinLabel = new QLabel(ui.widget);
	skinLabel->setPixmap(pixSkin);

	// ͬʱ�Ա�ǩ�����ô�С
	skinLabel->setFixedSize(ui.widget->size());

}

void TalkWindow::addPeopInfo(QTreeWidgetItem* pRootGroupItem, int employeeID)
{
	// ��Ҫ�������͵��ȹ���һ������
	QTreeWidgetItem* pChild = new QTreeWidgetItem();

	// ����ӽڵ�
	pChild->setData(0, Qt::UserRole, 1);							// ��0�У�UserRole����Ϊ1
	// ����˫���򿪽��������ʱ��Ҫ��������ĵ�ַ��Ϊ�ο��ı�ʶ
	// ��ɫ����Ϊ UserRole+1 �����ݵĻ� ���� pChild�ĵ�ַ
	// pChild �Ǹ�ָ��Ҫ����ת����
	// QString::number((int)pChild)���Ƚ�ָ��ת�� int���ͣ��ٽ� int���� ת�� �ַ���
	// �Դ������У�����ֵ
	pChild->setData(0, Qt::UserRole + 1, employeeID);

	// ����ͷ��Ҫ����һЩ����Բͷ���
	QPixmap pix1;
	pix1.load(":/Resources/MainWindow/head_mask.png");

	// ��ȡ���֣�ǩ����ͷ��
	QString strName, strSign, strPicturePath;
	QSqlQueryModel queryInfoModel;
	
	queryInfoModel.setQuery(QString("SELECT employee_name,employee_sign,picture FROM tab_employees WHERE employeeID = %1")
		.arg(employeeID));

	QModelIndex nameIndex, signIndex, picIndex;
	nameIndex = queryInfoModel.index(0, 0);				// 0�У�0��
	signIndex = queryInfoModel.index(0, 1);
	picIndex = queryInfoModel.index(0, 2);

	strName = queryInfoModel.data(nameIndex).toString();
	strSign = queryInfoModel.data(signIndex).toString();
	strPicturePath = queryInfoModel.data(picIndex).toString();

	QImage imageHead;
	imageHead.load(strPicturePath);

	// ��ϵ��
	// ��Ϊ���� �� ��д���ܣ����Ҫ�� �� ��Ϊ������
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);

	// ������Բͷ��
	// getRoundImage������ԭʼͼ��mask���� pix1���ߴ磩
	// ��Ϊ��� Բͷ��Ҫ�ŵ� ContactItem��ȥ�����д����� ContactItem������ ��ȡ�ߴ�Ľӿ�
	// ���Ҫ����һ��
	pContactItem->setHeadPixmap(CommonUtils::getRoundImage(QPixmap::fromImage(imageHead), pix1, pContactItem->getHeadLabelSize()));

	// �������֣�ǩ��
	pContactItem->setUserName(strName);
	pContactItem->setSignName(strSign);

	// ����ǰ�������ӵ�������
	pRootGroupItem->addChild(pChild);

	// �������ӵ� �� ��
	// ������Ŀ��������˭���ý�ȥ����0�У��������� ��ϵ�� pContactItem��
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	QString str = pContactItem->getUserName();		// ��ȡ�û���
	m_groupPeoMap.insert(pChild, str);						// ����ӳ�䣬������ʹ������ƣ���Ӧ����

}

void TalkWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	// �Ȼ�ȡ���ݣ���0�У�ת���� bool ����
	bool bIsChild = item->data(0, Qt::UserRole).toBool();

	// �ж��Ƿ�Ϊ����
	if (bIsChild)
	{
		QString talkId = item->data(0, Qt::UserRole + 1).toString();
		if (talkId == gLoginEmployeeID) {
			QMessageBox::information(this, "Tips", "donot talk with yourself!");
			return;
		}
		// ��ȡ��������
		QString strPeopleName = m_groupPeoMap.value(item);

		/*
		 ��ȡʵ��������´��ڣ�����ID�����飬������
		 ��һ�������� ID������д �Ѿ��еĵ�ǰ����ID��Ҫд �´���Ĵ���ID
		 ����Ҫͨ�� item����data��������ת���ַ���
		 ( ��̫���ף�Ϊʲô������ Qt::UserRole �������� Qt::UserRole+1 )

		 PTOP�������˸���˽�ģ����ܴ��뵱ǰȺ�ķ��飬����Ĭ�ϵ� m_groupType

		 ӳ���е� m_groupPeoMap ֵ������ value��������ȡ����
		 value() ����������Ĳ��� �����ǵ�ǰ�� Key�����ǵ�ǰ Item����
		*/
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString()); //PTOP, strPeopleName

	}


}
