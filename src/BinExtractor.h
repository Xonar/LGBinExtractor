/*
 * BinExtractor.h
 *
 *  Created on: Nov 18, 2012
 *      Author: xonar
 */

#ifndef BINEXTRACTOR_H
#define BINEXTRACTOR_H

#if defined(_WIN32) || defined(_WIN64)

/* I regret this bit*/
#define strcasecmp _stricmp

#define PROG_NAME "BinExtractor.exe"
#else
#define PROG_NAME "BinExtractor"
#endif

int displayGPT(const char* path);
int displayAP(const char* path);
int splitBinFile(const char* path);

void skipToNextLBA(FILE* f);

void printUsage();

_Bool canOpenFile(const char* path);

void printHexString_u(FILE* f, const unsigned char* string, const unsigned int len);
void printHexString_s(FILE* f, const char* string, const unsigned int len);
void printHexUINT32(FILE* f, uint32_t num);
void printHexUINT64(FILE* f, uint64_t num);

#endif /* BINEXTRACTOR_H_ */
