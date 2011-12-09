/**
 *  icon.cpp
 *
 *  Copyright (C) 2008  David Andrs <pda@jasnapaka.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"
//#include "../prssr/prssr.h"
#include "icon.h"
#include "../share/file.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
//#include "../debug/crtdbg.h"
//#define new MYDEBUG_NEW
#endif

#pragma pack(push, structs)
#pragma pack(1)

typedef struct IconDirectoryEntry {
	BYTE  bWidth;
	BYTE  bHeight;
	BYTE  bColorCount;
	BYTE  bReserved;
	WORD  wPlanes;
	WORD  wBitCount;
	DWORD dwBytesInRes;
	DWORD dwImageOffset;
} ICONDIRENTRY;

typedef struct ICONDIR {
	WORD          idReserved;
	WORD          idType;
	WORD          idCount;
} ICONHEADER;

#pragma pack(pop, structs)


HBITMAP ResizeBitmap(HBITMAP hBmp, WORD bitCount, int srcWd, int srcHt, int destWd, int destHt) {
	HDC srcDC = CreateCompatibleDC(NULL);
	HGDIOBJ oldSrc = SelectObject(srcDC, hBmp);

	HDC destDC = CreateCompatibleDC(NULL);
	BITMAPINFO bmi;
//	bmi.bmiHeader = bmpInfoHdr;
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = bitCount;
	bmi.bmiHeader.biWidth = destWd;
	bmi.bmiHeader.biHeight = destHt;
	void *pBits = NULL; 
	HBITMAP hbmResized = CreateDIBSection(destDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	HGDIOBJ oldDest = SelectObject(destDC, hbmResized);

	StretchBlt(destDC, 0, 0, destWd, destHt, srcDC, 0, 0, srcWd, srcHt, SRCCOPY);
/*	HBRUSH br;
	br = CreateSolidBrush(RGB(0x33, 0x66, 0x99));
	SelectObject(destDC, br);
	Rectangle(destDC, 0, 0, destWd, destHt);
*/
	SelectObject(srcDC, oldSrc);
	SelectObject(destDC, oldDest);

	DeleteDC(srcDC);
	DeleteDC(destDC);

	return hbmResized;
}

HICON DoIconLoad(BYTE *iconResData, ICONDIRENTRY &icon, int width, int height) {
	LOG0(5, "DoIconLoad");

	HICON hIcon = NULL;

	// header
	BYTE *icHeader = iconResData + 0;
	BITMAPINFOHEADER bmpInfoHdr;
	memcpy(&bmpInfoHdr, icHeader, sizeof(bmpInfoHdr));

	int pixels = icon.bWidth * icon.bHeight;
	RGBQUAD *icPalette = NULL;
	BYTE *icColor = NULL;
	BYTE *icMask = NULL;

	icColor = (iconResData + sizeof(bmpInfoHdr));
	if (bmpInfoHdr.biBitCount <= 8) {
		// palette
		int clrCount = 1 << bmpInfoHdr.biBitCount;
		icPalette = (RGBQUAD *) (icColor);

		icColor += clrCount * sizeof(RGBQUAD);
	}
	icMask = icColor + ((pixels * bmpInfoHdr.biBitCount) / 8);

	// read icon ////

	// convert to suitable format (RGB or RGBx)
	HBITMAP hbmColor;
	if (bmpInfoHdr.biBitCount == 8) {
		// convert from 256 colors to 24bpp
		BYTE *rgbData = new BYTE[3 * sizeof(BYTE) * pixels];

		int icIdx = 0;				// index in source data
		for (int row = 1; row <= icon.bHeight; row++) {
			int destIdx = 3 * (pixels - (row * icon.bWidth));

			for (int col = 0; col < icon.bWidth; col++) {
				RGBQUAD *rgb = icPalette + icColor[icIdx];

				rgbData[destIdx++] = rgb->rgbBlue;
				rgbData[destIdx++] = rgb->rgbGreen;
				rgbData[destIdx++] = rgb->rgbRed;

				icIdx++;
			}
		}

		hbmColor = CreateBitmap(icon.bWidth, icon.bHeight, 1, 24, rgbData);

		delete [] rgbData;
	}
	else if (bmpInfoHdr.biBitCount == 4) {
		// convert from 16 colors to 24bpp
		BYTE *rgbData = new BYTE[3 * sizeof(BYTE) * pixels];

		int icIdx = 0;				// index in source data
		for (int row = 1; row <= icon.bHeight; row++) {
			int destIdx = 3 * (pixels - (row * icon.bWidth));
			for (int col = 0; col < icon.bWidth; ) {
				RGBQUAD *rgb;

				rgb = icPalette + (icColor[icIdx] >> 4);
				rgbData[destIdx++] = rgb->rgbBlue;
				rgbData[destIdx++] = rgb->rgbGreen;
				rgbData[destIdx++] = rgb->rgbRed;
				col++;

				rgb = icPalette + (icColor[icIdx] & 0x0F);
				rgbData[destIdx++] = rgb->rgbBlue;
				rgbData[destIdx++] = rgb->rgbGreen;
				rgbData[destIdx++] = rgb->rgbRed;
				col++;

				icIdx++;
			}
		}

		hbmColor = CreateBitmap(icon.bWidth, icon.bHeight, 1, 24, rgbData);

		delete [] rgbData;
	}
	else {
		BYTE *rgbData = new BYTE[(pixels * bmpInfoHdr.biBitCount) / 8];

		// flip the bitmap data
		int rgbSz = bmpInfoHdr.biBitCount / 8;
		int rowBytes = icon.bWidth * rgbSz;			// no. of bytes on the row
		BYTE *src = icColor + 0;
		BYTE *dest = rgbData + (rgbSz * pixels) - rowBytes;
		for (int row = 0; row < icon.bHeight; row++) {
			memcpy(dest, src, rowBytes);
			dest -= rowBytes;
			src += rowBytes;
		}

		hbmColor = CreateBitmap(icon.bWidth, icon.bHeight, 1, rgbSz * 8, rgbData);

		delete [] rgbData;
	}

	// resize
	BOOL resize = FALSE;
	if (icon.bWidth != width || icon.bHeight != height) {
		resize = TRUE;
		HBITMAP hResized = ResizeBitmap(hbmColor, 24, icon.bWidth, icon.bHeight, width, height);
		DeleteObject(hbmColor);
		hbmColor = hResized;
	}

	// mask - for transparency
	HBITMAP hbmMask;
	if (icon.bWidth != width || icon.bHeight != height) {
		// resize the mask (from 16x16 to 32x32)
		int maskPixelsResized = (width * height) / 8;
		BYTE *maskResized = new BYTE [maskPixelsResized];
		memset(maskResized, 0, maskPixelsResized);

		int maskBytesPerRow = bmpInfoHdr.biWidth / 8;

		BYTE *src = icMask + 0;
		BYTE *dest = maskResized + maskPixelsResized - (2 * maskBytesPerRow);
		for (int row = 0; row < icon.bHeight; row++) {
			for (int j = 0; j < maskBytesPerRow; j++) {
				BYTE b = *(src + j);
				WORD w = 0;
				for (int k = 0; k < 8; k++) {
					if (b & (1 << k)) {
						w |= 3 << (2 * k);
					}
				}
				dest[2 * j] = (w >> 8) & 0xFF;
				dest[2 * j + 1] = w & 0xFF;
			}

			memcpy(dest - (2 * maskBytesPerRow), dest, 2 * maskBytesPerRow);

			dest -= 4 * maskBytesPerRow;
			src += 2 * maskBytesPerRow;
		}

		hbmMask = CreateBitmap(width, height, 1, 1, maskResized);
		delete [] maskResized;
	}
	else {
		int maskPixels = (icon.bWidth * icon.bHeight) / 8;

		// flip the mask
		BYTE *mask = new BYTE [maskPixels];
		memset(mask, 0x00, maskPixels);
		int maskBytesPerRow = icon.bWidth / 8;

		BYTE *src = icMask + 0;
		BYTE *dest = mask + maskPixels - maskBytesPerRow;
		for (int row = 0; row < icon.bHeight; row++) {
			memcpy(dest, src, maskBytesPerRow);
			dest -= maskBytesPerRow;
			src += 4; // 4 bytes per row in icon data for 16x16 and 32x32 icons
		}

		hbmMask = CreateBitmap(icon.bWidth, icon.bHeight, 1, 1, mask);
		delete [] mask;
	}

	// finally create the icon
	ICONINFO ii = { 0 };
	ii.fIcon = TRUE;
	ii.hbmMask = hbmMask;
	ii.hbmColor = hbmColor;
	hIcon = CreateIconIndirect(&ii);

//	LOG4(1, "mask: (%d x %d x %dbpp), hIcon = %d", bmpInfoHdr.biWidth, bmpInfoHdr.biHeight, bmpInfoHdr.biBitCount, hIcon);
//	LOG4(1, "(%d x %d x %dbpp), hIcon = %d", icon.bWidth, icon.bHeight, bmpInfoHdr.biBitCount, hIcon);

	DeleteObject(hbmMask);
	DeleteObject(hbmColor);

	return hIcon;
}

HICON LoadIconFromFile(LPCTSTR fileName, int width, int height) {
	LOG0(5, "LoadIconFromFile");

	HICON hIcon = NULL;
	CBufferedFile file;
	if (file.Create(fileName, GENERIC_READ, NULL, OPEN_EXISTING, 0)) {
		DWORD bytesRead;

		// read icon dir
		ICONHEADER iconHdr;
		if (file.Read(&iconHdr, sizeof(iconHdr), &bytesRead)) {
//			LOG1(1, "icon count: %d", iconHdr.idCount);
			ICONDIRENTRY *iconEntries = new ICONDIRENTRY [iconHdr.idCount];
			if (file.Read(iconEntries, iconHdr.idCount * sizeof(ICONDIRENTRY), &bytesRead)) {
				ICONDIRENTRY icon = { 0 };

				BOOL found = FALSE;
				// first pass (find the icon of exact size)
				for (int i = 0; i < iconHdr.idCount; i++) {
					if (iconEntries[i].bWidth == width && iconEntries[i].bHeight == height) {
//						LOG3(1, "wd = %d, ht = %d, bitcount = %d", iconEntries[i].bWidth, iconEntries[i].bHeight, iconEntries[i].wBitCount);
						if (iconEntries[i].wBitCount == 8) {
							icon = iconEntries[i];
							found = TRUE;
							break;
						}
						else if (iconEntries[i].wBitCount >= icon.wBitCount) {
							icon = iconEntries[i];
							found = TRUE;
						}
					}
				}

				// second pass (if icon not found in the first pass)
				if (!found) {
					for (int i = 0; i < iconHdr.idCount; i++) {
						if (iconEntries[i].bWidth == 16 && iconEntries[i].bHeight == 16) {
//							LOG3(1, "wd = %d, ht = %d, bitcount = %d", iconEntries[i].bWidth, iconEntries[i].bHeight, iconEntries[i].wBitCount);
							if (iconEntries[i].wBitCount == 8) {
								icon = iconEntries[i];
								found = TRUE;
								break;
							}
							else if (iconEntries[i].wBitCount >= icon.wBitCount) {
								icon = iconEntries[i];
								found = TRUE;
							}
						}
					}
				}

				// third pass (nothing found -> use the first icon)
/*				if (!found) {
					icon = iconEntries[0];
					found = TRUE;
				}
*/
				// create the icon
				if (found) {
//					LOG2(1, "ofs = %d, len = %d", icon.dwImageOffset, icon.dwBytesInRes);
					file.Seek(icon.dwImageOffset, FILE_BEGIN);
					
					DWORD bytesRead;
					BYTE *iconData = new BYTE [icon.dwBytesInRes];
					if (file.Read(iconData, icon.dwBytesInRes, &bytesRead)) {
//						LOG1(1, "read = %d", bytesRead);
						
//						LOG3(1, "wd = %d, ht = %d, bitcount = %d", icon.bWidth, icon.bHeight, icon.wBitCount);
						hIcon = DoIconLoad(iconData, icon, width, height);
					}

					delete [] iconData;
				}
			}		
			delete [] iconEntries;
		}

		file.Close();
	}

	return hIcon;
}
