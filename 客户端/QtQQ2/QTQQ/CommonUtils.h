#pragma once

#include <QPixmap>
#include <QSize>
#include <QProxyStyle>


// Ϊ�˸ı�Ĭ�ϵĲ������   Custom�Զ���
class CustomProxyStyle : public QProxyStyle
{
public:
	//���캯������ʼ��
	CustomProxyStyle(QObject* parent)
	{
		setParent(parent);
	}

	//��д                                                                                              
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
		QPainter* painter, const QWidget* widget = nullptr)const
	{
		//���ǵ�����һ��λ�õ�ʱ�򣬾ͻ����һ���߿�
		if (PE_FrameFocusRect == element)//���Ԫ������Ǳ߿�Ļ�ȡ�Ľ���
		{
			// ȥ��Windows�в���Ĭ�ϵı߿�, �� ���߿�
			// ������ȡ����ʱ, ֱ�ӷ��أ������л���
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
	CommonUtils();//���캯��

public:
	// ��ȡͼƬ��Դ   ��ȡԲͷ��               ��Դ     ��ͷ��Բͷ��   ��С  
	static QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize = QSize(0, 0));

	// ����Ƥ��  (������ʽ��)                               ��ʽ�������
	static void loadStyleSheet(QWidget* widget, const QString& sheetName);

	// ����Ĭ�ϵ���ɫ
	static void setDefaultSkinColor(const QColor& color);

	// ��ȡ��ɫ
	static QColor getDefaultSkinColor();

};

