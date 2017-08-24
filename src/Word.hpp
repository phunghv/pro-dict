/*
 * Word.hpp
 *
 *  Created on: Aug 25, 2017
 *      Author: rio09
 */

#ifndef WORD_HPP
#define WORD_HPP

#include <QObject>

class Word: public QObject
{
    Q_OBJECT

    Q_PROPERTY( int id READ getId )
    Q_PROPERTY( QString word READ getWord )
    Q_PROPERTY( QString ipa READ getIpa )
    Q_PROPERTY( QString type READ getType )
    Q_PROPERTY( QString meaning READ getMeaning )

public:
    Word(QObject *parent =0);
    Word(const int &id, const QString &word, const QString &ipa, const QString &type,
            const QString &meaning, QObject *parent = 0);

    QString getWord();
    QString getIpa();
    QString getType();
    QString getMeaning();
    int getId();

    void setId(const int &id);
    void setWord(const QString &word);
    void setIpa(const QString &ipa);
    void setType(const QString &type);
    void setMeaning(const QString &meaning);

private:
    int mId;
    QString mWord;
    QString mIpa;
    QString mType;
    QString mMeaning;
};

#endif /* WORD_HPP_ */
