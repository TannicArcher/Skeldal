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
 *  Last commit made by: $Id: FLC.C 7 2008-01-14 20:14:25Z bredysoft $
 */
//
//              Knihovna pro dekompresi FLC souboru
//

#include <stdio.h>
#include <conio.h>
#include <malloc.h>
#include <string.h>
#include <i86.h>
#include "flc.h"


void Open_FLC (char *filename)
{
	flc = fopen (filename, "rb");
	fread (&h_flc, sizeof(flcheader), 1, flc);
}

void Close_FLC (void)
{
	fclose (flc);
	free (flc_buffer);
}

void Get_first_frame (void)
{
	fseek (flc, h_flc.offset1, SEEK_SET);
	fread (&h_frame, sizeof(frameheader), 1, flc);
	flc_buffer = (char*)malloc((h_frame.size-sizeof(frameheader)));
	fread (flc_buffer, (h_frame.size-sizeof(frameheader)), 1, flc);
}

void Get_next_frame (void)
{
	free (flc_buffer);
	fread (&h_frame, sizeof(frameheader), 1, flc);
	flc_buffer = (char*)malloc((h_frame.size-sizeof(frameheader)));
	fread (flc_buffer, (h_frame.size-sizeof(frameheader)), 1, flc);
}

void Decompress_frame (void)
{
	unsigned short x,y,z,w;
	unsigned short changes, nfo_word, packets, offset, row;
	int frame_pos=0;
	unsigned int scr_pos;
	char c1, c2, c3, a;
	char hi, lo;

	for (z=1; z<=h_frame.actions; z++)
	{
		memmove ( &h_action, (flc_buffer+frame_pos), sizeof(actionheader) );
		switch (h_action.code)
		{
			case  4:
				a=1;//fcl_buffer[frame_pos];
//				b=0;//flc_buffer[frame_pos];
//				c=256; //0=256

				frame_pos+=10;
				outp (0x3c8, 0);
				for (x=0; x<=768; x++) outp (0x3c9, (flc_buffer[(x+frame_pos)]/=4) ); 
				frame_pos+=768;
				break;
			case  7:
				frame_pos+=6;
				lo=flc_buffer[frame_pos]; frame_pos++;
				hi=flc_buffer[frame_pos]; frame_pos++;
				changes=hi*256+lo;
				row=0;

				for (y=0; y<=(changes-1); y++)			// pocet menenych radek
				{
					lo=flc_buffer[frame_pos]; frame_pos++;
					hi=flc_buffer[frame_pos]; frame_pos++;
					nfo_word=hi*256+lo;

					scr_pos=row*h_flc.width;

					if (nfo_word>=0xC000)					// preskakovane radky
					{
					nfo_word=0xFFFF-nfo_word+1;
					row+=nfo_word;
					}

					else
					{
					for (z=1; z<=nfo_word; z++)			// pocet menenych bloku
					{
						x=1; a=0;

						offset = flc_buffer[frame_pos];	 // rel. offset bloku
						frame_pos++;
						scr_pos+=offset;
	
//						while (!a)					// zmena bloku
//						{
							c1 = flc_buffer[frame_pos];
							frame_pos++;

							if (c1>128)
							{
								c1=0xFF-c1+1;
								c2=flc_buffer[frame_pos];
								frame_pos++;
								c3=flc_buffer[frame_pos];
								frame_pos++;

								for (w=1; w<=c1; w++)
								{	frame_buffer[scr_pos]=c2;
									scr_pos++;
									frame_buffer[scr_pos]=c3;
									scr_pos++; }
							}
							else
							{
//								c3=0xFF-c3+1;
								for (w=1; w<=c1; w++)
								{	frame_buffer[scr_pos]=flc_buffer[frame_pos];
									frame_pos++;
									scr_pos++;
									frame_buffer[scr_pos]=flc_buffer[frame_pos];
									frame_pos++;
									scr_pos++; }
							}

//							if (x>=640) a=1;
//						}

					}
					row++;
					}
				}

//				frame_pos+=h_action.size;
				break;
			case 15:
				frame_pos+=6;
				for (y=0; y<=(h_flc.height-1); y++)
				{
					frame_pos++; x=1; //a=0;
					scr_pos=y*h_flc.width;

					while (x<=h_flc.width)
					{
						c1 = flc_buffer[frame_pos];
						frame_pos++;

						if (c1<128)
						{
							c2=flc_buffer[frame_pos];
							frame_pos++;
							for (w=1; w<=c1; w++)
							{	frame_buffer[scr_pos]=c2;
								scr_pos++;
								x++; }
						}
						else
						{
							c1=0xFF-c1+1;
							for (w=1; w<=c1; w++)
							{	frame_buffer[scr_pos]=flc_buffer[frame_pos];
								scr_pos++;
								frame_pos++;
								x++; }
						}
					}
				}
				break;
			default: frame_pos+=h_action.size;
				break;
		};
		
	}

}
