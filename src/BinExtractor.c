/*
 ============================================================================
 Name        : BinExtractor.c
 Author      : Xonar
 Version     :
 Copyright   : No Warrenty, No Guarentee.
 Description : Bin Extractor
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "GPT.h"
#include "BinExtractor.h"
#include "APHeader.h"

int main(void)
{
    //TODO

    return EXIT_SUCCESS;
}

void skipToNextLBA(FILE* f)
{
    uint16_t tmp =  ftell(f);
    if(tmp==-1)
        //Something Went Wrong
        fprintf(stderr,"ERROR");
    else if(tmp%512>0)
        fseek(f,512-tmp%512,SEEK_CUR);
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
    GPTPartitionEntry* pes = (GPTPartitionEntry*) calloc(tmp.pent_num, sizeof(GPTPartitionEntry));

    readGPTPartitionEntryArray(f, pes, tmp.pent_num);

    //PRINT GPT HEADER
    printFullGPTInfo(tmp, pes, stdout);

    free(pes);

    return EXIT_SUCCESS;
}

int displayAP(const char* path)
{
    FILE* f = fopen(path, "r");



    //READ AP HEADER
    APHeader tmp = readAPHeader(f);

    //PRINT AP HEADER
    printFullAPInfo(tmp,stdout);

    free(tmp.pent_arr);

    return EXIT_SUCCESS;
}

void printHexString( FILE* f, const char* string, const int len)
{
    fprintf(f, "%02X", (unsigned char)string[0]);

    int i = 1;
    for(; i < len; i++)
    {
        fprintf(f, " %02X", (unsigned char)string[i]);
    }
}

void printHexUINT64( FILE* f, uint64_t num)
{
    int i = 1;
    fprintf(f, "%02X", (unsigned int) (num % 256));

    for(; i < 8; i++)
    {
        num /= 256;
        fprintf(f, " %02X", (unsigned int) (num % 256));
    }
}

void printHexUINT32( FILE* f, uint32_t num)
{
    int i = 1;
    fprintf(f, "%02X", num % 256);

    for(; i < 4; i++)
    {
        num /= 256;
        fprintf(f, " %02X", num % 256);
    }
}
