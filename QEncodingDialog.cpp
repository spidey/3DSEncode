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

    startWorking();
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

void QEncodingDialog::processRead(){}

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

void QEncodingDialog::startWorking()
{
    state = Downloading;
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

void QEncodingDialog::workFinished(QString bannerText)
{
    state = Idle;

    ui->pbEncoding->setMaximum(100);
    ui->pbEncoding->setValue(100);
    addBannerToLog(bannerText);

    setupButtonBoxes(Close);
}

void QEncodingDialog::addBannerToLog(QString txt)
{
    QString mainText("######################################################");
    float padding = (mainText.length() - txt.length() - 2)/2.0;

    ui->txtEncoding->insertPlainText(
                                     QString(ui->txtEncoding->toPlainText().isEmpty() ? "" : "\n")
                                     + "======================================================\n"
                                     + mainText.left((int) ceil(padding))
                                     + " " + txt.toUpper() + " "
                                     + mainText.right((int) floor(padding)) + "\n"
                                     + "======================================================\n"
                                    );
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
