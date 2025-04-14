#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QVariantList>
#include <QMetaType>

#include "DatabaseManager.h"
#include "TransactionModel.h"
#include "NetworkManager.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    qRegisterMetaType<QVariantList>("QVariantList");
#endif

    QGuiApplication app(argc, argv);

    app.setOrganizationName("KatyaСorporation");        // имя компании
    app.setOrganizationDomain("KatyaСorporation.com");  // домен компании
    app.setApplicationName("CoinTrol");                 // имя приложения
    app.setApplicationVersion("0.0.1");                 // версия приложения

    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::Russia));

    QQmlApplicationEngine engine;

    DatabaseManager dbManager;
    TransactionModel transactionModel(&dbManager);
    NetworkManager networkManager;

    engine.rootContext()->setContextProperty("dbManager", &dbManager);
    engine.rootContext()->setContextProperty("transactionModel", &transactionModel);
    engine.rootContext()->setContextProperty("netManager", &networkManager);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
