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

#ifndef RGBLINE_H
#define RGBLINE_H


class RGBLine {
public:
  RGBLine(int nPixel);
  ~RGBLine();

  unsigned int * Pixels;
  int            nPixel;

  void set(unsigned int Pixel, int iPixel) { Pixels[iPixel] = Pixel; }

};

#endif // RGBLINE_H
