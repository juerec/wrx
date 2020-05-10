/****************************************************************************
**
**  This file is a part of the program "WRX".
**
**  Copyright © 2020 Jürgen Reche
**
** WRX is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** WRX is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see <https://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef WATERFALL_H
#define WATERFALL_H

#include <QtGui/QMainWindow>
#include <QFrame>

#include <pthread.h>

#define FFTSIZE          1024
#define WINDOWWIDTH       512
#define WINDOWHEIGHT       96
#define DATACYCLE        2048
#define  MAXLINES          96

typedef struct tagCOLOR {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;
} COLOR;


class SoundData {

  public :
    SoundData(short * samples, int count);
    ~SoundData();

  int   count;
  short * samples;
};

class Waterfall : public QFrame {

  Q_OBJECT

  protected :
    int      nFFTLen;
    short  * Data;
    int      DataPos;        //  Sample count in Data, only samples to use in next are counted
    int      InputPos;       //  Sample count delivered by setData() since last line start
    int      DataCycle;      //  Samples to read for start a new line => FFTSIZE + Samples to skip. Values < FFTSIZE are valid
    QPixmap  Pixmap;         //  holds all allready drawn lines
    QImage   CurrLineImage;  //  holds the line to draw newly
    int      CurrLine;       //  points to the latest drawn line
    QList<SoundData *> InputList;  // collects all data delivered from RadioSound
    pthread_mutex_t  InputListMutex;

    short     * aReal;
    short     * aImag;
    unsigned int * aColorTable;
    int         nColorTableLen;
    int       * loudbuf;
    int         WindowSize;
    pthread_mutex_t   mutex;
    int        RefreshId;
    int        RefreshIdOld;


  public :
    Waterfall(QWidget * parent=0, Qt::WindowFlags f=0);
    ~Waterfall();
    void setFFTSize(int fftSize);

  protected :
    void processData();
    void newDataM(short * samples, int count);

  public slots :
    void setDataM(short * samples, int count);
    void TimeOut100();

  protected :
    virtual void paintEvent(QPaintEvent * e);
    void CreatePowerColorGradientSimple(int nBits, unsigned int * pColorsOut, int nColorsOut);
};


#endif /* WATERFALL_H */
