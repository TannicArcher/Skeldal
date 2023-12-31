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
 *  Last commit made by: $Id: BASICOBJ.C 18 2009-05-28 20:26:40Z bredysoft $
 */
#include <skeldal_win.h>
// toto je include soubor, jenz je pouzit v knihovne GUI.C

#include "types.h"
#include <stdio.h>
#include <mem.h>
#include <malloc.h>
#include <time.h>
#include "memman.h"
#include "event.h"
#include "devices.h"
#include "bmouse.h"
#include "bgraph.h"
#include "gui.h"
#include "basicobj.h"

#define MEMTEXT "Pam�t: "

#define E_STATUS_LINE 60

//FC_TABLE f_bila={0xffff,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};

void highlight(CTL3D *c,word color)
  {
  c->light=color<<1;
  if (c->light & 0x0020) {c->light&=~RGB555(0,0,31);c->light|=RGB555(0,0,31);}
  if (c->light & 0x0400) {c->light&=~RGB555(0,31,0);c->light|=RGB555(0,31,0);}
  if (c->light & 0x8000) {c->light&=~RGB555(31,0,0);c->light|=RGB555(31,0,0);}
  c->shadow=color;
  c->shadow&=RGB555(30,30,31);
  c->shadow>>=1;
  }


CTL3D *def_border(int btype,int color)
  {
  static CTL3D ctl;

  highlight(&ctl,color);
  switch (btype)
     {
     case 0:ctl.bsize=0;
     case 1:ctl.light=color;ctl.shadow=color;ctl.bsize=1;break;
     case 2:ctl.bsize=2;ctl.ctldef=0;break;
     case 3:ctl.bsize=2;ctl.ctldef=3;break;
     case 4:ctl.bsize=1;ctl.ctldef=0;break;
     case 5:ctl.bsize=1;ctl.ctldef=1;break;
     case 6:ctl.bsize=2;ctl.ctldef=1;break;
     case 7:ctl.bsize=2;ctl.ctldef=2;break;
     }
  return &ctl;
  }

void sample_init(OBJREC *o,char *title)
  {
  title=get_title(title);
  o->userptr=(void *)getmem(strlen(title)+1);
  strcpy((char *)o->userptr,title);
  }


void sample_draw(int x1,int y1,int x2,int y2,OBJREC *o)
  {
  x2;y2;
  position(x1,y1);
  outtext((char *)o->userptr);
  }

void mid_label_draw(int x1,int y1,int x2, int y2,OBJREC *o)
  {
  x2;y2;
  set_aligned_position((x1+x2)/2,y1,1,0,(char *)o->userptr);
  outtext((char *)o->userptr);
  }


void sample_event(EVENT_MSG *msg,OBJREC *o)
  {
  MS_EVENT *ms;

  o;
  if (msg->msg==E_MOUSE)
      {
      ms=get_mouse(msg);
      if (ms->tl1) exit_wait=1;
      }
  }


void sample(OBJREC *o)
  {
  o->runs[0]=sample_init;
  o->runs[1]=sample_draw;
  o->runs[2]=sample_event;
  //o->runs[3]=sample_done;
  }

//------------------------------------------
void button_init(OBJREC *o,char *title)
  {
  char *v;
  title=get_title(title);
  o->userptr=(void *)getmem(strlen(title)+1);
  strcpy((char *)o->userptr,title);
  v=(char *)o->data;
  *v=0;
  }

void button_draw(int x1,int y1,int x2,int y2,OBJREC *o)
  {
  CTL3D x;
  char w;

  bar(x1,y1,x2,y2);
  highlight(&x,o->color);
  w=*(char *)o->data;
  x.bsize=2-w;
  x.ctldef=(w<<1)+w;
  draw_border(x1+2,y1+2,x2-x1-4,y2-y1-4,&x);
  set_aligned_position(((x1+x2)>>1)+(w<<1),((y1+y2)>>1)+(w<<1),1,1,(char *)o->userptr);
  outtext((char *)o->userptr);
  }

void button_draw2(int x1,int y1,int x2,int y2,OBJREC *o)
  {
  CTL3D x;
  char w;

  bar(x1,y1,x2,y2);
  highlight(&x,o->color);
  w=*(char *)o->data;
  x.bsize=1;
  x.ctldef=(w<<1)+w;
  draw_border(x1+1,y1+1,x2-x1-2,y2-y1-2,&x);
  if (!w)
  {
  curcolor=x.light;hor_line(x1,y1,x2);hor_line(x1,y1+1,x2);
  curcolor=x.shadow;hor_line(x1,y2,x2);hor_line(x1,y2-1,x2);
  }
  set_aligned_position(((x1+x2)>>1)+(w<<1),((y1+y2)>>1)+(w<<1),1,1,(char *)o->userptr);
  outtext((char *)o->userptr);
  }


void button_event(EVENT_MSG *msg,OBJREC *o)
  {
  MS_EVENT *ms;

  if (msg->msg==E_MOUSE)
     {
     ms=get_mouse(msg);
     if (ms->event_type & 0x06)
        {
        if (ms->tl1)
           {
           *(char *)o->data=1;
           redraw_object(o);
           }
        else if (*(char *)o->data)
              {
               *(char *)o->data=0;
               redraw_object(o);
               set_change();
              }
        }
        }

  if (msg->msg==E_GET_FOCUS || msg->msg==E_LOST_FOCUS)
     {
     *(char *)o->data=0;
     redraw_object(o);
     }
  }



void button(OBJREC *o)
  {
  o->runs[0]=button_init;
  o->runs[1]=button_draw;
  o->runs[2]=button_event;
  //o->runs[3]=button_done;
  o->datasize=1;
  }

void button2(OBJREC *o)
  {
  o->runs[0]=button_init;
  o->runs[1]=button_draw2;
  o->runs[2]=button_event;
  //o->runs[3]=button_done;
  o->datasize=1;
  }


//------------------------------------------

void draw_status_line(char *c)
  {
  static word *font;
  static FC_TABLE color;
  static word backgr;
  static word ysmax=0,y;
  word ysize;
  CTL3D ctl;

  if (c==NULL)
     {
     backgr=curcolor;
     memcpy(&color,&charcolors,sizeof(charcolors));
     font=curfont;
     return;
     }

  schovej_mysku();
  curfont=font;
  ysize=text_height(c);
  if (ysmax>ysize) ysize=ysmax;else
     ysmax=ysize;

  highlight(&ctl,backgr);
  ctl.bsize=2;
  ctl.ctldef=0;
  curcolor=backgr;
  memcpy(&charcolors,&color,sizeof(charcolors));
  y=SCR_WIDTH_Y-ysize-3;
  desktop_y_size=y-3;
  bar(0,y,SCR_WIDTH_X-1,SCR_WIDTH_Y-1);
  draw_border(2,y,SCR_WIDTH_X-5,ysize,&ctl);
  while (text_width(c)>SCR_WIDTH_X)
     {
     char *p;

     p=strchr(c,'\0');
     *(--p)='\0';
     if (p=c) break;
     }
  position(5,y);outtext(c);
  ukaz_mysku();
  showview(0,y-2,SCR_WIDTH_X-1,ysize+5);
  }

void *status_mem_info(EVENT_MSG *msg)
  {
  char *c;
  unsigned long long l;
  static char memtext[]=MEMTEXT;
  MEMORYSTATUSEX mem;
  mem.dwLength = sizeof(mem);

  if (msg->msg==E_INIT) return &status_mem_info;
  c=(char *)msg->data;
  strcpy(c,memtext);
  c+=strlen(memtext);
  GlobalMemoryStatusEx(&mem);
  l=mem.ullAvailPhys;
  sprintf(c,"%u KB ",l/1024);
  c=strchr(c,'\0');
  msg->data=(void *)c;
  return NULL;
  }

void *status_idle(EVENT_MSG *msg)
  {
  if (msg->msg==E_INIT) return &status_idle;
  send_message(E_STATUS_LINE,msg->msg);
  return NULL;
  }

void status_line(EVENT_MSG *msg,T_EVENT_ROOT **user_data)
  {
  T_EVENT_ROOT **p;
  static char st_line[256],oldline[256]={"\0"};
  EVENT_MSG tg;
  static char recurse=1;

  if(msg->msg==E_INIT)
     if (recurse)
     {
      T_EVENT_ROOT *p;
      recurse=0;
      send_message(E_ADD,E_IDLE,status_idle);
      send_message(E_ADD,E_REDRAW,status_idle);
      p=NULL;
      *user_data=p;
      draw_status_line(NULL);
      recurse=1;
      return;
      }
     else return;
  shift_msg(msg,tg);
       if (tg.msg==E_REDRAW)
        {
        draw_status_line(oldline);
        return;
        }
  p=user_data;
    if (tg.msg==E_IDLE)
     {
     EVENT_MSG msg;

     msg.msg=E_IDLE;
     msg.data=&st_line;
     enter_event(p,&msg);
     if (strcmp(st_line,oldline))
        {
        draw_status_line(st_line);
        strcpy(oldline,st_line);
        }
     }
  else
     tree_basics(p,&tg);
  return;
  }

void *mouse_xy(EVENT_MSG *msg)
  {
  char *c;

  if (msg->msg==E_INIT) return &mouse_xy;
  c=(char *)msg->data;
  sprintf(c," X: %d Y: %d",ms_last_event.x,ms_last_event.y);
  c=strchr(c,'\0');
  msg->data=(void *)c;
  return NULL;
  }

void *show_time(EVENT_MSG *msg)
  {
  char *c;
  time_t t;
  struct tm cas;

  if (msg->msg==E_INIT) return &show_time;
  c=(char *)msg->data;
  t=time(NULL);
  cas=*localtime(&t);

  sprintf(c,"%02d:%02d:%02d ",cas.tm_hour,cas.tm_min,cas.tm_sec);
  c=strchr(c,'\0');
  msg->data=(void *)c;
  return NULL;
  }
//------------------------------------------

void win_label_draw(int x1,int y1,int x2,int y2,OBJREC *o)
  {
  bar(x1,y1,x2,y2);
  set_aligned_position(x1+5,(y1+y2)/2,0,1,(char *)o->userptr);
  outtext((char *)o->userptr);
  }

void xor_rectangle(int x,int y,int xs,int ys)
  {
  curcolor=RGB555(31,31,31);
  if (x<0) x=0;
  if (y<0) y=0;
  if (x+xs>=SCR_WIDTH_X) xs=SCR_WIDTH_X-x-1;
  if (y+ys>=SCR_WIDTH_Y) ys=SCR_WIDTH_Y-y-1;
  schovej_mysku();
  hor_line_xor(x,y,x+xs);
  ver_line_xor(x,y,y+ys);
  if (xs && ys)
     {
  hor_line_xor(x,y+ys,x+xs);
  ver_line_xor(x+xs,y,y+ys);
     }
  ukaz_mysku();
  showview(x,y,x+xs,4);
  showview(x,y,4,ys+4);
  if (xs && ys)
     {
  showview(x,y+ys,x+xs,4);
  showview(x+xs,y,4,ys+4);
     }
  }

void win_label_move(EVENT_MSG *msg,OBJREC *o)
  {
  MS_EVENT *ms;
  static char run=0;
  static word xref,yref;
  static WINDOW w;
  static moved=0;
  static drawed=0;

  o;
  if (msg->msg==E_INIT) return;
  if (msg->msg==E_TIMER)
     {
     send_message(E_TIMER);
     if (!drawed)
     if (!moved)
     {
     drawed=1;
     redraw_desktop();
     moved=0;
     }
     else
     {
     drawed=0;
     moved=0;
     }
     }
  if (msg->msg==E_MOUSE)
     {
        ms=get_mouse(msg);
        if (run)
           {
           //xor_rectangle(w.x,w.y,w.xs,w.ys);
           if (ms->event_type & 4)
              {
              run=0;
              redraw_desktop();
              send_message(E_DONE,E_MOUSE,win_label_move);
              msg->msg=-1;
              return;
              }
           w.x=ms->x-xref;
           w.y=ms->y-yref;
           check_window(&w);
           //xor_rectangle(w.x,w.y,w.xs,w.ys);
           waktual->x=w.x;
           waktual->y=w.y;
           waktual->xs=w.xs;
           waktual->ys=w.ys;
           moved=1;drawed=0;
           redraw_window();
           redraw_desktop();
           }
        else
           if (ms->event_type & 2)
           {
           run=1;
           memcpy(&w,waktual,sizeof(WINDOW));
           //xor_rectangle(w.x,w.y,w.xs,w.ys);
           xref=ms->x-waktual->x;
           yref=ms->y-waktual->y;
           send_message(E_ADD,E_MOUSE,win_label_move);
           freeze_on_exit=1;
           }
     }
  if (msg->msg==E_LOST_FOCUS && run)
     {
              run=0;
              redraw_desktop();
              send_message(E_DONE,E_MOUSE,win_label_move);
              msg->msg=-1;
              return;
     }
msg->msg=-1;
return;
  }


void win_label(OBJREC *o)
  {
  o->runs[0]=sample_init;
  o->runs[1]=win_label_draw;
  o->runs[2]=win_label_move;
  //o->runs[3]=button_done;
  o->datasize=0;
  }

//------------------------------------------

void check_box_draw(int x1,int y1, int x2, int y2,OBJREC *o)
  {
  int x3;

  x1+=1;y1+=1;x2-=1;y2-=1;
  x3=x1+(y2-y1);
  draw_border(x1,y1,x3-x1,y2-y1,def_border(4,curcolor));
  bar(x1,y1,x3,y2);
  if (*(char *)o->data & 1)
     {
     curcolor=0x0000;
     line(x1,y1,x3,y2);line(x1+1,y1,x3,y2-1);line(x1,y1+1,x3-1,y2);
     line(x1,y2,x3,y1);line(x1+1,y2,x3,y1+1);line(x1,y2-1,x3-1,y1);
     }
  if (*(char *)o->data & 0x80)
     {
     curcolor=0x0000;
     hor_line(x1,y1,x3);ver_line(x3,y1,y2);
     ver_line(x1,y1,y2);hor_line(x1,y2,x3);
     }
  else
     {
     set_aligned_position(x3+5,(y2+y1)/2,0,1,(char *)o->userptr);
     outtext((char *)o->userptr);
     }
  }

void check_box_event(EVENT_MSG *msg,OBJREC *o)
  {
  MS_EVENT *ms;

  if (msg->msg==E_MOUSE)
     {
     ms=get_mouse(msg);
     if (ms->event_type & 0x06)
        {
        if (ms->tl1)
           {
           *(char *)o->data|=0x80;
           redraw_object(o);
           }
        else if (*(char *)o->data)
              {
               *(char *)o->data^=0x1;
               *(char *)o->data&=0x1;
               redraw_object(o);
               set_change();
              }
        }
        }

  if (msg->msg==E_GET_FOCUS || msg->msg==E_LOST_FOCUS)
     {
     *(char *)o->data&=0x1;
     redraw_object(o);
     }
  }


void check_box(OBJREC *o)
  {
  o->runs[0]=sample_init;
  o->runs[1]=check_box_draw;
  o->runs[2]=check_box_event;
  o->datasize=4;
  }

//------------------------------------------
void radio_butts_init(OBJREC *o,long *params)
  {
  char *c,*z;
  long cnt=0,*q,*d;
  int i;

  d=params;
  for (i=0;i<*params;i++)
     {
     d+=1;
     c=get_title(d);
     cnt+=strlen(c);cnt++;
     }
  q=(long *)getmem(cnt+8);
  o->userptr=(void *)q;
  *q++=1;*q++=*params;
  d=params;
  z=(char *)q;
  for (i=0;i<*params;i++)
     {
     d+=1;
     c=get_title(d);
     strcpy(z,c);
     z=strchr(z,'\0');z++;
     }
  }

void radio_butts_draw(int x1,int y1,int x2,int y2,OBJREC *o)
  {
  int step,size,sizpul,i,cr;
  long *params;
  char *texts;
  CTL3D ctl;

  cr=curcolor;
  highlight(&ctl,curcolor);
  params=(long *)o->userptr;
  if (*params) bar(x1,y1,x2,y2);
  step=(y2-y1)/(*(params+1));
  size=(step*9)/10;
  sizpul=size>>1;

  texts=(char *)(params+2);
  for (i=0;i<*(params+1);i++,y1+=step)
     {
     int j;
     curcolor=ctl.shadow;
     line(x1+sizpul,y1,x1,y1+sizpul);
     line(x1,y1+sizpul,x1+sizpul,y1+size-1);
     curcolor=ctl.light;
     line(x1+sizpul+1,y1,x1+size,y1+sizpul);
     line(x1+size,y1+sizpul,x1+sizpul+1,y1+size-1);
     if (*(long *)o->data==i) curcolor=0;else curcolor=cr;
     for (j=0;j<3;j++)
        {
        hor_line(x1+sizpul-j,y1+sizpul-2+j,x1+sizpul+j);
        hor_line(x1+sizpul-j,y1+sizpul+2-j,x1+sizpul+j);
        }
     if (*params)
        {
        set_aligned_position(x1+size+5,y1+sizpul,0,1,texts);
        outtext(texts);
        texts=strchr(texts,'\0')+1;
        }

     }
  }

void radio_butts_event(EVENT_MSG *msg,OBJREC *o)
  {
  MS_EVENT *ms;
  int sel;

  if (msg->msg==E_MOUSE)
     {
     ms=get_mouse(msg);
     if (ms->event_type & 0x02)
        {
        sel=(ms->y-o->locy)/(o->ys/(*((long *)o->userptr+1)));
        if (sel>=*((long *)o->userptr+1)) sel=*((long *)o->userptr+1)-1;
        *(long *)o->data=sel;
        *(long *)o->userptr=0;
        redraw_object(o);
        *(long *)o->userptr=1;
        set_change();
        }
     }

  }


void radio_butts(OBJREC *o)
  {
  o->runs[0]=radio_butts_init;
  o->runs[1]=radio_butts_draw;
  o->runs[2]=radio_butts_event;;
  o->datasize=4;
  }

//------------------------------------------



void toggle_button_event(EVENT_MSG *msg,OBJREC *o)
  {
  MS_EVENT *ms;
  static char toggle_exit=0;

  if (msg->msg==E_MOUSE)
     {
     ms=get_mouse(msg);
     if (ms->event_type & 0x06)
        {
        if (ms->tl1)
           {
           *(char *)o->data^=1;
           redraw_object(o);
           toggle_exit=1;
           }
        else if (toggle_exit)
              {
              set_change();
              toggle_exit=0;
              }
        }
        }

  if ((msg->msg==E_GET_FOCUS || msg->msg==E_LOST_FOCUS)&&toggle_exit)
     {
     *(char *)o->data^=1;
     redraw_object(o);
     toggle_exit=0;
     }
  }



void toggle_button(OBJREC *o)
  {
  o->runs[0]=button_init;
  o->runs[1]=button_draw;
  o->runs[2]=toggle_button_event;
  o->datasize=1;
  }

//------------------------------------------

void input_line_init(OBJREC *o,int *len)
  {
  o->datasize=(*len)+1;
  o->userptr=malloc(20);
  memset(o->userptr,0,20);
  memcpy(o->userptr,len,4);
  memcpy((int *)o->userptr+2,len+1,12);
  }

void input_line_draw(int x1,int y1, int x2, int y2, OBJREC *o)
  {
  char d[2]=" \0";
  int x;
  char *c;
  int len;
  int shift;

  bar(x1,y1,x2,y2);
  position(x1,y1);
  c=(char *)o->data;
  if (!*c) return;
  len=strlen(c);
  shift=*((int *)o->userptr+1);
  if (shift>=len) shift=0;
  c+=shift;
  d[0]=*c++;x=x1+text_width(d);
  while (x<x2)
     {
     outtext(d);
     if (!*c) break;
     d[0]=*c++;x=writeposx+text_width(d);
     }
  }

void input_line_event(EVENT_MSG *msg,OBJREC *o)
  {
   static cursor=0;
   int *len,*start,slen;
   char *c;
   static char *save;
   static char clear_kontext;

   len=(int *)o->userptr;
   start=len+1;
   c=(char *)o->data;
   slen=strlen(c);
   switch (msg->msg)
     {
     case E_GET_FOCUS:cursor=0;save=(char *)getmem(*len+1);
                      strcpy(save,c);clear_kontext=1;break;
     case E_LOST_FOCUS:cursor=0;*start=0;free(save);redraw_object(o);break;
     case E_CURSOR_TICK:
        {
        int xpos,i,j=-1,d;

        do
           {
           xpos=0;j++;
           for (i=*start;i<cursor;i++)
              {
              if ((d=charsize(curfont,c[i]) & 0xff)==0) xpos+=1;
              else xpos+=d;
              }
           if (xpos>=o->xs) (*start)+=1;
           if (xpos==0) (*start)-=1;
           if (*start<0) *start=0;
           }
        while ((xpos==0 || xpos>=o->xs)&& cursor);
        if (j) redraw_object(o);
        xor_rectangle(o->locx+xpos,o->locy,1,o->ys);
        };break;
     case E_MOUSE:
        {
        MS_EVENT *ms;int msx;

        ms=get_mouse(msg);
        msx=ms->x-o->locx;
        if (ms->event_type & 2)
           {
           int xpos;

           xpos=0;
           for (cursor=*start;cursor<slen;cursor++)
              {
              xpos+=charsize(curfont,c[cursor]) & 0xff;
              if (xpos>msx) break;
              }
           redraw_object(o);
           }
        }
        break;
     case E_KEYBOARD:
        {
        char key;

        cancel_event();
        key=(*(int *)msg->data) & 0xff;
        if (!key)
        switch (*(int *)msg->data >> 8)
           {
           case 'M':if (cursor<slen)  cursor++;break;
           case 'K':if (cursor>0)  cursor--;break;
           case 'S':if (cursor<slen) strcpy(&c[cursor],&c[cursor+1]);slen--;break;
           case 'G':cursor=0;
           case 'O':cursor=slen;
           }
        else
        if (key)
           switch (key)
           {
           case 8:if (cursor>0) {strcpy(&c[cursor-1],&c[cursor]);cursor--;}break;
           case 0:break;
           case 13:break;
           case 27:strcpy(c,save);slen=strlen(c);if (cursor>slen) cursor=slen;break;
           default:if (key>=' ') if (slen<*len || clear_kontext)
             {
             int i;

              if (clear_kontext)
                 {*c='\0';cursor=0;slen=0;}
              for (i=slen+1;i>cursor;i--) c[i]=c[i-1];
              c[cursor++]=key;
             }
           }
        if (!cursor) *start=0;
        redraw_object(o);
        msg->msg=E_CURSOR_TICK;
        input_line_event(msg,o);
        clear_kontext=0;
        msg->msg=-1;
        }
     }
  }





void input_line(OBJREC *o)
  {
  o->runs[0]=input_line_init;
  o->runs[1]=input_line_draw;
  o->runs[2]=input_line_event;
  }


//-------------------------------------------------------------
void label(OBJREC *o)
  {
  o->runs[0]=sample_init;
  o->runs[1]=sample_draw;
  }

void mid_label(OBJREC *o)
  {
  o->runs[0]=sample_init;
  o->runs[1]=mid_label_draw;
  }
//-------------------------------------------------------------

typedef struct scr_button
  {
  int step;
  int maxvalue;
  char *title;
  }SCR_BUTTON;

void scroll_button_init(OBJREC *o,int *param)
  {                           // int step, int maxvalue,char *title
  char *v;
  SCR_BUTTON *p;

  o->userptr=getmem(sizeof(SCR_BUTTON));
  p=(SCR_BUTTON *)o->userptr;
  p->step=*param++;
  p->maxvalue=*param++;
  p->title=(char *)*param++;
  v=(char *)o->data;
  *v=0;
  }



void scroll_button_draw(int x1,int y1,int x2,int y2,OBJREC *o)
  {
  CTL3D x;
  char w;
  SCR_BUTTON *param;

  param=(SCR_BUTTON *)o->userptr;
  bar(x1,y1,x2,y2);
  highlight(&x,o->color);
  w=*(char *)o->data;
  x.bsize=2-w;
  x.ctldef=(w<<1)+w;
  draw_border(x1+2,y1+2,x2-x1-4,y2-y1-4,&x);
  set_aligned_position(((x1+x2)>>1)+(w<<1),((y1+y2)>>1)+(w<<1),1,1,param->title);
  outtext(param->title);
  }

void scroll_button_event(EVENT_MSG *msg,OBJREC *o)
  {
  MS_EVENT *ms;

  if (msg->msg==E_MOUSE)
     {
     ms=get_mouse(msg);
     if (ms->event_type & 0x0e)
        {
        if (ms->tl1 || ms->event_type & 0x8)
           {
           *(char *)o->data=1;
           redraw_object(o);
           set_change();
           }
        }
     }
  if (msg->msg==E_TIMER && *(char *)o->data )
      if (ms_last_event.tl1) set_change();
      else if (!ms_last_event.tl2)
             {
              *(char *)o->data=0;
               redraw_object(o);
             }

  if (msg->msg==E_GET_FOCUS || msg->msg==E_LOST_FOCUS)
     {
     *(char *)o->data=0;
     redraw_object(o);
     }
  }

void scroll_button(OBJREC *o)
  {
  o->runs[0]=scroll_button_init;
  o->runs[1]=scroll_button_draw;
  o->runs[2]=scroll_button_event;
  o->datasize=1;
  }


//-------------------------------------------------------------
typedef struct scr_bar
  {
  int minvalue;
  int maxvalue;
  int parview;
  int bgcolor;
  int barsize;
  int stepsize;
  }SCR_BAR;

void scroll_bar_init(OBJREC *o,int *param)
  {
  SCR_BAR *p;

  o->userptr=getmem(sizeof(SCR_BAR));
  p=(SCR_BAR *)o->userptr;
  p->minvalue=*param++;
  p->maxvalue=*param++;
  p->parview=*param++;
  p->bgcolor=*param++;
  p->barsize=10;
  p->stepsize=10;
  }

void scroll_bar_v_draw(int x1,int y1,int x2,int y2,OBJREC *o)
  {
  int barsize;
  int wsize;
  int valsize;
  SCR_BAR *p;
  int *d;
  CTL3D ctl;
  int y;


  p=(SCR_BAR *)o->userptr;
  d=(int *)o->data;
  valsize=p->maxvalue-p->minvalue+p->parview;
  wsize=y2-y1;
  barsize=valsize?wsize*p->parview/valsize:10; if (barsize<2) barsize=2;
  if (barsize>wsize) barsize=wsize;
  wsize-=barsize;
  curcolor=p->bgcolor;
  bar(x1,y1,x2,y2);
  curcolor=o->color;
  highlight(&ctl,o->color);ctl.bsize=2;ctl.ctldef=0;
  y=valsize?(*d-p->minvalue)*(wsize+barsize)/valsize:0;
  p->stepsize=valsize?wsize/valsize:0;
  if (y>wsize) y=wsize;
  if (y<0) y=0;
  y+=y1;
  draw_border(x1+2,y+2,(x2-x1)-4,barsize-4,&ctl);
  if (barsize>4)bar(x1+2,y+2,x2-2,y+barsize-2);
  p->barsize=barsize;
  }

void scroll_bar_v_event(EVENT_MSG *msg,OBJREC *o)
  {
  SCR_BAR *p;
  int *d;
  int _d;

  p=(SCR_BAR *)o->userptr;
  d=(int *)o->data;
  switch (msg->msg)
     {
     case E_MOUSE:
          {
          int y;
          MS_EVENT *ms;

          ms=get_mouse(msg);
          y=(ms->y+(p->stepsize>>1)-o->locy-(p->barsize>>1));
          if (ms->tl1)
           {
           if (o->ys<=p->barsize) _d=p->minvalue; else
           _d=y*(p->maxvalue-p->minvalue)/(o->ys-p->barsize)+p->minvalue;
           if (_d>p->maxvalue) _d=p->maxvalue;
           if (_d<p->minvalue) _d=p->minvalue;
           if (_d!=*d)
              {
              *d=_d;
              redraw_object(o);
              set_change();
              }
           }
          }
          break;
     case E_CONTROL:
          {
          int *q;

          q=msg->data;
          p->minvalue=*q++;
          p->maxvalue=*q++;
          p->parview=*q++;
          }
          break;

     }
  }

void scroll_bar_v(OBJREC *o)
  {
  o->runs[0]=scroll_bar_init;
  o->runs[1]=scroll_bar_v_draw;
  o->runs[2]=scroll_bar_v_event;
  o->datasize=4;
  }

//-------------------------------------------------------------
void scroll_support()
  {
  int id,x;
  SCR_BAR *p;
  SCR_BUTTON *q;
  OBJREC *o,*r;


  id=o_aktual->id;
  id=(id/10)*10;
  o=find_object(waktual,id);
  p=(SCR_BAR *)o->userptr;
  q=(SCR_BUTTON*)o_aktual->userptr;
  x=f_get_value(0,id);
  x+=q->step;
  if (q->step<0)
     if (x<p->minvalue) x=p->minvalue;
  if (q->step>0)
     if (x>p->maxvalue) x=p->maxvalue;
  set_value(0,id,&x);
  r=o_aktual;
  o_aktual=o;
  o->events[3]();
  o_aktual=r;
  }
//-------------------------------------------------------------

void scroll_bar_h_draw(int x1,int y1,int x2,int y2,OBJREC *o)
  {
  int barsize;
  int wsize;
  int valsize;
  SCR_BAR *p;
  int *d;
  CTL3D ctl;
  int x;


  p=(SCR_BAR *)o->userptr;
  d=(int *)o->data;
  valsize=p->maxvalue-p->minvalue;
  wsize=x2-x1;
  barsize=wsize*p->parview/valsize;
  if (barsize>wsize) barsize=wsize;if (barsize<2) barsize=2;
  wsize-=barsize;
  curcolor=p->bgcolor;
  bar(x1,y1,x2,y2);
  curcolor=o->color;
  highlight(&ctl,o->color);ctl.bsize=2;ctl.ctldef=0;
  x=(*d-p->minvalue)*wsize/valsize;
  p->stepsize=wsize/valsize;
  if (x>wsize) x=wsize;
  if (x<0) x=0;
  x+=x1;
  draw_border(x+2,y1+2,barsize-4,(y2-y1)-4,&ctl);
  if (barsize>4)bar(x+2,y1+2,x+barsize-2,y2-2);
  p->barsize=barsize;
  }

void scroll_bar_h_event(EVENT_MSG *msg,OBJREC *o)
  {
  SCR_BAR *p;
  int *d;
  int _d;

  p=(SCR_BAR *)o->userptr;
  d=(int *)o->data;
  switch (msg->msg)
     {
     case E_MOUSE:
          {
          int x;
          MS_EVENT *ms;

          ms=get_mouse(msg);
          x=(ms->x+(p->stepsize>>1)-o->locx-(p->barsize>>1));
          if (ms->tl1)
           {
           if (o->xs<=p->barsize) _d=p->minvalue; else
           _d=x*(p->maxvalue-p->minvalue)/(o->xs-p->barsize)+p->minvalue;
           if (_d>p->maxvalue) _d=p->maxvalue;
           if (_d<p->minvalue) _d=p->minvalue;
           if (_d!=*d)
              {
              *d=_d;
              redraw_object(o);
              set_change();
              }
           }
          }
     }
  }





 void scroll_bar_h(OBJREC *o)
  {
  o->runs[0]=scroll_bar_init;
  o->runs[1]=scroll_bar_h_draw;
  o->runs[2]=scroll_bar_h_event;
  o->datasize=4;

  }


//-------------------------------------------------------------



#define MSG_SIZE (SCR_WIDTH_X*3/4)
#define MSG_L_MARGIN 10
#define MSG_R_MARGIN 50
#define MSG_A_MARGIN (MSG_L_MARGIN+MSG_R_MARGIN)
#define MSG_COLOR RGB555(15,0,0)
#define MSG_F_COLOR RGB555(31,31,0)

word *msg_box_font;
word *msg_icn_font;


int msg_box(char *title, char icone, char *text, ... )
  {
  int winx,winy,xp,yp,txt_h;
  int txt_max,temp1,temp2,i;
  char buf[256];
  char *p;
  CTL3D *ctl;
  char **c;
  FC_TABLE cl;

  curfont=msg_box_font;
  ctl=def_border(2,MSG_COLOR);
  winx=text_width(text)+MSG_A_MARGIN;
  winy=30;
  desktop_add_window(create_window(0,0,winx,300,MSG_COLOR,ctl));
  buf[1]='\0';buf[0]=icone;
  xp=text_width(buf);
  define(-1,(MSG_R_MARGIN>>1)-(xp>>1),20,xp,text_height(buf),1,label,buf);
  cl[1]=ctl->light;
  cl[0]=ctl->shadow;
  property(NULL,msg_icn_font,&cl,MSG_COLOR);
  curfont=msg_box_font;
  default_font=curfont;
  if (winx>MSG_SIZE) winx=MSG_SIZE;
  c=&text;c++;
  temp1=0;temp2=0;
   while (*c)
     {temp1+=text_width(*c++)+10;temp2++;}
  if (temp1>winx-2*MSG_L_MARGIN) winx=temp1+2*MSG_L_MARGIN;
  txt_max=winx-MSG_A_MARGIN;
  txt_h=0;
  while (*text)
     {
     memset(buf,0,sizeof(buf));
     p=buf;
     while (text_width(buf)<txt_max && *text && *text!='\n') *p++=*text++;
     if (text_width(buf)>txt_max) while (*(--p)!=' ') text--;
     if (*text=='\n') text++;
     *p='\0';
     txt_h=text_height(buf);
     define(-1,MSG_L_MARGIN,winy,txt_max,txt_h,0,label,&buf);
     property(NULL,NULL,flat_color(MSG_F_COLOR),MSG_COLOR);
     o_end->f_color[0]=0;
     winy+=txt_h;
     }
  winy+=40;
  xp=(SCR_WIDTH_X>>1)-(winx>>1);
  yp=(SCR_WIDTH_Y>>1)-(winy>>1);
  waktual->x=xp;waktual->y=yp;waktual->xs=winx;waktual->ys=winy;
  define(0,1,1,winx-2,text_height(title)+2,0,win_label,title);
  ctl=def_border(5,MSG_COLOR);
  property(ctl,NULL,flat_color(MSG_F_COLOR),0x10);
  ctl=def_border(1,0);
  c=&text;c++;
  for (i=1;i<=temp2;i++)
     {
     int sz;

     sz=(winx/(temp2+1))>>1;
     if (sz<text_width(*c)) sz=text_width(*c);
   define((i),i*winx/(temp2+1)-(sz>>1),10,sz+5,20,3,button,*c);
   property(ctl,NULL,flat_color(0),RGB555(24,24,24));on_change(terminate);
     c++;
     }
  set_window_modal();
  redraw_window();
  escape();
  temp2=o_aktual->id;
  close_window(waktual);
  return temp2;
  }


//-------------------------------------------------------------


void resizer_draw(int x1,int y1,int x2,int y2,OBJREC *o)
  {
  CTL3D ctl;

  highlight(&ctl,o->color);
  curcolor=o->color;
  bar(x1,y1,x2,y2);
  curcolor=ctl.light;
  line(x2-1,y1+1,x1+1,y2-1);
  line(x2-1,(y1+y2)>>1,(x1+x2)>>1,y2-1);
  curcolor=ctl.shadow;
  line(x2-1,y1+4,x1+4,y2-1);
  line(x2-1,y1+2,x2-1,y1+4);
  line(x1+2,y2-1,x1+4,y2-1);
  line(x2-1,((y1+y2)>>1)+4,((x1+x2)>>1)+4,y2-1);
  line(x2-1,((y1+y2)>>1)+2,x2-1,((y1+y2)>>1)+4);
  line(((x1+x2)>>1)+2,y2-1,((x1+x2)>>1)+4,y2-1);
  }

void resizer_event(EVENT_MSG *msg,OBJREC *o)
  {
  MS_EVENT *ms;
  static char run=0;
  static word xref,yref;
  static WINDOW w;
  static moved=0;
  static drawed=0;

  o;
  if (msg->msg==E_INIT) return;
  if (msg->msg==E_TIMER && !drawed)
     if (!moved)
     {
     drawed=1;
     redraw_desktop();
     moved=0;
     }
     else
     {
     drawed=0;
     moved=0;
     }
  if (msg->msg==E_MOUSE)
     {
        ms=get_mouse(msg);
        if (run)
           {
           char force_redraw=0;
           int new;
           //xor_rectangle(w.x,w.y,w.xs,w.ys);
           if (ms->event_type & 4)
              {
              run=0;
              redraw_desktop();
              send_message(E_DONE,E_MOUSE,resizer_event);
              msg->msg=-1;
              return;
              }
           new=ms->x-waktual->x+xref;if (new<w.xs) force_redraw=1;
           w.xs=new;
           new=ms->y-waktual->y+yref;if (new<w.ys) force_redraw=1;
           w.ys=new;
           check_window(&w);
           //xor_rectangle(w.x,w.y,w.xs,w.ys);
           movesize_win(waktual,w.x,w.y,w.xs,w.ys);
           moved=1;drawed=0;
           redraw_window();
           if (force_redraw) redraw_desktop();
           }
        else
           if (ms->event_type & 2)
           {
           run=1;
           memcpy(&w,waktual,sizeof(WINDOW));
           //xor_rectangle(w.x,w.y,w.xs,w.ys);
           xref=waktual->xs-(ms->x-waktual->x);
           yref=waktual->ys-(ms->y-waktual->y);
           send_message(E_ADD,E_MOUSE,resizer_event);
           freeze_on_exit=1;
           }
     }
  if (msg->msg==E_LOST_FOCUS && run)
     {
              run=0;
              redraw_desktop();
              send_message(E_DONE,E_MOUSE,resizer_event);
              msg->msg=-1;
              return;
     }
if (msg->msg!=E_TIMER)  msg->msg=-1;
  return;
  }



void resizer(OBJREC *o)
  {
  //o->runs[0]=resizer_init;
  o->runs[1]=resizer_draw;
  o->runs[2]=resizer_event;
  }
