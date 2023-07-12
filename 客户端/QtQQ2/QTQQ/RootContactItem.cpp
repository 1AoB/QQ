#include "RootContactItem.h"
#include <QPainter>

RootContactItem::RootContactItem(bool hasArrow, QWidget* parent)
	: QLabel(parent)
	, m_rotation(0)
	, m_hasArrow(hasArrow)
{
	// 设置固定高度
	setFixedHeight(32);
	// 尺寸，设置属性（拓展属性，固定的）
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	// 初始化属性动画
	// 让rotation，作为动画
	m_animation = new QPropertyAnimation(this, "rotation");
	// 设置 单次动画时长，50毫秒 完成动画实现
	m_animation->setDuration(50);
	// 设置动画曲线，曲率
	// 动画 缓和曲线类型 ，以(t^2)进行加速，可以看QT帮助文档
	m_animation->setEasingCurve(QEasingCurve::InQuad);

}

RootContactItem::~RootContactItem()
{
}

void RootContactItem::setText(const QString& title)
{
	m_titleText = title;
	update();
}

void RootContactItem::setExpanded(bool expand)
{
	// 判断是否展开，数值 就是角度
	if (expand)
	{
		// 如果展开，结束值设置为 90
		m_animation->setEndValue(90);
	}
	else
	{
		// 如果不用展开，结束值设置为 0
		m_animation->setEndValue(0);
	}

	m_animation->start();
}

int RootContactItem::rotation()
{
	return m_rotation;
}

void RootContactItem::setRotation(int rotation)
{
	m_rotation = rotation;			// 更新角度
	update();
}

void RootContactItem::paintEvent(QPaintEvent* event)
{
	// 创建第一个画家
	QPainter painter(this);			// 写this，是在当前窗口 画
	// 对文字，设置渲染风格
	// 文本反锯齿
	painter.setRenderHint(QPainter::TextAntialiasing, true);

	QFont font;
	font.setPointSize(10);
	painter.setFont(font);			// 设置字体大小

	// 绘制文本
	// 坐标 24, 0，宽度，高度，左对齐，上下空间居中对齐，绘制的文本
	painter.drawText(24, 0, width() - 24, height(), Qt::AlignLeft | Qt::AlignVCenter, m_titleText);
	// 设置渲染，以平滑像素转换
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.save();				// 保存

	// 如果有箭头
	if (m_hasArrow)
	{
		// 就加载图标
		QPixmap pixmap;
		pixmap.load(":/Resources/MainWindow/arrow.png");

		// 创建一个位图，保存 中间图片的效果
		QPixmap tmpPixmap(pixmap.size());
		tmpPixmap.fill(Qt::transparent);				// 设置透明

		// 再创建一个画家，专门用来保存 上面的 tmpPixmap
		QPainter p(&tmpPixmap);
		p.setRenderHint(QPainter::SmoothPixmapTransform, true);

		// 旋转角度之前，先对坐标系进行 偏移转换
		// （X方向偏移，Y方向偏移）
		p.translate(pixmap.width() / 2, pixmap.height() / 2);

		// 顺时针，旋转坐标系
		// 进行旋转，m_rotation就是旋转角度 传进去
		p.rotate(m_rotation);

		// 旋转之后，在这个位置，进行图片【绘制】
		// 因为之前进行过坐标偏移，所有用 0 减去坐标的一半
		p.drawPixmap(0 - pixmap.width() / 2, 0 - pixmap.height() / 2, pixmap);

		/*
			画家 p，是在 中间位图 tmpPixmap 上面画的
			接下来，要画到第一个 画家上去
			设备是 当前窗口，就是 this 上面
		*/

		// 原来的画家，绘制位图，
		// 画的图片就是 p画家在 tmpPixmap 上面画的图
		painter.drawPixmap(6, (height() - pixmap.height()) / 2, tmpPixmap);

		// 画完了之后，给画家恢复设置
		// 就是恢复到 save之前的设置
		painter.restore();

	}

	// 所有的都处理之后，再进行事件处理
	QLabel::paintEvent(event);
}
