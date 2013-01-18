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

    /*READ MAGIC NUMBER*/
    fread(&out, sizeof(char) * 4, 1, f);

    /*PASS TO CORRECT FUNCTION BASED ON MAGIC NUMBER*/
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
    int i = 0;

    out.magic[0] = 0xA5;
    out.magic[1] = 0xA5;
    out.magic[2] = 0x55;
    out.magic[3] = 0xAA;

    /*COUNT AND ALLOCATE AP ENTRIES*/
    while(1)
    {
        uint64_t tmp;
        fread(&tmp, sizeof(tmp), 1, f);
        if(tmp == UINT64_MAX) break;
        else out.pent_num++;
    }

    fseek(f, 4, SEEK_SET);

    out.pent_arr = (APPartitionEntry*) calloc(out.pent_num, sizeof(APPartitionEntry));

    /*READ FILE OFFSET AND SIZE*/
    for(; i < out.pent_num; i++)
    {
        fread(&out.pent_arr[i].file_off, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].file_size, sizeof(uint32_t), 1, f);
    }

    skipToNextLBA(f);

    /*READ DISK SIZE IGNORING FIRST ID REFERENCE*/
    for(i = 0; i < out.pent_num; i++)
    {
        /*fread(&out.pent_arr[i].pent_id,sizeof(char),1,f);*/
        fseek(f, 4, SEEK_CUR);
        fread(&out.pent_arr[i].disk_size, sizeof(uint32_t), 1, f);
        fseek(f, 504, SEEK_CUR);
    }

    fseek(f, 0x2200, SEEK_SET);

    /*READ ID AND NAME AND SET DISK OFF*/
    for(i = 0; i < out.pent_num; i++)
    {
        fread(&out.pent_arr[i].pent_id, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].name, sizeof(char), 256, f);
        out.pent_arr[i].disk_off = 0xFFFFFFFF;
    }

    return out;
}

APHeader readAPHeader44DD55AA_EAC86250(FILE *f)
{
    /*IDENTICAL FORMAT WITH DIFFERENT MAGIC NUMBER*/
    return readAPHeader44DD55AA_2BF67889(f);
}

APHeader readAPHeader44DD55AA_948B8349(FILE *f)
{
    /*IDENTICAL FORMAT WITH DIFFERENT MAGIC NUMBER*/
    return readAPHeader44DD55AA_2BF67889(f);
}

APHeader readAPHeader44DD55AA(FILE *f)
{
    APHeader out;
    uint32_t tmp[4] =
    { 0, 0, 0, 0 };

    out.magic[0] = 0x44;
    out.magic[1] = 0xDD;
    out.magic[2] = 0x55;
    out.magic[3] = 0xAA;

    /*CHECK MAGIC NUMBERS*/

    /*CHECK 1*/
    fseek(f, 0x8, SEEK_SET);
    fread(tmp, sizeof(uint32_t), 1, f);
    if(tmp[0] == 0x8978f62b)
    {
        return readAPHeader44DD55AA_2BF67889(f);
    }
    else if(tmp[0] == 0x5062c8ea)
    {
        return readAPHeader44DD55AA_EAC86250(f);
    }
    else if(tmp[0] == 0x49838b94)
    {
        return readAPHeader44DD55AA_948B8349(f);
    }
    else if(tmp[0] != 0xffffffff)
    {
        fprintf(stderr, "Unknown Magic Number at 0x8 : ");
        printHexUINT32(stderr, tmp[0]);
        fprintf(stderr, "\n");
        return out;
    }

    /*CHECK 2*/
    fseek(f, 0x600, SEEK_SET);
    fread(tmp, sizeof(uint32_t), 1, f);
    if(tmp[0] == 0xcc00bbaa)
    {
        return readAPHeader44DD55AA_AABB00CC(f);
    }
    else if(tmp[0] != 0xffffffff)
    {
        fprintf(stderr, "Unknown Magic Number at 0x600 : ");
        printHexUINT32(stderr, tmp[0]);
        fprintf(stderr, "\n");
        return out;
    }
    else
    {
        fprintf(stderr, "Did not find corresponding Magic Number!\n");
        return out;
    }
}

APHeader readAPHeader44DD55AA_2BF67889(FILE *f)
{
    APHeader out;
    uint32_t tmp[4] =
    { 0, 0, 0, 0 };

    out.magic[0] = 0x44;
    out.magic[1] = 0xDD;
    out.magic[2] = 0x55;
    out.magic[3] = 0xAA;

    /*CHECK MAGIC NUMBERS*/

    fseek(f, 0x2000, SEEK_SET);
    fread(tmp, sizeof(uint32_t), 1, f);
    if(tmp[0] == 0xaa55ec33)
    {
        return readAPHeader44DD55AA_2BF67889_AA55EC33(f);
    }
    else if(tmp[0] != 0xffffffff)
    {
        fprintf(stderr, "Unknown Magic Number at 0x2000 : ");
        printHexUINT32(stderr, tmp[0]);
        fprintf(stderr, "\n");
        return out;
    }
    else
    {
        fprintf(stderr, "Did not find corresponding Magic Number!\n");
        return out;
    }
}

APHeader readAPHeader44DD55AA_AABB00CC(FILE *f)
{
    APHeader out;
    uint32_t tmp[4] =
    { 0, 0, 0, 0 };

    out.magic[0] = 0x44;
    out.magic[1] = 0xDD;
    out.magic[2] = 0x55;
    out.magic[3] = 0xAA;

    /*CHECK MAGIC NUMBERS*/

    fseek(f, 0x2000, SEEK_SET);
    fread(tmp, sizeof(uint32_t), 1, f);
    if(tmp[0] == 0xaa55ec33)
    {
        return readAPHeader44DD55AA_AABB00CC_AA55EC33(f);
    }
    else if(tmp[0] != 0xffffffff)
    {
        fprintf(stderr, "Unknown Magic Number at 0x2000 : ");
        printHexUINT32(stderr, tmp[0]);
        fprintf(stderr, "\n");
        return out;
    }
    else
    {
        fprintf(stderr, "Did not find corresponding Magic Number!\n");
        return out;
    }
}

APHeader readAPHeader44DD55AA_2BF67889_AA55EC33(FILE *f)
{
    APHeader out;
    int i = 0;
    uint32_t tmp[4] =
    { 0, 0, 0, 0 };

    out.magic[0] = 0x44;
    out.magic[1] = 0xDD;
    out.magic[2] = 0x55;
    out.magic[3] = 0xAA;

    /*COUNT AND ALLOCATE AP ENTRIES*/
    fseek(f, 0x2004, SEEK_SET);

    fread(tmp, sizeof(uint32_t), 4, f);

    while(tmp[3] != 0xFFFFFFFF)
    {
        out.pent_num++;
        fread(tmp, sizeof(uint32_t), 4, f);
    }

    out.pent_arr = (APPartitionEntry*) calloc(out.pent_num, sizeof(APPartitionEntry));

    /*READ DISK OFFSET,FILE OFFSET, FILE SIZE*/
    fseek(f, 0x2010, SEEK_SET);

    for(; i < out.pent_num; i++)
    {
        out.pent_arr[i].pent_id = i;
        fread(&out.pent_arr[i].disk_off, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].file_off, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].file_size, sizeof(uint32_t), 1, f);

        /*Skip 0 char*/
        fseek(f, 4, SEEK_CUR);
    }

    /*READ NAME*/
    fseek(f, 0x4220, SEEK_SET);

    for(i = 0; i < out.pent_num; i++)
    {
        fread(out.pent_arr[i].name, sizeof(char), 32, f);
        out.pent_arr[i].disk_size = 0xffffffff;
    }

    return out;
}

APHeader readAPHeader44DD55AA_AABB00CC_AA55EC33(FILE *f)
{
    /*TODO Read Device Name @ 0x4000*/

    APHeader out;
    int i = 0;
    uint32_t tmp[4] =
    { 0, 0, 0, 0 };

    out.magic[0] = 0x44;
    out.magic[1] = 0xDD;
    out.magic[2] = 0x55;
    out.magic[3] = 0xAA;

    /*COUNT AND ALLOCATE AP ENTRIES*/
    fseek(f, 0x2004, SEEK_SET);

    fread(tmp, sizeof(uint32_t), 4, f);

    while(tmp[3] != 0xFFFFFFFF)
    {
        out.pent_num++;
        fread(tmp, sizeof(uint32_t), 4, f);
    }

    out.pent_arr = (APPartitionEntry*) calloc(out.pent_num, sizeof(APPartitionEntry));

    /*READ DISK OFFSET,FILE OFFSET, FILE SIZE*/
    fseek(f, 0x2010, SEEK_SET);

    for(; i < out.pent_num; i++)
    {
        fread(&out.pent_arr[i].disk_off, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].file_off, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].file_size, sizeof(uint32_t), 1, f);

        /*Skip 0 char*/
        fseek(f, 4, SEEK_CUR);
    }

    /*READ ID, NAME AND DISK SIZE*/
    fseek(f, 0x2400, SEEK_SET);

    for(i = 0; i < out.pent_num; i++)
    {
        fread(&out.pent_arr[i].pent_id, sizeof(uint32_t), 1, f);
        fread(&out.pent_arr[i].disk_size, sizeof(uint32_t), 1, f);
        /*Skip NULL*/
        fseek(f, 0x4, SEEK_CUR);
        fread(out.pent_arr[i].name, sizeof(char), 20, f);
        /*Skip Blanks*/
        fseek(f, 0x1E0, SEEK_CUR);
    }

    return out;
}

void printAPHeader(const APHeader h, FILE *f)
{
    fprintf(f, "AP HEADER\n----------\n%-30s", "Magic Number");
    printHexString_u(f, h.magic, 4);
    fprintf(f, "\n%-30s%d\n", "Number of Partitions", h.pent_num);
}

void printAPPartitionEntry(const APPartitionEntry pe, FILE *f)
{
    fprintf(f, "PARTITION ENTRY\n------------\n"
            "    %-26s%s\n"
            "    %-26s%d\n"
            "    %-26s%" PRIu32 "\n"
    "    %-26s%" PRIu32 "\n", "Data Block Name", pe.name, "Data Block ID", pe.pent_id,
            "Size on File", pe.file_size, "File Offset", pe.file_off);

    if(pe.disk_size != 0xFFFFFFFF)
    {
        fprintf(f, "    %-26s%" PRIu32 "\n", "Size on Disk", pe.disk_size);
    }

    if(pe.disk_off != 0xFFFFFFFF)
    {
        fprintf(f, "    %-26s%" PRIu32 "\n", "Disk Offset", pe.disk_off);
    }
}

void printFullAPInfo(const APHeader h, FILE *f)
{
    int i = 0;

    printAPHeader(h, f);

    fprintf(f, "\nPARTITION ENTRIES\n-----------------\n");

    for(; i < h.pent_num; i++)
    {
        fputs("\n", f);
        printAPPartitionEntry(h.pent_arr[i], f);
    }
}
