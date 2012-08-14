/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "scribblearea.h"

//! [0]
ScribbleArea::ScribbleArea(QWidget *parent)
    : QWidget(parent), oscDockWidget(0)
{
    setAttribute(Qt::WA_AcceptTouchEvents);
    setAttribute(Qt::WA_StaticContents);
    modified = false;

    this->setStyleSheet("background-color:black;");

    myPenColors
            << QColor("green")
            << QColor("purple")
            << QColor("red")
            << QColor("blue")
            << QColor("yellow")

            << QColor("pink")
            << QColor("orange")
            << QColor("brown")
            << QColor("grey")
            << QColor("black");
}
//! [0]

//! [1]
bool ScribbleArea::openImage(const QString &fileName)
//! [1] //! [2]
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    update();
    return true;
}
//! [2]

//! [3]
bool ScribbleArea::saveImage(const QString &fileName, const char *fileFormat)
//! [3] //! [4]
{
    QImage visibleImage = image;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        modified = false;
        return true;
    } else {
        return false;
    }
}
//! [4]

//! [9]
void ScribbleArea::clearImage()
//! [9] //! [10]
{
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
}
//! [10]

//! [12] //! [13]
void ScribbleArea::paintEvent(QPaintEvent *event)
//! [13] //! [14]
{
    QPainter painter(this);
    const QRect rect = event->rect();
    painter.drawImage(rect.topLeft(), image, rect);
}
//! [14]

//! [15]
void ScribbleArea::resizeEvent(QResizeEvent *event)
//! [15] //! [16]
{
    if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}
//! [16]

//! [19]
void ScribbleArea::resizeImage(QImage *image, const QSize &newSize)
//! [19] //! [20]
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}
//! [20]

//! [21]
void ScribbleArea::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog *printDialog = new QPrintDialog(&printer, this);
    //! [21] //! [22]
    if (printDialog->exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = image.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(image.rect());
        painter.drawImage(0, 0, image);
    }
#endif // QT_NO_PRINTER
}
//! [22]

bool ScribbleArea::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
        // MediaCycle adaptation:
        clearImage();
        std::cout << "TouchBegin" << std::endl;
        if(oscDockWidget && oscDockWidget->getFeedbackHandler()){
            oscDockWidget->getFeedbackHandler()->sendMessage( "/mediacycle/browser/reset_pointers" );
        }
        event->accept();
        //return true;
    case QEvent::TouchUpdate:
        //std::cout << "TouchUpdate" << std::endl;
        //return true;
    case QEvent::TouchEnd:
    {
        //std::cout << "TouchEnd" << std::endl;
        QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent *>(event)->touchPoints();
        foreach (const QTouchEvent::TouchPoint &touchPoint, touchPoints) {

            switch (touchPoint.state()) {
            case Qt::TouchPointReleased:
            {
                // This does never seem to work on Ubuntu
                QString mess = QString("/mediacycle/browser/") + QString().setNum(touchPoint.id()) + QString("/released");
                if(oscDockWidget && oscDockWidget->getFeedbackHandler()){
                    oscDockWidget->getFeedbackHandler()->sendMessage( mess.toStdString() );
                    if(oscDockWidget->getFeedbackHandler()->isActive())
                        std::cout << mess.toStdString() << std::endl;
                }
            }
            break;
            //continue;
            case Qt::TouchPointPressed:
            {

                QString mess = QString("/mediacycle/browser/") + QString().setNum(touchPoint.id()) + QString("/activated");
                if(oscDockWidget && oscDockWidget->getFeedbackHandler()){
                    oscDockWidget->getFeedbackHandler()->sendMessage( mess.toStdString() );
                    if(oscDockWidget->getFeedbackHandler()->isActive())
                        std::cout << mess.toStdString() << std::endl;
                }
            }
            //break;
            continue;
            //case Qt::TouchPointStationary:
            // don't do anything if this touch point hasn't moved
            //break;
            //	continue;
            //case Qt::TouchPointMoved:
            //	continue;
            default:
            {
                QRectF rect = touchPoint.rect();
                if (rect.isEmpty()) {
                    qreal diameter = qreal(50) * touchPoint.pressure();
                    QPointF center = rect.center();
                    rect.setSize(QSizeF(diameter, diameter));
                    rect.moveCenter(center);
                }

                QPainter painter(&image);
                painter.setPen(Qt::NoPen);
                painter.setBrush(myPenColors.at(touchPoint.id() % myPenColors.count()));
                painter.drawEllipse(rect);
                painter.end();

                modified = true;
                int rad = 2;
                update(rect.toRect().adjusted(-rad,-rad, +rad, +rad));

                // MediaCycle adaptation: sending the position of each pointer as hover to MediaCycle thru OSC
                QString mess = QString("/mediacycle/browser/") + QString().setNum(touchPoint.id()) + QString("/hover/xy");
                float mc_x(0.0f),mc_y(0.0f);
                mc_x = -1 + 2*touchPoint.normalizedPos().x();
                mc_y = 1 - 2*touchPoint.normalizedPos().y();
                if(oscDockWidget && oscDockWidget->getFeedbackHandler()){
                    oscDockWidget->getFeedbackHandler()->sendMessage( mess.toStdString() );
                    //if(oscDockWidget->getFeedbackHandler()->isActive())
                    //  std::cout << mess.toStdString() << " " << mc_x << " " << mc_y << std::endl;
                }
            }
            //break;
            continue;
            }
        }
        break;
    }
    default:
        return QWidget::event(event);
    }
    return true;
}
