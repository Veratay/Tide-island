import QtQuick
import IslandBackend

Rectangle {
    id: root

    property var items: []
    property var cavaLevels: []
    property real revealProgress: 0
    property real edgeMargin: 12
    property real maximumWidth: 400

    width: Math.min(maximumWidth, content.preferredWidth)
    height: UserConfig.islandHeight
    y: UserConfig.islandTopMargin
        - (1 - revealProgress) * (height + UserConfig.islandTopMargin + 8)
    visible: items.length > 0 && width > 0 && revealProgress > 0.001
    opacity: revealProgress
    scale: 0.96 + revealProgress * 0.04
    transformOrigin: Item.Top
    radius: height / 2
    color: Qt.rgba(StyleTokens.panel.r, StyleTokens.panel.g, StyleTokens.panel.b,
        UserConfig.islandBackgroundOpacity / 100.0)
    border.width: 1
    border.color: StyleTokens.surfaceBorder
    clip: true
    antialiasing: true
    z: 4

    SwipeCustomInfoLayer {
        id: content
        items: root.items
        cavaLevels: root.cavaLevels
        minimumWidth: 0
        maximumWidth: root.maximumWidth
        transitionProgress: -1
        showCondition: true
        showSecondaryText: false
        textFontFamily: UserConfig.heroFontFamily
        textPixelSize: UserConfig.bodyFontSize
        iconPixelSize: UserConfig.iconFontSize
    }
}
