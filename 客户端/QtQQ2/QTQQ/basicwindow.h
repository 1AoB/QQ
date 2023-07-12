#pragma once
//���ļ�ʵ����һЩ���ڻ����Ĳ������¼�
#include <QDialog>
#include "TitleBar.h"

class BasicWindow : public QDialog
{
	Q_OBJECT

public:
	BasicWindow(QWidget* parent = nullptr);
	virtual ~BasicWindow();//�麯��

public:
	// ������ʽ��
	void loadStyleSheet(const QString& sheetName);

	// ��ȡԲͷ��
	QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize = QSize(0, 0));


private:
	void initBackGroundColor();						// ��ʼ������ , ��ɫ


protected:
	void paintEvent(QPaintEvent* event);			// �����¼�
	void mousePressEvent(QMouseEvent* event);		// ��갴���¼�
	void mouseMoveEvent(QMouseEvent* event);		// ����ƶ��¼�
	void mouseReleaseEvent(QMouseEvent* event);		// ����ɿ��¼� , ���� ����ͷ�

protected:
	void initTitleBar(ButtonType buttonType = MIN_BUTTON);					// ��ʼ��������
	void setTitleBarTitle(const QString& title, const QString& icon = "");	// ���ñ���������


public slots://��(����):һ��ǰ׺��on���ǲ۷���
	void onShowClose(bool); //���ڹر�(û������)
	void onShowMin(bool);	//������С��
	void onShowHide(bool);	//��������
	void onShowNormal(bool);//����������ʾ
	void onShowQuit(bool);	//�����˳�, ���µ�½״̬Ϊ"����"
	void onSignalSkinChange(const QColor& color);// Ƥ���ı�

	void onButtonMinClicked();		//��С����ť
	void onButtonRestoreClicked();	//�ָ���ť
	void onButtonMaxClicked();		//��󻯰�ť
	void onButtonCloseClicked();	//�رհ�ť

protected:
	QPoint m_mousePoint;			// ���λ��
	bool m_mousePressed;			// ����Ƿ���
	QColor m_colorBackGround;		// ����ɫ
	QString m_styleName;			// ��ʽ�ļ�����
	TitleBar* m_titleBar;			// ������
};
