#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <MyFileSystemMonitor.h>

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    if ( argc <= 1) {
        qFatal("No arguments given. First argument is a path to be monitored.");
    }

    QString cwd = argv[1]; // path to monitor, current working directory

    auto cwdInfo = QFileInfo(cwd);
    if ( ! cwdInfo.exists() ) {
        qFatal("Path does not exist.");
    } else if ( ! cwdInfo.isDir() ) {
        qFatal("Path is not a directory.");
    }

    MyFileSystemMonitor monitor(cwd, "/home/user/logs.txt");

    return app.exec();
}
