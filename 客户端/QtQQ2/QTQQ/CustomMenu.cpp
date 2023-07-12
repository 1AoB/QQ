#include "CustomMenu.h"
#include "CommonUtils.h"

CustomMenu::CustomMenu(QWidget* parent)
	: QMenu(parent)
{
	setAttribute(Qt::WA_TranslucentBackground);		// 设置透明
	CommonUtils::loadStyleSheet(this, "Menu");			// 加载样式表

}

CustomMenu::~CustomMenu()
{
}

void CustomMenu::addCustomMenu(const QString& text, const QString& icon, const QString& Name)
{
	// 先创建一个动作按钮
	// 通过 图标 和 文本 的形式，添加
	QAction* pAction = addAction(QIcon(icon), Name);
	m_menuActionMap.insert(text, pAction);




}

QAction* CustomMenu::getAction(const QString& text)
{
	return m_menuActionMap[text];
}
