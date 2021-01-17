#ifndef ERRORS_H
#define ERRORS_H

#include <QMap>

enum class Errors
{
    ERR_NONE,
    ERR_LANG_DB,
    ERR_UNKNOWN_LANG
};

static QMap<Errors, QString> _errors = {
    {Errors::ERR_NONE, ""},
    {Errors::ERR_LANG_DB, "The same language has been used"},
    {Errors::ERR_UNKNOWN_LANG, "The languages are unknown"}
};

#endif // ERRORS_H
