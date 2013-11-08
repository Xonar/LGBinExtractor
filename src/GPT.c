/*
 * GPT.c
 *
 *  Created on: Nov 18, 2012
 *    Author: xonar
 */

#include "GPT.h"
#include "BinExtractor.h"

GPTHeader readGPTHeader(FILE* f)
{
  /*READ GPT HEADER INTO STRUCT*/
  GPTHeader out;
  fread(&out, sizeof(GPTHeader), 1, f);
  return out;
}

GPTPartitionEntry readGPTPartitionEntry(FILE* f)
{
  /*Declarations*/
  GPTPartitionEntry out;
  int i = 1, j = 2;

  fread(&out, sizeof(GPTPartitionEntry), 1, f);

  /*Do poor mans convention from UTF16 to UTF8
   Ignoring all non ASCII characters*/

  for(; i < 32; i++, j += 2)
    out.part_name[i] = out.part_name[j];

  out.part_name[32] = '\0';

  return out;
}

void readGPTPartitionEntryArray(FILE* f, GPTPartitionEntry* out, int num)
{
  fread(out, sizeof(GPTPartitionEntry), num, f);

  /*Do poor mans convention from UTF16 to UTF8
   Ignoring all non ASCII characters*/

  num--;
  for(; num >= 0; num--)
  {
    int i = 1;
    int j = 2;
    for(; i < 32; i++, j += 2)
      out[num].part_name[i] = out[num].part_name[j];

    out[num].part_name[32] = '\0';
  }
}

void printGPTHeader(const GPTHeader h, FILE* f)
{
  fprintf(f, "GPT HEADER\n----------\n%-30s", "Signature");

  printHex(f, h.signature, 8);

  fprintf(f, "\n%-30s%" PRIu32 "\n"
  "%-30s%" PRIu32 "\n"
  "%-30s", "Revision", h.revision, "Header Size", h.hsize, "CRC32 of Header");

  printHex(f, &h.crc,sizeof(uint32_t));

  fprintf(f, "\n%-30s%" PRIu64 "\n"
  "%-30s%" PRIu64 "\n"
  "%-30s%" PRIu64 "\n"
  "%-30s%" PRIu64 "\n"
  "%-30s", "Current Header LBA", h.current_lba, "Backup Header LBA", h.backup_lba,
      "First Usable LBA", h.first_lba, "Last Usable LBA", h.last_lba, "Disk GUID");

  printHex(f, h.disk_guid, 16);

  fprintf(f, "\n%-30s%" PRIu64 "\n"
  "%-30s%" PRIu32 "\n"
  "%-30s%" PRIu32 "\n"
  "%-30s", "Start of Partition Entries", h.pent_lba, "Number of Partition Entries", h.pent_num,
      "Size of Partition Entries", h.pent_size, "CRC32 of Partition Array");

  printHex(f, &h.crc_part,sizeof(uint32_t));

  fputc('\n', f);
}

void printGPTPartitionEntry(const GPTPartitionEntry pe, FILE* f)
{
  fprintf(f, "PARTITION ENTRY\n---------------\n  %-26s", "Partition Type GUID");

  printHex(f, pe.ptype_guid, 16);

  fprintf(f, "\n  %-26s", "Unique Partition GUID");

  printHex(f, pe.upart_guid, 16);

  fprintf(f, "\n  %-26s%" PRIu64 "\n"
  "  %-26s%" PRIu64 "\n"
  "  %-26s%" PRIu64 "\n"
  "  %-26s%s\n", "First LBA", pe.first_lba, "Last LBA", pe.last_lba, "Attributes", pe.att_flags,
      "Partition Name", pe.part_name);
}

void printFullGPTInfo(const GPTHeader h, const GPTPartitionEntry* pe, FILE* f)
{
  int i = 0;
  GPTPartitionEntry PE_NULL =
  {
  { 0 },
  { 0 }, 0, 0, 0, "" };

  printGPTHeader(h, f);

  fprintf(f, "\nPARTITION ENTRIES\n-----------------\n");

  for(; i < h.pent_num && memcmp(&pe[i], &PE_NULL, sizeof(GPTPartitionEntry)); i++)
  {
    fputs("\n", f);
    printGPTPartitionEntry(pe[i], f);
  }
}
