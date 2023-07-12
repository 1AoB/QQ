#include "CCMainWindow.h"
#include "SkinWindow.h"
#include "SysTray.h"
#include "NotifyManager.h"
#include "RootContactItem.h"
#include "ContactItem.h"
#include "WindowManager.h"
#include "TalkWindowShell.h"

#include <QHBoxLayout>
#include <QProxyStyle>
#include <QPainter>
#include <QTimer>
#include <QEvent>
#include <QTreeWidgetItem>
#include <QMouseEvent>
#include <QApplication>
#include <QtSql\qsqlquery.h>




// ����һ���ⲿ����
extern QString gLoginEmployeeID;
// ��ȡ�Լ���ͷ��
QString gstrLoginHeadPath;


// �ŵ�ͷ�ļ��� �� ����      Custom�Զ��� style���
class CCMainCustomProxyStyle : public QProxyStyle
{
public:
	// ��д drawPrimitive����
	// ���߿���Σ���ȡ���㣬�Ͳ����л��ƣ�ֱ�ӷ���
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
		QPainter* painter, const QWidget* widget = nullptr) const
	{
		// ��ԣ��߿��ȡ����ʱ�����д���
		// ���Ԫ�� ���� �߿���Σ���ֱ�ӷ��أ���������
		if (element == PE_FrameFocusRect)
		{
			return;
		}
		else
		{
			// ������ǣ��ͽ��� Ĭ�ϲ���
			QProxyStyle::drawPrimitive(element, option, painter, widget);
		}
	}
};


CCMainWindow::CCMainWindow(QString account, bool isAccountLogin, QWidget* parent)
	: BasicWindow(parent)
	, m_isAccountLogin(isAccountLogin)
	, m_account(account)
{
	ui.setupUi(this);

	// ���������ڷ��
	setWindowFlags(windowFlags() | Qt::Tool);

	// ���� ��ʽ��
	loadStyleSheet("CCMainWindow");

	// �Ȼ�ȡͷ��·����������ͷ��·��
	setHeadPixmap(getHeadPicPath());

	// ���ó�ʼ���ؼ�
	initControl();

	// ���ü�ʱ��
	initTimer();


}

CCMainWindow::~CCMainWindow()
{
}

void CCMainWindow::initControl()
{
	// ����ȡ����ʱ�������Ƶı߿�
	ui.treeWidget->setStyle(new CCMainCustomProxyStyle);
	setLevelPixmap(0);
	//setHeadPixmap(":/Resources/MainWindow/girl.png");
	setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");

	// �����壬ͷ���Աߵ�Сappͼ��
	QHBoxLayout* appupLayout = new QHBoxLayout;
	appupLayout->setContentsMargins(0, 0, 0, 0);			// ���ü�϶
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_7.png", "app_7"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_2.png", "app_2"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_3.png", "app_3"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_4.png", "app_4"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_5.png", "app_5"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_6.png", "app_6"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/skin.png", "app_skin"));
	appupLayout->addStretch();
	appupLayout->setSpacing(2);			// ����֮����2�����صĿ�λ��䣬�Եø�����
	// ���ò���
	ui.appWidget->setLayout(appupLayout);

	// �����壬�����Сappͼ��
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_10.png", "app_10"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_8.png", "app_8"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_11.png", "app_11"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_9.png", "app_9"));
	ui.bottomLayout_up->addStretch();		// �������������������գ��Ҳ��ɿ�λ���

	// ��ʼ����ϵ�ˣ���
	initContacTree();

	// ����ǩ��
	// ����¼����������õ�ǰ�����������¼�
	ui.lineEdit->installEventFilter(this);

	// ������
	ui.searchLineEdit->installEventFilter(this);

	// ��С��
	connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowHide(bool)));
	// �ر�
	connect(ui.sysclose, SIGNAL(clicked(bool)), this, SLOT(onShowClose(bool)));
	// �����������
	connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, [this]()
		{
			updateSeachStyle();
		});

	// �������̶���
	SysTray* systray = new SysTray(this);

}

void CCMainWindow::initTimer()
{
	QTimer* timer = new QTimer(this);
	timer->setInterval(500);				// 500����

	// lambda���ʽ������ʱ���źţ���������
	connect(timer, &QTimer::timeout, [this] {
		static int level = 0;
		if (level == 99)
		{
			level = 0;
		}
		level++;
		setLevelPixmap(level);
		});

	timer->start();			// ������ʱ��
}
//�����û���
void CCMainWindow::setUserName(const QString& username)
{
	ui.nameLabel->adjustSize();		// �ñ�ǩ���������ݵ����Լ��ĳߴ�

	// �ı���������������Դ��� ...
	// ������� fontMetrics() ���������ص��� QFontMetrics��������������
	// �ٵ��� "elidedText() ʡ���ı�����"��������ȣ����صĿ�ȣ��ͻ��ԡ�...�����Խ��д���
	// ��һ���������ı���	�ڶ���������ʡ��ģʽ
	// ����������ȡ� �Ĳ���int flags = 0���Ǳ�־
	QString name = ui.nameLabel->fontMetrics().elidedText(username, Qt::ElideRight, ui.nameLabel->width());

	ui.nameLabel->setText(name);			// �����û���

}

void CCMainWindow::setLevelPixmap(int level)
{
	// �ȼ�λͼ
	// ��С�ߴ磬���� ͷ�񲿼��Ĵ�С
	QPixmap levelPixmap(ui.levelBtn->size());
	levelPixmap.fill(Qt::transparent);

	// ����һ�����ң��� levelPixmap �Ͻ��л���
	QPainter painter(&levelPixmap);
	painter.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));

	int unitNum = level % 10;		// �ȼ� ��λ�� �� 1-9��
	int tenNum = level / 10;			// �ȼ� ��λ�� �� 10-99��

	// ʮλ ���ȼ�����ͼƬ����ȡͼƬ�еĲ��ֽ��л���
	// ֻ��������һ���֣�����Ҫ��ͼƬȫ������
	// 10,4 �ǻ�ͼ����ʼλ��
	// ��������������ͼƬ�ļ�
	// ��4,5�Ĳ�������sx��sy��ͼƬ���Ͻǵ�λ�ã�ָ���λͼ�� �߶�
	// ��6,7�Ĳ�������sw��sh��ָ���λͼ�� ���
	// ˵���ˣ�����ָ��Ҫ��������ͼƬ����һ����
	painter.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"),
		tenNum * 6, 0, 6, 7);

	// ��λ
	painter.drawPixmap(16, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"),
		unitNum * 6, 0, 6, 7);

	// ��ͷ�񲿼�������ͼ��
	ui.levelBtn->setIcon(levelPixmap);
	// ���ô�С
	ui.levelBtn->setIconSize(ui.levelBtn->size());
}

void CCMainWindow::setHeadPixmap(const QString& headPath)
{
	QPixmap pix;
	pix.load(":/Resources/MainWindow/head_mask.png");
	// ���ñ�ǩ��ͷ��
	ui.headLbael->setPixmap(getRoundImage(QPixmap(headPath),
		pix, ui.headLbael->size()));
}

void CCMainWindow::setStatusMenuIcon(const QString& statusPath)
{
	// ״̬λͼ�����ô�С
	QPixmap statusBtnPixmap(ui.stausBtn->size());
	statusBtnPixmap.fill(Qt::transparent);			// ͸��

	QPainter painter(&statusBtnPixmap);
	painter.drawPixmap(4, 4, QPixmap(statusPath));

	// ��ͼ�괫��ȥ
	ui.stausBtn->setIcon(statusBtnPixmap);
	ui.stausBtn->setIconSize(ui.stausBtn->size());

}

QWidget* CCMainWindow::addOtherAppExtension(const QString& appPath, const QString& appName)
{
	QPushButton* btn = new QPushButton(this);	// ��ǰ������Ϊ����
	btn->setFixedSize(20, 20);

	QPixmap pixmap(btn->size());
	pixmap.fill(Qt::transparent);

	QPainter painter(&pixmap);
	QPixmap appPixmap(appPath);
	painter.drawPixmap((btn->width() - appPixmap.width()) / 2,
		(btn->height() - appPixmap.height()) / 2, appPixmap);
	btn->setIcon(pixmap);
	btn->setIconSize(btn->size());
	btn->setObjectName(appName);			// ���ö�����
	btn->setProperty("hasborder", true);		// �ò����б߿�

	// ����Ƥ��
	connect(btn, &QPushButton::clicked, this, &CCMainWindow::onAppIconClicked);

	return btn;
}

void CCMainWindow::initContacTree()
{
	// չ��������ʱ���ź�
	// (QTreeWidgetItem*, int) ��������� ��һ���һ��
	// ���
	connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
		this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
	// �����չ��
	connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)),
		this, SLOT(onItemExpanded(QTreeWidgetItem*)));
	// ���������
	connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
		this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
	// ˫��
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
		this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	// ���ڵ�
	// pRootGroupItem������ ����ĸ��ڵ� ��Ⱥ��
	QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;
	// ����������û���������Զ����չʾ
	pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	// ���ø��ڵ����ݣ���0�У�ʹ�ý�ɫ��ֵ��
	pRootGroupItem->setData(0, Qt::UserRole, 0);		// �������ݵ�ֵ��Ϊ0

	RootContactItem* pItemName = new RootContactItem(true, ui.treeWidget);

	// ��ȡ��˾����ID
	// SELECT departmentID FROM tab_department WHERE department_name = 'ren1'
	QSqlQuery queryCompDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(QString::fromLocal8Bit("��˾Ⱥ")));
	queryCompDepID.exec();			// ִ��
	queryCompDepID.first();				// ָ�������ĵ�һ��
	int ComDepID = queryCompDepID.value(0).toInt();			// ת����int

	// ��ȡQQ��½�����ڵĲ���ID������Ⱥ�ţ�
	// ����Ĵ�ֵ��Ӧ���Ǵ�IDԱ����
	// ���û���½��ʱ�򣬵�½�ɹ��󣬱����½�ߵ��˺ţ�����������
	// ��һ�� ȫ�ֱ��� ����
	// SELECT departmentID FROM tab_employees WHERE employeeID = 10001
	QSqlQuery querySelfDepID(QString("SELECT departmentID FROM tab_employees WHERE employeeID = %1").arg(gLoginEmployeeID));
	querySelfDepID.exec();			// ִ��
	querySelfDepID.first();				// ָ�������ĵ�һ��
	int SelfDepID = querySelfDepID.value(0).toInt();			// ת����int

	// ��ʼ����˾Ⱥ���� ��½�� ���ڵ�Ⱥ
	addCompanyDeps(pRootGroupItem, ComDepID);		// ���빫˾ȺID
	addCompanyDeps(pRootGroupItem, SelfDepID);			// �����Լ�����Ⱥ��ID

	QString strGroupName = QString::fromLocal8Bit("���˿Ƽ�");
	pItemName->setText(strGroupName);

	// �������ڵ�
	// ��Ӷ�����
	ui.treeWidget->addTopLevelItem(pRootGroupItem);
	// ��� ��Ĳ��������ڵ���pRootGroupItem����0��
	// ���������������õĲ���
	ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItemName);

}

void CCMainWindow::addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID)
{
	QTreeWidgetItem* pChild = new QTreeWidgetItem;

	QPixmap pix;
	pix.load(":/Resources/MainWindow/head_mask.png");

	// ����ӽڵ㣬���� ��ϵ��
	// ���ڵ������ֵΪ0���������������Ϊ1����������
	pChild->setData(0, Qt::UserRole, 1);

	// �����촰�ڣ�����Ψһ��ʶ��ID
	// ���������Լ����� ָ�� ת���� int����
	// �����͸� Qt::UserRole + 1 ����������
	// �Ѿ��õ�Ⱥ���ˣ�ֱ�Ӵ���������Ⱥ�ű�ע��
	// ����֮ǰ�����ַ����ˣ�֮ǰ�� �ô��ڵ�ַ���б�ע
	pChild->setData(0, Qt::UserRole + 1, DepID);

	// ��ȡ���ڹ�˾�Ĳ���ͷ��
	QPixmap groupPix;
	// SELECT picture FROM tab_department WHERE departmentID = 2001
	QSqlQuery queryPicture(QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(DepID));
	if (!queryPicture.exec()) {
		qDebug() << "335�г���!" << "DepID:" << DepID;
	}
	if (!queryPicture.next()) {			// Ҳ������ queryPicture.next();
		qDebug() << "338�г���!" << "DepID:" << DepID;
	}
	qDebug() << "��ȡ���ڹ�˾�Ĳ���ͷ���·��" << queryPicture.value(0).toString();
	groupPix.load(queryPicture.value(0).toString());

	// ��ȡ��������
	QString strDepName;
	// SELECT department_name FROM tab_department WHERE departmentID = 2001
	QSqlQuery queryDepName(QString("SELECT department_name FROM tab_department WHERE departmentID = %1").arg(DepID));
	queryDepName.exec();
	queryDepName.first();
	strDepName = queryDepName.value(0).toString();



	// ���࣬���������
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);
	pContactItem->setHeadPixmap(getRoundImage(groupPix, pix, pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(strDepName);

	// �ٶԸ�������ӽڵ�
	pRootGroupItem->addChild(pChild);
	// �ٶ��������������Ĳ���
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	// ���ӽڵ㣬��ӽ� m_groupMap ��
	// �������ǣ���˾Ⱥ�����²�ɶ���� �� �������֣�
	// �������ݿ�֮�����ӳ��Ͳ���Ҫ��
	//m_groupMap.insert(pChild, sDeps);	

}

void CCMainWindow::resizeEvent(QResizeEvent* event)
{
	// �ӱ��ر��룬ת���Unicode
	setUserName(QString::fromLocal8Bit("Qt�Ƽ�"));
	BasicWindow::resizeEvent(event);

}
//�������¼�
bool CCMainWindow::eventFilter(QObject* obj, QEvent* event)
{
	// �ж� �����¼��Ķ����ǲ�����������
	if (ui.searchLineEdit == obj)
	{
		// �ж� �¼�������
		// event ���� type() ���������ص�ǰ�¼������� 
		// �Ƿ�Ϊ ���̻�ȡ�����¼�
		if (event->type() == QEvent::FocusIn)
		{
			// �� ������������������ʽ
			// ����Ϊ��ɫ
			// hover ���������pressed ����Ч��
			ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgb(255,255,255);border-bottom:1px solid rgba(%1,%2,%3,100)} \
																				QPushButton#searchBtn{border-image:url(:/new/Resources/MainWindow/search/main_search_deldown.png)} \
																				QPushButton#searchBtn:hover{border-image:url(:/new/Resources/MainWindow/search/main_search_delhighlight.png)} \
																				QPushButton#searchBtn:pressed{border-image:url(:/new/Resources/MainWindow/search/main_search_delhighdown.png)}")
				.arg(m_colorBackGround.red())
				.arg(m_colorBackGround.green())
				.arg(m_colorBackGround.blue()));
		}
		else if (event->type() == QEvent::FocusOut)
		{
			// ����ƿ������㲻��������ʱ��Ҫ��ԭ
			updateSeachStyle();
		}
	}


	return false;
}

void CCMainWindow::mousePressEvent(QMouseEvent* event)
{
	// ������ڲ���Ҫ�����ʱ�򣬽��н�������
	// ����Ĳ�������������ͬʱ�������� ���н��㣬��Ͳ�����
	// ��˽����� ���
	if (qApp->widgetAt(event->pos()) != ui.searchLineEdit && ui.searchLineEdit->hasFocus())
	{
		ui.searchLineEdit->clearFocus();
	}
	else if (qApp->widgetAt(event->pos()) != ui.lineEdit && ui.lineEdit->hasFocus())
	{
		ui.lineEdit->clearFocus();
	}

	// �����ģ����л������¼��Ĵ���
	BasicWindow::mousePressEvent(event);
}

void CCMainWindow::updateSeachStyle()
{
	// ���л�ԭ
	ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgba(%1,%2,%3,50);border-bottom:1px solid rgba(%1,%2,%3,30)}\
																		QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/search_icon.png)}")
		.arg(m_colorBackGround.red())
		.arg(m_colorBackGround.green())
		.arg(m_colorBackGround.blue()));
}

void CCMainWindow::onItemClicked(QTreeWidgetItem* item, int column)
{
	// �жϣ��Ƿ�������
	// �ж����ݣ���0�У���ɫ�� UserRole �ķ���ֵ 
	bool bIsChild = item->data(0, Qt::UserRole).toBool();

	// ������Ϊ�գ�˵��������
	if (!bIsChild)
	{
		// �ж��Ƿ�չ��
		// ��� û��չ������չ������
		item->setExpanded(!item->isExpanded());
	}

}

void CCMainWindow::onItemExpanded(QTreeWidgetItem* item)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (!bIsChild)
	{
		// �Ȱ� QWidgetָ�룬ת���� RootContactItem���͵�ָ��
		// dynamic_cast ���������ָ�루�����ã�ת�����̳���ָ��
		RootContactItem* prootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));

		// �ж�һ�£����нǶ�ת��
		if (prootItem)
		{
			prootItem->setExpanded(true);
		}

	}
}

void CCMainWindow::onItemCollapsed(QTreeWidgetItem* item)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (!bIsChild)
	{
		// �Ȱ� QWidgetָ�룬ת���� RootContactItem���͵�ָ��
		// dynamic_cast ���������ָ�루�����ã�ת�����̳���ָ��
		RootContactItem* prootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));

		// �ж�һ�£����нǶ�ת��
		if (prootItem)
		{
			prootItem->setExpanded(false);
		}

	}
}

void CCMainWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	// �ж�˫�����Ǹ����������
	// data��0�У���ɫ������ת���� bool ����
	bool bIsChild = item->data(0, Qt::UserRole).toBool();

	// �ж� �Ƿ�Ϊ��Чֵ
	if (bIsChild)
	{
		// ֱ����ӣ��µ����촰��
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());


		// ���ж� ID���򿪵���Ⱥ�ģ�����˽��
		// �������ݿ�֮�����ӳ��Ͳ���Ҫ��
		// QString strGroup = m_groupMap.value(item);

		
		/*if (strGroup == QString::fromLocal8Bit("��˾Ⱥ"))
		{
			// ��� �µ����촰��
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(),COMPANY);
		}
		else if(strGroup == QString::fromLocal8Bit("���²�"))
		{
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), PERSONELGROUP);
		}
		else if(strGroup == QString::fromLocal8Bit("�г���"))
		{
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), MARKETGROUP);
		}
		else if(strGroup == QString::fromLocal8Bit("�з���"))
		{
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), DEVELOPMENTGROUP);
		}*/
		
	}
}

QString CCMainWindow::getHeadPicPath()
{
	QString strPicPath;

	// �ж�
	if (!m_isAccountLogin)			// QQ�ŵ�¼
	{
		QSqlQuery queryPic(QString("SELECT picture FROM tab_employees WHERE employeeID = %1")
			.arg(gLoginEmployeeID));
		queryPic.exec();
		queryPic.next();			// ָ�������ĵ�һ����¼

		strPicPath = queryPic.value(0).toString();
	}
	else		// �˺ŵ�½
	{
		// �ٻ�ȡ�˺�֮ǰ�����õ� account�˺�
		// Ȼ�� ��ͨ�� account ȥ��� employeeID
		QSqlQuery queryEmployeeID(QString("SELECT employeeID FROM tab_accounts WHERE account = '%1'")
			.arg(m_account));
		queryEmployeeID.exec();
		queryEmployeeID.next();
		int employeeID = queryEmployeeID.value(0).toInt();

		// ���� employeeID ��ȡͷ��·��
		QSqlQuery queryPic(QString("SELECT picture FROM tab_employees WHERE employeeID = %1")
			.arg(employeeID));
		queryPic.exec();
		queryPic.next();			// ָ�������ĵ�һ����¼

		strPicPath = queryPic.value(0).toString();
	}

	gstrLoginHeadPath = strPicPath;
	return strPicPath;
}

void CCMainWindow::onAppIconClicked()
{
	// ����źŷ����ߵĶ��������� app_skin
	if (sender()->objectName() == "app_skin")
	{
		SkinWindow* skinWindow = new SkinWindow;
		skinWindow->show();
	}
}


