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

private:
    enum ProcessingState{
        Idle,
        Downloading,
        Converting
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
    void startWorking();
    bool confirmCancel();
    void workFinished(QString bannerText);
    void addBannerToLog(QString txt);
    void setupButtonBoxes(ButtonToShow btn);
    void cleanupResidualFiles();
};

#endif // QENCODINGDIALOG_H
