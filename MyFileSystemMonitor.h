#ifndef MYFILESYSTEMMONITOR_H
#define MYFILESYSTEMMONITOR_H
#include <QtCore/QObject>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QTextStream>
#include <QtCore/QDir>

class MyFileSystemMonitor : public QObject
{
Q_OBJECT

private:
    QDir _dir;
    QFileSystemWatcher _watcher;

    void addNewFiles()
    {
        _dir.refresh();
        for(QString &fileName : _dir.entryList())
            _watcher.addPath(_dir.path() + "/" + fileName);
    }
    void contentsChanged()
    {
        QTextStream(stdout) << "contents of the folder have been changed" << Qt::endl;
    }

public:
    MyFileSystemMonitor(const QString& path) : QObject(), _dir(path)
    {
        _dir.setFilter(QDir::Files);

        _watcher.addPath(path);
        addNewFiles();

        this->connect(&_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged()));
        this->connect(&_watcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged()));
    }

public slots:
    void directoryChanged()
    {
        addNewFiles();

        contentsChanged();
    }
    void fileChanged()
    {
        contentsChanged();
    }

};

#endif // MYFILESYSTEMMONITOR_H
