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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include "Word.hpp"
#include <bb/cascades/GroupDataModel>
using namespace bb::cascades;
namespace bb
{
    namespace cascades
    {
        class LocaleHandler;
    }
}

class QTranslator;

/*!
 * @brief Application UI object
 *
 * Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI: public QObject
{
    Q_OBJECT

    Q_PROPERTY(bb::cascades::DataModel* dataModel READ dataModel CONSTANT)
    Q_PROPERTY(QString word READ word CONSTANT)
    Q_PROPERTY(QString ipa READ ipa CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(QString meaning READ meaning CONSTANT)
public:
    ApplicationUI();

    Q_INVOKABLE void readRecords();
    Q_INVOKABLE bool createRecord();
    Q_INVOKABLE void showFilePicker();
    Q_INVOKABLE QString word() const;
    Q_INVOKABLE QString type() const;
    Q_INVOKABLE QString ipa() const;
    Q_INVOKABLE QString meaning() const;

private slots:
    void onSystemLanguageChanged();
    void onFileSelected(const QStringList& list);
    void onCanceled();
private:
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;
    void alert(const QString &message);
    void initDataModel();
    bool initDatabase();
    bb::cascades::GroupDataModel* dataModel() const;
    GroupDataModel* m_dataModel;
    QString m_word;
    QString m_type;
    QString m_meaning;
    QString m_ipa;

    int currentWord;
};

#endif /* ApplicationUI_HPP_ */
