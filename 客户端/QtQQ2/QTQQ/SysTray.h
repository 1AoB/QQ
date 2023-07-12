#pragma once

#include <QSystemTrayIcon>
#include <QWidget>

class SysTray : public QSystemTrayIcon
{
	Q_OBJECT

public:
	SysTray(QWidget* parent);
	~SysTray();

public slots:
	// 触发系统托盘图标
	// ActivationReason 触发原因
	void onIconActivateed(QSystemTrayIcon::ActivationReason reason);

private:
	void initSystemTray();				// 初始化系统托盘
	void addSystrayMenu();			    // 添加系统触发菜单

private:
	QWidget* m_parent;

};
