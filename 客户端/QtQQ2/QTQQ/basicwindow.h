#pragma once
//本文件实现了一些窗口基本的操作、事件
#include <QDialog>
#include "TitleBar.h"

class BasicWindow : public QDialog
{
	Q_OBJECT

public:
	BasicWindow(QWidget* parent = nullptr);
	virtual ~BasicWindow();//虚函数

public:
	// 加载样式表
	void loadStyleSheet(const QString& sheetName);

	// 获取圆头像
	QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize = QSize(0, 0));


private:
	void initBackGroundColor();						// 初始化背景 , 颜色


protected:
	void paintEvent(QPaintEvent* event);			// 绘制事件
	void mousePressEvent(QMouseEvent* event);		// 鼠标按下事件
	void mouseMoveEvent(QMouseEvent* event);		// 鼠标移动事件
	void mouseReleaseEvent(QMouseEvent* event);		// 鼠标松开事件 , 就是 鼠标释放

protected:
	void initTitleBar(ButtonType buttonType = MIN_BUTTON);					// 初始化标题栏
	void setTitleBarTitle(const QString& title, const QString& icon = "");	// 设置标题栏内容


public slots://槽(方法):一般前缀是on就是槽方法
	void onShowClose(bool); //窗口关闭(没有离线)
	void onShowMin(bool);	//窗口最小化
	void onShowHide(bool);	//窗口隐藏
	void onShowNormal(bool);//窗口正常显示
	void onShowQuit(bool);	//窗口退出, 更新登陆状态为"离线"
	void onSignalSkinChange(const QColor& color);// 皮肤改变

	void onButtonMinClicked();		//最小化按钮
	void onButtonRestoreClicked();	//恢复按钮
	void onButtonMaxClicked();		//最大化按钮
	void onButtonCloseClicked();	//关闭按钮

protected:
	QPoint m_mousePoint;			// 鼠标位置
	bool m_mousePressed;			// 鼠标是否按下
	QColor m_colorBackGround;		// 背景色
	QString m_styleName;			// 样式文件名称
	TitleBar* m_titleBar;			// 标题栏
};
