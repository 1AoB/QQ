#pragma once

#include <QObject>
#include "TalkWindowShell.h"



class WindowManager : public QObject
{
	Q_OBJECT

public:
	WindowManager();
	~WindowManager();


public:
	// 根据传入的窗口名称，进行查找窗口
	QWidget* findWindowName(const QString& qsWindowName);

	// 删除，就是关闭聊天窗口
	void deleteWindowName(const QString& qsWindowName);

	// 添加 窗口名，聊天窗口
	// 删除一个窗口之后，添加一个新的聊天窗口（传入窗口名字，传入一个聊天窗口）
	void addWindowName(const QString& qsWindowName, QWidget* qWidget);

	// 管理窗口，只有一个单例的，因此用 static静态的
	static WindowManager* getInstance();

	// 获取
	TalkWindowShell* getTalkWindowSheel();

	// 添加 新的窗口
	// uid，就是userRole，ID值
	// 分组，设置默认的为公司
	// 与单个人的聊天，设置默认为空
	void addNewTalkWindow(const QString& uid);/*, GroupType groupType = COMPANY, const QString& strPeople = "");*/

	// 获取正在创建的，聊天窗口ID m_strCreatingTalkID
	QString getCreatingTalkID();

private:
	// 设置一个变量，为了跟 TalkWindowShell 进行关联操作
	TalkWindowShell* m_talkwindowshell;

	// 管理资源的时候，也就是窗口ID，跟窗体的地址，要进行一一对应
	// 也就是聊天，联系人，双击项的时候，也就是 UserRole + 1 的值
	// ID 要与 最后发送信息的聊天窗口的地址，要 一一对应
	// 为了方便进行 添加窗口，删除关闭窗口
	QMap<QString, QWidget*> m_windowMap;

	// 正在创建的聊天窗口（QQ号）
	QString m_strCreatingTalkID = "";



};

