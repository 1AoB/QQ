#pragma once

#include <QObject>

class NotifyManager : public QObject
{
	Q_OBJECT

public:
	NotifyManager();//Notify通知 Manager管理者
	~NotifyManager();

//信号
signals:
	// 改变颜色，信号
	void signalSkinChanged(const QColor& color);			

public:
	// 获取操作实例
	static NotifyManager* getInstance();			
	//通知其他的窗口 参数:要改变的颜色
	void notifyOtherWindowChangeSkin(const QColor& color);


private:
	//数据:操作的示例     instance 例子，实例
	static NotifyManager* instance;


};
