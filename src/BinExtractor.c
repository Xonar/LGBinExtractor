/*
 ============================================================================
 Name        : BinExtractor
 Author      : Xonar
 Version     :
 Copyright   : No Warrenty, No Guarentee.
 Description : Extracts LG Bin Firmware files
 ============================================================================
 */

#include "GPT.h"
#include "BinExtractor.h"
#include "APHeader.h"

#include <dirent.h>

int main(int argc, char* args[])
{
    //REMOVE FIRST ARG IF ITS EXECUTION PATH
    if(argc > 0)
    {
        if(strcasecmp(PROG_NAME, args[0] + strlen(args[0]) - strlen(PROG_NAME)) == 0)
        {
            argc--;
            args++;
        }
    }

    //IF INVALID
    //NOTE: ITS SET OUT LIKE THIS AND NOT argc != 2 E SO THATS ITS EASY TO CHANGE
    if(argc < 2 || argc > 2)
    {
        printUsage();
        return EXIT_FAILURE;
    }

    if(strcmp("-daph", args[0]) == 0)
    {
        if(canOpenFile(args[1]))
        {
            displayAP(args[1]);
        }
        else
        {
            fprintf(stderr, "Couldn't open file : %s", args[1]);
            return EXIT_FAILURE;
        }
    }
    else if(strcmp("-dgpt", args[0]) == 0)
    {
        if(canOpenFile(args[1]))
        {
            displayGPT(args[1]);
        }
        else
        {
            fprintf(stderr, "Couldn't open file : %s", args[1]);
            return EXIT_FAILURE;
        }
    }
    else if(strcmp("-extract", args[0]) == 0)
    {
        if(canOpenFile(args[1]))
        {
            splitBinFile(args[1]);
        }
        else
        {
            fprintf(stderr, "Couldn't open file : %s", args[1]);
            return EXIT_FAILURE;
        }
    }
    else printUsage();

    return EXIT_SUCCESS;
}

void skipToNextLBA(FILE* f)
{
    uint16_t tmp = ftell(f);
    if(tmp == -1)
    //Something Went Wrong
    fprintf(stderr, "ERROR");
    else if(tmp % 512 > 0) fseek(f, 512 - tmp % 512, SEEK_CUR);
}

int displayGPT(const char* path)
{
    FILE* f = fopen(path, "r");

    //SKIP AP INFO BLOCK
    fseek(f, 0x100000, SEEK_SET);

    //SKIP MBR
    fseek(f, 512, SEEK_CUR);

    //READ GPT HEADER
    GPTHeader tmp = readGPTHeader(f);

    if(strcmp(tmp.signature, "EFI PART"))
    {
        fprintf(stderr, "Does not contain GPT at first data block");
        return EXIT_FAILURE;
    }

    GPTPartitionEntry* pes = (GPTPartitionEntry*) calloc(tmp.pent_num, sizeof(GPTPartitionEntry));

    readGPTPartitionEntryArray(f, pes, tmp.pent_num);

    //PRINT GPT HEADER
    printFullGPTInfo(tmp, pes, stdout);

    free(pes);
    fclose(f);

    return EXIT_SUCCESS;
}

int displayAP(const char* path)
{
    FILE* f = fopen(path, "r");

    //READ AP HEADER
    APHeader tmp = readAPHeader(f);

    if(tmp.pent_num == 0)
    {
        fprintf(stderr, "No Data to Write!\n");
        return -1;
    }

    //PRINT AP HEADER
    printFullAPInfo(tmp, stdout);

    free(tmp.pent_arr);
    fclose(f);

    return EXIT_SUCCESS;
}

int splitBinFile(const char* path)
{
    FILE* f = fopen(path, "r");

    //READ AP HEADER
    puts("Reading AP Header...\n");
    APHeader tmp = readAPHeader(f);
    int i = 0, j = 0;

    puts("Writing Files...");

    //WRITE FILES
    for(; i < tmp.pent_num; i++)
    {
        //WRITE FILE TO CUR DIR
        char* name = calloc(512, sizeof(char));
        snprintf(name, 511, "%d-%s.img", tmp.pent_arr[i].pent_id, tmp.pent_arr[i].name);

        FILE* out = fopen(name, "w");
        fseek(f, tmp.pent_arr[i].file_off * 512 + 0x100000, SEEK_SET);

        printf("\tWriting File : %-20s", name);
        fflush(stdout);

        char buff[512];

        for(j = 0; j < tmp.pent_arr[i].file_size; j++)
        {
            //DO 512 BLOCK
            fread(buff, sizeof(char), 512, f);
            fwrite(buff, sizeof(char), 512, out);
        }

        fclose(out);

        puts(" -- DONE --");
    }

    //DONE
    puts("\nFinished");

    free(tmp.pent_arr);
    fclose(f);

    return EXIT_SUCCESS;
}

void printUsage()
{
    //PRINT USAGE
    printf(
            "BinExtractor - A tool for extracting LG Bin Firmware files\n\nUsage :\n\t%-20s%s\n\t%-20s%s\n\t%-20s%s",
            "-daph file", "Display Header Information", "-dgpt file",
            "Display GPT Header Information", "-extract file", "Split Bin into Partitions");
}

_Bool canOpenFile(const char* path)
{
    //TODO Display Why file can't open
    FILE* f = fopen(path, "r");

    _Bool ret = f != NULL;

    fclose(f);

    return ret;
}

//PRINT HEX STRING FROM DATA
void printHexString(FILE* f, const char* string, const int len)
{
    int i = 1;
    fprintf(f, "%02X", (unsigned char) string[0]);

    for(; i < len; i++)
        fprintf(f, " %02X", (unsigned char) string[i]);
}

void printHexUINT64(FILE* f, uint64_t num)
{
    int i = 1;
    fprintf(f, "%02X", (unsigned int) (num % 256));

    for(; i < 8; i++)
    {
        num /= 256;
        fprintf(f, " %02X", (unsigned int) (num % 256));
    }
}

void printHexUINT32(FILE* f, uint32_t num)
{
    int i = 1;
    fprintf(f, "%02X", num % 256);

    for(; i < 4; i++)
    {
        num /= 256;
        fprintf(f, " %02X", num % 256);
    }
}
