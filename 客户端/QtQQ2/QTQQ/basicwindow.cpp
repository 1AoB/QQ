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



// �ⲿȫ�ֱ�������½��ID
extern QString gLoginEmployeeID;

//���캯��
BasicWindow::BasicWindow(QWidget* parent)
	: QDialog(parent)
{
	// ��ȡĬ�ϵ���ɫֵ  CommonUtils�����������ȡ����ɫ(Ĭ�ϵ���ɫֵ)
	m_colorBackGround = CommonUtils::getDefaultSkinColor();

	// �����ô��ڷ��
	setWindowFlags(Qt::FramelessWindowHint);			// �ޱ߿�
	setAttribute(Qt::WA_TranslucentBackground, true);	// ͸��Ч��

	// �������Ƥ���źţ��޸�Ƥ��   NotifyManager֪ͨ������
	connect(NotifyManager::getInstance(),
		SIGNAL(signalSkinChanged(const QColor&)),
		this,
		SLOT(onSignalSkinChange(const QColor&)));//�۷���:Ƥ���ı�
}

BasicWindow::~BasicWindow()//��������
{
}
// ������ʽ��
void BasicWindow::loadStyleSheet(const QString& sheetName)
{
	//m_styleName��ʽ�ļ�����
	m_styleName = sheetName;
	//�ļ�·��
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	//���ļ���ֻ��
	file.open(QFile::ReadOnly);

	//����ļ�����
	if (file.isOpen())
	{
		//�Ȱ���ʽ������Ϊ��
		setStyleSheet("");
		// ��������ȫ����ȡ����, ���浽 qstyleSheet�ַ�����
		QString qsstyleSheet = QLatin1String(file.readAll());			

		// ��ȡ��ǰ�û���Ƥ��RGBֵ
		QString r = QString::number(m_colorBackGround.red());
		QString g = QString::number(m_colorBackGround.green());
		QString b = QString::number(m_colorBackGround.blue());

		//�ټ�һЩ��ʽ
		/* 
		 titleskin ����Ƥ������Ϊ true
		 �ײ���Ƥ�� bottomskin Ϊ true
		*/
		// rgba �� a ,��͸����
		qsstyleSheet += QString("QWidget[titleskin=true]\
												{background-color:rgb(%1,%2,%3);\
												 border-top-left-radius:4px;}\
								QWidget[bottomskin=true]\
												{border-top:1px solid rgba(%1,%2,%3,100);\
												 background-color:rgba(%1,%2,%3,50);\
												 border-bottom-left-radius:4px;\
												 border-bottom-right-radius:4px;}")
			.arg(r).arg(g).arg(b);
		// ������ʽ��
		setStyleSheet(qsstyleSheet);
	}
	//���ǵùر��ļ�
	file.close();
}

//ͷ��תԲͷ��һ�㶼�Ƿ��ģ�
// src , ͼƬ��Դ  ,   mask , ����, һ���յ�Բͷ��  ,   maskSize, Բͷ��ĳߴ��С
QPixmap BasicWindow::getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize)
{
	if (maskSize == QSize(0, 0))
	{
		maskSize = mask.size();//���Բͷ��ĳߴ��СΪ0�������Ǿ�����������Ŀյ�Բͷ��Ĵ�С��Ϊ Բͷ��ĳߴ��С
	}
	else//Բͷ��ĳߴ��С����Ϊ0
	{
		//scaled��������������Ϊ������ͼƬ����Ӧ��
		// Qt::KeepAspectRatio , ����ʱ, �������Դ�ľ�������
		// Qt::SmoothTransformation , ƽ����ʽ
		mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	// ����ת�����ͷ��
	// QImage::Format_ARGB32_Premultiplied ��ʽ, ��ȡ��ͷ���Ƚ�����
	QImage resultImage(maskSize, QImage::Format_ARGB32_Premultiplied);

	// CompositionMode , ͼƬ�ĵ���ģʽ
	QPainter painter(&resultImage);				// ����һ������, �� resultImage
	painter.setCompositionMode(QPainter::CompositionMode_Source);		// ����ͼƬ����ģʽ, ��Դ�ļ��� ���ƽ��в��� �� Source ��ԭͼƬ��ֱ�ӿ�������
	painter.fillRect(resultImage.rect(), Qt::transparent);				// ������, ���εĴ�С, ����ͼƬ�Ĵ�С , ͬʱ��͸���� transparent
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);	// ��SourceOver��ʽ, ���е���
	painter.drawPixmap(0, 0, mask);										// �Կհ׵�Բ��ͼƬ , ���е���
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);		// ��SourceIn��ʽ, ���е���
	painter.drawPixmap(0, 0, src.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));		// �ڶ�Ŀ���QQͷ��, ���б������� 
	painter.end();

	return QPixmap::fromImage(resultImage);
}

//����ͼ
//��ʼ��������ɫ  ���Խ�������Ϊ���࣬Ȼ�����������м̳У�������һ��
void BasicWindow::initBackGroundColor()
{
	//���ѡ��
	QStyleOption opt;
	opt.init(this);//��ȡ��ǰ�����ķ��

	QPainter p(this);						// ��this�� "��ǰ"�������������
	this->style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);// ͼԴ, ���
}

// ���໯����ʱ, ��Ҫ��д��ͼ�¼� ���ñ���ͼ
void BasicWindow::paintEvent(QPaintEvent* event)
{
	initBackGroundColor();
	QDialog::paintEvent(event);
}
//��갴���¼�
void BasicWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)//���µ���������
	{
		// pos() , �¼�����ʱ, ����ڴ������Ͻǵ� ƫ������
		m_mousePressed = true;// ����Ƿ���
		m_mousePoint = event->globalPos() - pos();//��ǰ����-֮ǰ����
		event->accept();			// ���ܵ�ǰ����
	}
}
//����ƶ��¼����ô��ڽ����ƶ�
void BasicWindow::mouseMoveEvent(QMouseEvent* event)
{
	//��갴��            (event->buttons() && Qt::LeftButton)���µ������
	if (m_mousePressed && (event->buttons() && Qt::LeftButton))
	{
		// globalPos() , ��ȡ��ǰ�¼�����ʱ, ���ȫ�ֵ�����ֵ   globalȫ��
		move(event->globalPos() - m_mousePoint); //��ǰ����-֮ǰ����
		event->accept();//�Ե�ǰ�Ĳ����ǽ��ܵ�
	}
}
//����ͷ��¼�
void BasicWindow::mouseReleaseEvent(QMouseEvent* event)
{
	m_mousePressed = false;// ����Ƿ���
}

//�رմ���
void BasicWindow::onShowClose(bool)
{
	close();
}
//������С��
void BasicWindow::onShowMin(bool)
{
	showMinimized();
}
//���ش���
void BasicWindow::onShowHide(bool)
{
	hide();
}
//����������ʾ
void BasicWindow::onShowNormal(bool)
{
	show();									// ֱ����ʾ
	activateWindow();				// ����Ϊ��Ĵ���
}
//�˳����ڣ��������Ϊ�˳���¼��
void BasicWindow::onShowQuit(bool)
{
	// ���µ�½״̬Ϊ"����"
	QString strSqlStatus = QString("UPDATE tab_employees SET online = 1 WHERE employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery queryStatus(strSqlStatus);
	queryStatus.exec();

	QApplication::quit();
}

// Ƥ���ı�
void BasicWindow::onSignalSkinChange(const QColor& color)
{
	// �޸���ɫ , ������ʽ��, �������
	m_colorBackGround = color;
	loadStyleSheet(m_styleName);
}
//��С����ť������Ĳۺ���
void BasicWindow::onButtonMinClicked()
{
	// �жϵ�ǰ����,�ǲ��ǹ��� , 
	// ����ǵĻ� , �ͱ�ʾ ��ǰ���� �й��߷�� , �ͽ��� ����
	// ������� ,�� ������С��
	if (Qt::Tool == (windowFlags() & Qt::Tool))//���߷��
	{
		hide();//���ع��߷��
	}
	else
	{
		showMinimized();//��С������
	}
}
//��󻯻�ԭ��ť������Ĳۺ���
void BasicWindow::onButtonRestoreClicked()
{
	QPoint windowPos;//����
	QSize windowSize;//��С

	// ��ȡ�����λ�� , ��С��Ϣ
	m_titleBar->getRestoreInfo(windowPos, windowSize);//ֱ�����ô�ֵ
	//��ȡ���ٽ�������
	this->setGeometry(QRect(windowPos, windowSize));
}

//��󻯰�ť�����
void BasicWindow::onButtonMaxClicked()
{
	// �ȱ��洰��֮ǰ�� λ�� , ��С�߶�,���    �����ô�ֵ��
	m_titleBar->saveRestoreInfo(pos(), QSize(width(), height()));

	//��ȡ����������
	// desktopRect , �������                    availableGeometry���õļ�����Ϣ
	QRect desktopRect = QApplication::desktop()->availableGeometry(); // ��ȡ�����λ����Ϣ

	//�Ծ��ν����޸�
	// factRect , ʵ�ʾ���
	QRect factRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3,
		desktopRect.width() + 6, desktopRect.height() + 6);
	// ���þ�������
	this->setGeometry(factRect);
}
//�رհ�ť�����
void BasicWindow::onButtonCloseClicked()
{
	close();
}

//��ʼ��������
void BasicWindow::initTitleBar(ButtonType buttonType)
{
	m_titleBar = new TitleBar(this);
	m_titleBar->setButtonType(buttonType);//��ť����
	m_titleBar->move(0, 0);//�ƶ������Ͻǵ�λ��

	//�����ź���۵�����
	//��С����ť����ź�      signalButtonMinClicked()�ǹٷ����õķ���
	connect(m_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
	//��ԭ��ť����ź�
	connect(m_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonMinClicked()));
	//��󻯰�ť����ź�
	connect(m_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMinClicked()));
	//�رհ�ť����ź�
	connect(m_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonMinClicked()));
}

//����ͼ��ı���
void BasicWindow::setTitleBarTitle(const QString& title, const QString& icon)
{
	//����TitleBar��ķ���
	m_titleBar->setTitleIcon(icon);
	m_titleBar->setTitleContent(title);
}
