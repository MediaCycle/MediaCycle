/********************************************************************************
** Form generated from reading ui file 'VideoCycle.ui'
**
** Created: Wed Sep 16 18:42:57 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_VIDEOCYCLE_H
#define UI_VIDEOCYCLE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

#include <iostream>
QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QFrame *frame;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *OutputFilelineEdit;
    QLabel *VideoDirectorylabel;
    QLabel *OutPutFilelabel;
    QLineEdit *VideoDirectorylineEdit;
    QPushButton *BrowseVideoDirpushButton;
    QPushButton *GOpushButton;
    QPushButton *BrowseOutputFilepushButton;
    QLabel *numediartLogolabel;
    QLabel *label;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(581, 428);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(40, 70, 491, 191));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 3, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 2, 0, 1, 1);

        OutputFilelineEdit = new QLineEdit(frame);
        OutputFilelineEdit->setObjectName(QString::fromUtf8("OutputFilelineEdit"));

        gridLayout->addWidget(OutputFilelineEdit, 1, 2, 1, 2);

        VideoDirectorylabel = new QLabel(frame);
        VideoDirectorylabel->setObjectName(QString::fromUtf8("VideoDirectorylabel"));

        gridLayout->addWidget(VideoDirectorylabel, 0, 0, 1, 1);

        OutPutFilelabel = new QLabel(frame);
        OutPutFilelabel->setObjectName(QString::fromUtf8("OutPutFilelabel"));

        gridLayout->addWidget(OutPutFilelabel, 1, 0, 1, 1);

        VideoDirectorylineEdit = new QLineEdit(frame);
        VideoDirectorylineEdit->setObjectName(QString::fromUtf8("VideoDirectorylineEdit"));

        gridLayout->addWidget(VideoDirectorylineEdit, 0, 2, 1, 2);

        BrowseVideoDirpushButton = new QPushButton(frame);
        BrowseVideoDirpushButton->setObjectName(QString::fromUtf8("BrowseVideoDirpushButton"));

        gridLayout->addWidget(BrowseVideoDirpushButton, 0, 1, 1, 1);

        GOpushButton = new QPushButton(frame);
        GOpushButton->setObjectName(QString::fromUtf8("GOpushButton"));

        gridLayout->addWidget(GOpushButton, 2, 1, 1, 1);

        BrowseOutputFilepushButton = new QPushButton(frame);
        BrowseOutputFilepushButton->setObjectName(QString::fromUtf8("BrowseOutputFilepushButton"));

        gridLayout->addWidget(BrowseOutputFilepushButton, 1, 1, 1, 1);

        numediartLogolabel = new QLabel(centralwidget);
        numediartLogolabel->setObjectName(QString::fromUtf8("numediartLogolabel"));
        numediartLogolabel->setGeometry(QRect(410, 10, 121, 31));
		std::cout << "------------------" << std::endl;
		std::cout << QCoreApplication::applicationDirPath().toStdString() << std::endl;
		const char* nlogo = (QCoreApplication::applicationDirPath().toStdString() + "/../Resources/logo_numediart.png").c_str();
 		std::cout << "------------------" << std::endl;
		numediartLogolabel->setPixmap(QPixmap(QString::fromUtf8(nlogo)));
        numediartLogolabel->setScaledContents(true);
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setEnabled(true);
        label->setGeometry(QRect(150, 10, 241, 41));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        QBrush brush2(QColor(127, 127, 127, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush2);
        QBrush brush3(QColor(170, 170, 170, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        QBrush brush4(QColor(255, 255, 220, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        label->setPalette(palette);
        QFont font;
        font.setPointSize(16);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAutoFillBackground(false);
        label->setFrameShape(QFrame::Box);
        label->setScaledContents(false);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 581, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        VideoDirectorylabel->setText(QApplication::translate("MainWindow", "Video Directory", 0, QApplication::UnicodeUTF8));
        OutPutFilelabel->setText(QApplication::translate("MainWindow", "Output File", 0, QApplication::UnicodeUTF8));
        BrowseVideoDirpushButton->setText(QApplication::translate("MainWindow", "Browse...", 0, QApplication::UnicodeUTF8));
        GOpushButton->setText(QApplication::translate("MainWindow", "GO !", 0, QApplication::UnicodeUTF8));
        BrowseOutputFilepushButton->setText(QApplication::translate("MainWindow", "Browse...", 0, QApplication::UnicodeUTF8));
        numediartLogolabel->setText(QString());
        label->setText(QApplication::translate("MainWindow", "MediaCycle - Video Analysis", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(MainWindow);
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIDEOCYCLE_H
