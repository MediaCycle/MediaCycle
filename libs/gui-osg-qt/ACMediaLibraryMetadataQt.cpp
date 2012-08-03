/*
 *  ACMediaLibraryMetadataQt.cpp
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

#include "ACMediaLibraryMetadataQt.h"
#include <fstream>
#include <iomanip> // for setw
#include <cstdlib> // for atoi

ACMediaLibraryMetadataQt::ACMediaLibraryMetadataQt(QWidget *parent)
    : QMainWindow(parent),media_cycle(0)
{
    ui.setupUi(this); // first thing to do
    this->setAcceptDrops(true); // for drag and drop

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

ACMediaLibraryMetadataQt::~ACMediaLibraryMetadataQt(){
}

void ACMediaLibraryMetadataQt::on_pushButtonCoverLocate_clicked(){
    QFileDialog dialog(this,"Open Library Cover");
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
        QMessageBox::information(this, tr("Library Cover"),
                                             tr("Cannot load %1.").arg(fileName));
        return;
    }
    QSize label_size = ui.labelCoverImage->size();
    ui.labelCoverImage->setPixmap(QPixmap::fromImage(image).scaled(label_size, Qt::KeepAspectRatio));

    if(!media_cycle)
        return;
    if(!media_cycle->getLibrary())
        return;

    media_cycle->getLibrary()->setCover( fileName.toStdString() );
}

void ACMediaLibraryMetadataQt::on_lineEditAuthor_editingFinished(){
    if(!media_cycle)
        return;
    if(!media_cycle->getLibrary())
        return;
    media_cycle->getLibrary()->setAuthor( ui.lineEditAuthor->text().toStdString() );
}

void ACMediaLibraryMetadataQt::on_lineEditTitle_editingFinished(){
    if(!media_cycle)
        return;
    if(!media_cycle->getLibrary())
        return;
    media_cycle->getLibrary()->setTitle( ui.lineEditTitle->text().toStdString() );
}

void ACMediaLibraryMetadataQt::on_lineEditPublisher_editingFinished(){
    if(!media_cycle)
        return;
    if(!media_cycle->getLibrary())
        return;
    media_cycle->getLibrary()->setPublisher( ui.lineEditPublisher->text().toStdString() );
}

void ACMediaLibraryMetadataQt::on_lineEditWebsite_editingFinished(){
    if(!media_cycle)
        return;
    if(!media_cycle->getLibrary())
        return;
    media_cycle->getLibrary()->setWebsite( ui.lineEditWebsite->text().toStdString() );
}

void ACMediaLibraryMetadataQt::on_lineEditYear_editingFinished(){
    if(!media_cycle)
        return;
    if(!media_cycle->getLibrary())
        return;
    media_cycle->getLibrary()->setYear( ui.lineEditYear->text().toStdString() );
}

void ACMediaLibraryMetadataQt::on_pushButtonReset_clicked(){
    ui.lineEditAuthor->setText("");
    ui.lineEditPublisher->setText("");
    ui.lineEditTitle->setText("");
    ui.lineEditWebsite->setText("");
    ui.lineEditYear->setText("");
}

void ACMediaLibraryMetadataQt::updateLibrary(){
    if(!media_cycle)
        return;
    if(!media_cycle->getLibrary())
        return;

    if(media_cycle->getLibrarySize()>0){
       ui.lineEditAuthor->setText(QString(media_cycle->getLibrary()->getAuthor().c_str()));
       ui.lineEditPublisher->setText(QString(media_cycle->getLibrary()->getPublisher().c_str()));
       ui.lineEditTitle->setText(QString(media_cycle->getLibrary()->getTitle().c_str()));
       ui.lineEditWebsite->setText(QString(media_cycle->getLibrary()->getWebsite().c_str()));
       ui.lineEditYear->setText(QString(media_cycle->getLibrary()->getYear().c_str()));

       int license = -1;
       license = ui.comboBoxLicense->findText( QString(media_cycle->getLibrary()->getLicense().c_str()) );
       if(license == -1){
            license = ui.comboBoxLicense->findText( "Undefined" );
       }
       ui.comboBoxLicense->setCurrentIndex(license);


       QImage image = QImage(media_cycle->getLibrary()->getCover().c_str());
       if (!image.isNull()) {
           QSize label_size = ui.labelCoverImage->size();
           ui.labelCoverImage->setPixmap(QPixmap::fromImage(image).scaled(label_size, Qt::KeepAspectRatio));
       }
    }
    else
        this->clean();
}

void ACMediaLibraryMetadataQt::clean(){
    ui.lineEditAuthor->clear();
    ui.lineEditPublisher->clear();
    ui.lineEditTitle->clear();
    ui.lineEditWebsite->clear();
    ui.lineEditYear->clear();
    ui.labelCoverImage->clear();
}

void ACMediaLibraryMetadataQt::dragEnterEvent(QDragEnterEvent *event)
{
     std::cout <<"<drop content>" << std::endl;
     event->acceptProposedAction();
}

void ACMediaLibraryMetadataQt::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void ACMediaLibraryMetadataQt::dropEvent(QDropEvent *event)
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

void ACMediaLibraryMetadataQt::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}
