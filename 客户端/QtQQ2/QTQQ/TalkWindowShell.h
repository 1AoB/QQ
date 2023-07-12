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

// TCP 端口
const int gtcpPort = 8888;
// UDP 端口
const int gudpPort = 6666;


class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = Q_NULLPTR);
	~TalkWindowShell();

public:
	// 添加新的聊天窗口
	void addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString& uid); //GroupType groupType
	
	// 设置当前聊天窗口
	void setCurrentWidget(QWidget* widget);

	// 1）因为下面的 映射，是 private私有的，所以在 public里面，定义一个映射
	//      直接把 返回下面的 m_talkwindowItemMap
	// 2）因为，数据不会发生改变，所以定义函数的结尾，加 const
	// 3）因为这个值没有初始值，所以前面也要加 const，就是“返回类型 加const”否则报错
	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap()const;

private:
	void initControl();			// 初始化控件
	void initTcpSocket();		// 初始化TCP
	void initUdpSocket();		// 初始化UDP

	void getEmployeeID(QStringList& employeesList);			// 获取所有员工QQ号
	bool creatJSFile(QStringList &employeeList);			// 创建JS文件

	// 处理接收到数据，在“网页”上追加信息
	// 参数（发送者的QQ号，信息类型，数据）
	void handleReceivedMsg(int senderEmployeeID, int msgType, QString strMsg);

public slots:
	// 表情窗口，点击后执行的槽函数
	// 注意：这里点击的是表情窗口
	// 点击表情窗口之后，表情窗口的操作，窗口要隐藏
	void onEmotionBtnClicked(bool);

	// 更新，客户端tcp，要发送的tcp数据
	// 数据全放字符串里，不管什么信息
	// msgType，发送信息的类型
	// sFile，文件类信息比较特别，专门用来保存文件
	void updateSendTcpMsg(QString& strData, int& msgType, QString fileName = "");

private slots:
	// 左侧列表，点击后执行的槽函数
	// 选中的是哪个窗口，就更新哪一项
	void onTalkWindowItemClicked(QListWidgetItem* item);

	// 表情，选中的是哪一个（点击之后的信号）
	// 注意：这里点击的是，表情窗口 里面的表情
	// 触发的信号是，将表情，输入进 输入框中
	void onEmotionItemClicked(int emotionNum);

	// 处理UDP广播收到的数据
	void processPendingData();

private:
	// 打开的聊天窗口
	// 一个聊天项，对应一个部件
	QMap<QListWidgetItem*, QWidget*> m_talkwindowItemMap;

	// 表情窗口
	EmotionWindow* m_emotionWindow;

	Ui::TalkWindowClass ui;

private:
	QTcpSocket* m_tcpClientSocket;		// Tcp客户端
	QUdpSocket* m_udpRecevier;			// Udp接收端


};
