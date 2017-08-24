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

Page {
    Container {
        layout: AbsoluteLayout {

        }
        property int current: -1
        property int tap: 0
        onTouch: {
            if (event.touchType == TouchType.Move) {

                if (current == -1) {
                    current = event.localX;
                }
                if (ui.sdu(10) < (event.localX - current)) {
                    word1.wordText = qsTr("Right") + (event.localX - current);
                    word1.nextWord();
                } else if (ui.sdu(10) < (current - event.localX)) {
                    word1.wordText = qsTr("Left") + (current - event.localX);
                    word1.previousWord();
                }
            } else if (event.touchType == TouchType.Up) {
                //
            } else {
                current = event.localX;
                word1.wordText = qsTr("---")
            }
        }
        Button {
            text: qsTr("Create")
            maxWidth: 200
            layoutProperties: AbsoluteLayoutProperties {
                positionX: 350
                positionY: 600
            }
            onClicked: {
                _app.createRecord("a", "b", "c", "d")
            }
        }
        Button {
            text: qsTr("Read")
            maxWidth: 200
            layoutProperties: AbsoluteLayoutProperties {
                positionX: 100
                positionY: 600
            }
            onClicked: {
                _app.readRecords();
            }
        }
        Word {
            id: word1
            layoutProperties: AbsoluteLayoutProperties {
                positionX: 0
                positionY: ui.du(5)
            }
            showNextAnimStartX: - ui.sdu(80)
            showPreAnimStartX: ui.sdu(80)
            onChangeNextWord: {
                nextWordTail();
            }
            onChangePreviosWord: {
                previosWordTail();
            }
        }
        ListView {
            layoutProperties: AbsoluteLayoutProperties {
                positionX: 0
                positionY: 200
            }
            horizontalAlignment: HorizontalAlignment.Fill

            dataModel: _app.dataModel

            listItemComponents: [
                ListItemComponent {
                    type: "item"
                    StandardListItem {
                        title: qsTr("%1 %2").arg(ListItemData.word).arg(ListItemData.ipa)
                        description: qsTr("Unique Key: %1").arg(ListItemData.id)
                    }
                }
            ]
            accessibility.name: "List"
        }

    }
}
