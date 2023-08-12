import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 558
    height: 624
    title: "BLE Communication"

    Rectangle {
        anchors.fill: parent

        RowLayout {
            anchors.fill: parent

            // Left layout
            GridLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                columns: 2

                SpinBox {}

                Button {
                    text: "TOP"
                }

                Button {
                    text: "TOP LEFT"
                }

                Button {
                    text: "TOP RIGHT"
                }

                Slider {
                    value: 49
                }

                Slider {
                    value: 49
                    orientation: Qt.Vertical
                }

                Button {
                    text: "BOTTOM LEFT"
                }

                Button {
                    text: "BOTTOM RIGHT"
                }

                Button {
                    text: "BOTTOM"
                }
            }

            // Right layout
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                TextField {
                    placeholderText: "Enter IP Address..."
                    text: "24:7F:0E:E6:AE:34"
                }

                Button {
                    text: "Connect"
                }

                Button {
                    text: "Disconnect"
                    enabled: false
                }

                Button {
                    text: "Start Communication"
                    enabled: false
                }

                Button {
                    text: "Stop Communication"
                    enabled: false
                }

                TextField {
                    placeholderText: "Enter Message..."
                }

                Button {
                    text: "Send Message"
                    enabled: false
                }

                TextArea {
                    readOnly: true
                }
            }
        }
    }
}
