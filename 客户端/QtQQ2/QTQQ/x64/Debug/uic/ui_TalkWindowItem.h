/********************************************************************************
** Form generated from reading UI file 'TalkWindowItem.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TALKWINDOWITEM_H
#define UI_TALKWINDOWITEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TalkWindowItem
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *headlabel;
    QLabel *msgLabel;
    QPushButton *tclosebtn;

    void setupUi(QWidget *TalkWindowItem)
    {
        if (TalkWindowItem->objectName().isEmpty())
            TalkWindowItem->setObjectName(QString::fromUtf8("TalkWindowItem"));
        TalkWindowItem->resize(404, 42);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TalkWindowItem->sizePolicy().hasHeightForWidth());
        TalkWindowItem->setSizePolicy(sizePolicy);
        TalkWindowItem->setMinimumSize(QSize(0, 42));
        TalkWindowItem->setMaximumSize(QSize(16777215, 42));
        horizontalLayout = new QHBoxLayout(TalkWindowItem);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(6, 6, 6, 6);
        headlabel = new QLabel(TalkWindowItem);
        headlabel->setObjectName(QString::fromUtf8("headlabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(headlabel->sizePolicy().hasHeightForWidth());
        headlabel->setSizePolicy(sizePolicy1);
        headlabel->setMinimumSize(QSize(30, 30));
        headlabel->setMaximumSize(QSize(30, 30));

        horizontalLayout->addWidget(headlabel);

        msgLabel = new QLabel(TalkWindowItem);
        msgLabel->setObjectName(QString::fromUtf8("msgLabel"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(msgLabel->sizePolicy().hasHeightForWidth());
        msgLabel->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(msgLabel);

        tclosebtn = new QPushButton(TalkWindowItem);
        tclosebtn->setObjectName(QString::fromUtf8("tclosebtn"));
        sizePolicy1.setHeightForWidth(tclosebtn->sizePolicy().hasHeightForWidth());
        tclosebtn->setSizePolicy(sizePolicy1);
        tclosebtn->setMinimumSize(QSize(16, 16));
        tclosebtn->setMaximumSize(QSize(16, 16));

        horizontalLayout->addWidget(tclosebtn);


        retranslateUi(TalkWindowItem);

        QMetaObject::connectSlotsByName(TalkWindowItem);
    } // setupUi

    void retranslateUi(QWidget *TalkWindowItem)
    {
        TalkWindowItem->setWindowTitle(QCoreApplication::translate("TalkWindowItem", "TalkWindowItem", nullptr));
        headlabel->setText(QString());
        msgLabel->setText(QString());
        tclosebtn->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class TalkWindowItem: public Ui_TalkWindowItem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TALKWINDOWITEM_H
