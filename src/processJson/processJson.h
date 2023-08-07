#ifndef PROCESS_JSON_H
#define PROCESS_JSON_H

#include <QByteArray>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>

#include <initializer_list>

class ProcessJson {
public:
    QByteArray generateMessage(const QString &type, const QJsonArray &data) {
        QJsonObject object;
        object.insert("type", type);
        object.insert("data", data);
        return QJsonDocument(object).toJson(QJsonDocument::Compact);
    }

    template<typename DataType>
    QByteArray generateMessage(const QString &type, const std::initializer_list<DataType> &data) {
        QJsonArray array;
        for (auto item: data)
            array.push_back(item);
        return generateMessage(type, array);
    }

    QPair<QJsonValue, QJsonValue> loadJson(const QByteArray &json) {
        QJsonParseError jsonError;
        auto jsonDoc = QJsonDocument::fromJson(json, &jsonError);
        if (jsonError.error != QJsonParseError::NoError) {
            qDebug() << "[processJson.h] Json error, json data ignored.";
            return {};
        }
        auto jsonObj = jsonDoc.object();
        return {jsonObj.value("type"), jsonObj.value("data")};
    }
};

#endif //PROCESS_JSON_H
