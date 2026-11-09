import QtQuick
import IslandBackend

QtObject {
    // Older backends retain the initial three-island layout until upgraded.
    readonly property var savedLayout: typeof UserConfig.customPageLayout === "undefined"
        ? ({}) : UserConfig.customPageLayout
    readonly property var defaultLeftItems: ["date", "time"]
    readonly property var defaultRightItems: ["volume", "brightness", "cpu", "ram", "battery"]
    readonly property var leftItems: itemList(savedLayout.leftItems, defaultLeftItems)
    readonly property var rightItems: itemList(savedLayout.rightItems, defaultRightItems)
    readonly property var centerItems: itemList(savedLayout.centerItems,
        Array.from(UserConfig.dynamicIslandLeftSwipeItems).filter(id =>
            defaultLeftItems.indexOf(id) === -1 && defaultRightItems.indexOf(id) === -1))
    readonly property real edgeMargin: boundedNumber(savedLayout.edgeMargin, 12)
    readonly property real gap: boundedNumber(savedLayout.gap, 16)

    function itemList(value, fallback) {
        if (!value || typeof value === "string" || typeof value.length !== "number")
            return fallback;
        return Array.from(value);
    }

    function boundedNumber(value, fallback) {
        const number = Number(value === undefined ? fallback : value);
        return isFinite(number) ? Math.max(0, Math.min(200, Math.round(number))) : fallback;
    }
}
