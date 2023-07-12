#include "CommonUtils.h"
#include <QPainter>
#include <QFile>
#include <QWidget>
#include <QApplication>
#include <QSettings>

CommonUtils::CommonUtils()//���캯��
{
}
//��ȡԲͷ��                                     ��Դ   ��ͷ��Բͷ��        ��С
QPixmap CommonUtils::getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize)
{
	if (maskSize == QSize(0, 0))//��С�ǲ���Ϊ0
	{
		maskSize = mask.size();//Ϊ0,��Ϊָ���Ĵ�С��ʼ�� (�����ÿ�ͷ��Բͷ��)
	}
	else
	{
		// scaled , ����                 ���ֳ����           ƽ����ת��
		mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	//���洦��Ľ��
	QImage resultImage(maskSize, QImage::Format_ARGB32_Premultiplied);
	//����:�������ͼ
	QPainter painter(&resultImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);//���ü�����
	painter.fillRect(resultImage.rect(), Qt::transparent);//�������
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);//�����ص�
	painter.drawPixmap(0, 0, mask);//��λͼ
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.drawPixmap(0, 0, src.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();

	return QPixmap::fromImage(resultImage);//����ȡ����Բͷ����з���
}
// ����Ƥ��  (������ʽ��)                 ����                ��ʽ�������
void CommonUtils::loadStyleSheet(QWidget* widget, const QString& sheetName)
{
	//��ʽ����ļ�Ŀ¼
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);//ֻ����

	if (file.isOpen())//�Ƿ��
	{
		widget->setStyleSheet("");//���ò�������ʽΪ��           style sheet��ʽ��
		QString qsstyleSheet = QLatin1String(file.readAll());//��ȡ�ļ�����������
		widget->setStyleSheet(qsstyleSheet);//ȥ��������ʽ��������
	}

	file.close();//�ر��ļ�
}
// ����Ĭ�ϵ���ɫ (����Ĭ�ϵ�Ƥ��)                  ��ɫ
void CommonUtils::setDefaultSkinColor(const QColor& color)
{
	// applicationDirPath() , Ŀ¼·��
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	QSettings settings(path, QSettings::IniFormat);			// �������ļ����ж�д����
	//��tradeprintinfo.ini�ļ��ĺ��������ý���
	settings.setValue("DefaultSkin/red", color.red());
	settings.setValue("DefaultSkin/green", color.green());
	settings.setValue("DefaultSkin/blue", color.blue());

}
// ��ȡ��ɫ         ��ȡ��Ĭ�ϵ�Ƥ����ɫ
QColor CommonUtils::getDefaultSkinColor()
{
	QColor color;
	//tradeprintinfo.ini·��
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	//����ļ��Ƿ����
	if (!QFile::exists(path))
	{
		//QQ��������ɫ����Ϊ����ɫ
		setDefaultSkinColor(QColor(22, 154, 218));
	}
	//�������ļ����ж�д����
	QSettings settings(path, QSettings::IniFormat);
	//ͨ��settings��ȥ��ɫ��ֵ
	color.setRed(settings.value("DefaultSkin/red").toInt());
	color.setGreen(settings.value("DefaultSkin/green").toInt());
	color.setBlue(settings.value("DefaultSkin/blue").toInt());

	return color;
}


