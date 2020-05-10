#ifndef FREQUENCY_H
#define FREQUENCY_H

#include <QtCore>

#include "OpMode.h"
#include "RadioSetup.h"


class Frequency {

protected :

    Frequency();
    Frequency(QStringList & lst);

    static Frequency Dummy;

public :
    ~Frequency();

    static QList<Frequency*>  lstFrequencies;

public:

    static void init(QList<OptionsList> & lst);
    static void clear();
    static Frequency * find(QString Callsign);
    bool        isValid();
    const char * OpModeName();
    const char * OpModeModulationType();

    QString            Callsign;
    QString            Name;
    // QString            NameShort;
    int                Freq;       // Hz
    OpMode             opMode;
    QString            Radios;

};

#endif // FREQUENCY_H
