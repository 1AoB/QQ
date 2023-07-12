#pragma once

#include <QMenu>
#include <QMap>

class CustomMenu : public QMenu
{
	Q_OBJECT

public:
	CustomMenu(QWidget* parent = nullptr);
	~CustomMenu();

public:
	// 自定义添加菜单
	// text，动作按钮名称 ，  icon，菜单图标
	// Name ，菜单文本
	void addCustomMenu(const QString& text, const QString& icon, const QString& Name);

	// 返回动作按钮
	QAction* getAction(const QString& text);		// 通过动作按钮名称文本，来查找动作按钮



private:
	// 映射 菜单动作按钮 ， 键值对
	// QString，动作按钮名称文本
	// QAction*，值就是 动作按钮
	QMap<QString, QAction*> m_menuActionMap;


};
