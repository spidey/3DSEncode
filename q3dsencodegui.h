#ifndef Q3DSENCODEGUI_H
#define Q3DSENCODEGUI_H

#include <QMainWindow>
#include <QProcess>
#include <QProgressDialog>

namespace Ui {
    class Q3DSEncodeGUI;
}

class Q3DSEncodeGUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit Q3DSEncodeGUI(QWidget *parent = 0);
    ~Q3DSEncodeGUI();

protected:
    void changeEvent(QEvent *e);

private slots:
    //Main actions and menu
    void on_actionOpen_triggered();
    void on_actionSaveTo_triggered();
    void on_actionConvert_triggered();
    void on_actionAbout_triggered();
    void on_actionTutorial_triggered();
    void on_actionLanguage_triggered();
    //Encode process management
    void handle_processEncode_started();
    void handle_processEncode_finished(int, QProcess::ExitStatus);
    void handle_dialogEncode_canceled();
    
private:
    Ui::Q3DSEncodeGUI *ui;
    QProcess processEncode;
    QProgressDialog dialogEncode;
    //Methods
    bool validateInput();
    void startEncodingProcess(QStringList &);
    void cleanupResidualFiles();
};

#endif // Q3DSENCODEGUI_H
