#pragma once

#include <QPixmap>
#include <QSize>
#include <QProxyStyle>


// 为了改变默认的部件风格   Custom自定义
class CustomProxyStyle : public QProxyStyle
{
public:
	//构造函数、初始化
	CustomProxyStyle(QObject* parent)
	{
		setParent(parent);
	}

	//重写                                                                                              
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
		QPainter* painter, const QWidget* widget = nullptr)const
	{
		//就是当你点击一个位置的时候，就会出现一个边框
		if (PE_FrameFocusRect == element)//这个元素如果是边框的获取的焦点
		{
			// 去掉Windows中部件默认的边框, 或 虚线框
			// 部件获取焦点时, 直接返回，不进行绘制
			return;
		}
		else
		{
			QProxyStyle::drawPrimitive(element, option, painter, widget);
		}
	}

public:

};



// =====================================
class CommonUtils
{
public:
	CommonUtils();//构造函数

public:
	// 获取图片资源   获取圆头像               资源     空头的圆头像   大小  
	static QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize = QSize(0, 0));

	// 加载皮肤  (加载样式表)                               样式表的名字
	static void loadStyleSheet(QWidget* widget, const QString& sheetName);

	// 设置默认的颜色
	static void setDefaultSkinColor(const QColor& color);

	// 获取颜色
	static QColor getDefaultSkinColor();

};

