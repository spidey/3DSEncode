#include <QtGui/QApplication>
#include "q3dsencode.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Q3DSEncode w;
    w.setFixedSize(w.sizeHint());
    w.show();

    return app.exec();
}
