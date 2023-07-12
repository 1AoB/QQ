#include "CommonUtils.h"
#include <QPainter>
#include <QFile>
#include <QWidget>
#include <QApplication>
#include <QSettings>

CommonUtils::CommonUtils()//构造函数
{
}
//获取圆头像                                     资源   空头的圆头像        大小
QPixmap CommonUtils::getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize)
{
	if (maskSize == QSize(0, 0))//大小是不是为0
	{
		maskSize = mask.size();//为0,则为指定的大小初始化 (即启用空头的圆头像)
	}
	else
	{
		// scaled , 缩放                 保持长宽比           平滑的转换
		mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	//保存处理的结果
	QImage resultImage(maskSize, QImage::Format_ARGB32_Premultiplied);
	//画家:画结果的图
	QPainter painter(&resultImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);//设置兼容性
	painter.fillRect(resultImage.rect(), Qt::transparent);//进行填充
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);//进行重叠
	painter.drawPixmap(0, 0, mask);//画位图
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.drawPixmap(0, 0, src.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();

	return QPixmap::fromImage(resultImage);//将获取到的圆头像进行返回
}
// 加载皮肤  (加载样式表)                 部件                样式表的名称
void CommonUtils::loadStyleSheet(QWidget* widget, const QString& sheetName)
{
	//样式表的文件目录
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);//只读打开

	if (file.isOpen())//是否打开
	{
		widget->setStyleSheet("");//设置部件的样式为空           style sheet样式表
		QString qsstyleSheet = QLatin1String(file.readAll());//读取文件的所有内容
		widget->setStyleSheet(qsstyleSheet);//去部件的样式进行设置
	}

	file.close();//关闭文件
}
// 设置默认的颜色 (设置默认的皮肤)                  颜色
void CommonUtils::setDefaultSkinColor(const QColor& color)
{
	// applicationDirPath() , 目录路径
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	QSettings settings(path, QSettings::IniFormat);			// 对配置文件进行读写操作
	//将tradeprintinfo.ini文件的红绿蓝配置进来
	settings.setValue("DefaultSkin/red", color.red());
	settings.setValue("DefaultSkin/green", color.green());
	settings.setValue("DefaultSkin/blue", color.blue());

}
// 获取颜色         获取的默认的皮肤颜色
QColor CommonUtils::getDefaultSkinColor()
{
	QColor color;
	//tradeprintinfo.ini路径
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	//这个文件是否存在
	if (!QFile::exists(path))
	{
		//QQ背景的颜色设置为天蓝色
		setDefaultSkinColor(QColor(22, 154, 218));
	}
	//将配置文件进行读写操作
	QSettings settings(path, QSettings::IniFormat);
	//通过settings回去颜色的值
	color.setRed(settings.value("DefaultSkin/red").toInt());
	color.setGreen(settings.value("DefaultSkin/green").toInt());
	color.setBlue(settings.value("DefaultSkin/blue").toInt());

	return color;
}


