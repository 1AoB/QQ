/********************************************************************************
** Form generated from reading UI file 'SendFile.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SENDFILE_H
#define UI_SENDFILE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SendFile
{
public:
    QWidget *titleWidget;
    QWidget *bodyWidget;
    QPushButton *openBtn;
    QPushButton *sendBtn;
    QLineEdit *lineEdit;

    void setupUi(QWidget *SendFile)
    {
        if (SendFile->objectName().isEmpty())
            SendFile->setObjectName(QString::fromUtf8("SendFile"));
        SendFile->resize(519, 169);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SendFile->sizePolicy().hasHeightForWidth());
        SendFile->setSizePolicy(sizePolicy);
        SendFile->setMinimumSize(QSize(519, 169));
        SendFile->setMaximumSize(QSize(519, 169));
        titleWidget = new QWidget(SendFile);
        titleWidget->setObjectName(QString::fromUtf8("titleWidget"));
        titleWidget->setGeometry(QRect(0, 0, 519, 50));
        titleWidget->setProperty("titleskin", QVariant(true));
        bodyWidget = new QWidget(SendFile);
        bodyWidget->setObjectName(QString::fromUtf8("bodyWidget"));
        bodyWidget->setGeometry(QRect(0, 50, 519, 119));
        bodyWidget->setProperty("bottomskin", QVariant(true));
        openBtn = new QPushButton(bodyWidget);
        openBtn->setObjectName(QString::fromUtf8("openBtn"));
        openBtn->setGeometry(QRect(30, 50, 75, 23));
        sendBtn = new QPushButton(bodyWidget);
        sendBtn->setObjectName(QString::fromUtf8("sendBtn"));
        sendBtn->setGeometry(QRect(430, 90, 75, 23));
        lineEdit = new QLineEdit(bodyWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(120, 50, 371, 23));

        retranslateUi(SendFile);

        QMetaObject::connectSlotsByName(SendFile);
    } // setupUi

    void retranslateUi(QWidget *SendFile)
    {
        SendFile->setWindowTitle(QCoreApplication::translate("SendFile", "SendFile", nullptr));
        openBtn->setText(QCoreApplication::translate("SendFile", "\346\211\223\345\274\200...", nullptr));
        sendBtn->setText(QCoreApplication::translate("SendFile", "\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SendFile: public Ui_SendFile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SENDFILE_H
