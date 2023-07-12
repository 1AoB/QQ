#include "basicWindow.h"
#include "NotifyManager.h"

#include <QFile>
#include <QStyleOption>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <qdesktopwidget.h>
#include <QSqlQuery>
#include "CommonUtils.h"



// 外部全局变量，登陆者ID
extern QString gLoginEmployeeID;

//构造函数
BasicWindow::BasicWindow(QWidget* parent)
	: QDialog(parent)
{
	// 获取默认的颜色值  CommonUtils这个类用来获取背景色(默认的颜色值)
	m_colorBackGround = CommonUtils::getDefaultSkinColor();

	// 先设置窗口风格
	setWindowFlags(Qt::FramelessWindowHint);			// 无边框
	setAttribute(Qt::WA_TranslucentBackground, true);	// 透明效果

	// 发射更改皮肤信号，修改皮肤   NotifyManager通知管理类
	connect(NotifyManager::getInstance(),
		SIGNAL(signalSkinChanged(const QColor&)),
		this,
		SLOT(onSignalSkinChange(const QColor&)));//槽方法:皮肤改变
}

BasicWindow::~BasicWindow()//析构函数
{
}
// 加载样式表
void BasicWindow::loadStyleSheet(const QString& sheetName)
{
	//m_styleName样式文件名称
	m_styleName = sheetName;
	//文件路径
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	//打开文件，只读
	file.open(QFile::ReadOnly);

	//如果文件被打开
	if (file.isOpen())
	{
		//先把样式表设置为空
		setStyleSheet("");
		// 数据内容全部读取出来, 保存到 qstyleSheet字符串里
		QString qsstyleSheet = QLatin1String(file.readAll());			

		// 获取当前用户的皮肤RGB值
		QString r = QString::number(m_colorBackGround.red());
		QString g = QString::number(m_colorBackGround.green());
		QString b = QString::number(m_colorBackGround.blue());

		//再加一些样式
		/* 
		 titleskin 标题皮肤属性为 true
		 底部的皮肤 bottomskin 为 true
		*/
		// rgba 的 a ,是透明度
		qsstyleSheet += QString("QWidget[titleskin=true]\
												{background-color:rgb(%1,%2,%3);\
												 border-top-left-radius:4px;}\
								QWidget[bottomskin=true]\
												{border-top:1px solid rgba(%1,%2,%3,100);\
												 background-color:rgba(%1,%2,%3,50);\
												 border-bottom-left-radius:4px;\
												 border-bottom-right-radius:4px;}")
			.arg(r).arg(g).arg(b);
		// 设置样式表
		setStyleSheet(qsstyleSheet);
	}
	//最后记得关闭文件
	file.close();
}

//头像转圆头像（一般都是方的）
// src , 图片来源  ,   mask , 背景, 一个空的圆头像  ,   maskSize, 圆头像的尺寸大小
QPixmap BasicWindow::getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize)
{
	if (maskSize == QSize(0, 0))
	{
		maskSize = mask.size();//如果圆头像的尺寸大小为0，那我们就用这个给定的空的圆头像的大小作为 圆头像的尺寸大小
	}
	else//圆头像的尺寸大小不能为0
	{
		//scaled这个方法可以理解为：“让图片自适应”
		// Qt::KeepAspectRatio , 缩放时, 尽可能以大的矩形缩放
		// Qt::SmoothTransformation , 平滑方式
		mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	// 保存转换后的头像
	// QImage::Format_ARGB32_Premultiplied 格式, 获取的头像会比较清晰
	QImage resultImage(maskSize, QImage::Format_ARGB32_Premultiplied);

	// CompositionMode , 图片的叠加模式
	QPainter painter(&resultImage);				// 定义一个画家, 画 resultImage
	painter.setCompositionMode(QPainter::CompositionMode_Source);		// 设置图片叠加模式, 将源文件以 复制进行操作 ， Source 将原图片，直接拷贝过来
	painter.fillRect(resultImage.rect(), Qt::transparent);				// 填充矩形, 矩形的大小, 就是图片的大小 , 同时是透明的 transparent
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);	// 用SourceOver方式, 进行叠加
	painter.drawPixmap(0, 0, mask);										// 对空白的圆形图片 , 进行叠加
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);		// 用SourceIn方式, 进行叠加
	painter.drawPixmap(0, 0, src.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));		// 在对目标的QQ头像, 进行比例设置 
	painter.end();

	return QPixmap::fromImage(resultImage);
}

//背景图
//初始化背景颜色  可以将此类作为基类，然后再派生类中继承，来保存一致
void BasicWindow::initBackGroundColor()
{
	//风格选项
	QStyleOption opt;
	opt.init(this);//获取当前部件的风格

	QPainter p(this);						// 用this是 "当前"部件、窗体对象
	this->style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);// 图源, 风格
}

// 子类化部件时, 需要重写绘图事件 设置背景图
void BasicWindow::paintEvent(QPaintEvent* event)
{
	initBackGroundColor();
	QDialog::paintEvent(event);
}
//鼠标按下事件
void BasicWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)//按下的是鼠标左键
	{
		// pos() , 事件发生时, 相对于窗口左上角的 偏移坐标
		m_mousePressed = true;// 鼠标是否按下
		m_mousePoint = event->globalPos() - pos();//当前坐标-之前坐标
		event->accept();			// 接受当前操作
	}
}
//鼠标移动事件，让窗口进行移动
void BasicWindow::mouseMoveEvent(QMouseEvent* event)
{
	//鼠标按下            (event->buttons() && Qt::LeftButton)按下的是左键
	if (m_mousePressed && (event->buttons() && Qt::LeftButton))
	{
		// globalPos() , 获取当前事件发生时, 鼠标全局的坐标值   global全局
		move(event->globalPos() - m_mousePoint); //当前坐标-之前坐标
		event->accept();//对当前的操作是接受的
	}
}
//鼠标释放事件
void BasicWindow::mouseReleaseEvent(QMouseEvent* event)
{
	m_mousePressed = false;// 鼠标是否按下
}

//关闭窗口
void BasicWindow::onShowClose(bool)
{
	close();
}
//窗口最小化
void BasicWindow::onShowMin(bool)
{
	showMinimized();
}
//隐藏窗口
void BasicWindow::onShowHide(bool)
{
	hide();
}
//窗口正常显示
void BasicWindow::onShowNormal(bool)
{
	show();									// 直接显示
	activateWindow();				// 设置为活动的窗口
}
//退出窗口（可以理解为退出登录）
void BasicWindow::onShowQuit(bool)
{
	// 更新登陆状态为"离线"
	QString strSqlStatus = QString("UPDATE tab_employees SET online = 1 WHERE employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery queryStatus(strSqlStatus);
	queryStatus.exec();

	QApplication::quit();
}

// 皮肤改变
void BasicWindow::onSignalSkinChange(const QColor& color)
{
	// 修改颜色 , 加载样式表, 重新添加
	m_colorBackGround = color;
	loadStyleSheet(m_styleName);
}
//最小化按钮被点击的槽函数
void BasicWindow::onButtonMinClicked()
{
	// 判断当前窗体,是不是工具 , 
	// 如果是的话 , 就表示 当前窗体 有工具风格 , 就进行 隐藏
	// 如果不是 ,就 进行最小化
	if (Qt::Tool == (windowFlags() & Qt::Tool))//工具风格
	{
		hide();//隐藏工具风格
	}
	else
	{
		showMinimized();//最小化操作
	}
}
//最大化还原按钮被点击的槽函数
void BasicWindow::onButtonRestoreClicked()
{
	QPoint windowPos;//坐标
	QSize windowSize;//大小

	// 获取窗体的位置 , 大小信息
	m_titleBar->getRestoreInfo(windowPos, windowSize);//直接引用传值
	//获取了再进行设置
	this->setGeometry(QRect(windowPos, windowSize));
}

//最大化按钮被点击
void BasicWindow::onButtonMaxClicked()
{
	// 先保存窗体之前的 位置 , 大小高度,宽度    （引用传值）
	m_titleBar->saveRestoreInfo(pos(), QSize(width(), height()));

	//获取左边这个矩形
	// desktopRect , 桌面矩形                    availableGeometry可用的几何信息
	QRect desktopRect = QApplication::desktop()->availableGeometry(); // 获取桌面的位置信息

	//对矩形进行修改
	// factRect , 实际矩形
	QRect factRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3,
		desktopRect.width() + 6, desktopRect.height() + 6);
	// 设置矩形属性
	this->setGeometry(factRect);
}
//关闭按钮被点击
void BasicWindow::onButtonCloseClicked()
{
	close();
}

//初始化标题栏
void BasicWindow::initTitleBar(ButtonType buttonType)
{
	m_titleBar = new TitleBar(this);
	m_titleBar->setButtonType(buttonType);//按钮类型
	m_titleBar->move(0, 0);//移动到左上角的位置

	//触发信号与槽的连接
	//最小化按钮点击信号      signalButtonMinClicked()是官方内置的方法
	connect(m_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
	//还原按钮点击信号
	connect(m_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonMinClicked()));
	//最大化按钮点击信号
	connect(m_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMinClicked()));
	//关闭按钮点击信号
	connect(m_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonMinClicked()));
}

//设置图标的标题
void BasicWindow::setTitleBarTitle(const QString& title, const QString& icon)
{
	//调用TitleBar类的方法
	m_titleBar->setTitleIcon(icon);
	m_titleBar->setTitleContent(title);
}
