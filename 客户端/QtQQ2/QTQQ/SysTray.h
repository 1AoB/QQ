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
	// ����ϵͳ����ͼ��
	// ActivationReason ����ԭ��
	void onIconActivateed(QSystemTrayIcon::ActivationReason reason);

private:
	void initSystemTray();				// ��ʼ��ϵͳ����
	void addSystrayMenu();			    // ���ϵͳ�����˵�

private:
	QWidget* m_parent;

};
