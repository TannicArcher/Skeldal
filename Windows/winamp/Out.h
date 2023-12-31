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
 *  Last commit made by: $Id: Out.h 7 2008-01-14 20:14:25Z bredysoft $
 */

// ids:
// waveout: 32
// gapless: 64
// xfade: 63
// disk: 33
// dsound: 38
// NULL: 65
// mm2: 69

#define OUT_VER 0x10

typedef struct 
{
	int version;				// module version (OUT_VER)
	char *description;			// description of module, with version string
	int id;						// module id. each input module gets its own. non-nullsoft modules should
								// be >= 65536. 

	HWND hMainWindow;			// winamp's main window (filled in by winamp)
	HINSTANCE hDllInstance;		// DLL instance handle (filled in by winamp)

	void (*Config)(HWND hwndParent); // configuration dialog 
	void (*About)(HWND hwndParent);  // about dialog

	void (*Init)();				// called when loaded
	void (*Quit)();				// called when unloaded

	int (*Open)(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms); 
					// returns >=0 on success, <0 on failure
					// NOTENOTENOTE: bufferlenms and prebufferms are ignored in most if not all output plug-ins. 
					//    ... so don't expect the max latency returned to be what you asked for.
					// returns max latency in ms (0 for diskwriters, etc)
					// bufferlenms and prebufferms must be in ms. 0 to use defaults. 
					// prebufferms must be <= bufferlenms

	void (*Close)();	// close the ol' output device.

	int (*Write)(char *buf, int len);	
					// 0 on success. Len == bytes to write (<= 8192 always). buf is straight audio data. 
					// 1 returns not able to write (yet). Non-blocking, always.

	int (*CanWrite)();	// returns number of bytes possible to write at a given time. 
						// Never will decrease unless you call Write (or Close, heh)

	int (*IsPlaying)(); // non0 if output is still going or if data in buffers waiting to be
						// written (i.e. closing while IsPlaying() returns 1 would truncate the song

	int (*Pause)(int pause); // returns previous pause state

	void (*SetVolume)(int volume); // volume is 0-255
	void (*SetPan)(int pan); // pan is -128 to 128

	void (*Flush)(int t);	// flushes buffers and restarts output at time t (in ms) 
							// (used for seeking)

	int (*GetOutputTime)(); // returns played time in MS
	int (*GetWrittenTime)(); // returns time written in MS (used for synching up vis stuff)

} Out_Module;


