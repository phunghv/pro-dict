/*
 * DictExtractor.hpp
 *
 *  Created on: Aug 27, 2017
 *      Author: rio09
 */

#ifndef DICTEXTRACTOR_HPP_
#define DICTEXTRACTOR_HPP_
#include <QObject>

class DictExtractor
{
public:
    DictExtractor();
    virtual ~DictExtractor();
    bool loadFile(const QString& file, const QString&);
    int getInt(const int);
    int getInt(const QByteArray &, const int);
    qint16 getShort(const int); //the short is 16-bit integer
    qint32 getLong(const int); //while the long is actually 32-bit integer.
    QByteArray toHexString(const qint32);
    QByteArray toHexString(const qint16);
private:
    const bool trim;
    int position;
    int inflated_pos;
    QString ld2file;
    QByteArray ld2ByteArray;
    QTextCodec* xmlc; //XML Encoding
    QTextCodec* wordc; //Words Encoding
    void readDictionary(const int offsetWithIndex, const QString &);
    void inflateData(const QVector<int> &, QByteArray *);
    void decompress(QByteArray *, const int, const quint32);
    void extract(const QByteArray &, const int, const int, const QString &);
    void detectEncodings(const QByteArray &, const int, const int, const int, const int, int a[]);
    void readDefinitionData(const QByteArray &, const int, const int, const int, int a[],
            QString s[], const int);
    void getIdxData(const QByteArray &, const int, int a[]);
    QString strip(const QString &);

    const static QVector<QByteArray> availableEncodings;
};

#endif /* DICTEXTRACTOR_HPP_ */
