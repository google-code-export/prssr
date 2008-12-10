/**
 *  helpers.cpp : Miscelaneous helping functions
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

#ifdef PRSSR_APP
#include "../prssr/StdAfx.h"
#include "../prssr/prssr.h"
#include "../prssr/Config.h"
#elif defined PRSSR_TODAY
#include "../prssrtoday/StdAfx.h"
#include "../prssrtoday/prssrtoday.h"
#include "../prssrtoday/Config.h"
#endif

#include "helpers.h"
#include "date.h"

BOOL InWideMode() {
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	return (width > height && height < SCALEX(320)) ? TRUE : FALSE;
}


int DrawTextEndEllipsis(CDC &dc, const CString &strText, CRect &rc, UINT uFormat) {
	LOG1(5, "DrawTextEndEllipsis(, '%S', , )", strText);

	int nWidth = rc.Width();

	CRect rcTemp = rc;
	dc.DrawText(strText, rcTemp, uFormat | DT_CALCRECT);
	if (rcTemp.Width() > nWidth) {
		// Text doesn't fit in rect. We have to truncate it and add ellipsis to the end.
		CString strTemp;

		for (int i = strText.GetLength(); i >= 0; i--) {
			strTemp.Format(_T("%s..."), strText.Left(i));

			rcTemp = *rc;
			dc.DrawText(strTemp, rcTemp, uFormat | DT_CALCRECT);
			if (rcTemp.Width() < nWidth) {
				// Gotcha!
				break;
			}
		}
		return dc.DrawText(strTemp, rc, uFormat);
	}

	return dc.DrawText(strText, rc, uFormat);
}

BOOL FormatDateTime(CString &strDateTime, SYSTEMTIME date, BOOL relDates/* = FALSE*/) {
	if (date.wYear != 0) {
		int len;
		LPTSTR buffer;

		SYSTEMTIME today;
		GetLocalTime(&today);

		long int diff = MakeLinearDate(today) - MakeLinearDate(date);
		CString strDate;
		if (relDates && diff < 5) {
			if (diff == 0) strDate.LoadString(IDS_TODAY);
			else if (diff == 1) strDate.LoadString(IDS_YESTERDAY);
			else strDate.Format(IDS_DAYS_AGO, diff);
		}
		else {
			// insert date
			if (Config.DateFormat.IsEmpty()) {
				len = GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &date, NULL, NULL, 0);
				buffer = strDate.GetBufferSetLength(len);
				GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &date, NULL, buffer, len);
			}
			else {
				len = GetDateFormat(LOCALE_USER_DEFAULT, 0, &date, Config.DateFormat, NULL, 0);
				buffer = strDate.GetBufferSetLength(len);
				GetDateFormat(LOCALE_USER_DEFAULT, 0, &date, Config.DateFormat, buffer, len);
			}
		}

		CString strTime;
		// time
		LPCTSTR timeFormat = _T("H:mm");
		len = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &date, timeFormat, NULL, 0);
		buffer = strTime.GetBufferSetLength(len);
		GetTimeFormat(LOCALE_USER_DEFAULT, 0, &date, timeFormat, buffer, len);

		// put it together
		if (strTime.GetLength() > 0)
			strDateTime.Format(_T("%s, %s"), strDate, strTime);
		else
			strDateTime.Format(_T("%s"), strDate);

		return TRUE;
	}
	else {
		return FALSE;
	}
}

