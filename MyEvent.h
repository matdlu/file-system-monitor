#ifndef MYEVENT_H
#define MYEVENT_H
#include <QString>
#include <QFileInfo>

enum class MyFileType {
    FILE,
    FOLDER,
};

enum class MyEventType {
    CREATED,
    DELETED,
    EDITED,
    RENAMED,
};

class MyEvent {
    private:
        QString _fileLocation;
        MyFileType _fileType;
        MyEventType _eventType;

    public:
        MyEvent(QString fileLocation, MyFileType fileType, MyEventType eventType) {
            this->_fileLocation = fileLocation;
            this->_fileType = fileType;
            this->_eventType = eventType;
        };
        MyEvent(QString fileLocation, const QFileInfo &info, MyEventType eventType) : MyEvent(fileLocation, fileType(info), eventType) {};

        QString fileLocation() {
            return this->_fileLocation;
        }
        MyFileType fileType() {
            return this->_fileType;
        }
        MyEventType eventType() {
            return this->_eventType;
        }

        static MyFileType fileType(const QFileInfo& info) {
            return info.isDir() ? MyFileType::FOLDER : MyFileType::FILE;
        }

        static QString fileTypeToString(const MyFileType fileType) {
            return fileType == MyFileType::FILE ? "file" : "folder";
        }

        QString toString() {
            switch (_eventType) {
                case MyEventType::CREATED: return fileTypeToString(_fileType) + " created: " + _fileLocation;
                case MyEventType::DELETED: return fileTypeToString(_fileType) + " deleted: " + _fileLocation;
                case MyEventType::EDITED:  return fileTypeToString(_fileType) + " edited: "  + _fileLocation;
                case MyEventType::RENAMED: return fileTypeToString(_fileType) + " renamed: " + _fileLocation;
            }
            return 0;
        }
};

#endif // MYEVENT_H
