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

TabbedPane {
    showTabsOnActionBar: true
    Tab { //First tab
        // Localized text with the dynamic translation and locale updates support
        title: qsTr("Tab 1") + Retranslate.onLocaleOrLanguageChanged
        Page {

            Container {
                navigation.childrenBehavior: ChildrenNavigationBehavior.Default
                property int clicked: 0
                onClickedChanged: {
                    word.text = "clicked " + clicked;
                    clicked ++;
                }

                Label {
                    text: qsTr(" ")
                }
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
                        text: qsTr("noun ")
                        textStyle.fontSize: FontSize.Small
                        textStyle.fontStyle: FontStyle.Italic
                        horizontalAlignment: HorizontalAlignment.Left
                        rightMargin: 10.0
                        verticalAlignment: VerticalAlignment.Top
                        implicitLayoutAnimationsEnabled: true

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

            }
        }
    } //End of first tab
    Tab { //Second tab
        title: qsTr("Tab 2") + Retranslate.onLocaleOrLanguageChanged
        Page {
            Container {
                Label {
                    text: qsTr("Second tab") + Retranslate.onLocaleOrLanguageChanged
                }
            }
        }
    } //End of second tab
}
