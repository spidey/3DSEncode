#ifndef Q3DSENCODE_H
#define Q3DSENCODE_H

#include <QMainWindow>
#include <QProcess>
#include <QProgressDialog>
#include <QSettings>

namespace Ui {
    class Q3DSEncode;
}

class Q3DSEncode : public QMainWindow
{
    Q_OBJECT

public:
    explicit Q3DSEncode(QWidget *parent = 0);
    ~Q3DSEncode();

protected:
    void changeEvent(QEvent *e);

private slots:
    //Main actions and menu
    void on_actionOpen_triggered();
    void on_actionSaveTo_triggered();
    void on_actionConvert_triggered();
    void on_actionAbout_triggered();
    void on_actionLanguage_triggered();
    void on_actionLoadSettings_triggered();
    //Encode process management
    void handle_processEncode_started();
    void handle_processEncode_finished(int, QProcess::ExitStatus);
    void handle_dialogEncode_canceled();
    
private:
    Ui::Q3DSEncode *ui;
    QProcess processEncode;
    QProgressDialog dialogEncode;
    QSettings settings;
    //Methods
    void closeEvent(QCloseEvent *);
    void makeConnections();
    void configureUi();
    bool validateInput();
    void saveSettings();
    void startEncodingProcess(QStringList &);
    void cleanupResidualFiles();
};

#endif // Q3DSENCODE_H
