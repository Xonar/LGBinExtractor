/*
 * APHeader.c
 *
 *  Created on: Nov 18, 2012
 *      Author: xonar
 */

#include "APHeader.h"

APHeader readAPHeader(FILE *f)
{
    APHeader out;

    //Read Magic Number into Header Directly
    fread(&out, sizeof(char) * 4, 1, f);
    out.pent_num = 0;

    //Count how many Partition Entries
    //Might Break when there's 63 or more entries
    while(1)
    {
        uint64_t tmp;
        fread(&tmp, sizeof(tmp), 1, f);
        if(tmp == UINT64_MAX) break;
        else out.pent_num++;
    }

    fseek(f, 4, SEEK_SET);

    out.pent_arr = (APPartitionEntry*) calloc(out.pent_num, sizeof(APPartitionEntry));

    int i = 0;

    for(; i < out.pent_num; i++)
    {
        fread(&out.pent_arr[i].file_off, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].file_size, sizeof(uint32_t), 1, f);
    }

    skipToNextLBA(f);

    for(i = 0; i < out.pent_num; i++)
    {
        //fread(&out.pent_arr[i].pent_id,sizeof(char),1,f);
        fseek(f, 4, SEEK_CUR);
        fread(&out.pent_arr[i].disk_size, sizeof(uint32_t), 1, f);
        fseek(f, 504, SEEK_CUR);
    }

    fseek(f,0x2200,SEEK_SET);

    for(i = 0; i < out.pent_num; i++)
    {
        fread(&out.pent_arr[i].pent_id, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].name, sizeof(char), 256, f);
    }

    return out;
}

void printAPHeader(const APHeader h, FILE *f)
{
    fprintf(f, "AP HEADER\n----------\n%-30s", "Magic Number");
    printHexString(f, h.magic, 4);
    fprintf(f, "\n%-30s%d\n", "Number of Partitions", h.pent_num);
}

void printAPPartitionEntry(const APPartitionEntry pe, FILE *f)
{
    fprintf(f, "PARTITION ENTRY\n------------\n"
            "    %-26s%s\n"
            "    %-26s%d\n"
            "    %-26s%" PRIu32 "\n"
    "    %-26s%" PRIu32 "\n"
    "    %-26s%" PRIu32 "\n",  "Data Block Name", pe.name,"Data Block ID", pe.pent_id,
            "File Offset", pe.file_off, "Size on File", pe.file_size, "Size on Disk", pe.disk_size);
}

void printFullAPInfo(const APHeader h, FILE *f)
{
    printAPHeader(h, f);
    int i = 0;

    fprintf(f, "\nPARTITION ENTRIES\n-----------------\n");

    for(; i < h.pent_num; i++)
    {
        fputs("\n", f);
        printAPPartitionEntry(h.pent_arr[i], f);
    }
}
