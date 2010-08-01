/**
 *  gunzip.cpp
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

// based on gun.c from zlib package ()

#include "../StdAfx.h"
#include "../prssr.h"
#include "../../zlib/zlib.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "../debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* function declaration */
#define local static

/* buffer constants */
#define SIZE 32768U         /* input and output buffer sizes */
#define PIECE 16384         /* limits i/o chunks for 16-bit int case */

/* structure for infback() to pass to input function in() -- it maintains the
   input file and a buffer of size SIZE */
struct ind {
    HANDLE inFile;
    unsigned char *inBuf;
};

/* Load input buffer, assumed to be empty, and return bytes loaded and a
   pointer to them.  read() is called until the buffer is full, or until it
   returns end-of-file or error.  Return 0 on error. */
local unsigned in(void *in_desc, unsigned char **buf)
{
    int ret;
    unsigned len;
    unsigned char *next;
    struct ind *me = (struct ind *)in_desc;

    next = me->inBuf;
    *buf = next;
    len = 0;
    do {
        ret = PIECE;
        if ((unsigned) ret > SIZE - len)
            ret = (int) (SIZE - len);

		DWORD read;
		if (!ReadFile(me->inFile, next, ret, &read, NULL)) {
            len = 0;
            break;
        }
        next += ret;
        len += ret;
    } while (ret != 0 && len < SIZE);

    return len;
}

/* structure for infback() to pass to output function out() -- it maintains the
   output file, a running CRC-32 check on the output and the total number of
   bytes output, both for checking against the gzip trailer.  (The length in
   the gzip trailer is stored modulo 2^32, so it's ok if a long is 32 bits and
   the output is greater than 4 GB.) */
struct outd {
    HANDLE outFile;
    int check;                  /* true if checking crc and total */
    unsigned long crc;
    unsigned long total;
};

/* Write output buffer and update the CRC-32 and total bytes written.  write()
   is called until all of the output is written or an error is encountered.
   On success out() returns 0.  For a write failure, out() returns 1.  If the
   output file descriptor is -1, then nothing is written.
 */
local int out(void *out_desc, unsigned char *buf, unsigned len)
{
    int ret;
    struct outd *me = (struct outd *) out_desc;

    if (me->check) {
        me->crc = crc32(me->crc, buf, len);
        me->total += len;
    }
    if (me->outFile != NULL)
        do {
            ret = PIECE;
            if ((unsigned)ret > len)
                ret = (int)len;

			DWORD written;
			if (!WriteFile(me->outFile, buf, ret, &written, NULL))
				return 1;

            buf += ret;
            len -= ret;
        } while (len != 0);

    return 0;
}

/* next input byte macro for use inside lunpipe() and gunpipe() */
#define NEXT() (have ? 0 : (have = in(indp, &next)), \
                last = have ? (have--, (int)(*next++)) : -1)

/* memory for gunpipe() and lunpipe() --
   the first 256 entries of prefix[] and suffix[] are never used, could
   have offset the index, but it's faster to waste the memory */
unsigned char inbuf[SIZE];              /* input buffer */
unsigned char outbuf[SIZE];             /* output buffer */
unsigned short prefix[65536];           /* index to LZW prefix string */
unsigned char suffix[65536];            /* one-character LZW suffix */
unsigned char match[65280 + 2];         /* buffer for reversed match or gzip
                                           32K sliding window */

/* throw out what's left in the current bits byte buffer (this is a vestigial
   aspect of the compressed data format derived from an implementation that
   made use of a special VAX machine instruction!) */
#define FLUSHCODE() \
    do { \
        left = 0; \
        rem = 0; \
        if (chunk > have) { \
            chunk -= have; \
            have = 0; \
            if (NEXT() == -1) \
                break; \
            chunk--; \
            if (chunk > have) { \
                chunk = have = 0; \
                break; \
            } \
        } \
        have -= chunk; \
        next += chunk; \
        chunk = 0; \
    } while (0)

/* Decompress a compress (LZW) file from indp to outfile.  The compress magic
   header (two bytes) has already been read and verified.  There are have bytes
   of buffered input at next.  strm is used for passing error information back
   to gunpipe().

   lunpipe() will return Z_OK on success, Z_BUF_ERROR for an unexpected end of
   file, read error, or write error (a write error indicated by strm->next_in
   not equal to Z_NULL), or Z_DATA_ERROR for invalid input.
 */
local int lunpipe(int have, unsigned char *next, struct ind *indp,
                  HANDLE outFile, z_stream *strm)
{
    int last;                   /* last byte read by NEXT(), or -1 if EOF */
    int chunk;                  /* bytes left in current chunk */
    int left;                   /* bits left in rem */
    unsigned rem;               /* unused bits from input */
    int bits;                   /* current bits per code */
    unsigned code;              /* code, table traversal index */
    unsigned mask;              /* mask for current bits codes */
    int max;                    /* maximum bits per code for this stream */
    int flags;                  /* compress flags, then block compress flag */
    unsigned end;               /* last valid entry in prefix/suffix tables */
    unsigned temp;              /* current code */
    unsigned prev;              /* previous code */
    unsigned final;             /* last character written for previous code */
    unsigned stack;             /* next position for reversed string */
    unsigned outcnt;            /* bytes in output buffer */
    struct outd outd;           /* output structure */

    /* set up output */
    outd.outFile = outFile;
    outd.check = 0;

    /* process remainder of compress header -- a flags byte */
    flags = NEXT();
    if (last == -1)
        return Z_BUF_ERROR;
    if (flags & 0x60) {
        strm->msg = (char *)"unknown lzw flags set";
        return Z_DATA_ERROR;
    }
    max = flags & 0x1f;
    if (max < 9 || max > 16) {
        strm->msg = (char *)"lzw bits out of range";
        return Z_DATA_ERROR;
    }
    if (max == 9)                           /* 9 doesn't really mean 9 */
        max = 10;
    flags &= 0x80;                          /* true if block compress */

    /* clear table */
    bits = 9;
    mask = 0x1ff;
    end = flags ? 256 : 255;

    /* set up: get first 9-bit code, which is the first decompressed byte, but
       don't create a table entry until the next code */
    if (NEXT() == -1)                       /* no compressed data is ok */
        return Z_OK;
    final = prev = (unsigned)last;          /* low 8 bits of code */
    if (NEXT() == -1)                       /* missing a bit */
        return Z_BUF_ERROR;
    if (last & 1) {                         /* code must be < 256 */
        strm->msg = (char *)"invalid lzw code";
        return Z_DATA_ERROR;
    }
    rem = (unsigned)last >> 1;              /* remaining 7 bits */
    left = 7;
    chunk = bits - 2;                       /* 7 bytes left in this chunk */
    outbuf[0] = (unsigned char)final;       /* write first decompressed byte */
    outcnt = 1;

    /* decode codes */
    stack = 0;
    for (;;) {
        /* if the table will be full after this, increment the code size */
        if (end >= mask && bits < max) {
            FLUSHCODE();
            bits++;
            mask <<= 1;
            mask++;
        }

        /* get a code of length bits */
        if (chunk == 0)                     /* decrement chunk modulo bits */
            chunk = bits;
        code = rem;                         /* low bits of code */
        if (NEXT() == -1) {                 /* EOF is end of compressed data */
            /* write remaining buffered output */
            if (outcnt && out(&outd, outbuf, outcnt)) {
                strm->next_in = outbuf;     /* signal write error */
                return Z_BUF_ERROR;
            }
            return Z_OK;
        }
        code += (unsigned)last << left;     /* middle (or high) bits of code */
        left += 8;
        chunk--;
        if (bits > left) {                  /* need more bits */
            if (NEXT() == -1)               /* can't end in middle of code */
                return Z_BUF_ERROR;
            code += (unsigned)last << left; /* high bits of code */
            left += 8;
            chunk--;
        }
        code &= mask;                       /* mask to current code length */
        left -= bits;                       /* number of unused bits */
        rem = (unsigned)last >> (8 - left); /* unused bits from last byte */

        /* process clear code (256) */
        if (code == 256 && flags) {
            FLUSHCODE();
            bits = 9;                       /* initialize bits and mask */
            mask = 0x1ff;
            end = 255;                      /* empty table */
            continue;                       /* get next code */
        }

        /* special code to reuse last match */
        temp = code;                        /* save the current code */
        if (code > end) {
            /* Be picky on the allowed code here, and make sure that the code
               we drop through (prev) will be a valid index so that random
               input does not cause an exception.  The code != end + 1 check is
               empirically derived, and not checked in the original uncompress
               code.  If this ever causes a problem, that check could be safely
               removed.  Leaving this check in greatly improves gun's ability
               to detect random or corrupted input after a compress header.
               In any case, the prev > end check must be retained. */
            if (code != end + 1 || prev > end) {
                strm->msg = (char *)"invalid lzw code";
                return Z_DATA_ERROR;
            }
            match[stack++] = (unsigned char)final;
            code = prev;
        }

        /* walk through linked list to generate output in reverse order */
        while (code >= 256) {
            match[stack++] = suffix[code];
            code = prefix[code];
        }
        match[stack++] = (unsigned char)code;
        final = code;

        /* link new table entry */
        if (end < mask) {
            end++;
            prefix[end] = (unsigned short)prev;
            suffix[end] = (unsigned char)final;
        }

        /* set previous code for next iteration */
        prev = temp;

        /* write output in forward order */
        while (stack > SIZE - outcnt) {
            while (outcnt < SIZE)
                outbuf[outcnt++] = match[--stack];
            if (out(&outd, outbuf, outcnt)) {
                strm->next_in = outbuf; /* signal write error */
                return Z_BUF_ERROR;
            }
            outcnt = 0;
        }
        do {
            outbuf[outcnt++] = match[--stack];
        } while (stack);

        /* loop for next code with final and prev as the last match, rem and
           left provide the first 0..7 bits of the next code, end is the last
           valid table entry */
    }
}

/* Decompress a gzip file from infile to outfile.  strm is assumed to have been
   successfully initialized with inflateBackInit().  The input file may consist
   of a series of gzip streams, in which case all of them will be decompressed
   to the output file.  If outfile is -1, then the gzip stream(s) integrity is
   checked and nothing is written.

   The return value is a zlib error code: Z_MEM_ERROR if out of memory,
   Z_DATA_ERROR if the header or the compressed data is invalid, or if the
   trailer CRC-32 check or length doesn't match, Z_BUF_ERROR if the input ends
   prematurely or a write error occurs, or Z_ERRNO if junk (not a another gzip
   stream) follows a valid gzip stream.
 */
local int gunpipe(z_stream *strm, HANDLE inFile, HANDLE outFile)
{
    int ret, first, last;
    unsigned have, flags, len;
    unsigned char *next;
    struct ind ind, *indp;
    struct outd outd;

    /* setup input buffer */
    ind.inFile = inFile;
    ind.inBuf = inbuf;
    indp = &ind;

    /* decompress concatenated gzip streams */
    have = 0;                               /* no input data read in yet */
    first = 1;                              /* looking for first gzip header */
    strm->next_in = Z_NULL;                 /* so Z_BUF_ERROR means EOF */
    for (;;) {
        /* look for the two magic header bytes for a gzip stream */
        if (NEXT() == -1) {
            ret = Z_OK;
            break;                          /* empty gzip stream is ok */
        }
        if (last != 31 || (NEXT() != 139 && last != 157)) {
            strm->msg = (char *)"incorrect header check";
            ret = first ? Z_DATA_ERROR : Z_ERRNO;
            break;                          /* not a gzip or compress header */
        }
        first = 0;                          /* next non-header is junk */

        /* process a compress (LZW) file -- can't be concatenated after this */
        if (last == 157) {
            ret = lunpipe(have, next, indp, outFile, strm);
            break;
        }

        /* process remainder of gzip header */
        ret = Z_BUF_ERROR;
        if (NEXT() != 8) {                  /* only deflate method allowed */
            if (last == -1) break;
            strm->msg = (char *)"unknown compression method";
            ret = Z_DATA_ERROR;
            break;
        }
        flags = NEXT();                     /* header flags */
        NEXT();                             /* discard mod time, xflgs, os */
        NEXT();
        NEXT();
        NEXT();
        NEXT();
        NEXT();
        if (last == -1) break;
        if (flags & 0xe0) {
            strm->msg = (char *)"unknown header flags set";
            ret = Z_DATA_ERROR;
            break;
        }
        if (flags & 4) {                    /* extra field */
            len = NEXT();
            len += (unsigned)(NEXT()) << 8;
            if (last == -1) break;
            while (len > have) {
                len -= have;
                have = 0;
                if (NEXT() == -1) break;
                len--;
            }
            if (last == -1) break;
            have -= len;
            next += len;
        }
        if (flags & 8)                      /* file name */
            while (NEXT() != 0 && last != -1)
                ;
        if (flags & 16)                     /* comment */
            while (NEXT() != 0 && last != -1)
                ;
        if (flags & 2) {                    /* header crc */
            NEXT();
            NEXT();
        }
        if (last == -1) break;

        /* set up output */
        outd.outFile = outFile;
        outd.check = 1;
        outd.crc = crc32(0L, Z_NULL, 0);
        outd.total = 0;

        /* decompress data to output */
        strm->next_in = next;
        strm->avail_in = have;
        ret = inflateBack(strm, in, indp, out, &outd);
        if (ret != Z_STREAM_END) break;
        next = strm->next_in;
        have = strm->avail_in;
        strm->next_in = Z_NULL;             /* so Z_BUF_ERROR means EOF */

        /* check trailer */
        ret = Z_BUF_ERROR;
        if (NEXT() != (signed) (outd.crc & 0xff) ||
            NEXT() != (signed) ((outd.crc >> 8) & 0xff) ||
            NEXT() != (signed) ((outd.crc >> 16) & 0xff) ||
            NEXT() != (signed) ((outd.crc >> 24) & 0xff)) {
            /* crc error */
            if (last != -1) {
                strm->msg = (char *)"incorrect data check";
                ret = Z_DATA_ERROR;
            }
            break;
        }
        if (NEXT() != (signed) (outd.total & 0xff) ||
            NEXT() != (signed) ((outd.total >> 8) & 0xff) ||
            NEXT() != (signed) ((outd.total >> 16) & 0xff) ||
            NEXT() != (signed) ((outd.total >> 24) & 0xff)) {
            /* length error */
            if (last != -1) {
                strm->msg = (char *)"incorrect length check";
                ret = Z_DATA_ERROR;
            }
            break;
        }

        /* go back and look for another gzip stream */
    }

    /* clean up and return */
    return ret;
}


BOOL GunzipFile(LPCTSTR inFileName, LPCTSTR outFileName) {
	BOOL ret = FALSE;

    unsigned char *window;
    z_stream strm;

    /* initialize inflateBack state for repeated use */
    window = match;                         /* reuse LZW match buffer */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = inflateBackInit(&strm, 15, window);

	//
	HANDLE inFile = CreateFile(inFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (inFile != INVALID_HANDLE_VALUE) {
		HANDLE outFile = CreateFile(outFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (outFile != INVALID_HANDLE_VALUE) {
			// decompress
			ret = gunpipe(&strm, inFile, outFile);

			CloseHandle(outFile);
		}

		CloseHandle(inFile);
	}

    /* clean up */
    inflateBackEnd(&strm);

	return ret;
}

#define CHUNK					8192

int inf(HANDLE source, HANDLE dest) {
	BOOL mDummyStreamInitialised = FALSE;

	int ret;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	// allocate inflate state
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	DWORD read;
	// decompress until deflate stream ends or end of file
	do {
		if (strm.avail_in == 0) {
			if (!ReadFile(source, in, CHUNK, &read, NULL)) {
				(void) inflateEnd(&strm);
				return Z_ERRNO;
			}

			strm.avail_in = read;
			if (strm.avail_in == 0)
				break;
			strm.next_in = in;
		}

		// run inflate() on input until output buffer not full
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);

			if (ret == Z_DATA_ERROR) {
				// this is taken form firefox source code
				//
				// some servers (notably Apache with mod_deflate) don't generate zlib headers
				// insert a dummy header and try again
				static char dummy_head[2] = {
					0x8 + 0x7 * 0x10,
					(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
				};
				inflateReset(&strm);
				strm.next_in = (Bytef*) dummy_head;
				strm.avail_in = sizeof(dummy_head);

				ret = inflate(&strm, Z_NO_FLUSH);
				if (ret != Z_OK)
					return Z_ERRNO;

				// stop an endless loop caused by non-deflate data being labelled as deflate
				if (mDummyStreamInitialised) {
					// endless loop detected
					return Z_ERRNO;
				}
				mDummyStreamInitialised = TRUE;
				// reset stream pointers to our original data
				strm.next_in = in;
				strm.avail_in = (uInt) read;
			}
			else if (ret == Z_MEM_ERROR) {
				(void) inflateEnd(&strm);
				return ret;
			}
			else if (ret == Z_OK) {
				have = CHUNK - strm.avail_out;
				DWORD written;
				if (!WriteFile(dest, out, have, &written, NULL) || written != have) {
					(void) inflateEnd(&strm);
					return Z_ERRNO;
				}
			}
		} while (strm.avail_out == 0);

		// done when inflate() says it's done
	} while (ret != Z_STREAM_END);

	// clean up and return
	(void) inflateEnd(&strm);

	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

BOOL InflateFile(LPCTSTR inFileName, LPCTSTR outFileName) {
	BOOL ret = FALSE;

	//
	HANDLE inFile = CreateFile(inFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (inFile != INVALID_HANDLE_VALUE) {
		HANDLE outFile = CreateFile(outFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (outFile != INVALID_HANDLE_VALUE) {
			// decompress
			ret = inf(inFile, outFile);

			CloseHandle(outFile);
		}

		CloseHandle(inFile);
	}

	return ret;
}
