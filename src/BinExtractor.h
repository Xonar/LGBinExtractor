/*
 * BinExtractor.h
 *
 *  Created on: Nov 18, 2012
 *      Author: xonar
 */

#ifndef BINEXTRACTOR_H
#define BINEXTRACTOR_H

int displayGPT(const char* path);
int displayAP(const char* path);

void skipToNextLBA(FILE* f);

void printHexString( FILE* f, const char* string,  const int len);
void printHexUINT32( FILE* f, uint32_t num);
void printHexUINT64( FILE* f, uint64_t num);

#endif /* BINEXTRACTOR_H_ */
