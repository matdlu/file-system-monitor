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

    static bool compareFileInfo(const QFileInfo &a, const QFileInfo &b) {
        return a.isDir() ? a.birthTime() == b.birthTime() : a.size() == b.size() && a.birthTime() == b.birthTime();
    }
public:
    MyFileSystemMonitor(const QString& path) : QObject()
    {
        _dir = QDir(path);
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
                        QTextStream(stdout) << MyEvent(renamedInfo.filePath(), info, MyEventType::RENAMED).toString() << Qt::endl;
                        maybeRenamedOrDeleted.removeOne(renamedInfo);
                    }
                }

                QTextStream(stdout) <<  MyEvent(info.filePath(), info, MyEventType::CREATED).toString() << Qt::endl;
            }
        }

        for(QFileInfo &info : maybeRenamedOrDeleted) {
            QTextStream(stdout) <<  MyEvent(info.filePath(), info, MyEventType::DELETED).toString() << Qt::endl;
        }
    }
    void fileChanged(const QString &path) // File Edited
    {
        if ( QFile::exists(path) )
            QTextStream(stdout) << MyEvent(path, MyFileType::FILE, MyEventType::EDITED).toString() << Qt::endl;
    }

};

#endif // MYFILESYSTEMMONITOR_H
