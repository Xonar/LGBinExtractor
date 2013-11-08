#ifndef GPT_H
#define GPT_H

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/*GPT HEADER STRUCT*/
typedef struct _GPTHeader
{
  char signature[8];
  uint32_t revision;
  uint32_t hsize;
  uint32_t crc;
  uint32_t reserved;
  uint64_t current_lba;
  uint64_t backup_lba;
  uint64_t first_lba;
  uint64_t last_lba;
  unsigned char disk_guid[16];
  uint64_t pent_lba;
  uint32_t pent_num;
  uint32_t pent_size;
  uint32_t crc_part;
  char end[420];
} GPTHeader;

/*PARTITION ENTRY STRUCT*/
typedef struct _GPTPartitionEntry
{
  unsigned char ptype_guid[16];
  unsigned char upart_guid[16];
  uint64_t first_lba;
  uint64_t last_lba;
  uint64_t att_flags;
  char part_name[72];
} GPTPartitionEntry;

/*READ*/
GPTHeader readGPTHeader(FILE* f);
GPTPartitionEntry readGPTPartitionEntry(FILE* f);
void readGPTPartitionEntryArray(FILE* f, GPTPartitionEntry* out, int num);

/*DISPLAY*/
void printGPTHeader(const GPTHeader h, FILE* f);
void printGPTPartitionEntry(const GPTPartitionEntry pe, FILE* f);
void printFullGPTInfo(const GPTHeader h, const GPTPartitionEntry* pe, FILE* f);

#endif
