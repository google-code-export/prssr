// notif.h - notifications
//

#if !defined(_NOTIF_H_)
#define _NOTIF_H_

void SetupCradlingEvent(BOOL bEnable);

//void SetupPeriodicNotification(BOOL bEnable, int updateInterval);
//void SetupDailyNotification(BOOL bEnable, SYSTEMTIME &updateTime);

void AddSeconds(SYSTEMTIME &st, DWORD seconds);
void SetupUpdateEvent(LPCTSTR arg, BOOL bEnable, SYSTEMTIME *updateTime);

#endif
