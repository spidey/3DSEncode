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
    void changeEvent(QEvent *event);

private slots:
    //Main actions and menu
    void on_actionOpen_triggered();
    void on_actionSaveTo_triggered();
    void on_actionConvert_triggered();
    void on_actionAbout_triggered();
    void on_actionLanguage_triggered();
    void on_actionLoadSettings_triggered();

private:
    Ui::Q3DSEncode *ui;
    QSettings settings;
    //Methods
    void closeEvent(QCloseEvent *event);
    void configureUi();
    bool validateInput();
    void saveSettings();
};

#endif // Q3DSENCODE_H
