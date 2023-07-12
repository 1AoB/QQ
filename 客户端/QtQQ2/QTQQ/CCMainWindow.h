#pragma once

#include "BasicWindow.h"
#include "ui_CCMainWindow.h"


class QTreeWidgetItem;

class CCMainWindow : public BasicWindow
{
	Q_OBJECT

public:
	// ���isAccountLogin���ص��� true��˵�� account ���˺ŵ�½��
	// �����false��˵�� account �� QQ��
	CCMainWindow(QString account, bool isAccountLogin, QWidget* parent = Q_NULLPTR);
	~CCMainWindow();

public:
	// ��ʼ���ؼ�
	void initControl();
	void initTimer();				// ��ʼ����ʱ��
	void setUserName(const QString& username);		// �����û���
	void setLevelPixmap(int level);								// ���õȼ�ͼƬ
	void setHeadPixmap(const QString& headPath);	// ����ͷ��·��
	void setStatusMenuIcon(const QString& statusPath);		// ����״̬

	// ���Ӧ�ò���(appͼƬ·����app����������)
	QWidget* addOtherAppExtension(const QString& appPath, const QString& appName);

	// ��ʼ����ϵ��
	void initContacTree();
	// �������벿��ID��
	void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID);

private:
	// �����ڸı�ʱ���û���ǰ��Ҫ����һ��
	// ͨ������¼�ʵ��
	void resizeEvent(QResizeEvent* event);

	// ��д �¼�������
	bool eventFilter(QObject* obj, QEvent* event);

	// ��д ��갴���¼�
	void mousePressEvent(QMouseEvent* event);

	// ������������ʽ
	void updateSeachStyle();

private slots:
	void onAppIconClicked();		//����Ƥ��

	void onItemClicked(QTreeWidgetItem* item, int column);			// ���������
	void onItemExpanded(QTreeWidgetItem* item);							// չ��
	void onItemCollapsed(QTreeWidgetItem* item);							// ����
	void onItemDoubleClicked(QTreeWidgetItem* item, int column);		// ˫��

	QString getHeadPicPath();		// ��ȡͷ��·��


private:
	Ui::CCMainWindowClass ui;

	// �п������˺ţ�����QQ��
	// isAccountLogin����true�������˺�
	// isAccountLogin����false������QQ��
	bool m_isAccountLogin;
	QString m_account;


	// ���з������
	// ÿһ�����촰�ڣ����Ӧ������
	// <��˾Ⱥ�����²�ɶ���� �� ֵ����ID>
	// �������ݿ�֮�����ӳ��Ͳ���Ҫ��
	// QMap<QTreeWidgetItem*, QString> m_groupMap;
};