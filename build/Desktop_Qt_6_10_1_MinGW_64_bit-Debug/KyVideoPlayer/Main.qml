import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Controls.Basic
import CustomFBO 1.0

Window
{
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    color: active ? palette.active.window : palette.inactive.window

    FileDialog
    {
        id: openDialog
        title: "Select a file"
        nameFilters: ["Video files (*.avi *.wmv)", "Audio files (*.wav *.wma)", "All files (*)"]

        onAccepted:
        {
            urlField.text = selectedFile
            console.log("Selected file:", selectedFile)
        }

        onRejected:
        {
            console.log("User canceled")
        }
    }

    VideoOutputFBO
    {
        id: videoOutput
        width: parent.width
        height: parent.height * 0.7
        visible: true

        anchors
        {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: bottomButtons.top
            topMargin: 40
            bottomMargin: 80
        }
    }

    // URL Input
    TextField
    {
        id: urlField
        placeholderText: "Please input the URL..."
        width: parent.width * 0.9
        height: 40
        anchors.horizontalCenter: parent.horizontalCenter

        anchors.bottom: bottomButtons.top
        anchors.bottomMargin: 20
    }

    // Bottom button area
    Row
    {
        id: bottomButtons
        spacing: 20

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30

        Button
        {
            id: btnplay
            text: "Play"
            width: 120
            height: 40

            background: Rectangle {
                radius: 6
                color: btnplay.down ? "#be81b9ff" : "#b17fadff"
            }

            contentItem: Text {
                text: btnplay.text
                color: "white"
                anchors.centerIn: parent
                anchors.fill: parent
                anchors.margins: 4

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                font.pixelSize: height * 0.5
                font.bold: true
            }

            onClicked:
            {
                if (urlField.text && urlField.text.length > 0)
                    playerController.play(urlField.text)
            }
        }

        Button
        {
            id: btnStop
            text: "Stop"
            width: 120
            height: 40

            background: Rectangle {
                radius: 6
                color: btnStop.down ? "#be81b9ff" : "#b17fadff"
            }

            contentItem: Text {
                text: btnStop.text
                color: "white"
                anchors.centerIn: parent
                anchors.fill: parent
                anchors.margins: 4

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                font.pixelSize: height * 0.5
                font.bold: true
            }

            // onClicked: openDialog.open()
            onClicked: playerController.stop()
        }
    }

    Component.onCompleted:
    {
        playerController.setVideoOutput(videoOutput)
    }
}
