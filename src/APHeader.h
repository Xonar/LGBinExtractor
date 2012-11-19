/*
 * APHeader.h
 *
 *  Created on: Nov 18, 2012
 *      Author: xonar
 */

#ifndef APHEADER_H
#define APHEADER_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "BinExtractor.h"

typedef struct _APPartitionEntry {
    char name[256];
    uint32_t pent_id;
    uint32_t file_off;
    uint32_t file_size;
    uint32_t disk_size;
} APPartitionEntry;

typedef struct _APHeader {
    char magic[4];
    int pent_num;
    APPartitionEntry* pent_arr;
} APHeader;

APHeader readAPHeader(FILE *f);

void printAPHeader(const APHeader h,FILE *f);
void printAPPartitionEntry(const APPartitionEntry pe,FILE *f);
void printFullAPInfo(const APHeader h,FILE *f);

#endif /* APHEADER_H_ */
