/*
 * APHeader.h
 *
 *  Created on: Nov 18, 2012
 *    Author: xonar
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
  char name[257];/*256 + 1 for terminating char*/
  uint32_t pent_id;
  uint32_t file_off;
  uint32_t file_size;
  uint32_t disk_size;
  uint32_t disk_off;
} APPartitionEntry;

/*AP HEADER STRUCT*/

/*MAGIC NUMBER*/
typedef struct _MagicNumber
{
  uint32_t magic;
  unsigned int off;
  struct _MagicNumber* next;
} MagicNumber;

#define INIT_MAGICNUMBER(mn)        \
    mn.magic = 0;             \
    mn.off = 0;             \
    mn.next = NULL;

typedef struct _APHeader
{
  MagicNumber magic;
  int pent_num;
  APPartitionEntry* pent_arr;
} APHeader;

#define INIT_APHEADER(aph)          \
   aph.pent_num = 0;            \
   aph.pent_arr = NULL;           \
   aph.magic.magic = 0;           \
   aph.magic.off = 0;             \
   aph.magic.next = NULL;

typedef enum _DataType
{
  DISK_SIZE, DISK_OFF, FILE_SIZE, FILE_OFF, BLOCK_ID, BLOCK_NAME, SKIP
} DataType;

typedef struct _Item
{
  DataType type;
  size_t size;
} Item;

/*DATA BLOCK*/
typedef struct _DataBlock
{
  size_t blockOff;
  size_t blockSize;
  uint8_t numItems;
  Item* items;
  struct _DataBlock* next;
} DataBlock;

#define INIT_DATABLOCK(db)          \
    db.blockOff = 0;                \
    db.blockSize = 0                \
    db.numBlocks = 0;               \
    db.numItems = 0;                \
    db.items = NULL;                \
    db.next = NULL;

/*READ*/
APHeader readAPHeader(FILE *f);

APHeader readAPHeader44DD55AA(FILE *f);
APHeader readAPHeader44EC55AA(FILE *f);
APHeader readAPHeaderA5A555AA(FILE *f);

/*DISPLAY*/
void printAPHeader(const APHeader h, FILE *f);
void printAPPartitionEntry(const APPartitionEntry pe, FILE *f);
void printFullAPInfo(const APHeader h, FILE *f);

#endif /* APHEADER_H_ */
