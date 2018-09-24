#pragma once

#include <QUuid>
#include <QString>
#include <QObject>
#include <memory>
#include <QUrl>

namespace mobsya {
class ThymioDeviceManagerClientEndpoint;
class ThymioNode : public QObject {
    Q_OBJECT
public:
    enum class Status { Connected = 1, Available = 2, Busy = 3, Ready = 4, Disconnected = 5 };
    enum class NodeType { Thymio2 = 0, Thymio2Wireless = 1, SimulatedThymio2 = 2, DummyNode = 3, UnknownType = 4 };

    Q_ENUM(Status)
    Q_ENUM(NodeType)

    Q_PROPERTY(QUuid id READ uuid CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(NodeType type READ type CONSTANT)
    Q_PROPERTY(QUrl websocketEndpoint READ websocketEndpoint CONSTANT)


    ThymioNode(std::shared_ptr<ThymioDeviceManagerClientEndpoint>, const QUuid& uuid, const QString& name,
               mobsya::ThymioNode::NodeType type, QObject* parent = nullptr);

Q_SIGNALS:
    void nameChanged();
    void statusChanged();

public:
    QUuid uuid() const;
    QString name() const;
    Status status() const;
    NodeType type() const;
    QUrl websocketEndpoint() const;

    void setName(const QString& name);
    void setStatus(const Status& status);

    std::shared_ptr<ThymioDeviceManagerClientEndpoint> endpoint() const {
        return m_endpoint;
    }

private:
    std::shared_ptr<ThymioDeviceManagerClientEndpoint> m_endpoint;
    QUuid m_uuid;
    QString m_name;
    Status m_status;
    NodeType m_type;
};

}  // namespace mobsya

Q_DECLARE_METATYPE(mobsya::ThymioNode*)
Q_DECLARE_METATYPE(mobsya::ThymioNode::Status)
Q_DECLARE_METATYPE(mobsya::ThymioNode::NodeType)
