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
 *  Last commit made by: $Id: INTERFAC.C 7 2008-01-14 20:14:25Z bredysoft $
 */
#include <skeldal_win.h>
#include <dos.h>
#include <bios.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include <dpmi.h>
#include <conio.h>
#include <malloc.h>
#include <mem.h>
#include <pcx.h>
#include <types.h>
#include <bgraph.h>
#include <event.h>
#include <devices.h>
#include <bmouse.h>
#include <memman.h>
#include <zvuk.h>
#include <strlite.h>
#include <ctype.h>
#include <gui.h>
#include <basicobj.h>
#include <time.h>
#include <mgfplay.h>
#include <wav.h>
#include <io.h>
#include <fcntl.h>
#include "globals.h"
#include "engine1.h"

#define MES_MAXSIZE 500
#define CHECK_BOX_ANIM 6

static char *error_hack="....Source compiled.";
static char shadow_enabled=1;

word color_topbar[7]={0,RGB555(22,14,4),RGB555(24,16,6),RGB555(25,17,7)};

int input_txtr=H_LOADTXTR;

void create_frame(int x,int y,int xs,int ys,char clear)
   {
   word *line;
   word *col;
   int i;

   x-=VEL_RAMEC;
   y-=VEL_RAMEC;
   xs=(xs+VEL_RAMEC-1)/VEL_RAMEC+1;
   ys=(ys+VEL_RAMEC-1)/VEL_RAMEC+1;
   line=GetScreenAdr()+y*scr_linelen2+x;
   col=line;
   put_8bit_clipped(ablock(H_RAMECEK),col,0,VEL_RAMEC,VEL_RAMEC);col+=VEL_RAMEC;
   for(i=1;i<xs;i++)
     {
     put_8bit_clipped(ablock(H_RAMECEK),col,VEL_RAMEC,VEL_RAMEC,VEL_RAMEC);col+=VEL_RAMEC;
     }
  put_8bit_clipped(ablock(H_RAMECEK),col,VEL_RAMEC*2,VEL_RAMEC,VEL_RAMEC);
  line+=scr_linelen2*VEL_RAMEC;
  for(i=1;i<ys;i++)
     {
     put_8bit_clipped(ablock(H_RAMECEK),line,VEL_RAMEC*3,VEL_RAMEC,VEL_RAMEC);
     put_8bit_clipped(ablock(H_RAMECEK),line+VEL_RAMEC*xs,VEL_RAMEC*4,VEL_RAMEC,VEL_RAMEC);
     line+=scr_linelen2*VEL_RAMEC;
     }
  col=line;
  put_8bit_clipped(ablock(H_RAMECEK),col,VEL_RAMEC*5,VEL_RAMEC,VEL_RAMEC);col+=VEL_RAMEC;
  for(i=1;i<xs;i++)
     {
     put_8bit_clipped(ablock(H_RAMECEK),col,VEL_RAMEC*6,VEL_RAMEC,VEL_RAMEC);col+=VEL_RAMEC;
     }
  put_8bit_clipped(ablock(H_RAMECEK),col,VEL_RAMEC*7,VEL_RAMEC,VEL_RAMEC);
  if (clear)
     {
     curcolor=COL_RAMEC;
     x+=VEL_RAMEC;y+=VEL_RAMEC;
     xs=(xs-1)*VEL_RAMEC;
     ys=(ys-1)*VEL_RAMEC;
     bar(x,y,x+xs-1,y+ys-1);
     }
  }


void show_textured_button(int x,int y,int xs,int ys,int texture,CTL3D *border3d)
  {
  int i;i=0;
  if (texture) put_textured_bar(ablock(texture),x,y,xs,ys,border3d->light,border3d->shadow);

  for(i=border3d->bsize-1;i>=0;i--)
     if (border3d->ctldef & (1<<i))
        {
        trans_line_x(x+i,y+i,xs-(i<<1),0);
        trans_line_x(x+i,y+ys-i-1,xs-(i<<1),RGB555(31,31,31));
        trans_line_y(x+i,y+i,ys-(i<<1),0);
        trans_line_y(x+xs-i-1,y+i,ys-(i<<1),RGB555(31,31,31));
        }
     else
        {
        trans_line_x(x+i,y+i,xs-(i<<1),RGB555(31,31,31));
        trans_line_x(x+i,y+ys-i-1,xs-(i<<1),0);
        trans_line_y(x+i,y+i,ys-(i<<1),RGB555(31,31,31));
        trans_line_y(x+xs-i-1,y+i,ys-(i<<1),0);
        }

  }
void show_textured_win(struct window *w)
  {
  show_textured_button(w->x,w->y,w->xs-10,w->ys-10,w->color,&w->border3d);
  if (shadow_enabled)
    {
    trans_bar((w->x+10),w->y+w->ys-10,(w->xs-10),10,0);
    trans_bar(w->x+w->xs-10,w->y+10,10,w->ys-20,0);
    }
  else shadow_enabled=1;
  }

void add_window(int x,int y,int xs,int ys,int texture,int border,int txtx,int txty)
  {
  CTL3D wb;
  WINDOW *p;

  xs&=~1;
  wb.bsize=abs(border);
  wb.ctldef=-1*(border<0);
  wb.light=txtx;
  wb.shadow=txty;
  p=create_window(x,y,xs,ys,texture,&wb);
  p->draw_event=show_textured_win;
  desktop_add_window(p);

  }


void zalamovani(char *source,char *target,int maxxs,int *xs,int *ys)
  {
  strcpy(target,source);
  xs[0]=0;
  ys[0]=0;
  if ((xs[0]=text_width(target))>maxxs)
     {
     char c[2]=" ";
     char *ls,*ps,*cs;
     int sum;

     cs=ps=target;
     do
        {
        ls=NULL;
        sum=0;
        while (sum<maxxs || ls==NULL)
           {
           c[0]=*ps++;
           if (c[0]==0) {ls=NULL;break;}
           if (c[0]==32) ls=ps-1;
           sum+=text_width(c);
           }
        if (ls!=NULL)
           {
           *ls=0;
           ps=ls+1;ls=NULL;
           }
        ys[0]+=text_height(cs);
        cs=ps;
        }
     while (c[0]);
     xs[0]=maxxs;
     *ps=0;
     }
  else
     {
     char *c;
     c=target;c=strchr(c,0);c++;*c=0;
     ys[0]=text_height(target);
     }
  }

static T_CLK_MAP message_win[]=
  {
  {-1,0,0,0,0,NULL,0,H_MS_DEFAULT},
  {-1,0,0,639,479,empty_clk,0xff,H_MS_DEFAULT},
  };

#define MSG_COLOR1 (RGB555(30,30,23))

void open_message_win(int pocet_textu,char **texts)
  {
  int maxxs;
  int maxys,maxws,wscelk,wsx,wsy,wsys,y;
  int x1,y1,x2,y2;
  int i;
  char *text;

  set_font(H_FBOLD,MSG_COLOR1);
  text=alloca(strlen(texts[0])+2);
  zalamovani(texts[0],text,MES_MAXSIZE,&maxxs,&maxys);
  maxws=0;wsys=0;
  for(i=1;i<pocet_textu;i++)
     {
     int z1=text_width(texts[i]);
     int z2=text_height(texts[i]);
     if (z1>maxws) maxws=z1;
     if (z2>wsys) wsys=z2;
     }
  maxws+=10;
  if (maxws<50) maxws=50;
  wscelk=(pocet_textu-1)*(maxws+20);
  if (wscelk>maxxs) maxxs=wscelk;
  wsy=10+wsys;
  maxys+=wsy+40;
  if (maxys<50) maxys=50;
  maxxs+=20;
  add_window(x1=320-(maxxs>>1),y1=180-(maxys>>1),x2=maxxs+10,y2=maxys+10,H_WINTXTR,2,0,0);
  message_win[0].xlu=x1; message_win[0].ylu=y1; message_win[0].xrb=x1+x2;message_win[0].yrb=y1+y2;
  change_click_map(message_win,2);
  set_window_modal();
  y=10;
  while (text[0])
     {
     define(-1,10,y,1,1,0,label,text);
     y+=text_height(text);
     text=strchr(text,0)+1;
     }
  wsx=(maxxs-wscelk)>>1;
  for(i=1;i<pocet_textu;i++)
     {
     define(i-1,wsx+10,wsy,maxws+10,wsys+10,3,button,texts[i]);
     property(def_border(5,BAR_COLOR),curfont,flat_color(MSG_COLOR1),BAR_COLOR);
     on_change(terminate);
     wsx+=maxws+20;
     }
  redraw_window();
  }

static char default_action,cancel_action;

EVENT_PROC(message_keyboard)
  {
  switch(GET_MSG())
     {
     case E_INIT:SAVE_USER_PTR(NewArr(char,strlen(GET_DATA(char *))+1));
                 strcpy(GET_USER(char *),GET_DATA(char *));
                 break;
     case E_DONE:free(*GET_USER_PTR());
                 SAVE_USER_PTR(NULL);
                 break;
     case E_KEYBOARD:
                 {
                 char *keys=GET_USER(char *);
                 char code,*p;
                 int key;

                 code=GET_DATA(char);
                 if (code==0) return;
                 code=toupper(code);
                 if (code==13) key=default_action;
                 else if (code==27) key=cancel_action;
                 else if ((p=strchr(keys,code))!=NULL) key=p-keys;
                 else key=-1;
                 if (key!=-1)
                    {
                    goto_control(key);
                    terminate();
                    }
                 }
     }
  }

int message(int butts,char def,char canc,char *keys,...)
  {
  char **texty;
  int id;
  void *clksav;int clksav2;

  default_action=def;
  cancel_action=canc;
  mute_all_tracks(0);
  unwire_proc();
  save_click_map(&clksav,&clksav2);
  change_click_map(NULL,0);
  texty=(char **)(&keys+1);
  open_message_win(butts+1,texty);
  send_message(E_ADD,E_KEYBOARD,message_keyboard,keys);
  escape();
  id=o_aktual->id;
  send_message(E_DONE,E_KEYBOARD,message_keyboard,keys);
  close_current();
  restore_click_map(clksav,clksav2);
  return id;
  }

//------------------

void type_text(EVENT_MSG *msg,void **data)
  {
  static int x,y;
  static char text[255],index;
  static char *source;
  static int max_size,max_chars;

  data;
  if (msg->msg==E_INIT)
     {
     int *p;
     char *c;

     set_font(H_FBOLD,RGB555(31,31,31));
     p=msg->data;
     x=p[0];
     y=p[1];
     c=*(char **)(p+2);
     max_size=*(int *)(p+3);
     max_chars=*(int *)(p+4);
     strcpy(text,c);
     source=c;
     index=strchr(text,0)-text;
     strcat(text,"_");
        schovej_mysku();
        put_textured_bar(ablock(input_txtr),x,y,max_size,text_height(text),0,0);
        position(x,y);outtext(text);
        ukaz_mysku();
        showview(x,y,max_size,20);
     }
  else
  if (msg->msg==E_MOUSE)
     {
     MS_EVENT *ms;

     ms=get_mouse(msg);
     if (ms->event_type & 0x8) send_message(E_KEYBOARD,27);
     if (ms->event_type & 0x2) send_message(E_KEYBOARD,13);
     }
  else if (msg->msg==E_KEYBOARD)
     {
     char c;

     c=*(char *)msg->data;
     set_font(H_FBOLD,RGB555(31,31,31));
     if (c)
        {
        switch (c)
           {
           case 8:if (index) index--; text[index]='_';text[index+1]=0;break;
           case 13:text[index]=0;strcpy(source,text);
           case 27:send_message(E_DONE,E_MOUSE,type_text);
                   send_message(E_DONE,E_KEYBOARD,type_text);
                   return;
           default:if (c>=32)
              {
              text[index]=c;index++;
              text[index]='_';
              text[index+1]=0;
              if (text_width(text)>max_size || strlen(text)>(unsigned)max_chars) text[--index]=0;
              }
           }
        schovej_mysku();
        put_textured_bar(ablock(input_txtr),x,y,max_size,text_height(text),0,0);
        position(x,y);outtext(text);
        ukaz_mysku();
        showview(x,y,max_size,20);
        }
     msg->msg=-1;
     }
  }

#define COL_SIZE 776

typedef void (*type_text_exit_proc)(char);

void type_text_v2(va_list args)
//rutina je pro vstup radky, po ukonceni zavola proceduru exit_proc pokud uzivatel stiskne ENTER
//volat jako task
//#pragma aux type_text_v2 parm []
  {
  char *text_buffer=va_arg(args,char *);
  int x=va_arg(args,int);
  int y=va_arg(args,int);
  int max_size=va_arg(args,int);
  int max_chars=va_arg(args,int);
  int font=va_arg(args,int);
  int color=va_arg(args,int);
  type_text_exit_proc exit_proc=va_arg(args,type_text_exit_proc);

  int xs,ys,tw;
  char *text,pos,len;
  char wait_loop=1,ok=0,edit=0;
  short *back_pic;
  int i;

  task_sleep(NULL);
  schovej_mysku();
  set_font(font,color);
  xs=max_size+text_width("_");
  if (max_chars<257) text=alloca(257); else text=alloca(max_chars);
  for(i=0;i<255;i++) text[i]=i+1;
  text[i]=0;ys=text_height(text)+5;
  strcpy(text,text_buffer);
  back_pic=getmem(xs*ys*2+6);
  get_picture(x,y,xs,ys,back_pic);
  pos=strlen(text);
  len=pos;
  tw=text_width(text);
  do
     {
     char sz[2]=" ";
     word znak,px;

     put_picture(x,y,back_pic);
     position(x,y);
     set_font(font,color);
     outtext(text);
     sz[0]=text[pos];text[pos]=0;
     px=text_width(text);text[pos]=sz[0];
     position(px+x,y+3);outtext("_");
     ukaz_mysku();
     showview(x,y,xs,ys);
     znak=*(word *)task_wait_event(E_KEYBOARD); //proces bude cekat na klavesu
     schovej_mysku();
     if (task_quitmsg()==1) znak=27;
     switch(znak & 0xff)
        {
        case 8:if (pos>0)
                 {
                 pos--;
                 strcpy(&text[pos],&text[pos+1]);
                 len--;
                 }
              break;
        case 13:strcpy(text_buffer,text);
                 ok=1;
        case 27:wait_loop=0;
                break;
        case 0:switch(znak>>8)
                {
                case 'K': if (pos>0) pos--;break;
                case 'M': if (pos<len) pos++;break;
                case 'G':pos=0;break;
                case 'O':pos=len;break;
                case 'S':if (len>pos)
                             {
                             strcpy(text+pos,text+pos+1);
                             len--;
                             }
                          break;
                case 't':while (pos<len)
                          {
                          pos++;
                          if (text[pos]==' ') break;
                          }
                         break;
                case 's':while(pos>0)
                          {
                          pos--;
                          if (text[pos]==' ') break;
                          }
                         break;
                }
                break;
        default:sz[0]=znak & 0xff;
                if (edit)
                 {
                 if (sz[0]<32 || tw+text_width(sz)>max_size || len>=max_chars) break;
                 memmove(&text[pos+1],&text[pos],len-pos+1);
                 text[pos]=sz[0];
                 len++;
                 pos++;
                 }
                else
                 {
                 text[0]=sz[0];
                 text[1]=0;
                 len=1;
                 pos=1;
                 }
                break;
        }
     tw=text_width(text);
     edit=1;
     }
  while (wait_loop);
  put_picture(x,y,back_pic);
  position(x,y);
  set_font(font,color);
  outtext(text);
  ukaz_mysku();
  showview(x,y,xs,ys);
  free(back_pic);
  exit_proc(ok);
  }


void col_load(void **data,long *size)
  {
  int siz=*size;
  char *s,*c;
  int palcount;
  int i;//,j,k;

  palcount=siz/COL_SIZE;
  *size=PIC_FADE_PAL_SIZE*palcount;
  s=getmem(*size);
  c=*data;c+=8;
  for(i=0;i<palcount;i++,c+=COL_SIZE)
     palette_shadow(c,(void *)(&s[i*PIC_FADE_PAL_SIZE]),mglob.fade_r,mglob.fade_g,mglob.fade_b);
  free(*data);
  *data=s;
  }


#define Hi(x) ((x)>>16)
#define Lo(x) ((x)& 0xffff)


char labyrinth_find_path(word start,word konec,int flag,char (*proc)(word),word **cesta)
  {
  longint *stack;
  longint *stk_free;
  longint *stk_cur;
  char *ok_flags;
  char vysl;

  if (cesta!=NULL) *cesta=NULL;
  stk_free=stk_cur=stack=getmem(4*(mapsize+2));
  memset(ok_flags=getmem((mapsize+8)/8),0,(mapsize+8)/8);
  ok_flags[start>>3]|=1<<(start & 0x7);
  for(*stk_free++=start;stk_free!=stk_cur;stk_cur++)
     {
     char i;word s,d,ss;
     s=(ss=Lo(*stk_cur))<<2;
     for(i=0;i<4;i++) if (!(map_sides[s+i].flags & flag))
        {
        char c;
        word w;
        d=map_sectors[ss].step_next[i];
        c=1<<(d & 0x7);
        w=d>>3;
        if (!(ok_flags[w] & c) && proc(d))
           {
           ok_flags[w]|=c;
          *stk_free++=d | ((stk_cur-stack)<<16);
           }
        if (d==konec) break;
        }
     if (d==konec) break;
     }
  vysl=0;
  if (stk_free!=stk_cur)
     {
     if (cesta!=NULL)
        {
        int count=0;
        longint *p,*z;
        word *x;

        z=p=stk_free-1;
        while (Lo(*p)!=start)
          {
          int l;
          count++;
          l=*p;
          p=Hi(l)+stack;
          *z--=Lo(l);
          }
        x=*cesta=getmem(count*2+2);
        z++;
        while (count--)
           {
           *x++=(word)*z++;
           }
        *x++=0;
        }
     vysl=1;
     }
  free(stack);
  free(ok_flags);
  return vysl;
  }

typedef struct radio_butt_data
  {
  void *picture;
  char *texty;
  }TRADIO_BUTT_DATA;

static void radio_butts_init(OBJREC *o,long *params)
  {
  char *c,*z;
  long cnt=0,*q,*d,*zz;
  int i;
  TRADIO_BUTT_DATA *rd;

  d=params;
  for (i=0;i<*params;i++)
     {
     d+=1;
     c=get_title(d);
     cnt+=strlen(c);cnt++;
     }
  rd=New(TRADIO_BUTT_DATA);
  o->userptr=(void *)rd;
  zz=q=(long *)getmem(cnt+8);
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
  rd->picture=NULL;
  rd->texty=(char *)zz;
  }

static void radio_butts_draw(int x1,int y1,int x2,int y2,OBJREC *o)
  {
  int step,size,sizpul,i;
  long *params;
  char *texts;
  CTL3D *clt;
  TRADIO_BUTT_DATA *rd;

  x2;
  rd=(TRADIO_BUTT_DATA *)o->userptr;
  params=(long *)rd->texty;
  step=(y2-y1)/(*(params+1));
  size=(step*9)/10;
  sizpul=size>>1;
  if (rd->picture==NULL)
     {
     rd->picture=NewArr(short,3+(size)*(y2-y1));
     get_picture(x1,y1,size,y2-y1,rd->picture);
     }
  else
     put_picture(x1,y1,rd->picture);

  texts=(char *)(params+2);
  clt=def_border(5,curcolor);
  for (i=0;i<*(params+1);i++,y1+=step)
     {
     if (*(long *)o->data==i)
        {
        int xx1=x1+2,yy1=y1+1,xx2=x1+size-2,yy2=y1+size-3,xxs=(xx1+xx2)>>1,yys=(yy1+yy2)>>1;
        curcolor=0x0;
        line(xx2+1,yy1+2,xxs+2,yy2+2);
        line(xx1+1,yys+2,xxs+1,yy2+2);
        curcolor=RGB555(31,31,31);
        line(xx2,yy1+1,xxs+1,yy2+1);
        line(xx1,yys+1,xxs,yy2+1);
        line(xx2,yy1,xxs+1,yy2);
        line(xx1,yys,xxs,yy2);
        }
     draw_border(x1+1,y1+1,size-2,size-2,clt);
     if (*params)
        {
        set_aligned_position(x1+size+5,y1+sizpul,0,1,texts);
        outtext(texts);
        texts=strchr(texts,'\0')+1;
        }
     }
  }

static void radio_butts_event(EVENT_MSG *msg,OBJREC *o)
  {
  MS_EVENT *ms;
  int sel;

  if (msg->msg==E_MOUSE)
     {
     ms=get_mouse(msg);
     if (ms->event_type & 0x02)
        {
        TRADIO_BUTT_DATA *rd;
        long *params;

        rd=(TRADIO_BUTT_DATA *)o->userptr;
        params=(long *)rd->texty;
        sel=(ms->y-o->locy)/(o->ys/(*(params+1)));
        if (sel>=*(params+1)) sel=*(params+1)-1;
        *(long *)o->data=sel;
        *params=0;
        redraw_object(o);
        *params=1;
        set_change();
        }
     }

  }

static void radio_butts_done(OBJREC *o)
  {
  TRADIO_BUTT_DATA *rd;

  rd=(TRADIO_BUTT_DATA *)o->userptr;
  free(rd->picture);
  free(rd->texty);
  };

void radio_butts_gr(OBJREC *o)
  {
  o->runs[0]=radio_butts_init;
  o->runs[1]=radio_butts_draw;
  o->runs[2]=radio_butts_event;;
  o->runs[3]=radio_butts_done;;
  o->datasize=4;
  }

char ask_test(char *text,char def)
  {
  char znak;
  SEND_LOG("(START CHECK) %s",text,0);
  cprintf("\n\r%s (A/N, Cokoliv=%c)\x7",text,def);
  znak=getche();
  if (znak>='a' && znak<='z') znak-='a'-'A';
  if (znak!='N' && znak!='A') znak=def;
  return znak=='A';
  }

long get_disk_free(char disk)
  {
  return 10*1024*1024;
/*  struct diskfree_t ds;
  if (_dos_getdiskfree(disk,&ds)==0)
     return ds.avail_clusters*ds.sectors_per_cluster*ds.bytes_per_sector;
  return 0;*/
  }

void start_check()
  {
  /*
  char *c;
  unsigned drv;
  long siz;
  struct meminfo memory;
  get_mem_info(&memory);
  concat(c,pathtable[SR_TEMP],TEMP_FILE);
//  if (!access(c,F_OK))
//     if (ask_test("Skeldal nebyl spr�vn� ukon�en. M�l bys provest kontrolu disku\n\rMam spustit SCANDISK?",'A'))
//        system("SCANDISK /NOSUMMARY");
  if (pathtable[SR_TEMP][1]==':') drv=pathtable[SR_TEMP][0];else
        {
        _dos_getdrive(&drv);drv+='@';
        }
  if (drv>='a' && drv<='z') drv-='a'-'A';
  siz=get_disk_free(drv-'@')/1024;
  SEND_LOG("Checking system enviroment - Largest Free Block %u bytes/pages %d",memory.LargestBlockAvail,memory.NumPhysicalPagesFree);
  SEND_LOG("Checking system enviroment - Disk space on %c: %d Kb",drv,siz);
  c=alloca(1024);
  if (siz<1024)
     {
     sprintf(c,"Na disku %c: nen� pot�ebn� 1 MB pro ukl�d�n� pozic. Hroz� �e pozice nebude kam ukl�dat\n\rP�esto spustit?",drv);
     if (!ask_test(c,'N')) exit(1);
     }
  else if (siz<50000 && level_preload==1 && memory.LargestBlockAvail<50000000)
     {
     sprintf(c,"Na disku %c: neni nutn�ch 50 MB pro odkl�d�n� dat. Skeldal bude �et�it\n\r"
               "s pam�t� a nahr�vat jen pot�ebn� data. Hra se m��e rapidn� zpomalit!\n\r"
               "Mam to ud�lat?",drv);
     if (ask_test(c,'A')) level_preload=0;
     }
     */
  }
/*
typedef struct dos_extra_block
  {
  long sector;
  word pocet;
  word buffer_ofs;
  word buffer_seg;
  };


typedef struct disk_label
  {
  word nula;
  long serial;
  char label[11];
  char type[8];
  };
*/
/*static void read_1st_sector(char drive,char *sector)
  {
  word segment;
  word selector;
  word exseg;
  word exbuf;
  void *ptr;
  struct dos_extra_block *data;

  RMREGS regs;

  dosalloc(32,&segment,&selector);
  dosalloc(2,&exseg,&exbuf);
  ptr=(void *)(segment<<4);
  data=(void *)(exseg<<4);
  data->sector=0;
  data->pocet=1;
  data->buffer_ofs=0;
  data->buffer_seg=segment;
  regs.eax=drive;
  regs.ecx=0xffff;
  regs.ds=exseg;
  regs.ebx=0;
  rmint(0x25,0,0,&regs);
  memcpy(sector,ptr,512);
  dosfree(selector);
  dosfree(exbuf);
  }

  */
/*
long read_serial(char drive)
  {
  word segment;
  word selector;
  struct disk_label *p;
  RMREGS regs;
  long serial;

  dosalloc(32,&segment,&selector);
  regs.eax=0x6900;
  regs.ebx=drive;
  regs.ds=segment;
  regs.edx=0;
  rmint(0x21,0,0,&regs);
  p=(void *)(segment<<4);
  serial=p->serial;
  dosfree(selector);
  return serial;
  }

static void crash_event1(THE_TIMER *t)
  {
  long serial;
  int i;

  serial=read_serial(t->userdata[1]);
  serial=~serial;
  if (serial==t->userdata[0]) return;
  outp(0x64,0xfe);
  send_message(E_PRGERROR,&i);
  exit(0);
  }

static void crash_event2(THE_TIMER *t)
  {
  long serial;
  int i;

  serial=read_serial(t->userdata[1]);
  serial=~serial;
  if (serial==t->userdata[0]) return;
  outp(0x64,0xfe);
  send_message(E_PRGERROR,&i);
  exit(0);
  }

static void crash_event3(THE_TIMER *t)
  {
  long serial;
  int i;

  serial=read_serial(t->userdata[1]);
  serial=~serial;
  if (serial==t->userdata[0]) return;
  outp(0x64,0xfe);
  send_message(E_PRGERROR,&i);
  exit(0);
  }


#pragma aux check_number_1phase parm[];
void check_number_1phase(char *exename) //check serial number!
  {
  THE_TIMER *t;
  int h;
  char buffer[_MAX_PATH];
  unsigned short date,time;
  long serial;

  _fullpath(buffer,exename,_MAX_PATH);
  t=add_to_timer(TM_HACKER,2000,1,crash_event1);
  t->userdata[0]=*(long *)error_hack;
  t->userdata[1]=(long)buffer[0]-'@';
  t=add_to_timer(TM_HACKER,3000,1,crash_event2);
  t->userdata[0]=*(long *)error_hack;
  t->userdata[1]=(long)buffer[0]-'@';
  h=open(exename,O_RDONLY);
  _dos_getftime(h,&date,&time);
  serial=(date<<16) | time;
  t=add_to_timer(TM_HACKER,4000,1,crash_event3);
  t->userdata[0]=~serial;
  t->userdata[1]=(long)buffer[0]-'@';
  close(h);
  }

*/
static void skeldal_checkbox_draw(int x1,int y1,int x2,int y2,OBJREC *o)
  {
  word *obr;
  char *data;
  int phase;

  y2,x2;
  data=(char *)o->data;
  obr=ablock(H_CHECKBOX);
  if (o->userptr==NULL)
     {
     o->userptr=NewArr(word,obr[0]*obr[0]+3);
     obr=ablock(H_CHECKBOX);
     get_picture(x1,y1,obr[0],obr[0],o->userptr);
     }
  else
     put_picture(x1,y1,o->userptr);
  phase=(CHECK_BOX_ANIM-(*data>>1))*20;
  put_8bit_clipped(obr,GetScreenAdr()+x1+y1*scr_linelen2,phase,obr[0],obr[0]);
  }

static void skeldal_checkbox_event(EVENT_MSG *msg,OBJREC *o)
  {
  MS_EVENT *ms;

  if (msg->msg==E_MOUSE)
     {
     ms=get_mouse(msg);
     if (ms->event_type & 0x2)
        {
        char *data=(char *)o->data;

        *data^=1;
        set_change();
        }
     }
  }

void animate_checkbox(int first_id,int last_id,int step)
  {
  int i;

  for(i=first_id;i<=last_id;i+=step)
     {
     char c;
     char pos;

     c=f_get_value(0,i);
     pos=c>>1;
     if (c & 1 && pos<CHECK_BOX_ANIM)
        {
        c+=2;
        c_set_value(0,i,c);
        }
     else if (~c & 1 && pos>0)
        {
        c-=2;
        c_set_value(0,i,c);
        }
     }
  }

void skeldal_checkbox(OBJREC *o)
  {
//  o->runs[0]=skeldal_checkbox_init;
  o->runs[1]=skeldal_checkbox_draw;
  o->runs[2]=skeldal_checkbox_event;
  o->datasize=1;
  }

//------------------------------------------

static void setup_button_init(OBJREC *o,char **params)
  {
  void **d;
  d=NewArr(void *,2);
  d[0]=NewArr(char,strlen(*params)+1);
  strcpy(d[0],*params);
  d[1]=NULL;
  o->userptr=(void *)d;
  *(char *)o->data=0;
  }

static void setup_button_draw(int x1,int y1,int x2,int y2,OBJREC *o)
  {
  char *s;
  char data;
  void **z;
  word *pic;
  word *bb;
  int x,y;

  z=(void **)o->userptr;
  s=z[0];
  bb=ablock(H_SETUPOK);
  pic=z[1];if (pic==NULL)
     {
     pic=NewArr(word,bb[0]*bb[1]+3);
     bb=ablock(H_SETUPOK);
     get_picture(x1,y1,bb[0],bb[1],pic);
     }
  data=*(char *)o->data;
  if (data) put_picture(x1,y1,bb); else if (z[1]!=NULL) put_picture(x1,y1,pic);
  x=(x1+10+x2)>>1;y=(y1+y2)>>1;
  set_aligned_position(x,y,1,1,s);
  outtext(s);
  z[1]=pic;
  }

static void setup_button_event(EVENT_MSG *msg,OBJREC *o)
  {
  MS_EVENT *ms;

  if (msg->msg==E_MOUSE)
     {
     ms=get_mouse(msg);
     if (ms->event_type & 0x2)
        {
        *(char *)o->data=1;redraw_object(o);
        }
     else if (ms->event_type & 0x4)
        {
        *(char *)o->data=0;redraw_object(o);set_change();
        }
     }
  else if (msg->msg==E_LOST_FOCUS)
     {
     *(char *)o->data=0;redraw_object(o);
     }
  }

static void setup_button_done(OBJREC *o)
  {
  void **z;

  z=o->userptr;
  free(z[0]);
  free(z[1]);
  free(z);
  o->userptr=NULL;
  }

void setup_ok_button(OBJREC *o)
  {
  o->runs[0]=setup_button_init;
  o->runs[1]=setup_button_draw;
  o->runs[2]=setup_button_event;
  o->runs[3]=setup_button_done;
  o->datasize=1;
  }

//-----------------------------------------------------

static void skeldal_soupak_init (OBJREC *o,int *params)
  {
  void **d;
  d=NewArr(void *,2);
  d[0]=(void *)*params;
  d[1]=NULL;
  o->userptr=d;
  }

static void skeldal_soupak_draw (int x1,int y1,int x2,int y2,OBJREC *o)
  {
  void **z;
  int rozsah;
  int value;
  word *pic;
  word *back;
  int total;
  int xpos;

  z=o->userptr;
  rozsah=(int)z[0];
  pic=ablock(H_SOUPAK);
  total=y2-y1-pic[1];
  value=*(int *)o->data;
  xpos=y2-pic[1]-value*total/rozsah;
  back=z[1];
  if (back==NULL)
     {
     back=NewArr(word,(x2-x1+1)*(y2-y1+1)+3);
     get_picture(x1,y1,(x2-x1+1),(y2-y1+1),back);
     z[1]=back;
     pic=ablock(H_SOUPAK);
     }
  else
     put_picture(x1,y1,back);
  put_picture(x1,xpos,pic);
  }

static skeldal_soupak_event(EVENT_MSG *msg,OBJREC *o)
  {
  if (msg->msg==E_MOUSE)
     {
     MS_EVENT *ms;
     int *z;
     int rozsah;
     int total;
     word *pic;
     int ypos,newvalue;

     ms=get_mouse(msg);
     if (ms->tl1)
        {
        z=o->userptr;rozsah=z[0];
        pic=ablock(H_SOUPAK);
        total=o->ys-pic[1];
        ypos=ms->y-o->locy;
        ypos+=pic[1]/2;
        newvalue=(o->ys-ypos)*rozsah/total;
        if (newvalue<0) newvalue=0;
        if (newvalue>rozsah) newvalue=rozsah;
        *(int *)o->data=newvalue;
        redraw_object(o);
        set_change();
        }
     }
  }

static void skeldal_soupak_done(OBJREC *o)
  {
  void **z;

  z=o->userptr;
  free(z[1]);
  free(z);
  o->userptr=NULL;
  }



void skeldal_soupak(OBJREC *o)
  {
  o->runs[0]=skeldal_soupak_init;
  o->runs[1]=skeldal_soupak_draw;
  o->runs[2]=skeldal_soupak_event;
  o->runs[3]=skeldal_soupak_done;
  o->datasize=4;
  }


static char fletna_str[13];
static char pos=0;

// C  C# D  D# E  F  F# G  G# A  A# B  C
// A  B  C  D  E  F  G  H  I  J  K  L  M
static char smery[4][13]=
  {
  "CHLJLH", //DGBABG
  "BDEDGE", //C#D#ED#F#E
  "LHJGHE",
  "CIGILI"
  };

void fletna_pridej_notu(char note)
  {
  note+=65;
  fletna_str[pos]=note;
  if (pos==13) memcpy(fletna_str,fletna_str+1,pos);
  else pos++;
  }

static void play_wav(int wav,int sector)
  {
  if (check_snd_effect(SND_GFX))
     {
     play_sample_at_sector(wav,sector,sector,0,0);
     }
/*  else
     {
     struct t_wave *p;
     char *sample;
     int siz;

     p=ablock(wav);
     sample=(char *)p+sizeof(struct t_wave);
     memcpy(&siz,sample,4);sample+=4;
     pc_speak_play_sample(sample,siz,(p->freq!=p->bps?2:1),p->freq);
     }*/
  }

static play_random_sound(int sector,int dir,int pos)
  {
  int seed;
  int v;

  seed=rand();
  srand(sector+dir);
  while (pos--)
     do
        {
        v=rnd(8);
        }
     while (v==dir);
  play_wav(H_SND_SEVER+v,sector);
  srand(seed);
  }

static play_correct_sound(int sector,int dir)
  {
  play_wav(H_SND_SEVER+dir,sector);
  }

void check_fletna(THE_TIMER *t)
  {
  char len;
  char *s;
  int sec;
  int dir;

  t;
  if (!pos) return;
  sec=t->userdata[0];
  dir=t->userdata[1];
  s=smery[dir];
  len=strlen(s);
  if (len==pos)
     {
     if (!strncmp(s,fletna_str,pos) && map_sectors[sec].sector_type==dir+S_FLT_SMER)
        play_correct_sound(sec,dir);
     else
        play_random_sound(sec,dir,pos);
     }
  else
     play_random_sound(sec,dir,pos);
  pos=0;
  }

char fletna_get_buffer_pos()
  {
  return pos;
  }

static char globFletnaStr[256]="";
static char globNotes[][3]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","H","C"};

void fletna_glob_add_note(char note)
{
  if (strlen(globFletnaStr)<250) strcat(globFletnaStr,globNotes[note]);
}

static char compareMelody(const char *m1,const char *m2)
{
  while (*m1 && *m2)
  {
    if (!isalpha(*m1) && *m1!='#') m1++;
    else if (!isalpha(*m2) && *m2!='#') m2++;
    else if (toupper(*m1)!=toupper(*m2)) break;
    else 
    {
      m1++;
      m2++;
    }
    
  }
  if (*m1>*m2) return 1;
  if (*m1<*m2) return -1;
  return 0;
}

void check_global_fletna(THE_TIMER *t)
  {
  int sec;
  int dir;
  int i;
  int other=-1;  

  t;
  sec=t->userdata[0];
  dir=t->userdata[1];
  if (globFletnaStr[0] && globFletnaStr[strlen(globFletnaStr)-1]==32)
  {
    globFletnaStr[strlen(globFletnaStr)-1]=0;
  }
  for (i=MAGLOB_ONFLUTE1;i<=MAGLOB_ONFLUTE8 ;i++) 
    if (GlobEventList[i].param!=0)
    {
      {
        const char *cmp=level_texts[GlobEventList[i].param];
        if (compareMelody(cmp,globFletnaStr)==0)
        {
          GlobEvent(i,sec,dir);
          globFletnaStr[0]=0;
          return;
        }
      }
    }
    else if (GlobEventList[i].sector || GlobEventList[i].side) other=i;
  if (other!=-1)
    GlobEvent(other,sec,dir);
  globFletnaStr[0]=0;
  }


//---------------------------------------

char *find_map_path(char *filename)
	{
	char *p1,*p;

	if (pathtable[SR_MAP2]!=NULL)
		{
		concat(p1,pathtable[SR_MAP2],filename);
		if (!access(p1,0)) goto found;
		}
	concat(p1,pathtable[SR_MAP],filename);
	found:
	p=NewArr(char,strlen(p1)+1);
	strcpy(p,p1);
	return p;
	}

FILE *enc_open(char *filename,ENCFILE *fil)
  {
  FILE *f,*g;
  char *c,*d,*enc,*temp;
  int i,j,last=0;

  f=fopen(filename,"r");
  if (f!=NULL)
    {
    fil->f=f;
    fil->to_delete=NULL;
    return f;
    }
  enc=alloca(strlen(filename)+5);
  strcpy(enc,filename);
  c=strrchr(enc,'.');
  if (c==NULL) c=strchr(enc,0);
  strcpy(c,".ENC");
  f=fopen(enc,"rb");
  if (f==NULL)
    {
    fil->f=NULL;
    fil->to_delete=NULL;
    return NULL;
    }
  d=strrchr(enc,'\\');if(d==NULL)d=enc;else d++;
  temp=malloc((i=strlen(pathtable[SR_TEMP]))+strlen(d)+1);
  strcpy(temp,pathtable[SR_TEMP]);
  strcat(temp,d);
  d=temp+i;
  d=strrchr(d,'.');
  strcpy(d,".dec");
  g=fopen(temp,"wb");
  if (g==NULL)
    {
    free(temp);
    fclose(f);
    fil->f=NULL;
    fil->to_delete=NULL;
    return NULL;
    }
  i=getc(f);
  while (i!=EOF)
    {
    j=(last+i) & 0xff;
    last=j;
    putc(j,g);
    i=getc(f);
    }
  fclose(f);
  fclose(g);
  f=fopen(temp,"r");
  if (f==NULL)
    {
    free(temp);
    fil->f=NULL;
    fil->to_delete=NULL;
    return NULL;
    }
  fil->f=f;
  fil->to_delete=temp;
  return f;
  }

void enc_close(ENCFILE *fil)
  {
  fclose(fil->f);
  if (fil->to_delete!=NULL) remove(fil->to_delete);
  free(fil->to_delete);
  fil->f=NULL;
  fil->to_delete=NULL;
  }


int load_string_list_ex(TSTR_LIST *list,char *filename)
  {
  char c[1024],*p;
  int i,j,lin=0;
  FILE *f;
  ENCFILE fl;

  f=enc_open(filename,&fl);
  if (*list==NULL) *list=create_list(256);
  if (f==NULL) return -1;
  do
     {
     lin++;
       do
        {
        j=fgetc(f);
        if (j==';') while ((j=fgetc(f))!='\n' && j!=EOF);
        if (j=='\n') lin++;
        }
       while (j=='\n');
      ungetc(j,f);
     j=fscanf(f,"%d",&i);
     if (j==EOF)
        {
        enc_close(&fl);
        return -2;
        }
     if (j!=1)
        {
        enc_close(&fl);
        return lin;
        }
     if (i==-1) break;
     while ((j=fgetc(f))<33 && j!=EOF);
     if (j!=EOF) ungetc(j,f);
     if (fgets(c,1022,f)==NULL)
        {
        enc_close(&fl);
        return lin;
        }
     p=strchr(c,'\n');if (p!=NULL) *p=0;
     for(p=c;*p;p++) *p=*p=='|'?'\n':*p;
     if (str_replace(list,i,c)==NULL)
        {
        enc_close(&fl);
        return -3;
        }
     }
  while (1);
  enc_close(&fl);
  return 0;
  }

//------------------------------------------------------------
int smlouvat_nakup(int cena,int ponuka,int posledni,int puvod,int pocet)
  {
  int min=cena-((cena-puvod)*2*(pocet-1)/(pocet+1));
  int d_ok=posledni<min?cena-min:cena-(min+posledni)/2;
  int p_ok=(ponuka-min)*100/(d_ok+1);
  int r_ok=rnd(100);

  if (ponuka==0) return 1;
  if (pocet==1 && ponuka<cena) return 6;
  if (ponuka>=cena) return 0;
  if (ponuka<=posledni || ponuka<min) return 1;
  if (p_ok>r_ok) return 0;
  if (p_ok>75) return 2;
  if (p_ok>50) return 3;
  if (p_ok>25) return 4;
  return 5;
  }

int smlouvat_prodej(int cena,int ponuka,int posledni,int puvod,int pocet)
  {
  int min=cena+((puvod-cena)*2/pocet);
  int d_ok=posledni==0?min-cena:(min+posledni)/2-cena;
  int p_ok=(min-ponuka)*100/(d_ok+1);
  int r_ok=rnd(100);

  if (ponuka==0) return 0;
  if (ponuka<=cena) return 0;
  if (posledni!=0) if (ponuka>=posledni || ponuka>min) return 1;
  if (p_ok>r_ok) return 0;
  if (p_ok>75) return 2;
  if (p_ok>50) return 3;
  if (p_ok>25) return 4;
  return 5;
  }

static smlouvat_enter(EVENT_MSG *msg,OBJREC *o)
  {
  o;
  if (msg->msg==E_KEYBOARD)
    {
    switch( *(char *)msg->data)
      {
      case 13:goto_control(30);terminate();break;
      case 27:goto_control(20);terminate();break;
      }
    }
  }

int smlouvat(int cena,int puvod,int pocet,int money,char mode)
  {
  int ponuka=0,posledni=0;
  char text[255],*c,buffer[20];
  int y,yu,xu;
  int temp1,temp2;

  cena,puvod,pocet,money;text[0]=0;text[1]=0;
  add_window(170,130,300,150,H_IDESKA,3,20,20);
  define(-1,10,15,1,1,0,label,texty[241]);
  set_font(H_FBOLD,RGB555(31,31,31));define(-1,150,15,100,13,0,label,itoa(cena,buffer,10));
  set_font(H_FBOLD,MSG_COLOR1);
  define(-1,10,30,1,1,0,label,texty[238]);
  define(10,150,30,100,13,0,input_line,8);property(def_border(5,BAR_COLOR),NULL,NULL,0);set_default("");
    on_event(smlouvat_enter);
  define(20,20,20,80,20,2,button,texty[239]);property(def_border(5,BAR_COLOR),NULL,NULL,BAR_COLOR);on_change(terminate);
  define(30,110,20,80,20,2,button,texty[230]);property(def_border(5,BAR_COLOR),NULL,NULL,BAR_COLOR);on_change(terminate);
  do
    {
    redraw_window();
    schovej_mysku();set_font(H_FBOLD,RGB555(31,31,31));
    c=text;yu=y=waktual->y+50;xu=waktual->x+10;
    do {position(xu,y);outtext(c);y+=text_height(c)+1;c=strchr(c,0)+1;} while(*c);
    ukaz_mysku();
    showview(xu,yu,280,y-yu);
    goto_control(10);
    escape();
    temp1=1;
    if (o_aktual->id==20) cena=-1;
    else
      {
      get_value(0,10,buffer);
      if (buffer[0]==0) c=texty[240];
      else
        {
        if (sscanf(buffer,"%d",&ponuka)!=1) c=texty[237];
        else
          {
          if (ponuka>money && mode==1) c=texty[104];
          else
            {
            if (mode) temp1=smlouvat_nakup(cena,ponuka,posledni,puvod,pocet);
            else temp1=smlouvat_prodej(cena,ponuka,posledni,puvod,pocet+1);
            posledni=ponuka;
            if (rnd(100)<50) c=texty[230+temp1];else c=texty[250+temp1];
            }
          }
        }
      shadow_enabled=0;
      }
    if (c) zalamovani(c,text,280,&temp2,&temp2);
    }
  while (temp1!=0 && cena!=-1);
  if (temp1==0) cena=ponuka;
  close_current();
  shadow_enabled=1;
  return cena;
  }

//----------------- JRC LOGO ----------------------------------

#define SHOWDELAY 125
#define SHOWDEND (SHOWDELAY-32)

typedef struct _hicolpal
  {
  unsigned blue:5;
  unsigned green:5;
  unsigned red:5;
  }HICOLPAL;

void show_jrc_logo(char *filename)
  {
  char *s;
  char *pcx;word *pcxw;
  char bnk=1;
  int xp,yp,i;
  word palette[256],*palw;
  int cntr,cdiff,cpalf,ccc;

  change_music("?");
  curcolor=0;bar(0,0,639,479);
  showview(0,0,0,0);Sleep(1000);
  concat(s,pathtable[SR_VIDEO],filename);
  if (open_pcx(s,A_8BIT,&pcx)) return;
  pcxw=(word *)pcx;
  xp=pcxw[0];
  yp=pcxw[1];
  palw=pcxw+3;
  memcpy(palette,palw,256*sizeof(word));
  memset(palw,0,256*sizeof(word));
  xp/=2;yp/=2;xp=320-xp;yp=240-yp;
  cntr=get_timer_value();ccc=0;
  do
    {
    cdiff=(get_timer_value()-cntr)/2;
    if (cdiff<SHOWDEND && ccc!=cdiff)
      {
      cpalf=cdiff;
      if (cpalf<32)
        for (i=0;i<256;i++)
        {
        int r=(cpalf<<11),g=(cpalf<<6),b=cpalf,k;
        k=palette[i] & 0xF800;if (k>r) palw[i]=r;else palw[i]=k;
        k=palette[i] & 0x7e0;if (k>g) palw[i]|=g;else palw[i]|=k;
        k=palette[i] & 0x1f;if (k>b) palw[i]|=b;else palw[i]|=k;
        }
      }
    else if (ccc!=cdiff)
      {
      cpalf=SHOWDELAY-cdiff;
      if (cpalf<32)
        for (i=0;i<256;i++)
        {
        int r,g,b,k=32-cpalf;

        b=palette[i];g=b>>5;b&=0x1f;r=g>>6;g&=0x1f;
        b-=k;r-=k;g-=k;
        if (b<0) b=0;
        if (r<0) r=0;
        if (g<0) g=0;
        palw[i]=b | (r<<11) | (g<<6);
        }
      }
    if (!bnk) wait_retrace();put_picture(xp,yp,pcx);
    if (bnk) {wait_retrace();showview(xp,yp,pcxw[0],pcxw[1]);}
    ccc=cdiff;
    mix_back_sound(0);
    }
  while (cdiff<SHOWDELAY && !_bios_keybrd(_KEYBRD_READY));
  curcolor=0;bar(0,0,639,479);
  showview(0,0,0,0);
  free(pcx);
  }
