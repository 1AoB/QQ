#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>

enum ButtonType
{
	MIN_BUTTON = 0,				// ��С�� �� �رգ���ť
	MIN_MAX_BUTTON,				// ��С�� �� ��� �� �رգ���ť
	ONLY_CLOSE_BUTTON,			// ֻ�� �ر�, ��ť
};


// �Զ��������
class TitleBar : public QWidget
{
	Q_OBJECT

public:
	TitleBar(QWidget* parent = nullptr);//�Լ�������
	~TitleBar();

	void setTitleIcon(const QString& filePath);					// ���ñ�����ͼ��
	void setTitleContent(const QString& titleContent);			// ���ñ���������
	void setTitleWidth(int width);								// ���������
	void setButtonType(ButtonType buttonType);					// ��������ť����

	void saveRestoreInfo(const QPoint& point, const QSize& size);// ���洰�����ǰ,���ڵ�λ�� + ��С
	void getRestoreInfo(QPoint& point, QSize& size);			 // ��ȡ����� �������ǰ,���ڵ�λ�� + ��С



private:
	void paintEvent(QPaintEvent* event);				// ��ͼ�¼�
	void mouseDoubleClickEvent(QMouseEvent* event);		// ���˫���¼�
	void mousePressEvent(QMouseEvent* event);			// ��갴���¼�
	void mouseMoveEvent(QMouseEvent* event);			// ����ƶ��¼�
	void mouseReleaseEvent(QMouseEvent* event);			// ����ɿ��¼� , ���� ����ͷ��¼�

	void initControl();									// ��ʼ���ؼ�
	void initConnections();								// ��ʼ���ź���۵�����
	void loadStyleSheet(const QString& sheetName);		// ������ʽ��

	// �ź�
signals:
	// ��� ��������ť ʱ, ������ź�                   ���Ǳ������İ�ť
	void signalButtonMinClicked();						// ��С����ť
	void signalButtonRestoreClicked();					// ��󻯻�ԭ��ť
	void signalButtonMaxClicked();						// ��󻯰�ť
	void signalButtonCloseClicked();					// �رհ�ť


private slots:
	// ��ť��Ӧ�Ĳ�        
	void onButtonMinClicked();							// ��С����ť
	void onButtonRestoreClicked();						// ��󻯻�ԭ��ť
	void onButtonMaxClicked();							// ��󻯰�ť
	void onButtonCloseClicked();						// �رհ�ť


private:
	QLabel* m_pIcon;					// ������ͼ��      m:number   p:ָ��
	QLabel* m_pTitleContent;			// ����������
	QPushButton* m_pButtonMin;			// ��С����ť
	QPushButton* m_pButtonRestore;		// ��󻯻�ԭ��ť
	QPushButton* m_pButtonMax;			// ��󻯰�ť
	QPushButton* m_pButtonClose;		// �رհ�ť

	// ��󻯻�ԭ��ť����(���ڱ��洰��λ��+��С)
	QPoint m_restorePos;
	QSize m_restoreSize;

	// �ƶ����ڵı���
	bool m_isPressed;						// �Ƿ񱻰���
	QPoint m_startMovePos;					// ��ʼ�ƶ��ĵ�

	QString m_titleContent;					// ������������
	ButtonType m_buttonType;	     		// ������,���Ͻ�,��ť����
};
