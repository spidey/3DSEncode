#include <QtGui/QApplication>
#include "q3dsencodegui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Q3DSEncodeGUI w;
    w.setFixedSize(w.sizeHint());
    w.show();

    return a.exec();
}
