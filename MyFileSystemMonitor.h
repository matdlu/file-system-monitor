#ifndef MYFILESYSTEMMONITOR_H
#define MYFILESYSTEMMONITOR_H
#include <QtCore/QObject>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QTextStream>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QDateTime>
#include <MyEvent.h>

class MyFileSystemMonitor : public QObject
{
Q_OBJECT

private:
    QDir _dir;
    QFileSystemWatcher _watcher;
    QFile _logFile;

    static bool compareFileInfo(const QFileInfo &a, const QFileInfo &b) {
        return a.isDir() ? a.birthTime() == b.birthTime() : a.size() == b.size() && a.birthTime() == b.birthTime();
    }

    void log(MyEvent ev) {
        QString str = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") + ev.toString();
        QTextStream(stdout) << str << Qt::endl;
        _logFile.write(str.toUtf8()+"\n");
        _logFile.flush();
    }

public:
    MyFileSystemMonitor(const QString &path, const QString &logPath) : QObject(), _dir(path), _logFile(logPath)
    {
        if ( logPath.contains(_dir.path()) ) // will create infinite loop due to log file being updated, other solution would be to ignore the logfile
            qFatal("Log file cannot exist inside monitored directory.");

        _logFile.open(QIODevice::Append);

        _dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

        if ( ! _watcher.addPath(path) )
            qFatal("Path cannot be monitored.");

        for(QFileInfo &info : _dir.entryInfoList())
            _watcher.addPath(info.filePath());

        this->connect(&_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)));
        this->connect(&_watcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)));
    }

public slots:
    void directoryChanged(const QString &path) // File/Folder Created, File/Folder Deleted, File/Folder Renamed
    {
        if ( path != _dir.path() ) return; // do not consider subfolders

        QFileInfoList maybeRenamedOrDeleted;

        // old files
        for(QFileInfo &info : _dir.entryInfoList()) {
            if ( ! QFile::exists(info.filePath()) ) {
                maybeRenamedOrDeleted.append(info);
                _watcher.removePath(info.filePath());
            }
        }

        // new files
        _dir.refresh();
        for(QFileInfo &info : _dir.entryInfoList()) {
            if ( _watcher.addPath(info.filePath()) ) {
                for(QFileInfo &renamedInfo : maybeRenamedOrDeleted) {
                    if ( compareFileInfo(renamedInfo, info) ) {
                        log(MyEvent(renamedInfo.filePath(), info, MyEventType::RENAMED));
                        maybeRenamedOrDeleted.removeOne(renamedInfo);
                    }
                }

                log(MyEvent(info.filePath(), info, MyEventType::CREATED));
            }
        }

        for(QFileInfo &info : maybeRenamedOrDeleted) {
            log(MyEvent(info.filePath(), info, MyEventType::DELETED));
        }
    }

    void fileChanged(const QString &path) // File Edited
    {
        if ( QFile::exists(path) )
            log(MyEvent(path, MyFileType::FILE, MyEventType::EDITED));
    }
};

#endif // MYFILESYSTEMMONITOR_H
