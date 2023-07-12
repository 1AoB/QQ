#include "TitleBar.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QPainterPath>


#define BUTTON_HEIGHT	27			// 按钮高度 27像素
#define BUTTON_WIDTH	27			// 按钮宽度 27像素
#define TITLE_HEIGHT	27			// 标题栏高度 27像素

//构造函数
TitleBar::TitleBar(QWidget* parent)
	: QWidget(parent)
	, m_isPressed(false)				    // 是否按下
	, m_buttonType(MIN_MAX_BUTTON)			// 按钮类型
{
	initControl();							//初始阿虎控件
	initConnections();						//初始化信号与槽
	loadStyleSheet("Title");				//加载样式表
}

//析构
TitleBar::~TitleBar()
{
}

// 设置, 标题栏图标
void TitleBar::setTitleIcon(const QString& filePath)//参数传入的是一个图标的路径
{
	QPixmap titleIcon(filePath);								//标题栏的图标的位置
	m_pIcon->setFixedSize(titleIcon.size());					//以titleIcon的大小,来设置图标大小
	m_pIcon->setPixmap(titleIcon);								//设置图标
}

// 设置标题栏内容
void TitleBar::setTitleContent(const QString& titleContent)
{
	m_pTitleContent->setText(titleContent);						// 设置文本
	m_titleContent = titleContent;								// 内容更新
}

// 设置标题栏, 长度
void TitleBar::setTitleWidth(int width)
{
	setFixedWidth(width);//相当于this->setFixedWidth(width);
}

// 设置标题栏, 按钮类型
void TitleBar::setButtonType(ButtonType buttonType)//参数是按钮的类型
{
	m_buttonType = buttonType;			// 更新

	//好几种按钮的类型
	switch (buttonType)
	{
	case MIN_BUTTON:// 最小化 及 关闭，按钮
	{
		m_pButtonRestore->setVisible(false);	// setVisible ,是否可见
		m_pButtonMax->setVisible(false);
	}
	break;
	case MIN_MAX_BUTTON:// 最小化 及 最大化 及 关闭，按钮
	{
		m_pButtonRestore->setVisible(false);	//设置可见参数
	}
	break;
	case ONLY_CLOSE_BUTTON:// 只有 关闭, 按钮
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

//保存窗口最大化前窗口的位置及大小
void TitleBar::saveRestoreInfo(const QPoint& point, const QSize& size)
{
	m_restorePos = point;//位置
	m_restoreSize = size;//大小
}
//获取窗口最大化前窗口的位置及大小
void TitleBar::getRestoreInfo(QPoint& point, QSize& size)
{
	point = m_restorePos;//位置
	size = m_restoreSize;//大小
}
//绘制标题栏
void TitleBar::paintEvent(QPaintEvent* event)
{
	// 设置背景色
	QPainter painter(this);
	QPainterPath pathBack;							//绘制背景、绘图路径
	pathBack.setFillRule(Qt::WindingFill);					// 设置填充规则
	pathBack.addRoundedRect(QRect(0, 0, width(), height()), 3, 3);				// 添加圆角矩形, 到绘图路径
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);			// 平滑的像素映射转换

	// 当窗口最大化或还原后, 窗口长度改变, 标题栏相应做出改变
	// parentWidget() ,返回 父部件
	if (width() != parentWidget()->width())				// 父部件 parentWidget , 调用宽度
	{
		setFixedWidth(parentWidget()->width());
	}

	// 把标题栏重绘事件，添加到 默认中
	QWidget::paintEvent(event);
}

// 双击标题栏, 进行最大化/最小化操作
void TitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	// 只有存在最大化, 最小化按钮 , 才有效
	if (m_buttonType == MIN_MAX_BUTTON)//最大最小化按钮
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
//通过鼠标按下、鼠标移动、鼠标释放事件实现标题栏达到移动窗口效果
void TitleBar::mousePressEvent(QMouseEvent* event)
{
	if (m_buttonType == MIN_MAX_BUTTON)//最大化
	{
		// 在窗口最大化时, 禁止拖动窗口
		if (m_pButtonMax->isVisible())//是可见的
		{
			//移动窗口被按下
			m_isPressed = true;
			// globalPos()返回 事件发生时,鼠标在全局的位置
			m_startMovePos = event->globalPos();
		}
	}
	else//不是最大化
	{
		//移动窗口被按下
		m_isPressed = true;
		m_startMovePos = event->globalPos();				// 添加头文件 <QMouseEvent>
	}
	// 处理完了之后, 调用默认的方法, 操作
	return QWidget::mousePressEvent(event);					
}
//鼠标移动
void TitleBar::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)//是否被按下
	{
		// 返回当前的鼠标位置 ,减去 之前的位置
		QPoint movePoint = event->globalPos() - m_startMovePos;		
		// 返回部件的位置
		QPoint widgetPos = parentWidget()->pos();									
		//start位置进行更新
		m_startMovePos = event->globalPos();

		// 调用父部件的移动方法函数 , 移动前后坐标相加
		//开始位置+偏移位置
		parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
	}
	// 处理完了之后, 调用默认的方法, 操作
	return QWidget::mouseMoveEvent(event);
}
//鼠标释放
void TitleBar::mouseReleaseEvent(QMouseEvent* event)
{
	//释放就不按下了
	m_isPressed = false;
	// 处理完了之后, 调用默认的方法, 操作
	return QWidget::mouseReleaseEvent(event);
}

// 初始化标题栏控件
void TitleBar::initControl()
{
	// 先将部件、标签构造出来   
	m_pIcon = new QLabel(this);					// 标题栏图标
	m_pTitleContent = new QLabel(this);			// 标题栏内容
	m_pButtonMin = new QPushButton(this);		// 最小化按钮
	m_pButtonRestore = new QPushButton(this);	// 最大化还原按钮
	m_pButtonMax = new QPushButton(this);		// 最大化按钮
	m_pButtonClose = new QPushButton(this);		// 关闭按钮

	// 对按钮,设置固定大小（设置尺寸大小）
	m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));				// 最小化，最小化固定大小
	m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));			// 最大化还原按钮
	m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));				// 最大化
	m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));			// 关闭按钮

	// 设置对象名，名字都是所见即所得   ，setObjectName设置对象名
	m_pTitleContent->setObjectName("TitleContent");								// 标题内容、标题栏内容
	m_pButtonMin->setObjectName("ButtonMin");
	m_pButtonRestore->setObjectName("ButtonRestore");
	m_pButtonMax->setObjectName("ButtonMax");
	m_pButtonClose->setObjectName("ButtonClose");

	// 设置布局
	QHBoxLayout* mylayout = new QHBoxLayout(this);//水平布局
	// 添加各种按钮
	mylayout->addWidget(m_pIcon);				// 标题栏图标								
	mylayout->addWidget(m_pTitleContent);		// 标题栏内容
	mylayout->addWidget(m_pButtonMin);			// 最小化按钮
	mylayout->addWidget(m_pButtonRestore);		// 最大化还原按钮
	mylayout->addWidget(m_pButtonMax);			// 最大化按钮
	mylayout->addWidget(m_pButtonClose);		// 关闭按钮

	mylayout->setContentsMargins(5, 0, 0, 0);	// 设置布局, 间隙       四个参数：左上右下
	mylayout->setSpacing(0);					// 设置布局里部件之间, 间隙

	// 标题的尺寸策略
	// Expanding, 垂直方向 ； Fixed , 固定  （两者结合即垂直方向固定、永不改变）
	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setFixedHeight(TITLE_HEIGHT);				// 标题栏高度
	setWindowFlags(Qt::FramelessWindowHint);	// 无边框

}

// 信号与槽的连接
void TitleBar::initConnections()
{
	connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));			// 最小化按钮
	connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked())); // 最大化还原按钮
	connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));			// 最大化按钮
	connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));		// 关闭按钮
}

// 加载样式表
void TitleBar::loadStyleSheet(const QString& sheetName)
{
	QFile file(":/Resources/QSS/" + sheetName + ".css");//将文件的路径搞道      ":/"为当前路径,也就是打开titlebar.cpp的目录
	file.open(QFile::ReadOnly);							//打开文件,只读

	if (file.isOpen())//如果打开了文件
	{
		QString styleSheet = this->styleSheet();				// 保存样式表
		// 将文件中的样式表全部合进来 , 用 += 操作 , 对所有的文件内容进行读取
		// QLatin1String 保存的是字符串的地址, 资源开销小(这就是为什么不用QString的原因)
		styleSheet += QLatin1String(file.readAll());
		setStyleSheet(styleSheet);									// 对样式表进行设置
	}

}

//最小化按钮
void TitleBar::onButtonMinClicked()
{
	// 发射最小化按钮的 点击信号
	emit signalButtonMinClicked();
}

//最大化还原按钮
void TitleBar::onButtonRestoreClicked()
{
	//按了最大化还原按钮,最大化还原按钮就不可见,最大化按钮可见
	m_pButtonRestore->setVisible(false);//不可见
	m_pButtonMax->setVisible(true);//可见
	emit signalButtonRestoreClicked();//发送信号
}

//最大化按钮
void TitleBar::onButtonMaxClicked()
{
	//按了最大化按钮,最大化按钮就不可见,最大化还原按钮可见
	m_pButtonMax->setVisible(false);//不可见   
	m_pButtonRestore->setVisible(true);//可见
	emit signalButtonMaxClicked();//发送信号
}

void TitleBar::onButtonCloseClicked()
{
	emit signalButtonCloseClicked();
}
