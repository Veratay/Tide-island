#include "backend.hpp"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QTemporaryDir>
#include <QtTest>

class CustomPageEditorTests : public QObject {
    Q_OBJECT
private slots:
    void preservesGroupsAndEmptySelections() {
        QTemporaryDir temporary;
        QVERIFY(temporary.isValid());
        qputenv("XDG_CONFIG_HOME", temporary.path().toUtf8());
        QDir().mkpath(temporary.path() + "/tide-island");
        QFile initial(temporary.path() + "/tide-island/userconfig.json");
        QVERIFY(initial.open(QIODevice::WriteOnly));
        initial.write(R"({"dynamicIslandLeftSwipeItems":["date","time","albumcover","cava","cpu"],"islandWidth":140})");
        initial.close();
        const QString module = temporary.path() + "/TideIsland";
        QDir().mkpath(module);
        for (const QString &name : {"CustomPage.qml", "ConfigStore.qml", "Theme.qml"})
            QVERIFY(QFile::copy(QString(APP_SOURCE_DIR) + "/" + name, module + "/" + name));
        QFile qmldir(module + "/qmldir");
        QVERIFY(qmldir.open(QIODevice::WriteOnly));
        qmldir.write("module TideIsland\nCustomPage 1.0 CustomPage.qml\nsingleton ConfigStore 1.0 ConfigStore.qml\nsingleton Theme 1.0 Theme.qml\n");
        qmldir.close();
        Backend backend;
        QQmlApplicationEngine engine;
        engine.addImportPath(temporary.path());
        engine.rootContext()->setContextProperty("backend", &backend);
        QQmlComponent component(&engine, QUrl::fromLocalFile(module + "/CustomPage.qml"));
        QVERIFY2(component.isReady(), qPrintable(component.errorString()));
        QScopedPointer<QObject> page(component.create());
        QVERIFY2(page, qPrintable(component.errorString()));
        engine.globalObject().setProperty("page", engine.newQObject(page.data()));
        auto run = [&](const QString &script) {
            const auto result = engine.evaluate(script);
            if (result.isError()) qWarning() << result.toString();
            return result;
        };
        QCOMPARE(run("JSON.stringify(page.selectedIds())").toString(), "[\"albumcover\",\"cava\"]");
        run("page.selectGroup(0); page.addItem('weather', 1)");
        QCOMPARE(run("JSON.stringify(page.selectedIds())").toString(), "[\"date\",\"weather\",\"time\"]");
        run("page.moveItem(2, 0)");
        QCOMPARE(run("JSON.stringify(page.selectedIds())").toString(), "[\"time\",\"date\",\"weather\"]");
        run("page.selectGroup(1); page.removeItem(1); page.removeItem(0)");
        run("page.selectGroup(2); page.addItem('storage', 0)");
        run("page.loadFromConfig(); page.selectGroup(1)");
        QCOMPARE(run("JSON.stringify(page.selectedIds())").toString(), "[]");
        const auto layout = backend.userConfig().value("customPageLayout").toMap();
        QCOMPARE(layout.value("leftItems").toList(), (QVariantList{"time", "date", "weather"}));
        QVERIFY(layout.value("centerItems").toList().isEmpty());
        QCOMPARE(layout.value("rightItems").toList().first().toString(), "storage");
        QCOMPARE(backend.userConfig().value("islandWidth").toInt(), 140);
        QFile saved(backend.userConfigPath());
        QVERIFY(saved.open(QIODevice::ReadOnly));
        const auto savedLayout = QJsonDocument::fromJson(saved.readAll()).object().value("customPageLayout").toObject();
        QCOMPARE(savedLayout.toVariantMap(), layout);
    }
};
QTEST_MAIN(CustomPageEditorTests)
#include "custom_page_editor_tests.moc"
