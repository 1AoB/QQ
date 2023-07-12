/********************************************************************************
** Form generated from reading UI file 'RecevieFile.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RECEVIEFILE_H
#define UI_RECEVIEFILE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RecevieFile
{
public:
    QWidget *titleWidget;
    QWidget *bodyWidget;
    QLabel *label;
    QPushButton *okBtn;
    QPushButton *cancelBtn;

    void setupUi(QWidget *RecevieFile)
    {
        if (RecevieFile->objectName().isEmpty())
            RecevieFile->setObjectName(QString::fromUtf8("RecevieFile"));
        RecevieFile->resize(515, 160);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(RecevieFile->sizePolicy().hasHeightForWidth());
        RecevieFile->setSizePolicy(sizePolicy);
        RecevieFile->setMinimumSize(QSize(515, 160));
        RecevieFile->setMaximumSize(QSize(515, 160));
        titleWidget = new QWidget(RecevieFile);
        titleWidget->setObjectName(QString::fromUtf8("titleWidget"));
        titleWidget->setGeometry(QRect(0, 0, 515, 50));
        titleWidget->setProperty("titleskin", QVariant(true));
        bodyWidget = new QWidget(RecevieFile);
        bodyWidget->setObjectName(QString::fromUtf8("bodyWidget"));
        bodyWidget->setGeometry(QRect(0, 50, 515, 110));
        bodyWidget->setProperty("bottomskin", QVariant(true));
        label = new QLabel(bodyWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(30, 20, 381, 16));
        okBtn = new QPushButton(bodyWidget);
        okBtn->setObjectName(QString::fromUtf8("okBtn"));
        okBtn->setGeometry(QRect(290, 70, 75, 23));
        cancelBtn = new QPushButton(bodyWidget);
        cancelBtn->setObjectName(QString::fromUtf8("cancelBtn"));
        cancelBtn->setGeometry(QRect(390, 70, 75, 23));

        retranslateUi(RecevieFile);

        QMetaObject::connectSlotsByName(RecevieFile);
    } // setupUi

    void retranslateUi(QWidget *RecevieFile)
    {
        RecevieFile->setWindowTitle(QCoreApplication::translate("RecevieFile", "RecevieFile", nullptr));
        label->setText(QCoreApplication::translate("RecevieFile", "\346\235\245\350\207\252xxx\345\217\221\346\235\245\347\232\204\346\226\207\344\273\266\357\274\232\346\230\257\345\220\246\346\216\245\346\224\266\357\274\237", nullptr));
        okBtn->setText(QCoreApplication::translate("RecevieFile", "\347\241\256\345\256\232", nullptr));
        cancelBtn->setText(QCoreApplication::translate("RecevieFile", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RecevieFile: public Ui_RecevieFile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RECEVIEFILE_H
