/*
 *  ACUserProfileQt.cpp
 *  MediaCycle
 *
 *  @author Christian Frisson
 *  @date 4/05/2012
 *  @copyright (c) 2012 – UMONS - Numediart
 *  
 *  MediaCycle of University of Mons – Numediart institute is 
 *  licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 *  licence (the “License”); you may not use this file except in compliance 
 *  with the License.
 *  
 *  This program is free software: you can redistribute it and/or 
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  Each use of this software must be attributed to University of Mons – 
 *  Numediart Institute
 *  
 *  Any other additional authorizations may be asked to avre@umons.ac.be 
 *  <mailto:avre@umons.ac.be>
 *
 */

#include "ACUserProfileQt.h"
#include <fstream>
#include <iomanip> // for setw
#include <cstdlib> // for atoi
#include <MediaCycle.h>

//#include <QtPlugin>

//Q_IMPORT_PLUGIN(qjpeg)



ACUserProfileQt::ACUserProfileQt(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this); // first thing to do

    this->setAcceptDrops(true); // for drag and drop

    QSettings settings("numediart", "MediaCycle");
    ui.lineEditName->setText( settings.value("user.name").toString() );
    ui.lineEditEmail->setText( settings.value("user.email").toString() );
    ui.lineEditWebsite->setText( settings.value("user.website").toString() );
    ui.lineEditLocation->setText( settings.value("user.location").toString() );

    QString fileName = settings.value("user.picture").toString();
    QImage image = QImage(fileName);
    if (!image.isNull()) {
        ui.labelPictureImage->setPixmap(QPixmap::fromImage(image));
        QSize label_size = ui.labelPictureImage->size();
        ui.labelPictureImage->setPixmap(QPixmap::fromImage(image).scaled(label_size, Qt::KeepAspectRatio));
    }

    //CF this should be done once per application runtime
    QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
    QList<QByteArray>::Iterator format = supportedFormats.begin();
    mediaExts = "Image files (";
    while(format != supportedFormats.end()) {
        QString ext = QString(".") + QString((*format).data());
        if(ACMediaFactory::getInstance().getMediaTypeFromExtension(ext.toStdString()) == MEDIA_TYPE_IMAGE){
            if (format != supportedFormats.begin())
                    mediaExts.append(" ");
            mediaExts.append("*");
            mediaExts.append(ext);
        }
        ++format;
    }
    mediaExts.append(")");
}

ACUserProfileQt::~ACUserProfileQt(){
    QSettings settings("numediart", "MediaCycle");
    settings.setValue("user.name", ui.lineEditName->text());
    settings.setValue("user.email", ui.lineEditEmail->text());
    settings.setValue("user.website", ui.lineEditWebsite->text());
    settings.setValue("user.location", ui.lineEditLocation->text());
}

void ACUserProfileQt::on_pushButtonPictureLocate_clicked(){
    QFileDialog dialog(this,"Open Profile Picture");
    dialog.setNameFilter(mediaExts);
    dialog.setFileMode(QFileDialog::ExistingFile);
    QString fileName;
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    QStringList::Iterator file = fileNames.begin();
    if(file != fileNames.end()) {
            fileName = *file;
    }
    if(fileName.isEmpty())
        return;
    QImage image = QImage(fileName);
    if (image.isNull()) {
        QMessageBox::information(this, tr("Profile Picture"),
                                             tr("Cannot load %1.").arg(fileName));
        return;
    }
    QSize label_size = ui.labelPictureImage->size();
    ui.labelPictureImage->setPixmap(QPixmap::fromImage(image).scaled(label_size, Qt::KeepAspectRatio));

    QSettings settings("numediart", "MediaCycle");
    settings.setValue("user.picture", fileName);
}

void ACUserProfileQt::dragEnterEvent(QDragEnterEvent *event)
{
     std::cout <<"<drop content>" << std::endl;
     event->acceptProposedAction();
}

void ACUserProfileQt::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void ACUserProfileQt::dropEvent(QDropEvent *event)
{
     const QMimeData *mimeData = event->mimeData();

     /*if (mimeData->hasImage()) {
              setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
          } else if (mimeData->hasHtml()) {
              setText(mimeData->html());
              setTextFormat(Qt::RichText);
          } else if (mimeData->hasText()) {
              setText(mimeData->text());
              setTextFormat(Qt::PlainText);
          } else*/ if (mimeData->hasUrls()) {
              QList<QUrl> urlList = mimeData->urls();
              QString text;
              for (int i = 0; i < urlList.size() && i < 32; ++i) {
                  QString url = urlList.at(i).path();
                  text += url + QString("\n");
                  std::cout << url.toStdString() << std::endl;
              }
              //setText(text);
          } /*else {
              setText(tr("Cannot display data"));
          }*/

     /*if (mimeData){
         foreach (QString format, mimeData->formats()) {
             std::cout << format.toStdString() << std::endl;
         }
    }*/
     event->acceptProposedAction();
}

void ACUserProfileQt::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}
