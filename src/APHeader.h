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

/*PARTITION ENTRY STRUCT*/
typedef struct _APPartitionEntry
{
    char name[256];
    uint32_t pent_id;
    uint32_t file_off;
    uint32_t file_size;
    uint32_t disk_size;
    uint32_t disk_off;
} APPartitionEntry;

/*AP HEADER STRUCT*/
typedef struct _APHeader
{
    unsigned char magic[4];
    int pent_num;
    APPartitionEntry* pent_arr;
} APHeader;

#define INIT_APHEADER(aph,m1,m2,m3,m4)  \
   out.pent_num = 0;                    \
   out.pent_arr = NULL;                 \
                                        \
   out.magic[0] = m1;                   \
   out.magic[1] = m2;                   \
   out.magic[2] = m3;                   \
   out.magic[3] = m4;                   \
                                        \


/*READ*/
APHeader readAPHeader(FILE *f);

APHeader readAPHeader44DD55AA(FILE *f);
APHeader readAPHeader44DD55AA_2BF67889(FILE *f);
APHeader readAPHeader44DD55AA_2BF67889_AA55EC33(FILE *f);
APHeader readAPHeader44DD55AA_AF33BFDE(FILE *f);
APHeader readAPHeader44DD55AA_AABB00CC(FILE *f);
APHeader readAPHeader44DD55AA_AABB00CC_AA55EC33(FILE *f);
APHeader readAPHeader44DD55AA_EAC86250(FILE *f);
APHeader readAPHeaderA5A555AA(FILE *f);

/*DISPLAY*/
void printAPHeader(const APHeader h, FILE *f);
void printAPPartitionEntry(const APPartitionEntry pe, FILE *f);
void printFullAPInfo(const APHeader h, FILE *f);

#endif /* APHEADER_H_ */
