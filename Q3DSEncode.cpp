#include "Q3DSEncode.h"
#include "ui_Q3DSEncode.h"
#include "QEncodingDialog.h"
#include <QtGui>

Q3DSEncode::Q3DSEncode(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Q3DSEncode),
    settings("Spidey", "3DSEncode")
{
    ui->setupUi(this);
    configureUi();

    setFixedSize(sizeHint());
}

Q3DSEncode::~Q3DSEncode()
{
    delete ui;
}

void Q3DSEncode::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void Q3DSEncode::closeEvent(QCloseEvent *event)
{
    settings.setValue("window/geometry", saveGeometry());
    QWidget::closeEvent(event);
}

void Q3DSEncode::configureUi()
{
    restoreGeometry(settings.value("window/geometry").toByteArray());

    //Load icons
    ui->actionOpen->setIcon(QIcon::fromTheme("document-open", QIcon(":/images/document-open.png")));
    ui->actionSaveTo->setIcon(QIcon::fromTheme("document-save", QIcon(":/images/document-save.png")));
    ui->actionConvert->setIcon(QIcon::fromTheme("video-x-generic", QIcon(":/images/video-x-generic.png")));
    ui->actionQuit->setIcon(QIcon::fromTheme("application-exit", QIcon(":/images/application-exit.png")));
    ui->actionLanguage->setIcon(QIcon::fromTheme("preferences-desktop-locale", QIcon(":/images/preferences-desktop-locale.png")));
    ui->actionAbout->setIcon(QIcon::fromTheme("help-about", QIcon(":/images/help-about.png")));

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
    ui->datDate->setDateTime(QDateTime::currentDateTime());
}

void Q3DSEncode::saveSettings()
{
    //Paths
    QString src = ui->txtSourceVideo->text();
    QFileInfo srcFile(src);
    if (srcFile.exists()){ //May be an URL
        settings.setValue("path/source", srcFile.dir().absolutePath());
    }
    settings.setValue("path/output", QFileInfo(ui->txtOutputVideo->text()).dir().absolutePath());

    settings.setValue("videoType/3D", ui->gbVideoType->isChecked());
    settings.setValue("videoType/SBS", ui->radSideBySide->isChecked());
    settings.setValue("videoType/TB", ui->radTopBottom->isChecked());
    settings.setValue("videoType/RI", ui->radRowInterleaved->isChecked());
    settings.setValue("videoType/CI", ui->radColumnInterleaved->isChecked());
    settings.setValue("videoType/reversed", ui->chkReversed->isChecked());
    settings.setValue("audio/channels", ui->cmbChannels->currentIndex());
    settings.setValue("audio/sampleRate", ui->cmbSampleRate->currentIndex());
    settings.setValue("video/compression", ui->spnCompression->value());
    settings.setValue("video/fps", ui->cmbFPS->currentIndex());
    settings.setValue("video/resolution", ui->cmbResolution->currentIndex());
    settings.setValue("advanced/threads", ui->cmbThreads->currentIndex());
    settings.setValue("advanced/split", ui->chkSplit->isChecked());
}

void Q3DSEncode::on_actionLoadSettings_triggered()
{
    ui->gbVideoType->setChecked(settings.value("videoType/3D", ui->gbVideoType->isChecked()).toBool());
    ui->radSideBySide->setChecked(settings.value("videoType/SBS", ui->radSideBySide->isChecked()).toBool());
    ui->radTopBottom->setChecked(settings.value("videoType/TB", ui->radTopBottom->isChecked()).toBool());
    ui->radRowInterleaved->setChecked(settings.value("videoType/RI", ui->radRowInterleaved->isChecked()).toBool());
    ui->radColumnInterleaved->setChecked(settings.value("videoType/CI", ui->radColumnInterleaved->isChecked()).toBool());
    ui->chkReversed->setChecked(settings.value("videoType/reversed", ui->chkReversed->isChecked()).toBool());
    ui->cmbChannels->setCurrentIndex(settings.value("audio/channels", ui->cmbChannels->currentIndex()).toInt());
    ui->cmbSampleRate->setCurrentIndex(settings.value("audio/sampleRate", ui->cmbSampleRate->currentIndex()).toInt());
    ui->spnCompression->setValue(settings.value("video/compression", ui->spnCompression->value()).toInt());
    ui->cmbFPS->setCurrentIndex(settings.value("video/fps", ui->cmbFPS->currentIndex()).toInt());
    ui->cmbResolution->setCurrentIndex(settings.value("video/resolution", ui->cmbResolution->currentIndex()).toInt());
    ui->cmbThreads->setCurrentIndex(settings.value("advanced/threads", ui->cmbThreads->currentIndex()).toInt());
    ui->chkSplit->setChecked(settings.value("advanced/split", ui->chkSplit->isChecked()).toBool());
}

void Q3DSEncode::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("3DSEncodeGUI"), tr("<h1>3DSEncode</h1>"
                                                    "<p>3DSEncode is a all-in-one encoding solution to the 3DS, based on libav and youtube-dl.</p>"
                                                    "<br>"
                                                    "<p>This is free software, made by Spidey (<a href='mailto:spideybr@gmail.com'>spideybr@gmail.com</a>).</p>"
                                                    "<p>This software is licensed under the GPLv3 (not yet).</p>"
                                                    "<br>"
                                                    "<p>Thanks to GBATemp users spinal_cord, Lokao0, SifJar and DiscostewSM.</p>"
                                                    "<p>Application icon by CristopherOS (<a href='http://cristopheros.deviantart.com/'>http://cristopheros.deviantart.com/</a>)."));
}

void Q3DSEncode::on_actionLanguage_triggered()
{
    QMessageBox::information(this, tr("Not Available"), tr("This feature is not yet available."));
}

void Q3DSEncode::on_actionOpen_triggered()
{
    QString old_src = ui->txtSourceVideo->text();
    QString src = QFileDialog::getOpenFileName(this, tr("Open source video file"), old_src.isEmpty() ? settings.value("path/source", QDir::homePath()).toString() : old_src, tr("Video files(*.*)"));
    if (!src.isEmpty()) ui->txtSourceVideo->setText(src);
}

void Q3DSEncode::on_actionSaveTo_triggered()
{
    QString old_out = ui->txtOutputVideo->text();
    QString out = QFileDialog::getSaveFileName(this, tr("Save converted video to"), old_out.isEmpty() ? settings.value("path/output", QDir::homePath()).toString() + "/VID_0001.avi" : old_out, tr("AVI video file(*.avi)"));
    if (!out.isEmpty()) ui->txtOutputVideo->setText(out);
}

void Q3DSEncode::on_actionConvert_triggered()
{
    if (!validateInput()){ return; } //validateInput will handle errors

    saveSettings();

    QMap<QString, QString> args;

    //Main parameters
    args["source"] = ui->txtSourceVideo->text();
    args["output"] = ui->txtOutputVideo->text();
    //2D/3D
    if (!ui->gbVideoType->isChecked())
        args["videoType"] = "2D";
    else if (ui->radSideBySide->isChecked())
        args["videoType"] = "SBS";
    else if(ui->radTopBottom->isChecked())
        args["videoType"] = "TB";
    else if(ui->radRowInterleaved->isChecked())
        args["videoType"] = "RI";
    else if(ui->radColumnInterleaved->isChecked())
        args["videoType"] = "CI";
    args["reversed"] = (ui->chkReversed->isChecked() ? "Y" : "N");
    //Video
    args["compression"] = QString::number(ui->spnCompression->value());
    args["fps"] = ui->cmbFPS->currentText().replace("fps", "").trimmed();
    args["width"] = ui->cmbResolution->currentText().left(3);
    args["height"] = ui->cmbResolution->currentText().right(3);
    //Audio
    switch (ui->cmbSampleRate->currentIndex()){
        case 0:
            args["samplerate"] = "22050";
            break;
        case 1:
            args["samplerate"] = "44100";
            break;
        case 2:
            args["samplerate"] = "48000";
            break;
        case 3:
            args["samplerate"] = "96000";
            break;
        default:
            args["samplerate"] = "44100";
            break;
    }
    args["channels"] = QString::number(ui->cmbChannels->currentIndex() + 1);
    //Advanced
    args["date"] = ui->datDate->dateTime().toString("yyyy-MM-dd hh:mm");
    args["threads"] = ui->cmbThreads->currentText();
    args["split"] = (ui->chkSplit->isChecked() ? "Y" : "N");

    QEncodingDialog encodingDialog(args);
    encodingDialog.exec();
}

bool Q3DSEncode::validateInput()
{
    const QString warningTitle(tr("Input Validation Error"));

    QString src = ui->txtSourceVideo->text();
    if (src.isEmpty())
    {
        QMessageBox::warning(this, warningTitle, tr("Source video not informed."));
        ui->txtSourceVideo->setFocus();
        return false;
    }else if(!QFile(src).exists() && !src.contains(QRegExp("^(http://|www)"))){
        QMessageBox::warning(this, warningTitle, tr("Source video is neither a valid video file or an URL."));
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

    QString out = ui->txtOutputVideo->text();
    if (out.isEmpty())
    {
        QMessageBox::warning(this, warningTitle, tr("Output video file wasn't informed."));
        ui->txtOutputVideo->setFocus();
        return false;
    }else {
        QRegExp fmt("[a-zA-Z][a-zA-Z][a-zA-Z]_[0-9][0-9]01\\.avi");
        QRegExpValidator v(fmt, this);
        int pos;
        QString outFileName(QFileInfo(out).fileName());
        if (!v.validate(outFileName, pos)){
            QMessageBox::warning(this, warningTitle, tr("Output video file should be in\nLLL_DD01.avi format (e.g., VID_1201.avi)."));
            ui->txtOutputVideo->setFocus();
            return false;
        } else if (ui->chkSplit->isChecked()){ //If split is set, check if files could be overwritten
            QDir outDir(QFileInfo(out).dir());
            if (outDir.entryList(QStringList(outFileName.replace("01", "[1-9][1-9]"))).size() > 0){
                QMessageBox::StandardButton r = QMessageBox::warning(this,
                                                                     tr("Files may be overwritten"),
                                                                     tr("There are some files in the output directory that may be overwritten"
                                                                        " during the encoding process.\nAre you sure you want to continue?"
                                                                        ),
                                                                     QMessageBox::Yes | QMessageBox::No
                                                                     );
                if (r == QMessageBox::No){
                    ui->txtOutputVideo->setFocus();
                    return false;
                }
            }
        }
    }

    return true;
}
