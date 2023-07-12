#include "TitleBar.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QPainterPath>


#define BUTTON_HEIGHT	27			// ��ť�߶� 27����
#define BUTTON_WIDTH	27			// ��ť��� 27����
#define TITLE_HEIGHT	27			// �������߶� 27����

//���캯��
TitleBar::TitleBar(QWidget* parent)
	: QWidget(parent)
	, m_isPressed(false)				    // �Ƿ���
	, m_buttonType(MIN_MAX_BUTTON)			// ��ť����
{
	initControl();							//��ʼ�����ؼ�
	initConnections();						//��ʼ���ź����
	loadStyleSheet("Title");				//������ʽ��
}

//����
TitleBar::~TitleBar()
{
}

// ����, ������ͼ��
void TitleBar::setTitleIcon(const QString& filePath)//�����������һ��ͼ���·��
{
	QPixmap titleIcon(filePath);								//��������ͼ���λ��
	m_pIcon->setFixedSize(titleIcon.size());					//��titleIcon�Ĵ�С,������ͼ���С
	m_pIcon->setPixmap(titleIcon);								//����ͼ��
}

// ���ñ���������
void TitleBar::setTitleContent(const QString& titleContent)
{
	m_pTitleContent->setText(titleContent);						// �����ı�
	m_titleContent = titleContent;								// ���ݸ���
}

// ���ñ�����, ����
void TitleBar::setTitleWidth(int width)
{
	setFixedWidth(width);//�൱��this->setFixedWidth(width);
}

// ���ñ�����, ��ť����
void TitleBar::setButtonType(ButtonType buttonType)//�����ǰ�ť������
{
	m_buttonType = buttonType;			// ����

	//�ü��ְ�ť������
	switch (buttonType)
	{
	case MIN_BUTTON:// ��С�� �� �رգ���ť
	{
		m_pButtonRestore->setVisible(false);	// setVisible ,�Ƿ�ɼ�
		m_pButtonMax->setVisible(false);
	}
	break;
	case MIN_MAX_BUTTON:// ��С�� �� ��� �� �رգ���ť
	{
		m_pButtonRestore->setVisible(false);	//���ÿɼ�����
	}
	break;
	case ONLY_CLOSE_BUTTON:// ֻ�� �ر�, ��ť
	{
		m_pButtonRestore->setVisible(false);
		m_pButtonMax->setVisible(false);
		m_pButtonMin->setVisible(false);
	}
	break;
	default:
		break;
	}

}

//���洰�����ǰ���ڵ�λ�ü���С
void TitleBar::saveRestoreInfo(const QPoint& point, const QSize& size)
{
	m_restorePos = point;//λ��
	m_restoreSize = size;//��С
}
//��ȡ�������ǰ���ڵ�λ�ü���С
void TitleBar::getRestoreInfo(QPoint& point, QSize& size)
{
	point = m_restorePos;//λ��
	size = m_restoreSize;//��С
}
//���Ʊ�����
void TitleBar::paintEvent(QPaintEvent* event)
{
	// ���ñ���ɫ
	QPainter painter(this);
	QPainterPath pathBack;							//���Ʊ�������ͼ·��
	pathBack.setFillRule(Qt::WindingFill);					// ����������
	pathBack.addRoundedRect(QRect(0, 0, width(), height()), 3, 3);				// ���Բ�Ǿ���, ����ͼ·��
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);			// ƽ��������ӳ��ת��

	// ��������󻯻�ԭ��, ���ڳ��ȸı�, ��������Ӧ�����ı�
	// parentWidget() ,���� ������
	if (width() != parentWidget()->width())				// ������ parentWidget , ���ÿ��
	{
		setFixedWidth(parentWidget()->width());
	}

	// �ѱ������ػ��¼�����ӵ� Ĭ����
	QWidget::paintEvent(event);
}

// ˫��������, �������/��С������
void TitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	// ֻ�д������, ��С����ť , ����Ч
	if (m_buttonType == MIN_MAX_BUTTON)//�����С����ť
	{
		if (m_pButtonMax->isVisible())
		{
			onButtonMaxClicked();
		}
		else
		{
			onButtonRestoreClicked();
		}
	}
}
//ͨ����갴�¡�����ƶ�������ͷ��¼�ʵ�ֱ������ﵽ�ƶ�����Ч��
void TitleBar::mousePressEvent(QMouseEvent* event)
{
	if (m_buttonType == MIN_MAX_BUTTON)//���
	{
		// �ڴ������ʱ, ��ֹ�϶�����
		if (m_pButtonMax->isVisible())//�ǿɼ���
		{
			//�ƶ����ڱ�����
			m_isPressed = true;
			// globalPos()���� �¼�����ʱ,�����ȫ�ֵ�λ��
			m_startMovePos = event->globalPos();
		}
	}
	else//�������
	{
		//�ƶ����ڱ�����
		m_isPressed = true;
		m_startMovePos = event->globalPos();				// ���ͷ�ļ� <QMouseEvent>
	}
	// ��������֮��, ����Ĭ�ϵķ���, ����
	return QWidget::mousePressEvent(event);					
}
//����ƶ�
void TitleBar::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)//�Ƿ񱻰���
	{
		// ���ص�ǰ�����λ�� ,��ȥ ֮ǰ��λ��
		QPoint movePoint = event->globalPos() - m_startMovePos;		
		// ���ز�����λ��
		QPoint widgetPos = parentWidget()->pos();									
		//startλ�ý��и���
		m_startMovePos = event->globalPos();

		// ���ø��������ƶ��������� , �ƶ�ǰ���������
		//��ʼλ��+ƫ��λ��
		parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
	}
	// ��������֮��, ����Ĭ�ϵķ���, ����
	return QWidget::mouseMoveEvent(event);
}
//����ͷ�
void TitleBar::mouseReleaseEvent(QMouseEvent* event)
{
	//�ͷžͲ�������
	m_isPressed = false;
	// ��������֮��, ����Ĭ�ϵķ���, ����
	return QWidget::mouseReleaseEvent(event);
}

// ��ʼ���������ؼ�
void TitleBar::initControl()
{
	// �Ƚ���������ǩ�������   
	m_pIcon = new QLabel(this);					// ������ͼ��
	m_pTitleContent = new QLabel(this);			// ����������
	m_pButtonMin = new QPushButton(this);		// ��С����ť
	m_pButtonRestore = new QPushButton(this);	// ��󻯻�ԭ��ť
	m_pButtonMax = new QPushButton(this);		// ��󻯰�ť
	m_pButtonClose = new QPushButton(this);		// �رհ�ť

	// �԰�ť,���ù̶���С�����óߴ��С��
	m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));				// ��С������С���̶���С
	m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));			// ��󻯻�ԭ��ť
	m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));				// ���
	m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));			// �رհ�ť

	// ���ö����������ֶ�������������   ��setObjectName���ö�����
	m_pTitleContent->setObjectName("TitleContent");								// �������ݡ�����������
	m_pButtonMin->setObjectName("ButtonMin");
	m_pButtonRestore->setObjectName("ButtonRestore");
	m_pButtonMax->setObjectName("ButtonMax");
	m_pButtonClose->setObjectName("ButtonClose");

	// ���ò���
	QHBoxLayout* mylayout = new QHBoxLayout(this);//ˮƽ����
	// ��Ӹ��ְ�ť
	mylayout->addWidget(m_pIcon);				// ������ͼ��								
	mylayout->addWidget(m_pTitleContent);		// ����������
	mylayout->addWidget(m_pButtonMin);			// ��С����ť
	mylayout->addWidget(m_pButtonRestore);		// ��󻯻�ԭ��ť
	mylayout->addWidget(m_pButtonMax);			// ��󻯰�ť
	mylayout->addWidget(m_pButtonClose);		// �رհ�ť

	mylayout->setContentsMargins(5, 0, 0, 0);	// ���ò���, ��϶       �ĸ���������������
	mylayout->setSpacing(0);					// ���ò����ﲿ��֮��, ��϶

	// ����ĳߴ����
	// Expanding, ��ֱ���� �� Fixed , �̶�  �����߽�ϼ���ֱ����̶��������ı䣩
	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setFixedHeight(TITLE_HEIGHT);				// �������߶�
	setWindowFlags(Qt::FramelessWindowHint);	// �ޱ߿�

}

// �ź���۵�����
void TitleBar::initConnections()
{
	connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));			// ��С����ť
	connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked())); // ��󻯻�ԭ��ť
	connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));			// ��󻯰�ť
	connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));		// �رհ�ť
}

// ������ʽ��
void TitleBar::loadStyleSheet(const QString& sheetName)
{
	QFile file(":/Resources/QSS/" + sheetName + ".css");//���ļ���·�����      ":/"Ϊ��ǰ·��,Ҳ���Ǵ�titlebar.cpp��Ŀ¼
	file.open(QFile::ReadOnly);							//���ļ�,ֻ��

	if (file.isOpen())//��������ļ�
	{
		QString styleSheet = this->styleSheet();				// ������ʽ��
		// ���ļ��е���ʽ��ȫ���Ͻ��� , �� += ���� , �����е��ļ����ݽ��ж�ȡ
		// QLatin1String ��������ַ����ĵ�ַ, ��Դ����С(�����Ϊʲô����QString��ԭ��)
		styleSheet += QLatin1String(file.readAll());
		setStyleSheet(styleSheet);									// ����ʽ���������
	}

}

//��С����ť
void TitleBar::onButtonMinClicked()
{
	// ������С����ť�� ����ź�
	emit signalButtonMinClicked();
}

//��󻯻�ԭ��ť
void TitleBar::onButtonRestoreClicked()
{
	//������󻯻�ԭ��ť,��󻯻�ԭ��ť�Ͳ��ɼ�,��󻯰�ť�ɼ�
	m_pButtonRestore->setVisible(false);//���ɼ�
	m_pButtonMax->setVisible(true);//�ɼ�
	emit signalButtonRestoreClicked();//�����ź�
}

//��󻯰�ť
void TitleBar::onButtonMaxClicked()
{
	//������󻯰�ť,��󻯰�ť�Ͳ��ɼ�,��󻯻�ԭ��ť�ɼ�
	m_pButtonMax->setVisible(false);//���ɼ�   
	m_pButtonRestore->setVisible(true);//�ɼ�
	emit signalButtonMaxClicked();//�����ź�
}

void TitleBar::onButtonCloseClicked()
{
	emit signalButtonCloseClicked();
}
