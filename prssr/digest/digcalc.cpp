#include "md5/global.h"
#include "md5/MD5.h"
#include <string.h>
#include "digcalc.h"

void CvtHex(
	IN HASH Bin,
	OUT HASHHEX Hex
	)
{
	unsigned short i;
	unsigned char j;

	for (i = 0; i < HASHLEN; i++) {
		j = (Bin[i] >> 4) & 0xf;
		if (j <= 9)
			Hex[i*2] = (j + '0');
		else
			Hex[i*2] = (j + 'a' - 10);
		j = Bin[i] & 0xf;
		if (j <= 9)
			Hex[i*2+1] = (j + '0');
		else
			Hex[i*2+1] = (j + 'a' - 10);
	};
	Hex[HASHHEXLEN] = '\0';
};

/* calculate H(A1) as per spec */
void DigestCalcHA1(
	IN char * pszAlg,
	IN char * pszUserName,
	IN char * pszRealm,
	IN char * pszPassword,
	IN char * pszNonce,
	IN char * pszCNonce,
	OUT HASHHEX SessionKey
	)
{
	MD5_CTX Md5Ctx;
	HASH HA1;
	HASHHEX HA1Hex;

	MD5Init(&Md5Ctx);
	MD5Update(&Md5Ctx, (unsigned char *) pszUserName, strlen(pszUserName));
	MD5Update(&Md5Ctx, (unsigned char *) ":", 1);
	MD5Update(&Md5Ctx, (unsigned char *) pszRealm, strlen(pszRealm));
	MD5Update(&Md5Ctx, (unsigned char *) ":", 1);
	MD5Update(&Md5Ctx, (unsigned char *) pszPassword, strlen(pszPassword));
	MD5Final(HA1, &Md5Ctx);
	if (_stricmp(pszAlg, "md5-sess") == 0) {
		CvtHex(HA1, HA1Hex);
		MD5Init(&Md5Ctx);
		MD5Update(&Md5Ctx, (unsigned char *) HA1Hex, HASHHEXLEN);
		MD5Update(&Md5Ctx, (unsigned char *) ":", 1);
		MD5Update(&Md5Ctx, (unsigned char *) pszNonce, strlen(pszNonce));
		MD5Update(&Md5Ctx, (unsigned char *) ":", 1);
		MD5Update(&Md5Ctx, (unsigned char *) pszCNonce, strlen(pszCNonce));
		MD5Final(HA1, &Md5Ctx);
	};
	CvtHex(HA1, SessionKey);
};

/* calculate request-digest/response-digest as per HTTP Digest spec */
void DigestCalcResponse(
	IN HASHHEX HA1,           /* H(A1) */
	IN char * pszNonce,       /* nonce from server */
	IN char * pszNonceCount,  /* 8 hex digits */
	IN char * pszCNonce,      /* client nonce */
	IN char * pszQop,         /* qop-value: "", "auth", "auth-int" */
	IN char * pszMethod,      /* method from the request */
	IN char * pszDigestUri,   /* requested URL */
	IN HASHHEX HEntity,       /* H(entity body) if qop="auth-int" */
	OUT HASHHEX Response      /* request-digest or response-digest */
	)
{
	MD5_CTX Md5Ctx;
	HASH HA2;
	HASH RespHash;
	HASHHEX HA2Hex;

	// calculate H(A2)
	MD5Init(&Md5Ctx);
	MD5Update(&Md5Ctx, (unsigned char *) pszMethod, strlen(pszMethod));
	MD5Update(&Md5Ctx, (unsigned char *) ":", 1);
	MD5Update(&Md5Ctx, (unsigned char *) pszDigestUri, strlen(pszDigestUri));
	if (_stricmp(pszQop, "auth-int") == 0) {
		MD5Update(&Md5Ctx, (unsigned char *) ":", 1);
		MD5Update(&Md5Ctx, (unsigned char *) HEntity, HASHHEXLEN);
	};
	MD5Final(HA2, &Md5Ctx);
	CvtHex(HA2, HA2Hex);

	// calculate response
	MD5Init(&Md5Ctx);
	MD5Update(&Md5Ctx, (unsigned char *) HA1, HASHHEXLEN);
	MD5Update(&Md5Ctx, (unsigned char *) ":", 1);
	MD5Update(&Md5Ctx, (unsigned char *) pszNonce, strlen(pszNonce));
	MD5Update(&Md5Ctx, (unsigned char *) ":", 1);
	if (*pszQop) {
		MD5Update(&Md5Ctx, (unsigned char *) pszNonceCount, strlen(pszNonceCount));
		MD5Update(&Md5Ctx, (unsigned char *) ":", 1);
		MD5Update(&Md5Ctx, (unsigned char *) pszCNonce, strlen(pszCNonce));
		MD5Update(&Md5Ctx, (unsigned char *) ":", 1);
		MD5Update(&Md5Ctx, (unsigned char *) pszQop, strlen(pszQop));
		MD5Update(&Md5Ctx, (unsigned char *) ":", 1);
	};
	MD5Update(&Md5Ctx, (unsigned char *) HA2Hex, HASHHEXLEN);
	MD5Final(RespHash, &Md5Ctx);
	CvtHex(RespHash, Response);
};
