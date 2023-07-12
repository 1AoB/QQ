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
	// ���ݴ���Ĵ������ƣ����в��Ҵ���
	QWidget* findWindowName(const QString& qsWindowName);

	// ɾ�������ǹر����촰��
	void deleteWindowName(const QString& qsWindowName);

	// ��� �����������촰��
	// ɾ��һ������֮�����һ���µ����촰�ڣ����봰�����֣�����һ�����촰�ڣ�
	void addWindowName(const QString& qsWindowName, QWidget* qWidget);

	// �����ڣ�ֻ��һ�������ģ������ static��̬��
	static WindowManager* getInstance();

	// ��ȡ
	TalkWindowShell* getTalkWindowSheel();

	// ��� �µĴ���
	// uid������userRole��IDֵ
	// ���飬����Ĭ�ϵ�Ϊ��˾
	// �뵥���˵����죬����Ĭ��Ϊ��
	void addNewTalkWindow(const QString& uid);/*, GroupType groupType = COMPANY, const QString& strPeople = "");*/

	// ��ȡ���ڴ����ģ����촰��ID m_strCreatingTalkID
	QString getCreatingTalkID();

private:
	// ����һ��������Ϊ�˸� TalkWindowShell ���й�������
	TalkWindowShell* m_talkwindowshell;

	// ������Դ��ʱ��Ҳ���Ǵ���ID��������ĵ�ַ��Ҫ����һһ��Ӧ
	// Ҳ�������죬��ϵ�ˣ�˫�����ʱ��Ҳ���� UserRole + 1 ��ֵ
	// ID Ҫ�� �������Ϣ�����촰�ڵĵ�ַ��Ҫ һһ��Ӧ
	// Ϊ�˷������ ��Ӵ��ڣ�ɾ���رմ���
	QMap<QString, QWidget*> m_windowMap;

	// ���ڴ��������촰�ڣ�QQ�ţ�
	QString m_strCreatingTalkID = "";



};

