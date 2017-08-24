import bb.cascades 1.4

Container {
    id: wordContainer
    maxWidth: ui.sdu(80)
    minWidth: ui.sdu(80)
    property alias showNextAnimStartX: afterNext.fromX
    property alias showPreAnimStartX: afterPrevious.fromX
    property alias wordText: word.text
    property int clicked: 0
    signal changeNextWord
    signal changePreviosWord
    layout: StackLayout {
    }
    animations: [
        TranslateTransition {
            id: beforeNext
            toX: ui.sdu(80)
            duration: 500
            easingCurve: StockCurve.CircularInOut
            onEnded: {
                changeNextWord();
            }
        },
        TranslateTransition {
            id: afterNext
            fromX: 0
            toX: 0
            duration: 500
            easingCurve: StockCurve.CircularInOut
        },

        TranslateTransition {
            id: afterPrevious
            fromX: ui.sdu(80)
            toX: 0
            duration: 500
            easingCurve: StockCurve.CircularInOut

        },
        TranslateTransition {
            id: beforePrevious
            toX: - ui.sdu(80)
            duration: 500
            easingCurve: StockCurve.CircularInOut
            onEnded: {
                changePreviosWord();
            }
        }

    ]

    Label {
        id: word
        text: qsTr("Question")
        horizontalAlignment: HorizontalAlignment.Center
        textStyle {
            fontSize: FontSize.XXLarge
            textAlign: TextAlign.Center
        }
    }
    Container {
        horizontalAlignment: HorizontalAlignment.Center
        layout: GridLayout {
            columnCount: 2
        }
        Label {
            id: type
            text: qsTr("adjective ")
            textStyle.fontSize: FontSize.Small
            textStyle.fontStyle: FontStyle.Italic
            horizontalAlignment: HorizontalAlignment.Left
            verticalAlignment: VerticalAlignment.Top
        }
        Label {
            id: ipa
            text: qsTr("/") + qsTr("ˈkwes.tʃən") + qsTr("/")
            textFormat: TextFormat.Plain
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            textStyle {
                fontFamily: 'DejaVu Sans'
                fontSize: FontSize.Large
                color: Color.create(191, 0, 255)
            }
        }
    }
    attachedObjects: [
        ImplicitAnimationController {
            id: offScreenController
            propertyName: "translationX"
            enabled: false
        }
    ]

    function nextWordTail() {
        translationX = 0;
        afterNext.play();
    }
    function nextWord() {
        beforeNext.play();
    }
    function previosWordTail() {
        translationX = 0;
        afterPrevious.play();
    }
    function previousWord() {
        beforePrevious.play();
    }
}
