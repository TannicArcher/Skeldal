/*
 *  This file is part of Skeldal project
 * 
 *  Skeldal is free software: you can redistribute 
 *  it and/or modify it under the terms of the GNU General Public 
 *  License as published by the Free Software Foundation, either 
 *  version 3 of the License, or (at your option) any later version.
 *
 *  OpenSkeldal is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Skeldal.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  --------------------
 *
 *  Project home: https://sourceforge.net/projects/skeldal/
 *  
 *  Last commit made by: $Id: LZW_EX.C 7 2008-01-14 20:14:25Z bredysoft $
 */
#include "lzw.h"
#include "lzwc.c"  //include nahrazuje MAK soubor.
#include <stdio.h>

#define MAX_BUFF 65536

#define INPUT "testx"
#define OUTPUT "testx.l"
#define ARCH "e:\\SYSLOG.LZW"

char input_buffer[MAX_BUFF*2];
char output_buffer[MAX_BUFF*2];

FILE *in,*out;

int datasize;
int compsize;

void open_files(char *sr,char *tg)
  {
  in=fopen(sr,"rb");
  out=fopen(tg,"wb");
  if (in==NULL || out==NULL) abort();
  }

void close_files()
  {
  fclose(in);
  fclose(out);
  }


void read_normal()
  {

  datasize=fread(input_buffer,1,MAX_BUFF,in);
  }

void save_normal()
  {

  fwrite(output_buffer,1,datasize,out);
  }


int read_comp()
  {
  fread(&datasize,1,sizeof(datasize),in);
  fread(&compsize,1,sizeof(datasize),in);
  return fread(input_buffer,1,compsize,in);
  }

void save_comp()
  {
  fwrite(&datasize,1,sizeof(datasize),out);
  fwrite(&compsize,1,sizeof(datasize),out);
  fwrite(output_buffer,1,compsize,out);
  }


main()
  {
  puts("");

  init_lzw_compressor(8);
  open_files(INPUT,ARCH);
  read_normal();
  while (datasize!=0)
     {
     memset(output_buffer,0,sizeof(output_buffer));
     compsize=lzw_encode(input_buffer,output_buffer,datasize);
     printf("Origin: %d Packed: %d Ratio %d%%\n",datasize,compsize,compsize*100/datasize);
     save_comp();
     read_normal();
     reinit_lzw();
     }
  close_files();
  done_lzw_compressor();
  init_lzw_compressor(8);
  open_files(ARCH,OUTPUT);
  while (read_comp())
     {
     lzw_decode(input_buffer,output_buffer);
     save_normal();
     reinit_lzw();
     }
  done_lzw_compressor();
  printf("Compressed file has been extracted back\n");
  close_files();
  printf("and saved as %s.\n",OUTPUT);
  }

