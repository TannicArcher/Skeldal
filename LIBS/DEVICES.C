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
 *  Last commit made by: $Id: DEVICES.C 7 2008-01-14 20:14:25Z bredysoft $
 */
#include <skeldal_win.h>
#include "types.h"
#include <stdio.h>
#include <dos.h>
//#include <i86.h>
#include <bios.h>
#include "event.h"
#include "devices.h"
#include <time.h>

/* Data touched at mouse callback time -- they are in a structure to
        simplify calculating the size of the region to lock.
*/

extern MS_EVENT win_mouseEvent;


TMS_BASIC_INFO ms_basic_info={0};
static char ms_keys;

void get_ms_event(MS_EVENT *event)
  {  
  CheckMessageQueue();
  *event=win_mouseEvent;
  win_mouseEvent.event=0;
  }

char cz_table_1[]=" 1!3457�908+,-./�+��������\"�?=:_2ABCDEFGHIJKLMNOPQRSTUVWXYZ�\\)6=;abcdefghijklmnopqrstuvwxyz/|(; ";
char cz_table_2[]=" !\"#$%&'()*+,-./0123456789:;<=>?@�BCD�FGH�JK�MN�PQ�ST�VWX�Z[\\]^_`�bcd�fgh�jk�mn�pq�st�vwx�z{|}~ ";
char cz_table_3[]=" !\"#$%&'()*+,-./0123456789:;<=>?@AB���FGHIJK�M��PQ����VWXY�[\\]^_`ab���fghijk�m��pq����vwxy�{|}~ ";
char *cz_key_tabs[]={cz_table_1,cz_table_2,cz_table_3};

void keyboard(EVENT_MSG *msg,void *user_data)
  {
  int i;
  static char cz_mode=0;
  char c,d;

  msg;user_data;
  if (msg->msg==E_WATCH)
     {
     *otevri_zavoru=1;
     if (!_bios_keybrd(_KEYBRD_READY)) return;
     i=_bios_keybrd(_KEYBRD_READ);
     d=i>>8;
     c=i & 0xff;
     if (c=='+' && d<55 && !cz_mode) cz_mode=2;
     else if (c=='=' && d<55 && !cz_mode) cz_mode=1;
     else if (c>32 && c<127 && d<=53)
              {
              c=cz_key_tabs[cz_mode][c-32];
              i=d;
              i=(i<<8)+c;
              send_message(E_KEYBOARD,i);
              cz_mode=0;
              }
     else
       send_message(E_KEYBOARD,i);

     }
  }

char ms_get_keycount()
  {
  return ms_keys;
  }
