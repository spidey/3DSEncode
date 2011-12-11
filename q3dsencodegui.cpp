#include "q3dsencodegui.h"
#include "ui_q3dsencodegui.h"
#include <QtGui>

Q3DSEncodeGUI::Q3DSEncodeGUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Q3DSEncodeGUI),
    processEncode(this),
    dialogEncode(this)
{
    ui->setupUi(this);

    //Maximum threads
    int c = QThread::idealThreadCount();
    if (c < 0) c = 1;
    int i = 1;
    while (i <= c){
        ui->cmbThreads->addItem(QString::number(i));
        i++;
    }
    ui->cmbThreads->setCurrentIndex(c-1);

    //Setup date
    ui->datCreated->setDateTime(QDateTime::currentDateTime());
    
    //Setup progress dialog (hidden)
    dialogEncode.setLabelText(tr("Encoding video file...\nThis may take several minutes to complete."));
    dialogEncode.setMinimum(0);
    dialogEncode.setMaximum(0);
    dialogEncode.setWindowModality(Qt::ApplicationModal);

    connect(&processEncode, SIGNAL(started()), this, SLOT(handle_processEncode_started()));
    connect(&processEncode, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(handle_processEncode_finished(int, QProcess::ExitStatus)));
    connect(&dialogEncode, SIGNAL(canceled()), this, SLOT(handle_dialogEncode_canceled()));
}

Q3DSEncodeGUI::~Q3DSEncodeGUI() {
    delete ui;
}

void Q3DSEncodeGUI::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Q3DSEncodeGUI::on_actionAbout_triggered() {
    QMessageBox::about(this, tr("3DSEncodeGUI"), tr("3DSEncodeGUI, a GUI for the encode3ds script.\n\nThis is free software, made by Spidey (spideybr@gmail.com).\n\n\n"
                                                    "This software is licensed under the GPLv3.\n\n"
                                                    "Thanks to GBATemp users spinal_cord, Lokao0, SifJar and DiscostewSM"));
}

void Q3DSEncodeGUI::on_actionTutorial_triggered() {
    QMessageBox::information(this, tr("Not Available"), tr("This feature is not available yet."));
}

void Q3DSEncodeGUI::on_actionLanguage_triggered() {
    QMessageBox::information(this, tr("Not Available"), tr("This feature is not available yet."));
}

void Q3DSEncodeGUI::on_actionOpen_triggered() {
    QString old_src = ui->txtSourceVideo->text();
    QString src = QFileDialog::getOpenFileName(this, tr("Open source video file"), old_src.isEmpty() ? QDir::homePath() : old_src, tr("Video files (*.*)"));
    if (!src.isEmpty()) ui->txtSourceVideo->setText(src);
}

void Q3DSEncodeGUI::on_actionSaveTo_triggered() {
    QString old_out = ui->txtOutputVideo->text();
    QString out = QFileDialog::getSaveFileName(this, tr("Save converted video to"), old_out.isEmpty() ? QDir::homePath() + "/VID_0000.avi" : old_out, tr("AVI video file(*.avi)"));
    if (!out.isEmpty()) ui->txtOutputVideo->setText(out);
}

void Q3DSEncodeGUI::on_actionConvert_triggered() {
    if (!validateInput()){ return; } //validateInput will handle errors
    
    QString tmp;
    QStringList args;
    //Main parameters
    args << "3dsencode";
    args << ui->txtSourceVideo->text();
    args << ui->txtOutputVideo->text();
    //2D/3D
    if (!ui->gbVideoType->isChecked())
        args << "2D";
    else if (ui->radSideBySide->isChecked())
        args << "SBS";
    else if(ui->radTopBottom->isChecked())
        args << "TB";
    else if(ui->radRowInterlaced->isChecked())
        args << "RI";
    else if(ui->radColumnInterlaced->isChecked())
        args << "CI";
    else if(ui->radCheckerboardInterlaced->isChecked())
        args << "CB";
    args << (ui->chkReversed->isChecked() ? "R" : "N");
    //Video
    args << QString::number(ui->spnCompression->value());
    tmp = ui->cmbFPS->currentText();
    tmp.chop(3);
    args << tmp.trimmed();
    tmp = ui->cmbResolution->currentText();
    args << tmp.left(3);
    args << tmp.right(3);
    //Video > Crop
    args << (ui->gbCrop->isChecked() ? QString::number(ui->spnCropTop->value()) : "0");
    args << (ui->gbCrop->isChecked() ? QString::number(ui->spnCropRight->value()) : "0");
    args << (ui->gbCrop->isChecked() ? QString::number(ui->spnCropBottom->value()) : "0");
    args << (ui->gbCrop->isChecked() ? QString::number(ui->spnCropLeft->value()) : "0");
    //Audio
    switch (ui->cmbSampleRate->currentIndex()){
        case 0:
            tmp = "22050";
            break;
        case 1:
            tmp = "44100";
            break;
        case 2:
            tmp = "48000";
            break;
        case 3:
            tmp = "96000";
            break;
        default:
            tmp = "44100";
            break;
    }
    args << tmp;
    tmp = ui->cmbAudioBitrate->currentText();
    tmp.chop(4);
    args << tmp.trimmed();
    args << QString::number(ui->cmbChannels->currentIndex() + 1);
    //Advanced
    args << ui->datCreated->dateTime().toString("yyyy-MM-dd hh:mm");
    args << ui->cmbThreads->currentText();
    args << (ui->chkSplit->isChecked() ? "S" : "N");

    processEncode.start("/bin/bash", args);
}

void Q3DSEncodeGUI::handle_dialogEncode_canceled(){
    if (processEncode.state() != QProcess::NotRunning)
        processEncode.close();
}

void Q3DSEncodeGUI::handle_processEncode_started(){
    dialogEncode.show();
}

void Q3DSEncodeGUI::handle_processEncode_finished(int exitCode, QProcess::ExitStatus exitStatus){
    dialogEncode.hide();
    if (exitStatus == QProcess::CrashExit) {
        cleanupResidualFiles();
        //QMessageBox::information(this, tr("Encoding Cancelled"), tr("You cancelled the conversion process."));
    } else if (exitCode != 0) {
        cleanupResidualFiles();
        QMessageBox::warning(this, tr("Encoding Error"), tr("An error occurred while encoding the video.\n") + QString(processEncode.readAllStandardError()));
    } else
        QMessageBox::information(this, tr("Encoding Completed"), tr("Conversion is completed.\nNow, transfer %1 to your 3DS SD card.").arg(QFileInfo(ui->txtOutputVideo->text()).fileName()));
}

void Q3DSEncodeGUI::cleanupResidualFiles(){
    if (ui->chkSplit->isChecked()){
        QFileInfo out(ui->txtOutputVideo->text());
        QDir outDir(out.dir());
        QString fmt = out.fileName().replace("00.avi", "[1-9][1-9].avi");
        QFileInfoList outDirList(outDir.entryInfoList(QStringList(fmt), QDir::Files | QDir::NoSymLinks));
        foreach (QFileInfo file, outDirList){
            QFile(file.filePath()).remove();
        }
    }
    QFile(ui->txtOutputVideo->text()).remove();
}

bool Q3DSEncodeGUI::validateInput() {
    const QString warningTitle(tr("Input Validation Error"));

    QString src = ui->txtSourceVideo->text();
    if (src.isEmpty() || !QFile(src).exists())
    {
        QMessageBox::warning(this, warningTitle, tr("Source video file does not exist."));
        ui->txtSourceVideo->setFocus();
        return false;
    }

    QString fps = ui->cmbFPS->currentText();
    QRegExpValidator vFps(QRegExp(tr("[1-9][0-9]* ?(fps)?"), Qt::CaseInsensitive), this);
    int posFps = 0;
    if (vFps.validate(fps, posFps) != QValidator::Acceptable){
        QMessageBox::warning(this, warningTitle, tr("Invalid FPS value specified."));
        ui->cmbFPS->setFocus();
        return false;
    }

    QString bitrate = ui->cmbAudioBitrate->currentText();
    QRegExpValidator vBitrate(QRegExp(tr("[1-9][0-9]* ?(Kbps)?"), Qt::CaseInsensitive), this);
    int posBitrate = 0;
    if (vBitrate.validate(bitrate, posBitrate) != QValidator::Acceptable){
        QMessageBox::warning(this, warningTitle, tr("Invalid Bitrate value specified."));
        ui->cmbAudioBitrate->setFocus();
        return false;
    }

    QString out = ui->txtOutputVideo->text();
    if (out.isEmpty())
    {
        QMessageBox::warning(this, warningTitle, tr("Output video file wasn't informed."));
        ui->txtOutputVideo->setFocus();
        return false;
    }else {
        QRegExp fmt("[a-zA-Z][a-zA-Z][a-zA-Z]_[0-9][0-9]00\\.avi");
        QRegExpValidator v(fmt, this);
        int pos;
        QString outFileName(QFileInfo(out).fileName());
        if (!v.validate(outFileName, pos)){
            QMessageBox::warning(this, warningTitle, tr("Output video file should be in\nLLL_DD00.avi format (e.g., VID_1200.avi)."));
            ui->txtOutputVideo->setFocus();
            return false;
        } else if (ui->chkSplit->isChecked()){ //If split is set, check if files could be overwritten
            QDir outDir(QFileInfo(out).dir());
            if (outDir.entryList(QStringList(outFileName.replace("00", "[1-9][1-9]"))).size() > 0){
                QMessageBox::StandardButton r = QMessageBox::warning(this, tr("Files may be overwritten"), tr("There are some files in the output directory that may be overwritten"
                                                                                                " during the encoding process.\nAre you sure you want to continue?"),
                                    QMessageBox::Yes | QMessageBox::Cancel);
                if (r == QMessageBox::Cancel){
                    ui->txtOutputVideo->setFocus();
                    return false;
                }
            }
        }
    }

    return true;
}
