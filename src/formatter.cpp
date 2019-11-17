#include "formatter.hpp"

#include "utils.hpp"
#include <QDateTime>
#include <QRegExp>
#include <QRegularExpression>
#include <QStringList>

QString formatter::format(QString toFormat, QString ext, QMap<QString, QString> variables)
{
    QString formatted(toFormat);

    QRegExp dateRegex("%(?!%)\\((.+)\\)date");
    dateRegex.indexIn(toFormat);
    QStringList capturedTexts(dateRegex.capturedTexts());
    QDateTime date = QDateTime::currentDateTime();
    for (int i = 0; i < capturedTexts.length(); i += 2)
        formatted = formatted.replace(capturedTexts.at(i), date.toString(capturedTexts.at(i + 1)));

    QRegExp randomRegex("%(?!%)\\((.+)\\)random");
    randomRegex.indexIn(toFormat);
    QStringList randomTexts(randomRegex.capturedTexts());

    bool ok = false;
    int random_len = 0;
    if (randomTexts.length() == 2)
        random_len = randomTexts.at(1).toInt(&ok);

    // If the only argument was successfully parsed as a uint, then we can replace it with random chars
    // Otherwise, fall back to picking a random word from the match
    if (ok)
    {
        if (random_len < 1)
            random_len = 10;
        formatted = formatted.replace(randomTexts.at(0), utils::randomString(random_len));
    }
    else
    {
        for (int i = 0; i < randomTexts.length(); i += 2)
        {
            QStringList list = randomTexts.at(i + 1).split('|');
            formatted = formatted.replace(randomTexts.at(i), list.at(rand() % (list.length())));
        }
    }


    for (QString var : variables.keys())
        formatted.replace("%" + var, variables[var]);

    formatted = formatted.replace(QRegularExpression("%(?!%)ext"), ext);
    return formatted;
}
