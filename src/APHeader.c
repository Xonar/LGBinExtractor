/*
 * APHeader.c
 *
 *  Created on: Nov 18, 2012
 *      Author: xonar
 */

#include "APHeader.h"

APHeader readAPHeader(FILE *f)
{
    APHeader out =
    {
    { 0, 0, 0, 0 }, 0, 0 };
    uint32_t* magic = (uint32_t*) &out.magic;

    //READ MAGIC NUMBER
    fread(&out, sizeof(char) * 4, 1, f);

    //Split into Magic Numbers
    switch(*magic)
    {
        case 0xaa55a5a5:
            out = readAPHeaderA5A555AA(f);
            break;
        case 0xaa55dd44:
            out = readAPHeader44DD55AA(f);
            break;
        default:
            fprintf(stderr, "Unsupported Bin Type : ");
            printHexUINT32(stderr, *magic);
            fprintf(stderr, "\n");
            break;
    }

    return out;
}

APHeader readAPHeaderA5A555AA(FILE *f)
{
    APHeader out;

    out.magic[0] = 0xA5;
    out.magic[1] = 0xA5;
    out.magic[2] = 0x55;
    out.magic[3] = 0xAA;

    //COUNT AND ALLOCATE AP ENTRIES
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

    //READ FILE OFFSET AND SIZE
    for(; i < out.pent_num; i++)
    {
        fread(&out.pent_arr[i].file_off, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].file_size, sizeof(uint32_t), 1, f);
    }

    skipToNextLBA(f);

    //READ DISK SIZE IGNORING FIRST ID REFERENCE
    for(i = 0; i < out.pent_num; i++)
    {
        //fread(&out.pent_arr[i].pent_id,sizeof(char),1,f);
        fseek(f, 4, SEEK_CUR);
        fread(&out.pent_arr[i].disk_size, sizeof(uint32_t), 1, f);
        fseek(f, 504, SEEK_CUR);
    }

    fseek(f, 0x2200, SEEK_SET);

    //READ ID AND NAME AND SET DISK OFF
    for(i = 0; i < out.pent_num; i++)
    {
        fread(&out.pent_arr[i].pent_id, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].name, sizeof(char), 256, f);
        out.pent_arr[i].disk_off = 0xFFFFFFFF;
    }

    return out;
}

APHeader readAPHeader44DD55AA(FILE *f)
{
    //I did this with only the header data so it might not be 100%

    APHeader out;
    uint32_t tmp[4] =
    { 0, 0, 0, 0 };

    out.magic[0] = 0x44;
    out.magic[1] = 0xDD;
    out.magic[2] = 0x55;
    out.magic[3] = 0xAA;

    //CHECK OTHER MAGIC NUMBERS

    fseek(f, 0x600, SEEK_SET);
    fread(tmp, sizeof(uint32_t), 1, f);
    if(tmp[0] != 0xcc00bbaa)
    {
        fprintf(stderr, "Failed on secondary magic check : ");
        printHexUINT32(stderr, tmp[0]);
        fprintf(stderr, "\n");
    }

    fseek(f, 0x2000, SEEK_SET);
    fread(tmp, sizeof(uint32_t), 1, f);
    if(tmp[0] != 0xaa55ec33)
    {
        fprintf(stderr, "Failed on tertiary magic check : ");
        printHexUINT32(stderr, tmp[0]);
        fprintf(stderr, "\n");
    }

    //COUNT AND ALLOCATE AP ENTRIES
    fseek(f, 0x2004, SEEK_SET);

    fread(tmp, sizeof(uint32_t), 4, f);

    while(tmp[3] != 0xFFFFFFFF)
    {
        out.pent_num++;
        fread(tmp, sizeof(uint32_t), 4, f);
    }

    out.pent_arr = (APPartitionEntry*) calloc(out.pent_num, sizeof(APPartitionEntry));

    //READ DISK OFFSET,FILE OFFSET, FILE SIZE
    fseek(f, 0x2010, SEEK_SET);
    int i = 0;
    for(; i < out.pent_num; i++)
    {
        //TODO FIX THIS
        fread(&out.pent_arr[i].disk_off, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].file_off, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].file_size, sizeof(uint32_t), 1, f);

        //Skip 0 char
        //fseek(f,4,SEEK_CUR);
    }

    //READ ID, NAME AND DISK SIZE
    fseek(f, 0x2400, SEEK_SET);

    for(i = 0; i < out.pent_num; i++)
    {
        fread(&out.pent_arr[i].pent_id, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].disk_size, sizeof(uint32_t), 1, f);
        //Skip NULL
        fseek(f, 0x4, SEEK_CUR);
        fread(out.pent_arr[i].name, sizeof(char), 20, f);
        //Skip Blanks
        fseek(f, 0x1E0, SEEK_CUR);
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
    "    %-26s%" PRIu32 "\n", "Data Block Name", pe.name, "Data Block ID", pe.pent_id,
            "Size on File", pe.file_size, "File Offset", pe.file_off, "Size on Disk", pe.disk_size);

    if(pe.disk_off != 0xFFFFFFFF)
    {
        fprintf(f, "    %-26s%" PRIu32 "\n", "Disk Offset", pe.disk_off);
    }
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
