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
 *  Last commit made by: $Id: MGFPLAY.H 7 2008-01-14 20:14:25Z bredysoft $
 */
#define SMD_256 1
#define SMD_HICOLOR 2
#define SMD_256_FULL (1+128)
#define SMD_HICOLOR_FULL (2+128)
void play_animation(char *filename,char mode,int posy,char sound);
void set_title_list(char **titles);
void set_play_attribs(void *screen,char rdraw,char bm,char colr64);
