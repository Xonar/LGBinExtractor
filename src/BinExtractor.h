/*
 * BinExtractor.h
 *
 *  Created on: Nov 18, 2012
 *    Author: xonar
 */

#ifndef BINEXTRACTOR_H
#define BINEXTRACTOR_H

#if defined(_WIN32) || defined(_WIN64)
#define PROG_NAME "BinExtractor.exe"
#else
#define PROG_NAME "BinExtractor"
#endif

#define DEBUG 0

int displayGPT(const char* path);
int displayAP(const char* path);
int splitBinFile(const char* path);
int extractBinHeaderFile(const char* path);

void skipToNextLBA(FILE* f);

void printUsage();

_Bool canOpenFile(const char* path);

void printHex(FILE* f,const void* data,const unsigned int len);

#endif /* BINEXTRACTOR_H_ */
