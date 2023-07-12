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

	// 在创建聊天窗口的时候，把值添加进映射里面，进行窗口资源管理
	// 否则，窗口可以无限增加，就是会存在多个“公司群”，这种情况
	WindowManager::getInstance()->addWindowName(m_talkId, this);

	setAttribute(Qt::WA_DeleteOnClose);


	// 在初始化控件之前，初始化状态，否则群聊状态永远都是false了
	initGroupTalkStatus();
	initControl();

}

TalkWindow::~TalkWindow()
{
	// 在 TalkWindow() 构造函数中，窗口名字是 uid
	// 所以 直接传入 m_talkId 就可以了
	WindowManager::getInstance()->deleteWindowName(m_talkId);
}


void TalkWindow::onFileOpenBtnClicked(bool)
{
	// 构造发送文件对话框的对象，直接显示
	SendFile* sendFile = new SendFile(this);
	sendFile->show();
}

void TalkWindow::addEmotionImage(int emotionNum)
{
	// 先获取焦点
	ui.textEdit->setFocus();
	// 调用，添加表情图片
	ui.textEdit->addEmotionUrl(emotionNum);
}

void TalkWindow::onSendBtnClicked(bool)
{
	// 发送信息不能为空，如果为空 给个提示
	// 转换成 群文本 toPlainText()
	if (ui.textEdit->toPlainText().isEmpty())
	{
		// 显示文本，showText(坐标，字符串文本，窗口，绘制矩形，显示2000毫秒)
		// 先给个坐标，是一个相对位置，要转成 全局坐标位置
		QToolTip::showText(
			this->mapToGlobal(QPoint(630, 660)),
			QString::fromLocal8Bit("发送的信息不能为空"),
			this,
			QRect(0, 0, 120, 100),
			2000);
		return;
	}

	// 将文本 转成 html类型
	//QString& html = ui.textEdit->document()->toHtml();
	QString& html = ui.textEdit->document()->toHtml();

	// 文本html，如果没有字体，则添加字体
	if (!html.contains(".png") && !html.contains("</span>"))
	{
		QString fontHtml;
		QString text = ui.textEdit->toPlainText();
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly))
		{
			fontHtml = file.readAll();
			// 将html文件里的 %1，用字符串 text 替换
			fontHtml.replace("%1", text);
			file.close();
		}
		else
		{
			// this，当前聊天部件，作为父窗体
			QMessageBox::information(this, QString::fromLocal8Bit("提示"),
				QString::fromLocal8Bit("文件 msgFont.txt 不存在！"));
			return;
		}

		// 判断转换后，有没有包含 fontHtml
		if (!html.contains(fontHtml))
		{
			html.replace(text, fontHtml);
		}
	}

	// 清空
	ui.textEdit->clear();
	ui.textEdit->deleteAllEmotionImage();

	// 收信息窗口，追加信息
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
	// 先给右边分裂器，设置尺寸
	QList<int> rightWidgetSize;
	rightWidgetSize << 600 << 138;

	// 参数是一个链表，
	// 如果是 左右的分裂器，会给每个部分设置 宽度
	// 如果是 上下的分裂器，会给每个部分设置 高度
	ui.bodySplitter->setSizes(rightWidgetSize);

	ui.textEdit->setFontPointSize(10);
	ui.textEdit->setFocus();						// 获取焦点，打开窗口时候，直接就可以输入，不需要先用鼠标点一下

	// 最小化，窗口右上角关闭，关闭（发送旁边的）
	connect(ui.sysmin, SIGNAL(clicked(bool)), parent(), SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.closeBtn, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));

	// 表情按钮被点击
	connect(ui.faceBtn, SIGNAL(clicked(bool)), parent(), SLOT(onEmotionBtnClicked(bool)));

	// 发送
	connect(ui.sendBtn, SIGNAL(clicked(bool)), this, SLOT(onSendBtnClicked(bool)));

	// 发送文件信号
	connect(ui.fileopenBtn, SIGNAL(clicked(bool)), this, SLOT(onFileOpenBtnClicked(bool)));

	// 树，被双击的时候
	// 要带参数，双击的是哪一项，和哪一项的地址
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
		this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	// 判断 是否为 群聊
	if (m_isGroupTalk)
	{
		initTalkWindow();		// 群聊
	}
	else
	{
		initPtoPTalk();				// 单聊
	}

}

void TalkWindow::initGroupTalkStatus()
{
	QSqlQueryModel sqlDepModel;
	// SELECT * FROM tab_department WHERE departmentID = 2001
	QString strSql = QString("SELECT * FROM tab_department WHERE departmentID = %1").arg(m_talkId);

	sqlDepModel.setQuery(strSql);

	int rows = sqlDepModel.rowCount();			// 返回总共的模型行号

	// ==0 为单聊
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
		.arg(QString::fromLocal8Bit("公司群")));
	queryDepID.exec();
	queryDepID.next();				// 调用 queryDepID.first(); 也行

	return queryDepID.value(0).toInt();
}



void TalkWindow::initTalkWindow()
{
	// 先构造根项
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();

	// 接下来，设置孩子，指示器的策略
	// 指示器设置的策略，不管这个子项有没有存在，
	// 这个指示器都会存在，都可以用于收缩
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	// 开始设置数据
	pRootItem->setData(0, Qt::UserRole, 0);		// 参数（第0列，用户，数据设置为0）

	// 构造根项
	// 第一个参数，是否需要箭头，不需要就写false
	// 第二个参数，让 树，作为他的父部件
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);					// 设置 Shell高度 减去 shell头高（talkwindow titleWidget）

	// 获取群的名称
	QString strGroupName;
	// 查询分组名称
	QSqlQuery queryGroupName(QString("SELECT department_name FROM tab_department WHERE departmentID = %1").arg(m_talkId));
	queryGroupName.exec();

	if (queryGroupName.next())
	{
		strGroupName = queryGroupName.value(0).toString();
	}

	/*
		如果不是公司群，就要进行筛选
		在员工表里面，想查一下人事部，有几个员工，
		只要进行departmentID等于人事部的ID，进行筛选

		如果是2000的时候，就要进行特殊处理
		那就将员工表里面的数据，全部拿过来，直接 select * 全部选择
		在根据 departmentID ，进行筛选
	*/
	// 先获取群号，再与 m_talkID 进行比对
	QSqlQueryModel queryEmployeeModel;

	// 如果是一样的，那就是公司群
	if (getCompDepID() == m_talkId.toInt())
	{
		// 查询，状态为1，
		queryEmployeeModel.setQuery("SELECT employeeID FROM tab_employees WHERE status = 1");
	}
	else
	{	// 如果不是公司群，通过公司群号，进行过滤
		queryEmployeeModel.setQuery(QString("SELECT employeeID FROM tab_employees WHERE status = 1 AND departmentID = %1")
			.arg(m_talkId));
	}

	// 获取到当前员工的数量
	int nEmployeeNum = queryEmployeeModel.rowCount();

	// 群聊名字，在线人数是多少，员工数量，
	QString qsGroupName = QString::fromLocal8Bit("%1 %2/%3")
		.arg(strGroupName).arg(0).arg(nEmployeeNum);
	//QMessageBox::information(this, "Tips", qsGroupName);
	// 将 qsGroupName 这个文本，设置为当前项 pItemName 的文本
	pItemName->setText(qsGroupName);

	//定时刷新数据////////////////////////刷新在线人数/////////////////////////
	m_timer = new QTimer(this);
	m_timer->setInterval(150);//150毫秒更新一次
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

		// 插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);			// 添加最上面的一项，就添加一项
	// 设置项目部件（对根项 pRootItem 设置，第0列，设置它的部件是 pItemName）
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	// 将所有的子项，默认展开
	pRootItem->setExpanded(true);

	// 添加子项
	for (int i = 0; i < nEmployeeNum; i++)
	{
		// 查询员工模型索引，获取员工QQ号
		QModelIndex modelindex = queryEmployeeModel.index(i, 0);
		int employeeID = queryEmployeeModel.data(modelindex).toInt();

		// 传入员工ID，这样每次添加的员工都是独一无二的
		addPeopInfo(pRootItem, employeeID);
	}
}

void TalkWindow::initPtoPTalk()
{
	// 单独私聊的时候，在右侧放一张图片
	QPixmap pixSkin;
	pixSkin.load(":/Resources/MainWindow/skin.png");

	ui.widget->setFixedSize(pixSkin.size());			// 设置尺寸

	// 给widget，设置图片
	QLabel* skinLabel = new QLabel(ui.widget);
	skinLabel->setPixmap(pixSkin);

	// 同时对标签，设置大小
	skinLabel->setFixedSize(ui.widget->size());

}

void TalkWindow::addPeopInfo(QTreeWidgetItem* pRootGroupItem, int employeeID)
{
	// 想要添加子项，就得先构造一个子项
	QTreeWidgetItem* pChild = new QTreeWidgetItem();

	// 添加子节点
	pChild->setData(0, Qt::UserRole, 1);							// 第0列，UserRole设置为1
	// 进行双击打开进行聊天的时候，要根据子项的地址作为参考的标识
	// 角色设置为 UserRole+1 ，数据的话 传进 pChild的地址
	// pChild 是个指针要进行转换，
	// QString::number((int)pChild)，先将指针转成 int类型，再将 int类型 转成 字符串
	// 以此来进行，设置值
	pChild->setData(0, Qt::UserRole + 1, employeeID);

	// 设置头像，要进行一些处理，圆头像等
	QPixmap pix1;
	pix1.load(":/Resources/MainWindow/head_mask.png");

	// 获取名字，签名，头像
	QString strName, strSign, strPicturePath;
	QSqlQueryModel queryInfoModel;
	
	queryInfoModel.setQuery(QString("SELECT employee_name,employee_sign,picture FROM tab_employees WHERE employeeID = %1")
		.arg(employeeID));

	QModelIndex nameIndex, signIndex, picIndex;
	nameIndex = queryInfoModel.index(0, 0);				// 0行，0列
	signIndex = queryInfoModel.index(0, 1);
	picIndex = queryInfoModel.index(0, 2);

	strName = queryInfoModel.data(nameIndex).toString();
	strSign = queryInfoModel.data(signIndex).toString();
	strPicturePath = queryInfoModel.data(picIndex).toString();

	QImage imageHead;
	imageHead.load(strPicturePath);

	// 联系项
	// 因为是在 树 中写功能，因此要以 树 作为父窗体
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);

	// 先设置圆头像
	// getRoundImage（传入原始图像，mask就是 pix1，尺寸）
	// 因为这个 圆头像，要放到 ContactItem中去，进行处理，而 ContactItem里面有 获取尺寸的接口
	// 因此要调用一波
	pContactItem->setHeadPixmap(CommonUtils::getRoundImage(QPixmap::fromImage(imageHead), pix1, pContactItem->getHeadLabelSize()));

	// 设置名字，签名
	pContactItem->setUserName(strName);
	pContactItem->setSignName(strSign);

	// 将当前的子项，添加到根项中
	pRootGroupItem->addChild(pChild);

	// 将子项，添加到 树 中
	// 设置项目部件（将谁设置进去，第0列，部件就是 联系项 pContactItem）
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	QString str = pContactItem->getUserName();		// 获取用户名
	m_groupPeoMap.insert(pChild, str);						// 进行映射，将子项和窗口名称，对应起来

}

void TalkWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	// 先获取数据，第0列，转换成 bool 类型
	bool bIsChild = item->data(0, Qt::UserRole).toBool();

	// 判断是否为子项
	if (bIsChild)
	{
		QString talkId = item->data(0, Qt::UserRole + 1).toString();
		if (talkId == gLoginEmployeeID) {
			QMessageBox::information(this, "Tips", "donot talk with yourself!");
			return;
		}
		// 获取窗口名字
		QString strPeopleName = m_groupPeoMap.value(item);

		/*
		 获取实例，添加新窗口（窗口ID，分组，人名）
		 第一个参数是 ID，不能写 已经有的当前窗口ID，要写 新传入的窗口ID
		 所以要通过 item调用data方法，再转成字符串
		 ( 不太明白，为什么不是用 Qt::UserRole ，而是用 Qt::UserRole+1 )

		 PTOP，才是人跟人私聊，不能传入当前群聊分组，就是默认的 m_groupType

		 映射中的 m_groupPeoMap 值，调用 value方法，获取人名
		 value() 方法，里面的参数 ，就是当前的 Key，就是当前 Item的项
		*/
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString()); //PTOP, strPeopleName

	}


}
