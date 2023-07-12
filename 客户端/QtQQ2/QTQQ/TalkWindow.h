#pragma once

#include <QWidget>
#include "TalkWindowShell.h"
#include "ui_TalkWindow.h"
#include <QTimer>

class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	TalkWindow(QWidget* parent, const QString& uid);/* GroupType groupType);*/
	~TalkWindow();

public:
	void addEmotionImage(int emotionNum);					// ��ӱ���
	void setWindowName(const QString& name);			// ���ô�������
	QString getTalkID();		// ��ȡ���촰�ڵ�ID


private:
	void initControl();						// ��ʼ��
	void initGroupTalkStatus();			// ��ʼ��Ⱥ��״̬
	int getCompDepID();				// ��ȡ��˾����ID��

	//void initCompanylTalk();			// ��ʼ�� ��˾Ⱥ����
	//void initPersonelTalk();			// ��ʼ�� ���²�����
	//void initDeveloplTalk();			// ��ʼ�� �з�������
	//void initMarketTalk();				// ��ʼ�� �г�������
	void initTalkWindow();			// ��ʼ�� Ⱥ��
	void initPtoPTalk();					// ��ʼ�� ����

	// ����һ����ӣ�����Ҫ��һ����ַ��ȥ
	void addPeopInfo(QTreeWidgetItem* pRootGroupItem, int employeeID);



private slots:


	void onFileOpenBtnClicked(bool);

	void onSendBtnClicked(bool);			// �����ź�
	void onItemDoubleClicked(QTreeWidgetItem* item, int column);		// ˫���¼�


private:

	QTimer* m_timer;	//��ʱˢ������

	Ui::TalkWindow ui;

	QString m_talkId;							// ���촰�ڵ�ID

	// ��־���Ƿ�ΪȺ��
	bool m_isGroupTalk;

	// Ⱥ������ �� Ⱥ��Ա��ӳ��
	// ������Ⱥ���棬�� ����1���촰�ڣ�ֻ�ܵõ�ֵ�����ǵò�����������
	// ��ˣ�Ҫ��һ��ӳ�䣬��ֵ�ʹ������֣�һһ��Ӧ����
	QMap<QTreeWidgetItem*, QString> m_groupPeoMap;

	// ���ó���Ԫ��
	// ���� TalkWindowShell �У��ſ��Է��� TalkWindow ��˽�г�Ա
	friend class TalkWindowShell;

};
