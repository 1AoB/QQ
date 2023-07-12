#include "WindowManager.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"

#include <QSqlQueryModel>

// 单例模式，创建全局静态对象
Q_GLOBAL_STATIC(WindowManager, theInstance)


WindowManager::WindowManager()
	: QObject(nullptr)
	, m_talkwindowshell(nullptr)
{

}

WindowManager::~WindowManager()
{
}

QWidget* WindowManager::findWindowName(const QString& qsWindowName)
{
	// 根据传进来的窗口名称，来做判断
	// 映射中，如果包含 传进来的窗体
	if (m_windowMap.contains(qsWindowName))
	{
		// 如果包含，就返回 m_windowMap 的值
		return m_windowMap.value(qsWindowName);
	}

	return nullptr;			// 如果没有就返回空
}

void WindowManager::deleteWindowName(const QString& qsWindowName)
{
	// 从映射中，进行删除，就是关闭窗口
	// 根据窗口名称，进行删除
	m_windowMap.remove(qsWindowName);
}

void WindowManager::addWindowName(const QString& qsWindowName, QWidget* qWidget)
{
	// 在映射中，添加窗口名
	// 先判断，映射中有没有包含 传进来的窗口名
	// 没有的话，再进行添加
	if (!m_windowMap.contains(qsWindowName))
	{
		m_windowMap.insert(qsWindowName, qWidget);
	}
}

WindowManager* WindowManager::getInstance()
{
	// 返回当前实例
	return theInstance();
}

TalkWindowShell* WindowManager::getTalkWindowSheel()
{
	return m_talkwindowshell;
}

void WindowManager::addNewTalkWindow(const QString& uid)/*, GroupType groupType, const QString& strPeople*/
{
	// 判断，先保证 父窗口已经创建出来了，才能添加
	// 如果是空的，就得先构造 父窗口
	if (m_talkwindowshell == nullptr)
	{
		m_talkwindowshell = new TalkWindowShell;

		// 当 TalkWindowShell ，关闭，销毁的时候，
		// 对这个指针 m_talkwindowshell ，也进行一下销毁，
		connect(m_talkwindowshell, &TalkWindowShell::destroyed, [this](QObject* obj)
			{
				m_talkwindowshell = nullptr;
			});
	}

	// 判断，窗口的名字
	// 传进来的uid，
	// 如果是私聊的话，这个uid就是聊天对象的QQ号
	// 如果是群聊的话，就是 群号
	QWidget* widget = findWindowName(uid);

	// 开始做判断
	// 如果不存在聊天窗口，就构造一个聊天窗口
	if (!widget)
	{
		// 这是就是 正在构造中的聊天窗口，
		// 因此在这里给 m_strCreatingTalkID赋值，而且uid已经有了
		m_strCreatingTalkID = uid;

		TalkWindow* talkwindow = new TalkWindow(m_talkwindowshell, uid);/*, groupType*/
		TalkWindowItem* talkwindowItem = new TalkWindowItem(talkwindow);

		// 这里，聊天窗口使用完了，再还原成 空值
		m_strCreatingTalkID = "";

		// 判断是群聊，还是单聊
		QSqlQueryModel sqlDepModel;		// 用模型查询
		// 现在只有一个uid，去部门群里看看能不能搜到记录，能搜到就说明是群聊
		// SELECT department_name,sign FROM tab_departments WHERE departmentID = 2001
		QString strSql = QString("SELECT department_name,sign FROM tab_departments WHERE departmentID = %1").arg(uid);
		// 给查询模型，设置SQL语句
		sqlDepModel.setQuery(strSql);
		int rows = sqlDepModel.rowCount();

		// 窗口名，签名信息名
		QString strWindowName, strMsgLabel;

		// 判断行号
		if (rows == 0)			// 单聊
		{
			// 根据uid，QQ号，查询 员工名字，员工签名
			// SELECT employee_name,employee_sign FROM tab_employees WHERE employeeID = 10001
			QString sql = QString("SELECT employee_name,employee_sign FROM tab_employees WHERE employeeID = %1").arg(uid);
			sqlDepModel.setQuery(sql);		// 设置查询

		}

		// 拿数据，需要 模型索引
		// 部门索引，签名索引
		QModelIndex indexDepIndex, signIndex;
		indexDepIndex = sqlDepModel.index(0, 0);			// 第0行，第0列
		signIndex = sqlDepModel.index(0, 1);					// 第0行，第1列

		// 窗口名称
		strWindowName = sqlDepModel.data(signIndex).toString();
		// 签名
		strMsgLabel = sqlDepModel.data(indexDepIndex).toString();

		// 窗口名称
		talkwindow->setWindowName(strWindowName);
		// 左侧联系人显示
		talkwindowItem->setMsgLabelContent(strMsgLabel);
		m_talkwindowshell->addTalkWindow(talkwindow, talkwindowItem, uid);


		/*
		switch (groupType)
		{
		case COMPANY:
		{
			talkwindow->setWindowName(QStringLiteral("武当派"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("公司群"));
			break;
		}
		case PERSONELGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("少林派"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("人事部"));
			break;
		}
		case MARKETGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("锦衣卫"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("市场部"));
			break;
		}
		case DEVELOPMENTGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("君子堂"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("研发部"));
			break;
		}
		case PTOP:		// 一对一，私聊
		{
			// 暂时不设置标题名字，左侧标签就设置对方的名字
			talkwindow->setWindowName(QStringLiteral(""));
			talkwindowItem->setMsgLabelContent(strPeople);
			break;
		}
		default:
			break;
		}

		// 添加聊天窗口
		m_talkwindowshell->addTalkWindow(talkwindow, talkwindowItem, groupType);
		*/
	}
	else
	{
		// 对左边进行设置，左侧聊天列表设为被选中
		// 通过这个 widget ，返回链表项
		// 调用 key，获得值
		QListWidgetItem* item = m_talkwindowshell->getTalkWindowItemMap().key(widget);
		item->setSelected(true);

		// 对右边进行设置 当前聊天窗口
		m_talkwindowshell->setCurrentWidget(widget);
	}

	m_talkwindowshell->show();
	m_talkwindowshell->activateWindow();			// 设置为活动窗体
}

QString WindowManager::getCreatingTalkID()
{
	return m_strCreatingTalkID;
}


