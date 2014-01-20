/*
 ============================================================================
 Name    : BinExtractor
 Author    : Xonar
 Version   :
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
  if(argc > 0 && **args != '-')
  {
    argc--;
    args++;
  }

  /*PARSE CMDLINE*/
  if(argc == 2)
  {
    if(strcmp("-ebh", args[0]) == 0)
    {
      if(canOpenFile(args[1]))
      {
        extractBinHeaderFile(args[1]);
      }
      else
      {
        return EXIT_FAILURE;
      }
    }
    else if(strcmp("-daph", args[0]) == 0)
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

  f = fopen(path, "rb");

  /*SKIP AP INFO BLOCK*/
  fseek(f, 0x100000, SEEK_SET);

  /*SKIP MBR*/
  fseek(f, 512, SEEK_CUR);

  /*READ GPT HEADER*/
  tmp = readGPTHeader(f);

  if(memcmp(tmp.signature, "EFI PART", 8))
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
  FILE* f = fopen(path, "rb");

  /*READ AP HEADER*/
  APHeader tmp = readAPHeader(f);

  if(tmp.pent_num == 0)
  {
    fprintf(stderr, "DISPLAYING INCOMPLETE APHEADER!\n\n");
    printAPHeader(tmp, stdout);
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
  APHeader aph;
  GPTHeader gpt;
  GPTPartitionEntry* pes;
  int i = 0, j = 0, *parts, cur = 0;
  _Bool addWhitespace = 0;

  f = fopen(path, "rb");

  /*READ AP HEADER*/
  puts("Reading AP Header...");
  aph = readAPHeader(f);

  /*READ GPT HEADER*/
  puts("Reading GPT Header...");
  fseek(f, 0x100200, SEEK_SET);
  gpt = readGPTHeader(f);

  if(memcmp(gpt.signature, "EFI PART", 8))
  {
    fprintf(stdout, "\tDoes not contain GPT at first data block\n");
    pes = NULL;
  }
  else
  {
    pes = (GPTPartitionEntry*) calloc(gpt.pent_num, sizeof(GPTPartitionEntry));
    readGPTPartitionEntryArray(f, pes, gpt.pent_num);
  }

  /*GENERATE PART DATA*/
  parts = calloc(aph.pent_num, sizeof(int));
  for(; i < aph.pent_num; i++)
    parts[i] = 1;

  /*CHECK FOR DUPLICATE NAMES OF PARTITION ENTRIES*/
  for(i = 1; i < aph.pent_num; i++)
  {
    if(strcmp(aph.pent_arr[i].name, aph.pent_arr[i - 1].name) == 0)
    {
      char c;

      printf("\nThere are Data Blocks with duplicate names.\n"
          "Do you want to merge them? Y/N : ");

      c = getchar();
      while(getchar() != '\n')
        ;
      
#if DEBUG
      fprintf(stderr,"Input Char : '%c'\n",c);
#endif

      if(c == 'y' || c == 'Y')
      {
        /*UPDATE PART DATA*/
        j = i - 1;
        parts[j]++;
        i++;

        for(; i < aph.pent_num; i++)
          if(strcmp(aph.pent_arr[i].name, aph.pent_arr[i - 1].name) == 0) parts[j]++;
          else j++;

        parts[j] = 0;
      }

      /*ADD TRAILING WHITESPACE*/
      if(pes != NULL )
      {
        printf("\nSome Merged partitions requires the file to be\n"
            "the full size and won't mount if it's not.\n"
            "Do you want to add trailing whitespace? Y/N : ");

        c = getchar();
        while(getchar() != '\n')
          ;

        if(c == 'y' || c == 'Y') addWhitespace = 1;
      }
      else
      {
        printf("\nSome Merged partitions requires the file to be\n"
            "the full size and won't mount if it's not.\n"
            "The Bin file does not contain a GPT Header and\n"
            "The whitespace can't be added automatically\n");
      }

      break;
    }
  }
  
#if DEBUG
  int k = 0;
  puts("");
  for(;k<aph.pent_num && parts[k]!=0;k++)
  {
    fprintf(stderr,"Part %d : %d\n",k,parts[k]);
  }
#endif

  puts("\nWriting Files...");
  
  /*Incase offset is not defined*/
  fseek(f, 0x100000, SEEK_SET);

  /*WRITE FILES*/
  for(i = 0; i < aph.pent_num; i++)
  {
    /*WRITE FILE TO CUR DIR*/
    char* name, buff[512];
    FILE* out;
    int len = 0;
    _Bool merged = 0;

    name = calloc(512, sizeof(char));

    /*Add safe implementation Cross Platform change*/

    /*%d cannot ever exceed 512 char due to int limit*/
    sprintf(name, "%d", aph.pent_arr[i].pent_id);

    len = strlen(aph.pent_arr[i].name) + 5 /* - + .img */+ 1 /* NULL Terminating*/
    + strlen(name);

    if(len > 512)
    {
      /*RESIZE BUFFER*/
      printf("Resizing File buffer to accommodate extreme file name!\n");

      name = realloc(name, sizeof(char) * len);
    }

    /*END - Add safe implementation Cross Platform change*/

    sprintf(name, "%d-%s.img", aph.pent_arr[i].pent_id, aph.pent_arr[i].name);

    out = fopen(name, "wb");
    
    /*If file_off is not defined assume that it start after previous one ends*/
    if(aph.pent_arr[i].file_off != 0xffffffff)
      fseek(f, aph.pent_arr[i].file_off * 512 + 0x100000, SEEK_SET);

    printf("\tWriting File : %-20s", name);
    fflush(stdout);

    for(j = 0; j < aph.pent_arr[i].file_size; j++)
    {
      /*DO 512 BLOCK*/
      fread(buff, sizeof(char), 512, f);
      fwrite(buff, sizeof(char), 512, out);
    }

    parts[cur]--;

    while((parts[cur]--) > 0)
    {
      i++;
      merged = 1;

      printf("\n\t\tAppending to File");

      fseek(f, aph.pent_arr[i].file_off * 512 + 0x100000, SEEK_SET);

      /*ADD ACTUAL WHITESPACE AND NOT META DATA*/
      for(j = 0; j < 512; j++)
        buff[j] = '\0';

      for(j = 0;j < aph.pent_arr[i].disk_off - aph.pent_arr[i - 1].disk_off - aph.pent_arr[i - 1].file_size; j++)
      {
        fwrite(buff, sizeof(char), 512, out);
      }
      /*EOF - ADD ACTUAL WHITESPACE AND NOT META DATA*/

      for(j = 0; j < aph.pent_arr[i].file_size; j++)
      {
        /*DO 512 BLOCK*/
        fread(buff, sizeof(char), 512, f);
        fwrite(buff, sizeof(char), 512, out);
      }
    }

    /*ADD WHITESPACE*/
    if(merged && addWhitespace)
    {
      int curGPTE = -1;
      int curLBA = aph.pent_arr[i].disk_off + aph.pent_arr[i].file_size;

      /*FIND CORROSPONDING GPT ENTRY*/
      for(j = 0; j < gpt.pent_num; j++)
      {
        if(pes[j].first_lba < curLBA && pes[j].last_lba >= curLBA)
        {
          curGPTE = j;
          break;
        }
      }

      /*WRITE WHITESPACE*/
      for(j = 0; j < 512; j++)
        buff[j] = '\0';

      for(j = 0; j <= pes[curGPTE].last_lba - curLBA; j++)
      {
        fwrite(buff, sizeof(char), 512, out);
      }
    }

    fclose(out);
    free(name);
    cur++;

    if(merged)
      printf("      ");

    puts(" -- DONE --");
  }

  /*DONE*/
  puts("\nFinished");

  free(aph.pent_arr);
  fclose(f);

  return EXIT_SUCCESS;
}

int extractBinHeaderFile(const char* path)
{
  char buf[512];
  int i = 0;
  FILE* f;
  FILE* out;
  char isBinTot;

  /*Check that file extension is bin/tot*/
  i = strlen(path);
  isBinTot = strcmp(path + i - 4, ".bin") == 0;
  isBinTot |= (strcmp(path + i - 4, ".tot") == 0) << 1;

  if(!isBinTot)
  {
    char* c = strrchr(path , '.');
    if( c == NULL )
      printf("No File Extension!\n");
    else
      printf("Invalid file type : %s\n", c);
    return EXIT_FAILURE;
  }

  f = fopen(path, "rb");
  out = fopen((isBinTot & 1) ? "header.bin" : "header.tot", "wb");

  for(i = 0; i < 0x800; i++)
  {
    /*DO 512 BLOCK*/
    fread(buf, sizeof(char), 512, f);
    fwrite(buf, sizeof(char), 512, out);
  }

  fclose(f);

  return EXIT_SUCCESS;
}

void printUsage()
{
  /*PRINT USAGE*/
  printf(
      "BinExtractor - A tool for extracting LG Bin Firmware files\n\nUsage :\n\t%-20s%s\n\t%-20s%s\n\t%-20s%s\n\t%-20s%s\n",
      "-daph file", "Display Header Information", "-dgpt file",
      "Display GPT Header Information", "-extract file", "Split Bin into Partitions",
      "-ebh file", "Extract Bin Header from file");
}

_Bool canOpenFile(const char* path)
{
  FILE* f = fopen(path, "rb");

  _Bool ret = f != NULL;

  if(!ret)
  {
    fprintf(stderr, "Failed to Open File \'%s\' : %s\n", path, strerror(errno));
    return ret;
  }

  fclose(f);
  return ret;
}

/*PRINT HEX STRING FROM DATA*/
void printHex(FILE* f, const void* data, const unsigned int len)
{
  int i = 1;
  fprintf(f, "%02X", ((unsigned char*) data)[0]);

  for(; i < len; i++)
    fprintf(f, " %02X", ((unsigned char*) data)[i]);
}
