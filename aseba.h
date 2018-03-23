#ifndef ASEBA_CLIENT_H
#define ASEBA_CLIENT_H

#include <QVariantMap>
#include <QThread>
#include <QTimer>
#include "aseba/common/msg/msg.h"
#include "aseba/common/msg/NodesManager.h"
#include "aseba/compiler/compiler.h"
#include "thymio/ThymioManager.h"

class AsebaDescriptionsManager : public QObject, public Aseba::NodesManager {
    Q_OBJECT
signals:
    void nodeProtocolVersionMismatch(unsigned nodeId, const std::wstring& nodeName,
                                     uint16_t protocolVersion) Q_DECL_OVERRIDE;
    void nodeDescriptionReceived(unsigned nodeId) Q_DECL_OVERRIDE;
    void nodeConnected(unsigned nodeId) Q_DECL_OVERRIDE;
    void nodeDisconnected(unsigned nodeId) Q_DECL_OVERRIDE;
    void sendMessage(const Aseba::Message& message) Q_DECL_OVERRIDE;
public slots:
    void pingNetwork() {
        Aseba::NodesManager::pingNetwork();
    }
};

class AsebaNode;

class AsebaClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(const QList<QObject*> nodes MEMBER nodes NOTIFY nodesChanged)
    QThread thread;
    AsebaDescriptionsManager m_nodesManager;
    mobsya::ThymioManager m_thymioManager;
    QTimer managerTimer;
    std::unique_ptr<mobsya::DeviceQtConnection> m_connection;
    QList<QObject*> nodes;

public:
    AsebaClient();
    ~AsebaClient();
public slots:
    void start(QString target = ASEBA_DEFAULT_TARGET);
    void send(const Aseba::Message& message);
    void messageReceived(std::shared_ptr<Aseba::Message>);
    void sendUserMessage(int eventId, QList<int> args);
signals:
    void userMessage(unsigned type, QList<int> data);
    void nodesChanged();
    void connectionError(QString source, QString reason);

private:
    void connect(const mobsya::ThymioInfo& thymio);
};

class AsebaNode : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    unsigned nodeId;
    Aseba::TargetDescription description;
    Aseba::VariablesMap variablesMap;

public:
    explicit AsebaNode(AsebaClient* parent, unsigned nodeId,
                       const Aseba::TargetDescription* description);
    AsebaClient* parent() const {
        return static_cast<AsebaClient*>(QObject::parent());
    }
    unsigned id() const {
        return nodeId;
    }
    QString name() {
        return QString::fromStdWString(description.name);
    }
    static Aseba::CommonDefinitions commonDefinitionsFromEvents(QVariantMap events);
public slots:
    void setVariable(QString name, QList<int> value);
    QString setProgram(QVariantMap events, QString source);
};

#endif    // ASEBA_CLIENT_H
