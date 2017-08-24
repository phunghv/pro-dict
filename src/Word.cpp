/*
 * Word.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: rio09
 */

#include "Word.hpp"

Word::Word(QObject *parent) :
        QObject(parent)
{
    // TODO Auto-generated constructor stub

}

Word::Word(const int &id, const QString &word, const QString &ipa, const QString &type,
        const QString &meaning, QObject *parent) :
        QObject(parent), mId(id), mWord(word), mIpa(ipa), mType(type), mMeaning(meaning)
{

}

QString Word::getWord()
{
    return mWord;
}
QString Word::getIpa()
{
    return mIpa;
}
QString Word::getType()
{
    return mType;
}
QString Word::getMeaning()
{
    return mMeaning;
}
int Word::getId()
{
    return mId;
}

void Word::setId(const int &id)
{
    mId = id;
}
void Word::setWord(const QString &word)
{
    if (word != mWord) {
        mWord = word;
    }
}
void Word::setIpa(const QString &ipa)
{
    if (ipa != mIpa) {
        mIpa = ipa;
    }
}
void Word::setType(const QString &type)
{
    if (type != mType) {
        mType = type;
    }
}
void Word::setMeaning(const QString &meaning)
{
    if (meaning != mMeaning) {
        mMeaning = meaning;
    }
}
