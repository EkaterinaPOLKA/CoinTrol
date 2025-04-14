#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkInterface>

class NetworkManager : public QObject {
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE QString getUserIP() {
        for (const QNetworkInterface &interface : QNetworkInterface::allInterfaces()) {
            if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
                !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
                for (const QNetworkAddressEntry &entry : interface.addressEntries()) {
                    if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                        return entry.ip().toString();
                    }
                }
            }
        }
        return "127.0.0.1";
    }
};

#endif // NETWORKMANAGER_H
