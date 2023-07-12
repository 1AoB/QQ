#include "NotifyManager.h"
#include "CommonUtils.h"

//开始的时候是空指针
NotifyManager* NotifyManager::instance = nullptr;

//基类
NotifyManager::NotifyManager()
	:QObject(nullptr)
{

}
//构造函数
NotifyManager::~NotifyManager()
{
}
//获得当前操作的实例
NotifyManager* NotifyManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new NotifyManager();
	}

	return instance;
}
//通过其他窗口进行皮肤颜色的改变
void NotifyManager::notifyOtherWindowChangeSkin(const QColor& color)
{
	//发送信号:皮肤颜色改变
	emit signalSkinChanged(color);
	// 设置默认颜色
	CommonUtils::setDefaultSkinColor(color);			
}
