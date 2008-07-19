// Properties.cpp : implementation of the properties
//

#include "stdafx.h"
#include "prssr.h"
#include "Properties.h"

#include "ctrls/CePropertySheet.h"
#include "PropGeneralPg.h"
#include "PropArchivePg.h"
#include "PropUpdatingPg.h"
#include "PropCachingPg.h"
#include "PropRewritingPg.h"
#include "PropAuthenticationPg.h"


int SiteProperties(CSiteItem *siteItem, CWnd *parent) {
	LOG0(1, "SiteProperties()");

	CPropGeneralPg pgGeneral;
	CPropCachingPg pgCaching;
	CPropArchivePg pgArchive;
//	CPropUpdatingPg pgUpdating;
	CPropRewritingPg pgRewriting;
	CPropAuthenticationPg pgAuthentication;

	CCePropertySheet sheet(IDS_PROPERTIES, parent);
	sheet.SetMenu(IDR_DONE);
	sheet.AddPage(&pgGeneral);
	sheet.AddPage(&pgCaching);
//	sheet.AddPage(&pgUpdating);
	sheet.AddPage(&pgArchive);
	sheet.AddPage(&pgRewriting);
	sheet.AddPage(&pgAuthentication);

	// general
	pgGeneral.m_strName = siteItem->Name;
	pgGeneral.m_strURL = siteItem->Info->XmlUrl;
	pgGeneral.m_bShowOnToday = siteItem->Info->TodayShow;

/*	// updating
	if (siteItem->Info->UpdateInterval > 0) {
		pgUpdating.m_nUpdate = 1;
		pgUpdating.m_nUpdateMinutes = siteItem->Info->UpdateInterval;
	}
	else {
		if (siteItem->Info->UpdateInterval == UPDATE_INTERVAL_NO_AUTO)
			pgUpdating.m_nUpdate = 2;
		else
			pgUpdating.m_nUpdate = 0;
	}
	if (siteItem->Feed != NULL)
		pgUpdating.m_nSuggestInterval = siteItem->Feed->UpdateInterval;
	else
		pgUpdating.m_nSuggestInterval = 0;
*/
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

	// rewrite rules
	for (int i = 0; i < siteItem->Info->RewriteRules.GetSize(); i++) {
		CRewriteRule *rr = siteItem->Info->RewriteRules[i];

		CRewriteRule *dupRR = new CRewriteRule();
		*dupRR = *rr;
		pgRewriting.Rules.SetAtGrow(i, dupRR);
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

/*		// updating
		switch (pgUpdating.m_nUpdate) {
			default:
			case 0: siteItem->Info->UpdateInterval = UPDATE_INTERVAL_GLOBAL; break;
			case 1: siteItem->Info->UpdateInterval = pgUpdating.m_nUpdateMinutes; break;
			case 2: siteItem->Info->UpdateInterval = UPDATE_INTERVAL_NO_AUTO; break;
		}
*/
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

		// rewrite rules
		int i;
		for (i = 0; i < siteItem->Info->RewriteRules.GetSize(); i++)
			delete siteItem->Info->RewriteRules[i];
		for (i = 0; i < pgRewriting.Rules.GetSize(); i++)
			siteItem->Info->RewriteRules.SetAtGrow(i, pgRewriting.Rules[i]);

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
	else {
		// free previously allocated rewrite rules (duplicates)
		for (int i = 0; i < pgRewriting.Rules.GetSize(); i++)
			delete pgRewriting.Rules[i];
	}

	return res;
}
