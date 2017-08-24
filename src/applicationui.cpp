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

#include "applicationui.hpp"
#include "Word.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/data/SqlDataAccess>
#include <bb/data/DataAccessError>
#include <bb/cascades/LocaleHandler>
#include <QtSql/QtSql>
#include <bb/system/SystemDialog>

using namespace bb::cascades;
using namespace bb::data;
using namespace bb::system;

const QString DB_PATH = "./data/proDictDB.db";

ApplicationUI::ApplicationUI() :
        m_dataModel(0)
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);
    initDataModel();
    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this,
            SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("_app", this);
    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    Application::instance()->setScene(root);
    const bool dbInited = initDatabase();
    root->setProperty("databaseOpen", dbInited);
}

void ApplicationUI::initDataModel()
{
    // Note: The Group Data Model is joining this objects tree as a child (for memory management)
    m_dataModel = new GroupDataModel(this);
    m_dataModel->setSortingKeys(QStringList() << "id");
    m_dataModel->setGrouping(ItemGrouping::None);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("ProundDict_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

bool ApplicationUI::initDatabase()
{
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(DB_PATH);

    if (database.open() == false) {
        const QSqlError error = database.lastError();
        alert(tr("Error opening connection to the database: %1").arg(error.text()));
        qDebug() << "\nDatabase NOT opened.";
        return false;
    }

    SqlDataAccess *sqlda = new SqlDataAccess(DB_PATH);
    sqlda->execute("DROP TABLE IF EXISTS words");
    if (!sqlda->hasError()) {
        qDebug() << "Table dropped.";
    } else {
        const DataAccessError error = sqlda->error();
        alert(tr("Drop table error: %1").arg(error.errorMessage()));
    }

    const QString createSQL = "CREATE TABLE words "
            "  (id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "  word VARCHAR, "
            "  ipa VARCHAR, "
            "  type VARCHAR, "
            "  meaning VARCHAR);";
    sqlda->execute(createSQL);
    if (!sqlda->hasError()) {
        qDebug() << "Table created.";
    } else {
        const DataAccessError error = sqlda->error();
        alert(tr("Create table error: %1").arg(error.errorMessage()));
        return false;
    }

    return true;
}
GroupDataModel* ApplicationUI::dataModel() const
{
    return m_dataModel;
}
bool ApplicationUI::createRecord()
{
    SqlDataAccess *sqlda = new SqlDataAccess(DB_PATH);
    QVariantList contact;
    QString word = "lass";
    QString ipa = "lass";
    QString type = "z";
    QString meaning = "asd";
    contact << word << ipa << type << meaning;
    sqlda->execute("INSERT INTO words"
            "    (word, ipa, type, meaning) "
            "    VALUES (:word, :ipa, :type, :meaning)", contact);

    bool success = false;
    if (!sqlda->hasError()) {
        alert(tr("Create record succeeded."));
        success = true;
    } else {
        const DataAccessError error = sqlda->error();
        alert(tr("Create record error: %1").arg(error.errorMessage()));
    }
    return success;
}

void ApplicationUI::readRecords()
{
    // 1. Create the local DB connection via SqlDataAccess instance. Note, creating instance
    //    Will automatically open a connection to the database.
    SqlDataAccess *sqlda = new SqlDataAccess(DB_PATH);

    // 2. Create a query to search for the records
    //    IMPORTANT NOTE: If accepting user input and not using bindings, be sure
    //    to escape it to allow quote characters, and prevent SQL Injection attacks.
    //    The below example is not a prepared statement and does not use bindings as
    //    there is no user input to accept.

    const QString sqlQuery = "SELECT id, word, ipa, type, meaning FROM words";

    QVariant result = sqlda->execute(sqlQuery);
    if (!sqlda->hasError()) {
        int recordsRead = 0;
        m_dataModel->clear();
        if (!result.isNull()) {
            QVariantList list = result.value<QVariantList>();
            recordsRead = list.size();
            for (int i = 0; i < recordsRead; i++) {
                QVariantMap map = list.at(i).value<QVariantMap>();
                Word *word = new Word(map["id"].toInt(), map["word"].toString(),
                        map["ipa"].toString(), map["type"].toString(), map["meaning"].toString());
                Q_UNUSED(word);
                m_dataModel->insert(word);
            }
        }

        qDebug() << "Read " << recordsRead << " records succeeded";

        if (recordsRead == 0) {
            alert(tr("The customer table is empty."));
        }
    } else {
        alert(tr("Read records failed: %1").arg(sqlda->error().errorMessage()));
    }
}
void ApplicationUI::alert(const QString &message)
{
    SystemDialog *dialog; // SystemDialog uses the BB10 native dialog.
    dialog = new SystemDialog(tr("OK"), 0); // New dialog with on 'Ok' button, no 'Cancel' button
    dialog->setTitle(tr("Alert")); // set a title for the message
    dialog->setBody(message); // set the message itself
    dialog->setDismissAutomatically(true); // Hides the dialog when a button is pressed.

    // Setup slot to mark the dialog for deletion in the next event loop after the dialog has been accepted.
    bool ok = connect(dialog, SIGNAL(finished(bb::system::SystemUiResult::Type)), dialog,
            SLOT(deleteLater()));
    Q_ASSERT(ok);
    dialog->show();
}

