#include "QEncodingDialog.h"
#include "ui_QEncodingDialog.h"
#include <QtGui>

QEncodingDialog::QEncodingDialog(QMap<QString, QString> &args, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QEncodingDialog),
    args(args),
    state(Idle)
{
    ui->setupUi(this);
    ui->txtEncoding->hide();

    ui->bbBottom->hide();
    setupButtonBoxes(Cancel);

    adjustSize();

    process.setReadChannelMode(QProcess::MergedChannels);
    connect(&process, SIGNAL(readyRead()), this, SLOT(processRead()));
    connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));

    startDownloading();
}

QEncodingDialog::~QEncodingDialog(){
    delete ui;
}

void QEncodingDialog::on_btnAdvanced_toggled(bool toggled){
    ui->bbTop->setHidden(toggled);
    ui->txtEncoding->setVisible(toggled);
    ui->bbBottom->setVisible(toggled);
    adjustSize();
}

void QEncodingDialog::processRead(){
    while (process.canReadLine()){
        QString line = process.readLine();

        if (state == Downloading){
            if (line.startsWith("[download] ")){
                QString text = line.mid(11);
                if (text.startsWith("Destination: ")){
                    args["source"] = text.mid(13);
                    createdFiles << args["sources"];
                    ui->pbEncoding->setMaximum(100);
                }else if(text.contains("ETA")){
                    float downloadedProgress = text.left(5).toFloat();
                    ui->pbEncoding->setValue(int(downloadedProgress));
                }else if(text.contains(" has already been downloaded")){
                    args["source"] = text.left(text.indexOf(" has already been downloaded"));
                    createdFiles << args["sources"];
                }
            }
        }else{ //Converting
            
        }

        ui->txtEncoding->insertPlainText(line);
        ui->txtEncoding->ensureCursorVisible();
    }
}

void QEncodingDialog::processFinished(int exitCode, QProcess::ExitStatus exitStatus){
    if (exitCode != 0 || exitStatus == QProcess::CrashExit){
        switch (state){
            case Downloading:
                workFinished(tr("Download Error: %1").arg(exitCode));
                cleanupResidualFiles();
                break;
            case Encoding:
                workFinished(tr("Convertion Error: %1").arg(exitCode));
                cleanupResidualFiles();
                break;
            case Idle:
                workFinished(tr("Program Error: Report Bug"));
                cleanupResidualFiles();
                break;
        }
    }else{
        switch (state){
            case Downloading:
                startEncoding();
                break;
            case Encoding:
                workFinished(tr("Convertion completed"));
                break;
            case Idle:
                workFinished(tr("Program Error: Report Bug"));
                cleanupResidualFiles();
                break;
        }
    }
}

void QEncodingDialog::reject(){
    if (state == Idle){
        done(QDialog::Rejected);
    }else{
        if (confirmCancel()){
            process.close();
            workFinished(tr("Convertion Canceled"));
        }
    }
}

void QEncodingDialog::startEncoding(){
    state = Encoding;
    ui->lblConvertionStatus->setText(tr("Encoding video"));
    writeBannerToLog(tr("Starting video encoding"));

    QStringList params;
    QString vt = args["videoType"];
    //Global parameters
    params << "-threads";
    params << args["threads"];
    params << "-y";

    //Input parameters
    params << "-i";
    params << args["source"];
    if (vt != "2D"){
        params << "-i";
        params << args["source"];
    }

    //Output parameters
    params << "-c:v";
    params << "mjpeg";
    params << "-c:a";
    params << "adpcm_ima_wav";

    params << "-s";
    QString cp;
    if (vt == "TB" || vt == "RI" || vt == "2D"){
        cp += args["width"];
    }else{
        cp += QString::number(args["width"].toInt() * 2);
    }
    cp += "x";
    if (vt == "SBS" || vt == "CI" || vt == "2D"){
        cp += args["height"];
    }else{
        cp += QString::number(args["height"].toInt() * 2);
    }
    params << cp;

    params << "-r";
    params << args["fps"];

    params << "-q:v";
    params << args["compression"];

    params << "-ar";
    params << args["samplerate"];

    params << "-ac";
    params << args["channels"];

    if (vt != "2D"){
        int i=0;
        if (args["reversed"] == "Y") i++;
        
        params << "-filter:v:" + QString::number(i);
        if (vt == "SBS" || vt == "TB"){
            params << "crop=" + args["width"] + ":" + args["height"] + ":0:0";
        }
        params << "-filter:v:" + QString::number(++i%2);
        if (vt == "SBS"){
            params << "crop=" + args["width"] + ":" + args["height"] + ":" + args["width"] + ":0";
        }else if (vt == "TB"){
            params << "crop=" + args["width"] + ":" + args["height"] + ":0:" + args["height"];
        }

        params << "-map";
        params << "0";
        params << "-map";
        params << "1:v";
    }

    params << "-metadata";
    params << "date=" + args["date"];

    params << args["output"];

    qDebug() << params;

    process.start("bin/avconv", params);
}

void QEncodingDialog::startDownloading()
{
    if (args["source"].contains(QRegExp("^(http://|www)"))){
        state = Downloading;
        ui->lblConvertionStatus->setText(tr("Downloading video"));
        writeBannerToLog(tr("Starting video download"));

        QString outDirPath = QFileInfo(args["output"]).dir().absolutePath();

        QStringList params;
        params << "bin/youtube-dl";
        params << "-o";
        params << outDirPath + "/%(stitle)s.%(ext)s";
        params << "--no-part";
        params << "-c";
        params << args["source"];
        process.start("python", params);
    } else {
        startEncoding();
    }
}

bool QEncodingDialog::confirmCancel()
{
    return QMessageBox::Yes == QMessageBox::warning(
                                                    this,
                                                    tr("Cancel Convertion"),
                                                    tr("Are you sure you want to cancel the video encoding process?"),
                                                    QMessageBox::Yes | QMessageBox::No,
                                                    QMessageBox::No
                                                   );
}

void QEncodingDialog::workFinished(QString msg)
{
    state = Idle;

    ui->pbEncoding->setMaximum(100);
    ui->pbEncoding->setValue(100);
    ui->lblConvertionStatus->setText(msg);
    writeBannerToLog(msg);

    setupButtonBoxes(Close);
}

void QEncodingDialog::writeBannerToLog(QString txt)
{
    if(txt.length() > 50) txt = txt.left(50);

    QString mainText("######################################################");
    float padding = (mainText.length() - txt.length() - 2)/2.0;

    ui->txtEncoding->insertPlainText(
                                     "======================================================\n"
                                     + mainText.left((int) ceil(padding))
                                     + " " + txt.toUpper() + " "
                                     + mainText.right((int) floor(padding)) + "\n"
                                     + "======================================================\n"
                                    );
    ui->txtEncoding->ensureCursorVisible();
}

void QEncodingDialog::setupButtonBoxes(ButtonToShow btn)
{
    bool showCancel = (btn == Cancel);

    QList<QAbstractButton *> lstButtons;

    lstButtons = ui->bbTop->buttons();
    foreach (QAbstractButton *btn, lstButtons){
        if (ui->bbTop->standardButton(btn) == QDialogButtonBox::Cancel){
            btn->setVisible(showCancel);
        }else{
            btn->setHidden(showCancel);
        }
    }

    lstButtons = ui->bbBottom->buttons();
    foreach (QAbstractButton *btn, lstButtons){
        if (ui->bbBottom->standardButton(btn) == QDialogButtonBox::Cancel){
            btn->setVisible(showCancel);
        }else{
            btn->setHidden(showCancel);
        }
    }
}

void QEncodingDialog::cleanupResidualFiles()
{
    foreach (QString filePath, createdFiles){
        QFile(filePath).remove();
    }
}
