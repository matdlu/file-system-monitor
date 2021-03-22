#ifndef MYEVENT_H
#define MYEVENT_H
#include <QtCore/QString>
#include <QFileInfo>

enum class FileType {
    FILE,
    FOLDER,
};

enum class EventType {
    CREATED,
    DELETED,
    EDITED,
    RENAMED,
};

class MyEvent {
    private:
        QString _fileLocation;
        FileType _fileType;
        EventType _eventType;

    public:
        MyEvent(QString fileLocation, FileType fileType, EventType eventType) {
            this->_fileLocation = fileLocation;
            this->_fileType = fileType;
            this->_eventType = eventType;
        };
        MyEvent(QString fileLocation, const QFileInfo &info, EventType eventType) : MyEvent(fileLocation, fileType(info), eventType) {};

        QString fileLocation() {
            return this->_fileLocation;
        }
        FileType fileType() {
            return this->_fileType;
        }
        EventType eventType() {
            return this->_eventType;
        }

        static FileType fileType(const QFileInfo& info) {
            return info.isDir() ? FileType::FOLDER : FileType::FILE;
        }

        static QString fileTypeToString(const FileType fileType) {
            return fileType == FileType::FILE ? "file" : "folder";
        }

        QString toString() {
            switch (_eventType) {
                case EventType::CREATED: return fileTypeToString(_fileType) + " created: " + _fileLocation;
                case EventType::DELETED: return fileTypeToString(_fileType) + " deleted: " + _fileLocation;
                case EventType::EDITED:  return fileTypeToString(_fileType) + " edited: "  + _fileLocation;
                case EventType::RENAMED: return fileTypeToString(_fileType) + " renamed: " + _fileLocation;
            }
            return 0;
        }
};

#endif // MYEVENT_H
