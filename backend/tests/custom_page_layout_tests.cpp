#include "UserConfigBackend.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QTemporaryDir>
#include <QtTest>

class CustomPageLayoutTests : public QObject {
    Q_OBJECT
private slots:
    void reloadsSavedLayout() {
        QTemporaryDir temporary;
        QVERIFY(temporary.isValid());
        qputenv("XDG_CONFIG_HOME", temporary.path().toUtf8());
        QDir().mkpath(temporary.path() + "/tide-island");
        const QString path = temporary.path() + "/tide-island/userconfig.json";
        auto write = [&](const QVariantMap &data) {
            QFile file(path);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
            file.write(QJsonDocument::fromVariant(data).toJson());
            file.close();
            return true;
        };
        QVERIFY(write({{"dynamicIslandLeftSwipeItems", QVariantList{"time", "cava"}}}));
        UserConfigBackend config;
        QVERIFY(config.customPageLayout().isEmpty());
        QCOMPARE(config.dynamicIslandLeftSwipeItems(), (QVariantList{"time", "cava"}));
        const QVariantMap layout {
            {"leftItems", QVariantList{"date", "time"}},
            {"centerItems", QVariantList{"cava", "albumcover"}},
            {"rightItems", QVariantList{}},
            {"edgeMargin", 24}, {"gap", 32}
        };
        QSignalSpy changes(&config, &UserConfigBackend::customPageLayoutChanged);
        QVERIFY(write({{"customPageLayout", layout}}));
        QTRY_COMPARE(config.customPageLayout(), layout);
        QCOMPARE(changes.count(), 1);
        QVERIFY(config.customPageLayout().value("rightItems").toList().isEmpty());
        QVERIFY(write({}));
        QTRY_VERIFY(config.customPageLayout().isEmpty());
    }
};
QTEST_GUILESS_MAIN(CustomPageLayoutTests)
#include "custom_page_layout_tests.moc"
