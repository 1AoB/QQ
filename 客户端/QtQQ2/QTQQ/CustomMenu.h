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
	// �Զ�����Ӳ˵�
	// text��������ť���� ��  icon���˵�ͼ��
	// Name ���˵��ı�
	void addCustomMenu(const QString& text, const QString& icon, const QString& Name);

	// ���ض�����ť
	QAction* getAction(const QString& text);		// ͨ��������ť�����ı��������Ҷ�����ť



private:
	// ӳ�� �˵�������ť �� ��ֵ��
	// QString��������ť�����ı�
	// QAction*��ֵ���� ������ť
	QMap<QString, QAction*> m_menuActionMap;


};
