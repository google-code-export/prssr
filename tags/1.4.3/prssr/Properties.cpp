/**
 *  Properties.cpp
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
#include "prssr.h"
#include "Properties.h"

#include "ctrls/CePropertySheet.h"
#include "PropGeneralPg.h"
#include "PropArchivePg.h"
#include "PropUpdatingPg.h"
#include "PropCachingPg.h"
#include "PropAuthenticationPg.h"


int SiteProperties(CSiteItem *siteItem, CWnd *parent) {
	LOG0(1, "SiteProperties()");

	CPropGeneralPg pgGeneral;
	CPropCachingPg pgCaching;
	CPropArchivePg pgArchive;
	CPropAuthenticationPg pgAuthentication;

	CCePropertySheet sheet(IDS_PROPERTIES, parent);
	sheet.SetMenu(IDR_DONE);
	sheet.AddPage(&pgGeneral);
	sheet.AddPage(&pgCaching);
	sheet.AddPage(&pgArchive);
	sheet.AddPage(&pgAuthentication);

	// general
	pgGeneral.m_strName = siteItem->Name;
	pgGeneral.m_strURL = siteItem->Info->XmlUrl;
	pgGeneral.m_bShowOnToday = siteItem->Info->TodayShow;

	// caching
	pgCaching.m_bSpecificCaching = !siteItem->Info->UseGlobalCacheOptions;
	pgCaching.m_bCacheOnlineContent = siteItem->Info->CacheHtml;
	pgCaching.m_bStoreImages = siteItem->Info->CacheItemImages;

	pgCaching.m_bCacheEnclosures = siteItem->Info->CacheEnclosures;
	if (siteItem->Info->EnclosureLimit > 0) {
		pgCaching.m_bEnclosureLimit = TRUE;
		pgCaching.m_nMB = siteItem->Info->EnclosureLimit;
	}
	else {
		pgCaching.m_bEnclosureLimit = FALSE;
		pgCaching.m_nMB = 0;
	}

	// archive
	if (siteItem->Info->CacheLimit > 0) {
		pgArchive.m_nCacheLimit = 3;
		pgArchive.m_nCacheItems = siteItem->Info->CacheLimit;
	}
	else {
		switch (siteItem->Info->CacheLimit) {
			default:
			case CACHE_LIMIT_DEFAULT:	pgArchive.m_nCacheLimit = 0; break;
			case CACHE_LIMIT_DISABLED:	pgArchive.m_nCacheLimit = 1; break;
			case CACHE_LIMIT_UNLIMITED:	pgArchive.m_nCacheLimit = 2; break;
		}
	}

	// authentication
	if (siteItem->Info->UserName.IsEmpty()) {
		pgAuthentication.m_bAuthRequired = FALSE;
		pgAuthentication.m_strUserName.Empty();
		pgAuthentication.m_strPassword.Empty();
	}
	else {
		pgAuthentication.m_bAuthRequired = TRUE;
		pgAuthentication.m_strUserName = siteItem->Info->UserName;
		pgAuthentication.m_strPassword = siteItem->Info->Password;
	}

	int res = sheet.DoModal();
	if (res) {
		// general
		siteItem->Name = pgGeneral.m_strName;
		siteItem->Info->XmlUrl = pgGeneral.m_strURL;
		siteItem->Info->TodayShow = pgGeneral.m_bShowOnToday;

		// caching
		siteItem->Info->UseGlobalCacheOptions = !pgCaching.m_bSpecificCaching;
		siteItem->Info->CacheItemImages = pgCaching.m_bStoreImages;
		siteItem->Info->CacheHtml= pgCaching.m_bCacheOnlineContent;

		siteItem->Info->CacheEnclosures = pgCaching.m_bCacheEnclosures;
		if (pgCaching.m_bEnclosureLimit)
			siteItem->Info->EnclosureLimit = pgCaching.m_nMB;
		else
			siteItem->Info->EnclosureLimit = 0;

		// archive
		switch (pgArchive.m_nCacheLimit) {
			default:
			case 0: siteItem->Info->CacheLimit = CACHE_LIMIT_DEFAULT; break;
			case 1: siteItem->Info->CacheLimit = CACHE_LIMIT_DISABLED; break;
			case 2: siteItem->Info->CacheLimit = CACHE_LIMIT_UNLIMITED; break;
			case 3: siteItem->Info->CacheLimit = pgArchive.m_nCacheItems; break;
		}

		// authentication
		if (pgAuthentication.m_bAuthRequired) {
			siteItem->Info->UserName = pgAuthentication.m_strUserName;
			siteItem->Info->Password = pgAuthentication.m_strPassword;
		}
		else {
			siteItem->Info->UserName.Empty();
			siteItem->Info->Password.Empty();
		}
	}

	return res;
}
