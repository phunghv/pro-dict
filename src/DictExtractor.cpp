/*
 * DictExtractor.cpp
 *
 *  Created on: Aug 27, 2017
 *      Author: rio09
 */

#include "DictExtractor.hpp"
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <exception>
#include <QtCore/QtEndian>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtCore/QRegExp>
#include <bb/data/SqlDataAccess>
#include <bb/data/DataAccessError>
#include <QtSql/QtSql>

using namespace bb::data;

const QVector<QByteArray> DictExtractor::availableEncodings = QVector<QByteArray>() << "UTF-8"
        << "UTF-16LE" << "UTF-16BE" << "EUC-JP";

DictExtractor::DictExtractor() :
        trim(1)
{
    xmlc = NULL;
    wordc = NULL;
    position = 0;
    inflated_pos = 0;
}

DictExtractor::~DictExtractor()
{

}
bool DictExtractor::loadFile(const QString& filename, const QString &outputfile)
{
    qDebug() << "Load file " << filename;
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    ld2ByteArray = file.readAll();
    file.close();

    qDebug() << "File:" << ld2file;
    qDebug() << "Type:" << ld2ByteArray.mid(1, 3);
    qDebug() << "Version:"
            << QByteArray::number(getShort(0x18)) + "." + QByteArray::number(getShort(0x1A));
    qDebug() << "ID:" << "0x" + toHexString(getLong(0x1C));
    int offsetData = getInt(0x5C) + 0x60;
    if (ld2ByteArray.size() > offsetData) {
        qDebug() << "Summary Addr:" << toHexString(offsetData);
        int dtype = getInt(offsetData);
        qDebug() << "Summary Type:" << toHexString(dtype);
        int offsetWithInfo = getInt(offsetData + 4) + offsetData + 12;
        if (dtype == 3) {
            readDictionary(offsetData, outputfile);
        } else if (ld2ByteArray.size() > offsetWithInfo - 0x1C) {
            readDictionary(offsetWithInfo, outputfile);
        } else {
            qWarning() << "This File Doesn't Contain Dictionary.";
        }
    } else {
        qWarning() << "This File Doesn't Contain Dictionary.";
    }
    return true;
}
void DictExtractor::readDictionary(const int offsetWithIndex, const QString &outputfile)
{
    //analyze dictionary file's header
    qDebug() << "Dictionary Type:" << "0x" + QByteArray::number(getInt(offsetWithIndex));
    const int limit = getInt(offsetWithIndex + 4) + offsetWithIndex + 8;
    const int offsetIndex = offsetWithIndex + 0x1C;
    const int offsetCompressedDataHeader = getInt(offsetWithIndex + 8) + offsetIndex;
    const int inflatedWordsIndexLength = getInt(offsetWithIndex + 12);
    const int inflatedWordsLength = getInt(offsetWithIndex + 16);
    const int inflatedXmlLength = getInt(offsetWithIndex + 20);
    const int definitions = (offsetCompressedDataHeader - offsetIndex) / 4;
    QVector<int> deflateStreams;
    position = offsetCompressedDataHeader + 8; //position here represents ByteBuffer's position() in Java
    int offset = getInt(position); //In java, ByteBuffer's getInt() will increase the position by four(an Integer size).
    position += sizeof(int); //Hence, we need to add it to position manually.
    while (offset + position < limit) {
        offset = getInt(position);
        deflateStreams << offset;
        position += sizeof(int);
    }
    int offsetCompressedData = position;
    qDebug() << "Index Numbers:" << definitions;
    qDebug()
            << QString("Index Address/Size: 0x%1 / %2B").arg(toHexString(offsetIndex),
                    QString::number(offsetCompressedDataHeader - offsetIndex));
    qDebug()
            << QString("Compressed Data Address/Size: 0x%1 / %2B").arg(
                    toHexString(offsetCompressedData),
                    QString::number(limit - offsetCompressedData));
    qDebug()
            << QString("Phrases Index Address/Size(Decompressed): 0x0 / %1B").arg(
                    inflatedWordsIndexLength);
    qDebug()
            << QString("Phrases Address/Size(Decompressed): 0x%1 / %2B").arg(
                    toHexString(inflatedWordsIndexLength), QString::number(inflatedWordsLength));
    qDebug()
            << QString("XML Address/Size(Decompressed): 0x%1 / %2B").arg(
                    toHexString(inflatedWordsIndexLength + inflatedWordsLength),
                    QString::number(inflatedXmlLength));
    qDebug()
            << QString("File Size(Decompressed): %1KB").arg(
                    (inflatedWordsIndexLength + inflatedWordsLength + inflatedXmlLength) / 1024);
    QByteArray inflatedData;
    inflateData(deflateStreams, &inflatedData);
    if (!inflatedData.isEmpty()) {
        position = offsetIndex + sizeof(int) * definitions;
        extract(inflatedData, inflatedWordsIndexLength,
                inflatedWordsIndexLength + inflatedWordsLength, outputfile);
    } else {
        qWarning() << "ERROR: Inflated Data is Empty.";
    }
}

void DictExtractor::inflateData(const QVector<int> &deflateStreams, QByteArray *inflatedData)
{
    qDebug() << QString("Decompressing %1 data streams.").arg(deflateStreams.size());
    int startOffset = position;
    int offset = -1;
    int lastOffset = startOffset;
    try {
        foreach(int offsetRelative, deflateStreams){
        offset = startOffset + offsetRelative;
        decompress(inflatedData, lastOffset, offset - lastOffset);
        lastOffset = offset;
    }
} catch (std::exception e) {
    qWarning() << "Failed decompressing" << offset << ":" << e.what();
} catch (const char* e2) {
    qWarning() << e2;
}
}

void DictExtractor::decompress(QByteArray *inflatedData, const int offset, const quint32 length)
{
//    uncompress deflate datastream
    try {
        QByteArray data = ld2ByteArray.mid(offset, length);
        QByteArray header(4, '\0');
        //FIXME: we should prepend expected extracted data length instead of compressed data length
        qToBigEndian(length, reinterpret_cast<uchar*>(header.data())); //see http://doc.qt.io/qt-5/qbytearray.html#qUncompress
        data.prepend(header);
        inflatedData->append(qUncompress(data));
    } catch (std::exception e) {
        qWarning() << e.what();
    } catch (const char* e2) {
        qWarning() << e2;
    }
}

void DictExtractor::extract(const QByteArray &inflatedBytes, const int offsetDefs,
        const int offsetXml, const QString &outputfile)
{
//    QFile fileout(outputfile);
//    fileout.open(QIODevice::WriteOnly | QIODevice::Text);
//    QTextStream out(&fileout);
    int counter = 0;
    const int dataLen = 10;
    const int defTotal = (offsetDefs / dataLen) - 1;
//    QString line;
    int idxData[6];
    QString defData[2];
    detectEncodings(inflatedBytes, offsetDefs, offsetXml, defTotal, dataLen, idxData);

    qDebug() << "Extracting...";
    SqlDataAccess *sqlda = new SqlDataAccess("./data/proDictDB.db");
    inflated_pos = 8;
    for (int i = 0; i < defTotal; i++) {
        readDefinitionData(inflatedBytes, offsetDefs, offsetXml, dataLen, idxData, defData, i);
//        line.append(defData[0]);
//        line.append(" = ");
//        line.append(defData[1]);

        QVariantList contact;
        contact << defData[0] << "ipa" << "type" << defData[1];
        sqlda->execute("INSERT INTO words"
                "    (word, ipa, type, meaning) "
                "    VALUES (:word, :ipa, :type, :meaning)", contact);
        if (!sqlda->hasError()) {
            qDebug() << "add success....:" << contact;
        } else {
            const DataAccessError error = sqlda->error();
            qDebug() << "Add failed : " << (error.errorMessage());
        }
        // out << line << endl;
//        line.clear();
        counter++;
    }
//    out.flush();
//    fileout.close();
    qDebug() << "Extracted" << counter << "entries.";
}

void DictExtractor::detectEncodings(const QByteArray &inflatedBytes, const int offsetWords,
        const int offsetXml, const int defTotal, const int dataLen, int idxData[])
{
    int wordc_id = 0, xmlc_id = 0;
    int testIdx = qMin(defTotal, 10);

    while (testIdx-- > 0) { //test at most 10 times
        getIdxData(inflatedBytes, dataLen * testIdx, idxData);
        int lastWordPos = idxData[0];
        int lastXmlPos = idxData[1];
        int refs = idxData[3];
        const int currentWordOffset = idxData[4];
        int currenXmlOffset = idxData[5];
        QByteArray xmlBytes = inflatedBytes.mid(offsetXml + lastXmlPos,
                currenXmlOffset - lastXmlPos);
        while (refs-- > 0) {
            int ref = getInt(inflatedBytes, offsetWords + lastWordPos);
            getIdxData(inflatedBytes, dataLen * ref, idxData);
            lastXmlPos = idxData[1];
            currenXmlOffset = idxData[5];
            xmlBytes.append(
                    inflatedBytes.mid(offsetXml + lastXmlPos, currenXmlOffset - lastXmlPos));
            lastWordPos += 4;
        }
        QByteArray wordBytes = inflatedBytes.mid(offsetWords + lastWordPos,
                currentWordOffset - lastWordPos);

        wordc = QTextCodec::codecForName(availableEncodings.at(wordc_id));
        QString word_res = wordc->toUnicode(wordBytes);
        qDebug() << wordc->name() << "decodes phrase as" << word_res;
        if (!word_res.isSimpleText()) {
            if (wordc_id < availableEncodings.size() - 1) {
                ++wordc_id;
            } else {
                qWarning() << "Cannot detect a valid encoding for Words. Output may be corrupted.";
            }
        }

        xmlc = QTextCodec::codecForName(availableEncodings.at(xmlc_id));
        QString xml_res = strip(xmlc->toUnicode(xmlBytes));
        qDebug() << xmlc->name() << "decodes XML as" << xml_res;
        if (!xml_res.isSimpleText()) {
            if (xmlc_id < availableEncodings.size() - 1) {
                ++xmlc_id;
            } else {
                qWarning() << "Cannot detect a valid encoding for XML. Output may be corrupted.";
            }
        }
    }
//
    qDebug() << "Phrases Encoding:" << wordc->name();
    qDebug() << "XML Encoding:" << xmlc->name();
//
//    //Before we can address encoding detection, let's give users a chance to specify codec.
//    QTextStream stdinStream(stdin);
//    QChar accepted;
//    qDebug() << "Continue with automatically detected encodings? (y/n)";
//    stdinStream >> accepted;
//    if (accepted == 'n' || accepted == 'N') {
//        QByteArray enc;
//        qDebug() << "Please input the encoding for phrases. Enter dd to use default. Default:"
//                << wordc->name();
//        stdinStream >> enc;
//        if (enc != "dd") {
//            wordc = QTextCodec::codecForName(enc);
//        }
//        enc.clear();
//        qDebug() << "Please input the encoding for XML. Enter dd to use default. Default:"
//                << xmlc->name();
//        stdinStream >> enc;
//        if (enc != "dd") {
//            xmlc = QTextCodec::codecForName(enc);
//        }
//        qDebug() << "Phrases Encoding:" << wordc->name();
//        qDebug() << "XML Encoding:" << xmlc->name();
//    }
}

void DictExtractor::readDefinitionData(const QByteArray &inflatedBytes, const int offsetWords,
        const int offsetXml, const int dataLen, int idxData[], QString defData[], const int i)
{
//get all data from definition area
    getIdxData(inflatedBytes, dataLen * i, idxData);
    int lastWordPos = idxData[0];
    int lastXmlPos = idxData[1];
    int refs = idxData[3];
    const int currentWordOffset = idxData[4];
    int currenXmlOffset = idxData[5];
    QString xml = strip(
            xmlc->toUnicode(
                    inflatedBytes.mid(offsetXml + lastXmlPos, currenXmlOffset - lastXmlPos)));
    while (refs-- > 0) {
        int ref = getInt(inflatedBytes, offsetWords + lastWordPos);
        getIdxData(inflatedBytes, dataLen * ref, idxData);
        lastXmlPos = idxData[1];
        currenXmlOffset = idxData[5];
        if (xml.isEmpty()) {
            xml = strip(
                    xmlc->toUnicode(
                            inflatedBytes.mid(offsetXml + lastXmlPos,
                                    currenXmlOffset - lastXmlPos)));
        } else {
            xml = strip(
                    xmlc->toUnicode(
                            inflatedBytes.mid(offsetXml + lastXmlPos,
                                    currenXmlOffset - lastXmlPos))) + ", " + xml;
        }
        lastWordPos += 4;
    }
    defData[1] = xml;
    QString word = wordc->toUnicode(
            inflatedBytes.mid(offsetWords + lastWordPos, currentWordOffset - lastWordPos));
    defData[0] = word;
}

QString DictExtractor::strip(const QString &xml)
{
    if (!trim) {
        return xml;
    }
    /*
     * Strip some formats characters.
     * TODO: strip HTML tags such as <TD> <TR>
     */
    int open = 0;
    int end = 0;
    const int SZ = QString("<![CDATA[").length();
    if ((open = xml.indexOf("<![CDATA[")) != -1) {
        if ((end = xml.indexOf("]]>", open)) != -1) {
            return xml.mid(open + SZ, end - SZ - open).replace('\t', ' ').replace('\n', ' ').replace(
                    '\u001e', ' ').replace('\u001f', ' ');
        }
    } else if ((open = xml.indexOf("<Ô")) != -1) {
        if ((end = xml.indexOf("</Ô", open)) != -1) {
            open = xml.indexOf(">", open + 1);
            return xml.mid(open + 1, end - open - 1).replace('\t', ' ').replace('\n', ' ').replace(
                    '\u001e', ' ').replace('\u001f', ' ');
        }
    } else {
        QString sb;
        end = 0;
        open = xml.indexOf('<');
        do {
            if ((open - end) > 1) {
                sb.append(xml.mid(end + 1, open - end - 1));
            }
            open = xml.indexOf('<', open + 1);
            end = xml.indexOf('>', end + 1);
        } while ((open != -1) && (end != -1));
        return sb.simplified();
    }
    return QString();
}

void DictExtractor::getIdxData(const QByteArray &inflatedBytes, const int pos, int wordIdxData[])
{
    inflated_pos = pos;
    wordIdxData[0] = getInt(inflatedBytes, inflated_pos);
    inflated_pos += sizeof(int);
    wordIdxData[1] = getInt(inflatedBytes, inflated_pos);
    inflated_pos += sizeof(int);
    wordIdxData[2] = inflatedBytes[inflated_pos] & 0xff;
    inflated_pos++;
    wordIdxData[3] = inflatedBytes[inflated_pos] & 0xff;
    inflated_pos++;
    wordIdxData[4] = getInt(inflatedBytes, inflated_pos);
    inflated_pos += sizeof(int);
    wordIdxData[5] = getInt(inflatedBytes, inflated_pos);
    inflated_pos += sizeof(int);
}

int DictExtractor::getInt(const int index)
{
    return *(reinterpret_cast<int *>(ld2ByteArray.mid(index, sizeof(int)).data()));
}

int DictExtractor::getInt(const QByteArray &ba, const int index)
{
    return *(reinterpret_cast<int *>(ba.mid(index, sizeof(int)).data()));
}

qint16 DictExtractor::getShort(const int index)
{
    return *(reinterpret_cast<qint16 *>(ld2ByteArray.mid(index, sizeof(qint16)).data()));
}

qint32 DictExtractor::getLong(const int index)
{
    return *(reinterpret_cast<qint32*>(ld2ByteArray.mid(index, sizeof(qint32)).data()));
}

QByteArray DictExtractor::toHexString(const qint32 num)
{
    return QByteArray::number(num, 16).toUpper();
}

QByteArray DictExtractor::toHexString(const qint16 num)
{
    return QByteArray::number(num, 16).toUpper();
}

