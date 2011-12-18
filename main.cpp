#include <QtGui/QApplication>
#include "Q3DSEncode.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Q3DSEncode w;
    w.show();

    return app.exec();
}
