#include "CustomMenu.h"
#include "CommonUtils.h"

CustomMenu::CustomMenu(QWidget* parent)
	: QMenu(parent)
{
	setAttribute(Qt::WA_TranslucentBackground);		// ����͸��
	CommonUtils::loadStyleSheet(this, "Menu");			// ������ʽ��

}

CustomMenu::~CustomMenu()
{
}

void CustomMenu::addCustomMenu(const QString& text, const QString& icon, const QString& Name)
{
	// �ȴ���һ��������ť
	// ͨ�� ͼ�� �� �ı� ����ʽ�����
	QAction* pAction = addAction(QIcon(icon), Name);
	m_menuActionMap.insert(text, pAction);




}

QAction* CustomMenu::getAction(const QString& text)
{
	return m_menuActionMap[text];
}
