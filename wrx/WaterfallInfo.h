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

#ifndef WATERFALLINFO_H
#define WATERFALLINFO_H

class WaterfallInfo {

public :

  WaterfallInfo() { nBins = 512; binLeft = 0; Zoom = 1; Gain = 0.03f; }
  WaterfallInfo(int nbins, int binleft, int zoom, float gain)
             : nBins(nbins), binLeft(binleft), Zoom(zoom), Gain(gain) {}

  int     nBins;
  int     binLeft;
  int     Zoom;
  float   Gain;

};

#endif // WATERFALLINFO_H
