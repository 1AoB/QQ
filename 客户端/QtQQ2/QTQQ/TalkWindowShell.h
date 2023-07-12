#pragma once

#include "BasicWindow.h"
#include "EmotionWindow.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"

#include <QTcpSocket>
#include <QWidget>
#include <QMap>
#include "ui_TalkWindowShell.h"
#include <QUdpSocket>


class TalkWindow;
class TalkWindowItem;
class QListWidgetItem;
class EmotionWindow;

// TCP �˿�
const int gtcpPort = 8888;
// UDP �˿�
const int gudpPort = 6666;


class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = Q_NULLPTR);
	~TalkWindowShell();

public:
	// ����µ����촰��
	void addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString& uid); //GroupType groupType
	
	// ���õ�ǰ���촰��
	void setCurrentWidget(QWidget* widget);

	// 1����Ϊ����� ӳ�䣬�� private˽�еģ������� public���棬����һ��ӳ��
	//      ֱ�Ӱ� ��������� m_talkwindowItemMap
	// 2����Ϊ�����ݲ��ᷢ���ı䣬���Զ��庯���Ľ�β���� const
	// 3����Ϊ���ֵû�г�ʼֵ������ǰ��ҲҪ�� const�����ǡ��������� ��const�����򱨴�
	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap()const;

private:
	void initControl();			// ��ʼ���ؼ�
	void initTcpSocket();		// ��ʼ��TCP
	void initUdpSocket();		// ��ʼ��UDP

	void getEmployeeID(QStringList& employeesList);			// ��ȡ����Ա��QQ��
	bool creatJSFile(QStringList &employeeList);			// ����JS�ļ�

	// ������յ����ݣ��ڡ���ҳ����׷����Ϣ
	// �����������ߵ�QQ�ţ���Ϣ���ͣ����ݣ�
	void handleReceivedMsg(int senderEmployeeID, int msgType, QString strMsg);

public slots:
	// ���鴰�ڣ������ִ�еĲۺ���
	// ע�⣺���������Ǳ��鴰��
	// ������鴰��֮�󣬱��鴰�ڵĲ���������Ҫ����
	void onEmotionBtnClicked(bool);

	// ���£��ͻ���tcp��Ҫ���͵�tcp����
	// ����ȫ���ַ��������ʲô��Ϣ
	// msgType��������Ϣ������
	// sFile���ļ�����Ϣ�Ƚ��ر�ר�����������ļ�
	void updateSendTcpMsg(QString& strData, int& msgType, QString fileName = "");

private slots:
	// ����б������ִ�еĲۺ���
	// ѡ�е����ĸ����ڣ��͸�����һ��
	void onTalkWindowItemClicked(QListWidgetItem* item);

	// ���飬ѡ�е�����һ�������֮����źţ�
	// ע�⣺���������ǣ����鴰�� ����ı���
	// �������ź��ǣ������飬����� �������
	void onEmotionItemClicked(int emotionNum);

	// ����UDP�㲥�յ�������
	void processPendingData();

private:
	// �򿪵����촰��
	// һ���������Ӧһ������
	QMap<QListWidgetItem*, QWidget*> m_talkwindowItemMap;

	// ���鴰��
	EmotionWindow* m_emotionWindow;

	Ui::TalkWindowClass ui;

private:
	QTcpSocket* m_tcpClientSocket;		// Tcp�ͻ���
	QUdpSocket* m_udpRecevier;			// Udp���ն�


};
