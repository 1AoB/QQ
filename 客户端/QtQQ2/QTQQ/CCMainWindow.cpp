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




// 声明一个外部变量
extern QString gLoginEmployeeID;
// 获取自己的头像
QString gstrLoginHeadPath;


// 放到头文件里 ， 试试      Custom自定义 style风格
class CCMainCustomProxyStyle : public QProxyStyle
{
public:
	// 重写 drawPrimitive方法
	// 当边框矩形，获取焦点，就不进行绘制，直接返回
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
		QPainter* painter, const QWidget* widget = nullptr) const
	{
		// 针对，边框获取焦点时，进行处理
		// 如果元素 等于 边框矩形，就直接返回，不绘制它
		if (element == PE_FrameFocusRect)
		{
			return;
		}
		else
		{
			// 如果不是，就进行 默认操作
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

	// 设置主窗口风格
	setWindowFlags(windowFlags() | Qt::Tool);

	// 加载 样式名
	loadStyleSheet("CCMainWindow");

	// 先获取头像路径，再设置头像路径
	setHeadPixmap(getHeadPicPath());

	// 调用初始化控件
	initControl();

	// 调用计时器
	initTimer();


}

CCMainWindow::~CCMainWindow()
{
}

void CCMainWindow::initControl()
{
	// 树获取焦点时，不绘制的边框
	ui.treeWidget->setStyle(new CCMainCustomProxyStyle);
	setLevelPixmap(0);
	//setHeadPixmap(":/Resources/MainWindow/girl.png");
	setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");

	// 主窗体，头像旁边的小app图标
	QHBoxLayout* appupLayout = new QHBoxLayout;
	appupLayout->setContentsMargins(0, 0, 0, 0);			// 设置间隙
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_7.png", "app_7"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_2.png", "app_2"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_3.png", "app_3"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_4.png", "app_4"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_5.png", "app_5"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_6.png", "app_6"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/skin.png", "app_skin"));
	appupLayout->addStretch();
	appupLayout->setSpacing(2);			// 部件之间以2个像素的空位填充，显得更紧凑
	// 设置布局
	ui.appWidget->setLayout(appupLayout);

	// 主窗体，下面的小app图标
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_10.png", "app_10"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_8.png", "app_8"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_11.png", "app_11"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_9.png", "app_9"));
	ui.bottomLayout_up->addStretch();		// 加伸缩量，部件更紧凑，右侧变成空位填充

	// 初始化联系人，树
	initContacTree();

	// 个性签名
	// 添加事件过滤器，让当前对象监视这个事件
	ui.lineEdit->installEventFilter(this);

	// 搜索栏
	ui.searchLineEdit->installEventFilter(this);

	// 最小化
	connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowHide(bool)));
	// 关闭
	connect(ui.sysclose, SIGNAL(clicked(bool)), this, SLOT(onShowClose(bool)));
	// 更新搜索风格
	connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, [this]()
		{
			updateSeachStyle();
		});

	// 创建托盘对象
	SysTray* systray = new SysTray(this);

}

void CCMainWindow::initTimer()
{
	QTimer* timer = new QTimer(this);
	timer->setInterval(500);				// 500毫秒

	// lambda表达式，发射时间信号，进行升级
	connect(timer, &QTimer::timeout, [this] {
		static int level = 0;
		if (level == 99)
		{
			level = 0;
		}
		level++;
		setLevelPixmap(level);
		});

	timer->start();			// 启动计时器
}
//设置用户名
void CCMainWindow::setUserName(const QString& username)
{
	ui.nameLabel->adjustSize();		// 让标签，根据内容调整自己的尺寸

	// 文本过长，则进行缩略处理 ...
	// 对象调用 fontMetrics() 方法，返回的是 QFontMetrics字体体积的类对象
	// 再调用 "elidedText() 省略文本方法"，超过宽度（像素的宽度）就会以“...”缩略进行代替
	// 第一个参数，文本。	第二个参数，省略模式
	// 三参数，宽度。 四参数int flags = 0，是标志
	QString name = ui.nameLabel->fontMetrics().elidedText(username, Qt::ElideRight, ui.nameLabel->width());

	ui.nameLabel->setText(name);			// 设置用户名

}

void CCMainWindow::setLevelPixmap(int level)
{
	// 等级位图
	// 大小尺寸，调用 头像部件的大小
	QPixmap levelPixmap(ui.levelBtn->size());
	levelPixmap.fill(Qt::transparent);

	// 设置一个画家，在 levelPixmap 上进行绘制
	QPainter painter(&levelPixmap);
	painter.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));

	int unitNum = level % 10;		// 等级 个位数 ， 1-9级
	int tenNum = level / 10;			// 等级 两位数 ， 10-99级

	// 十位 ，等级数字图片，截取图片中的部分进行绘制
	// 只绘制其中一部分，不需要把图片全部绘制
	// 10,4 是绘图的起始位置
	// 第三个参数，是图片文件
	// 第4,5的参数，是sx，sy，图片左上角的位置，指这个位图的 高度
	// 第6,7的参数，是sw，sh，指这个位图的 宽度
	// 说白了，就是指定要拷贝整个图片的哪一部分
	painter.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"),
		tenNum * 6, 0, 6, 7);

	// 个位
	painter.drawPixmap(16, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"),
		unitNum * 6, 0, 6, 7);

	// 给头像部件，设置图标
	ui.levelBtn->setIcon(levelPixmap);
	// 设置大小
	ui.levelBtn->setIconSize(ui.levelBtn->size());
}

void CCMainWindow::setHeadPixmap(const QString& headPath)
{
	QPixmap pix;
	pix.load(":/Resources/MainWindow/head_mask.png");
	// 设置标签的头像
	ui.headLbael->setPixmap(getRoundImage(QPixmap(headPath),
		pix, ui.headLbael->size()));
}

void CCMainWindow::setStatusMenuIcon(const QString& statusPath)
{
	// 状态位图，设置大小
	QPixmap statusBtnPixmap(ui.stausBtn->size());
	statusBtnPixmap.fill(Qt::transparent);			// 透明

	QPainter painter(&statusBtnPixmap);
	painter.drawPixmap(4, 4, QPixmap(statusPath));

	// 将图标传进去
	ui.stausBtn->setIcon(statusBtnPixmap);
	ui.stausBtn->setIconSize(ui.stausBtn->size());

}

QWidget* CCMainWindow::addOtherAppExtension(const QString& appPath, const QString& appName)
{
	QPushButton* btn = new QPushButton(this);	// 当前对象作为父类
	btn->setFixedSize(20, 20);

	QPixmap pixmap(btn->size());
	pixmap.fill(Qt::transparent);

	QPainter painter(&pixmap);
	QPixmap appPixmap(appPath);
	painter.drawPixmap((btn->width() - appPixmap.width()) / 2,
		(btn->height() - appPixmap.height()) / 2, appPixmap);
	btn->setIcon(pixmap);
	btn->setIconSize(btn->size());
	btn->setObjectName(appName);			// 设置对象名
	btn->setProperty("hasborder", true);		// 让部件有边框

	// 更换皮肤
	connect(btn, &QPushButton::clicked, this, &CCMainWindow::onAppIconClicked);

	return btn;
}

void CCMainWindow::initContacTree()
{
	// 展开与收缩时的信号
	// (QTreeWidgetItem*, int) ，点击的是 哪一项，哪一列
	// 点击
	connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
		this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
	// 点击，展开
	connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)),
		this, SLOT(onItemExpanded(QTreeWidgetItem*)));
	// 点击，收缩
	connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
		this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
	// 双击
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
		this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	// 根节点
	// pRootGroupItem，保存 分组的根节点 ，群组
	QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;
	// 不管树里有没有子项，都永远进行展示
	pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	// 设置根节点数据（第0列，使用角色，值）
	pRootGroupItem->setData(0, Qt::UserRole, 0);		// 根项数据的值，为0

	RootContactItem* pItemName = new RootContactItem(true, ui.treeWidget);

	// 获取公司部门ID
	// SELECT departmentID FROM tab_department WHERE department_name = 'ren1'
	QSqlQuery queryCompDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(QString::fromLocal8Bit("公司群")));
	queryCompDepID.exec();			// 执行
	queryCompDepID.first();				// 指向结果集的第一条
	int ComDepID = queryCompDepID.value(0).toInt();			// 转换成int

	// 获取QQ登陆者所在的部门ID（部门群号）
	// 这里的传值，应该是传ID员工号
	// 在用户登陆的时候，登陆成功后，保存登陆者的账号，传到这里来
	// 用一个 全局变量 保存
	// SELECT departmentID FROM tab_employees WHERE employeeID = 10001
	QSqlQuery querySelfDepID(QString("SELECT departmentID FROM tab_employees WHERE employeeID = %1").arg(gLoginEmployeeID));
	querySelfDepID.exec();			// 执行
	querySelfDepID.first();				// 指向结果集的第一条
	int SelfDepID = querySelfDepID.value(0).toInt();			// 转换成int

	// 初始化公司群，和 登陆者 所在的群
	addCompanyDeps(pRootGroupItem, ComDepID);		// 传入公司群ID
	addCompanyDeps(pRootGroupItem, SelfDepID);			// 传入自己所在群的ID

	QString strGroupName = QString::fromLocal8Bit("联盟科技");
	pItemName->setText(strGroupName);

	// 插入分组节点
	// 添加顶级项
	ui.treeWidget->addTopLevelItem(pRootGroupItem);
	// 添加 项的部件，根节点是pRootGroupItem，第0列
	// 第三个参数，设置的部件
	ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItemName);

}

void CCMainWindow::addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID)
{
	QTreeWidgetItem* pChild = new QTreeWidgetItem;

	QPixmap pix;
	pix.load(":/Resources/MainWindow/head_mask.png");

	// 添加子节点，就是 联系项
	// 根节点的数据值为0，这里子项就设置为1，按次序排
	pChild->setData(0, Qt::UserRole, 1);

	// 将聊天窗口，设置唯一的识别ID
	// 数据用他自己，把 指针 转换成 int类型
	// 这样就给 Qt::UserRole + 1 设置了数据
	// 已经拿到群号了，直接传进来，用群号标注，
	// 不用之前的那种方法了，之前是 用窗口地址进行标注
	pChild->setData(0, Qt::UserRole + 1, DepID);

	// 获取所在公司的部门头像
	QPixmap groupPix;
	// SELECT picture FROM tab_department WHERE departmentID = 2001
	QSqlQuery queryPicture(QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(DepID));
	if (!queryPicture.exec()) {
		qDebug() << "335行出错!" << "DepID:" << DepID;
	}
	if (!queryPicture.next()) {			// 也可以用 queryPicture.next();
		qDebug() << "338行出错!" << "DepID:" << DepID;
	}
	qDebug() << "获取所在公司的部门头像的路径" << queryPicture.value(0).toString();
	groupPix.load(queryPicture.value(0).toString());

	// 获取部门名称
	QString strDepName;
	// SELECT department_name FROM tab_department WHERE departmentID = 2001
	QSqlQuery queryDepName(QString("SELECT department_name FROM tab_department WHERE departmentID = %1").arg(DepID));
	queryDepName.exec();
	queryDepName.first();
	strDepName = queryDepName.value(0).toString();



	// 父类，就是这个树
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);
	pContactItem->setHeadPixmap(getRoundImage(groupPix, pix, pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(strDepName);

	// 再对根项添加子节点
	pRootGroupItem->addChild(pChild);
	// 再对这个树，设置项的部件
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	// 把子节点，添加进 m_groupMap 中
	// 括号里是（公司群和人事部啥的项 ， 部门名字）
	// 连接数据库之后，这个映射就不需要了
	//m_groupMap.insert(pChild, sDeps);	

}

void CCMainWindow::resizeEvent(QResizeEvent* event)
{
	// 从本地编码，转变成Unicode
	setUserName(QString::fromLocal8Bit("Qt科技"));
	BasicWindow::resizeEvent(event);

}
//过滤器事件
bool CCMainWindow::eventFilter(QObject* obj, QEvent* event)
{
	// 判断 监视事件的对象，是不是搜索部件
	if (ui.searchLineEdit == obj)
	{
		// 判断 事件的类型
		// event 调用 type() 方法，返回当前事件的类型 
		// 是否为 键盘获取焦点事件
		if (event->type() == QEvent::FocusIn)
		{
			// 给 搜索栏部件，设置样式
			// 背景为白色
			// hover 鼠标悬浮，pressed 按下效果
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
			// 鼠标移开，焦点不在搜索框时，要还原
			updateSeachStyle();
		}
	}


	return false;
}

void CCMainWindow::mousePressEvent(QMouseEvent* event)
{
	// 输入框，在不需要焦点的时候，进行焦点的清除
	// 点击的不是搜索部件，同时搜索部件 还有焦点，这就不合理
	// 因此将焦点 清除
	if (qApp->widgetAt(event->pos()) != ui.searchLineEdit && ui.searchLineEdit->hasFocus())
	{
		ui.searchLineEdit->clearFocus();
	}
	else if (qApp->widgetAt(event->pos()) != ui.lineEdit && ui.lineEdit->hasFocus())
	{
		ui.lineEdit->clearFocus();
	}

	// 其它的，进行基础的事件的处理
	BasicWindow::mousePressEvent(event);
}

void CCMainWindow::updateSeachStyle()
{
	// 进行还原
	ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgba(%1,%2,%3,50);border-bottom:1px solid rgba(%1,%2,%3,30)}\
																		QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/search_icon.png)}")
		.arg(m_colorBackGround.red())
		.arg(m_colorBackGround.green())
		.arg(m_colorBackGround.blue()));
}

void CCMainWindow::onItemClicked(QTreeWidgetItem* item, int column)
{
	// 判断，是否有子项
	// 判断数据，第0列，角色是 UserRole 的返回值 
	bool bIsChild = item->data(0, Qt::UserRole).toBool();

	// 如果子项不为空，说明有子项
	if (!bIsChild)
	{
		// 判断是否展开
		// 如果 没有展开，则展开子项
		item->setExpanded(!item->isExpanded());
	}

}

void CCMainWindow::onItemExpanded(QTreeWidgetItem* item)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (!bIsChild)
	{
		// 先把 QWidget指针，转换成 RootContactItem类型的指针
		// dynamic_cast 将基类对象指针（或引用）转换到继承类指针
		RootContactItem* prootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));

		// 判断一下，进行角度转换
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
		// 先把 QWidget指针，转换成 RootContactItem类型的指针
		// dynamic_cast 将基类对象指针（或引用）转换到继承类指针
		RootContactItem* prootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));

		// 判断一下，进行角度转换
		if (prootItem)
		{
			prootItem->setExpanded(false);
		}

	}
}

void CCMainWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	// 判断双击的是根项，还是子项
	// data（0列，角色），再转换成 bool 类型
	bool bIsChild = item->data(0, Qt::UserRole).toBool();

	// 判断 是否为有效值
	if (bIsChild)
	{
		// 直接添加，新的聊天窗口
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());


		// 再判断 ID，打开的是群聊，还是私聊
		// 连接数据库之后，这个映射就不需要了
		// QString strGroup = m_groupMap.value(item);

		
		/*if (strGroup == QString::fromLocal8Bit("公司群"))
		{
			// 添加 新的聊天窗口
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(),COMPANY);
		}
		else if(strGroup == QString::fromLocal8Bit("人事部"))
		{
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), PERSONELGROUP);
		}
		else if(strGroup == QString::fromLocal8Bit("市场部"))
		{
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), MARKETGROUP);
		}
		else if(strGroup == QString::fromLocal8Bit("研发部"))
		{
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), DEVELOPMENTGROUP);
		}*/
		
	}
}

QString CCMainWindow::getHeadPicPath()
{
	QString strPicPath;

	// 判断
	if (!m_isAccountLogin)			// QQ号登录
	{
		QSqlQuery queryPic(QString("SELECT picture FROM tab_employees WHERE employeeID = %1")
			.arg(gLoginEmployeeID));
		queryPic.exec();
		queryPic.next();			// 指向结果集的第一条记录

		strPicPath = queryPic.value(0).toString();
	}
	else		// 账号登陆
	{
		// 再获取账号之前，先拿到 account账号
		// 然后 再通过 account 去获得 employeeID
		QSqlQuery queryEmployeeID(QString("SELECT employeeID FROM tab_accounts WHERE account = '%1'")
			.arg(m_account));
		queryEmployeeID.exec();
		queryEmployeeID.next();
		int employeeID = queryEmployeeID.value(0).toInt();

		// 再拿 employeeID 获取头像路径
		QSqlQuery queryPic(QString("SELECT picture FROM tab_employees WHERE employeeID = %1")
			.arg(employeeID));
		queryPic.exec();
		queryPic.next();			// 指向结果集的第一条记录

		strPicPath = queryPic.value(0).toString();
	}

	gstrLoginHeadPath = strPicPath;
	return strPicPath;
}

void CCMainWindow::onAppIconClicked()
{
	// 如果信号发送者的对象名，是 app_skin
	if (sender()->objectName() == "app_skin")
	{
		SkinWindow* skinWindow = new SkinWindow;
		skinWindow->show();
	}
}


