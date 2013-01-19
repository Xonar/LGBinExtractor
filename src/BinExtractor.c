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
#include <strings.h>
#include <errno.h>

int main(int argc, char* args[])
{
    /*REMOVE FIRST ARG IF ITS EXECUTION PATH*/
    if(argc > 0)
    {
        if(strcasecmp(PROG_NAME, args[0] + strlen(args[0]) - strlen(PROG_NAME)) == 0)
        {
            argc--;
            args++;
        }
    }

    /*IF INVALID
     NOTE: ITS SET OUT LIKE THIS AND NOT argc != 2 E SO THATS ITS EASY TO CHANGE*/
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
    /*Something Went Wrong*/
    fprintf(stderr, "ERROR\n");
    else if(tmp % 512 > 0) fseek(f, 512 - tmp % 512, SEEK_CUR);
}

int displayGPT(const char* path)
{
    /*Declarations*/
    FILE* f;
    GPTHeader tmp;
    GPTPartitionEntry* pes;

    f = fopen(path, "r");

    /*SKIP AP INFO BLOCK*/
    fseek(f, 0x100000, SEEK_SET);

    /*SKIP MBR*/
    fseek(f, 512, SEEK_CUR);

    /*READ GPT HEADER*/
    tmp = readGPTHeader(f);

    if(memcmp(tmp.signature, "EFI PART",8))
    {
        fprintf(stderr, "Does not contain GPT at first data block\n");
        return EXIT_FAILURE;
    }

    pes = (GPTPartitionEntry*) calloc(tmp.pent_num, sizeof(GPTPartitionEntry));

    readGPTPartitionEntryArray(f, pes, tmp.pent_num);

    /*PRINT GPT HEADER*/
    printFullGPTInfo(tmp, pes, stdout);

    free(pes);
    fclose(f);

    return EXIT_SUCCESS;
}

int displayAP(const char* path)
{
    FILE* f = fopen(path, "r");

    /*READ AP HEADER*/
    APHeader tmp = readAPHeader(f);

    if(tmp.pent_num == 0)
    {
        fprintf(stderr, "No Data to Write!\n");
        return -1;
    }

    /*PRINT AP HEADER*/
    printFullAPInfo(tmp, stdout);

    free(tmp.pent_arr);
    fclose(f);

    return EXIT_SUCCESS;
}

int splitBinFile(const char* path)
{
    /*Declarations*/
    FILE* f;
    APHeader tmp;
    int i = 0, j = 0, *parts, cur = 0;

    f = fopen(path, "r");

    /*READ AP HEADER*/
    puts("Reading AP Header...");
    tmp = readAPHeader(f);

    /*GENERATE PART DATA*/
    parts = calloc(tmp.pent_num, sizeof(int));
    for(; i < tmp.pent_num; i++)
        parts[i] = 1;

    /*CHECK FOR DUPLICATE NAMES OF PARTITION ENTRIES*/
    for(i = 1; i < tmp.pent_num; i++)
    {
        if(strcmp(tmp.pent_arr[i].name, tmp.pent_arr[i - 1].name) == 0)
        {
            char c;

            printf("\nThere are Data Blocks with duplicate names.\n"
                    "Do you want to merge them? Y/N : ");


            c = getchar();
            while(getchar()!='\n');

            if(c == 'y' || c == 'Y')
            {
                /*UPDATE PART DATA*/
                j = i - 1;
                parts[j]++;
                i++;

                for(; i < tmp.pent_num; i++)
                    if(strcmp(tmp.pent_arr[i].name, tmp.pent_arr[i - 1].name) == 0) parts[j]++;
                    else j++;

                parts[j] = 0;
            }

            break;
        }
    }

    puts("\nWriting Files...");

    /*WRITE FILES*/
    for(i = 0; i < tmp.pent_num; i++)
    {
        /*WRITE FILE TO CUR DIR*/
        char* name,buff[512];
        FILE* out;
        int len = 0,start;

        name = calloc(512, sizeof(char));

        /*Add safe implementation Cross Platform change*/

        /*%d cannot ever exceed 512 char due to int limit*/
        sprintf(name, "%d", tmp.pent_arr[i].pent_id);

        len = strlen(tmp.pent_arr[i].name) + 5 /* - + .img */+ 1 /* NULL Terminating*/
        + strlen(name);

        if(len > 512)
        {
            /*RESIZE BUFFER*/
            printf("Resizing File buffer to accommodate extreme file name!\n");

            name = realloc(name, sizeof(char) * len);
        }

        /*END - Add safe implementation Cross Platform change*/

        sprintf(name, "%d-%s.img", tmp.pent_arr[i].pent_id, tmp.pent_arr[i].name);

        out = fopen(name, "w");
        fseek(f, tmp.pent_arr[i].file_off * 512 + 0x100000, SEEK_SET);

        printf("\tWriting File : %-20s", name);
        fflush(stdout);

        for(j = 0; j < tmp.pent_arr[i].file_size; j++)
        {
            /*DO 512 BLOCK*/
            fread(buff, sizeof(char), 512, f);
            fwrite(buff, sizeof(char), 512, out);
        }

        start = tmp.pent_arr[i].disk_off;

        parts[cur]--;

        while((parts[cur]--)>0)
        {
            i++;

            printf("\n\t\tAppending to File");


            fseek(f, tmp.pent_arr[i].file_off * 512 + 0x100000, SEEK_SET);
            fseek(out,(tmp.pent_arr[i].disk_off-start)*512,SEEK_SET);

            for(j = 0; j < tmp.pent_arr[i].file_size; j++)
            {
                /*DO 512 BLOCK*/
                fread(buff, sizeof(char), 512, f);
                fwrite(buff, sizeof(char), 512, out);
            }
        }

        fclose(out);
        free(name);
        cur++;

        puts(" -- DONE --");
    }

    /*DONE*/
    puts("\nFinished");

    free(tmp.pent_arr);
    fclose(f);

    return EXIT_SUCCESS;
}

void printUsage()
{
    /*PRINT USAGE*/
    printf(
            "BinExtractor - A tool for extracting LG Bin Firmware files\n\nUsage :\n\t%-20s%s\n\t%-20s%s\n\t%-20s%s\n",
            "-daph file", "Display Header Information", "-dgpt file",
            "Display GPT Header Information", "-extract file", "Split Bin into Partitions");
}

_Bool canOpenFile(const char* path)
{
    FILE* f = fopen(path, "r");

    _Bool ret = f != NULL;



    if(!ret)
    {
        fprintf(stderr,"Failed to Open File \'%s\' : %s\n",path,strerror(errno));
        return ret;
    }

    fclose(f);
    return ret;
}

/*PRINT HEX STRING FROM DATA*/
void printHexString_u(FILE* f, const unsigned char* string, const unsigned int len)
{
    int i = 1;
    fprintf(f, "%02X", (unsigned char) string[0]);

    for(; i < len; i++)
        fprintf(f, " %02X", (unsigned char) string[i]);
}

void printHexString_s(FILE* f, const char* string, const unsigned int len)
{
    int i = 1;
    fprintf(f, "%02X", (signed char) string[0]);

    for(; i < len; i++)
        fprintf(f, " %02X", (signed char) string[i]);
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
