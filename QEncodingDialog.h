#ifndef QENCODINGDIALOG_H
#define QENCODINGDIALOG_H

#include <QDialog>
#include <QMap>
#include <QProcess>
#include <QAbstractButton>

namespace Ui {
    class QEncodingDialog;
}

class QEncodingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QEncodingDialog(QMap<QString, QString> &args, QWidget *parent = 0);
    ~QEncodingDialog();

private slots:
    void on_btnAdvanced_toggled(bool toggled);
    void processRead();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    enum ProcessingState{
        Idle,
        Downloading,
        Encoding
    };

    enum ButtonToShow{
        Cancel,
        Close
    };

    Ui::QEncodingDialog *ui;
    QMap<QString, QString> args;
    QProcess process;
    QStringList createdFiles;
    ProcessingState state;
    //methods
    void reject();
    void startDownloading();
    void startEncoding();
    bool confirmCancel();
    void workFinished(QString msg);
    void writeBannerToLog(QString txt);
    void setupButtonBoxes(ButtonToShow btn);
    void cleanupResidualFiles();
};

#endif // QENCODINGDIALOG_H
