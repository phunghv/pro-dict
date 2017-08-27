/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.4
import bb.cascades.pickers 1.0

Page {
    content: Container {
        layout: AbsoluteLayout {

        }
        id: root
        property int current: -1
        property int tap: 0
        property bool databaseOpen: false
        property int currentId: 0
        property string word
        property string ipa
        property string type
        property string meaning
        onTouch: {
            if (event.touchType == TouchType.Move) {

                if (current == -1) {
                    current = event.localX;
                }
                if (ui.sdu(10) < (event.localX - current)) {
                    wordView.wordText = root.word;
                    wordView.nextWord();
                } else if (ui.sdu(10) < (current - event.localX)) {
                    wordView.wordText = qsTr("Left") + (current - event.localX);
                    wordView.previousWord();
                }
            } else if (event.touchType == TouchType.Up) {
            } else {
                current = event.localX;
                wordView.wordText = qsTr("---")
            }
        }
        Word {
            id: wordView
            layoutProperties: AbsoluteLayoutProperties {
                positionX: 0
                positionY: ui.du(5)
            }
            showNextAnimStartX: - ui.sdu(80)
            showPreAnimStartX: ui.sdu(80)
            onChangeNextWord: {
                _app.readRecords();
                wordView.wordText = _app.word;
                nextWordTail();
            }
            onChangePreviosWord: {
                _app.createRecord();
                wordView.wordText = _app.word;
                previosWordTail();
            }
        }

        TextArea {
            layoutProperties: AbsoluteLayoutProperties {
                positionX: 0
                positionY: ui.sdu(30)
            }
            id: viewMeaning
            text: _app.meaning
        }
        Button {
            id: btnSelectFile
            text: qsTr("Load")
            layoutProperties: AbsoluteLayoutProperties {
                positionX: 0
                positionY: ui.sdu(50)
            }
            onClicked: {
                _app.showFilePicker();
            }
        }
    }
}
