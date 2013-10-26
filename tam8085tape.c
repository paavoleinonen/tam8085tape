/*
 * tam8085tape.c
 * 
 * Generates "tape" audio for TAM8085 single board computer
 * 
 */
 
/*
 * Copyright (c) 2012-2013 Paavo Leinonen <paavo.leinonen@iki.fi>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PAGE 256

#define CYCLELEN 10
#define C_UNIT 8
#define C_SYNC ((C_UNIT)*250)

/*
Toneburst is generated with following python-program:

import math
for i in range(0,10):
	print int(127*(math.sin(2*math.pi/10*i)+math.sin(2*math.pi/10*i*2)*0.1)/1.1)
*/

const signed char cycle_tone[CYCLELEN]={0,78,116,103,56,0,-56,-103,-116,-78};
const signed char cycle_silence[CYCLELEN]={0,0,0,0,0,0,0,0,0,0};

int tone(int fd, int cycles, const signed char *pattern)
{
	while (cycles--)
	{
		// This is inefficient
		if (write(fd,pattern,CYCLELEN)!=CYCLELEN)
			return -1;
	}
	return 0;
}

int modulate(int fdin, int fdout)
{
	unsigned char buf[PAGE];
	unsigned char *ptr=buf;
	int bitcount;
	ssize_t count;
	
	count=read(fdin,buf,sizeof(buf));
	if (count==0)
		return 0;	// No more pages

	if (count<sizeof(buf))
		memset(buf+count,0x00,sizeof(buf)-count);

	// Sync header
	if (tone(fdout,C_SYNC,cycle_tone))
		goto error;
	if (tone(fdout,C_UNIT,cycle_silence))
		goto error;

	// Bits
	for (count=0;count<256;count++)
	{
		// The 9th bit is a stopbit of value 0.
		for (bitcount=0;bitcount<9;bitcount++)
		{
			if (*ptr&0x01)
			{
				if (tone(fdout,C_UNIT*2,cycle_tone))
					goto error;
				if (tone(fdout,C_UNIT,cycle_silence))
					goto error;
			}
			else
			{
				if (tone(fdout,C_UNIT,cycle_tone))
					goto error;
				if (tone(fdout,C_UNIT*2,cycle_silence))
					goto error;
			}
			*ptr>>=1;
		}

		ptr++;
	}

	return 1;	// More pages might follow

error:
	perror("Unable to write output");
	return -1;
}

void usage(void)
{
	puts(
"tam8085tape <input> <output>\n\n"
"Converts input file to TAM8085-tape format\n"
"Input is a raw byte stream.\n"
"Output is signed 8 bit samples with 32000 sps.\n"
"Output consists of 256 byte pages. Input is padded to full page with zeros.\n"
	);
}

int main(int argc, char **argv)
{
	int infd=-1, outfd=-1;
	int retval;

	if (argc<3)
	{
		usage();
		return 0;
	}


	if (strcmp(argv[1],"-")==0)
	{
		infd=0;
	}
	else
	{
		infd=open(argv[1],O_RDONLY);
	}

	if (infd<0)
	{
		perror("Error opening input file");
		return -1;
	}


	if (strcmp(argv[2],"-")==0)
	{
		outfd=1;
	}
	else
	{
		outfd=open(argv[2],O_WRONLY|O_CREAT|O_EXCL,0777);
	}

	if (outfd<0)
	{
		perror("Error opening output file");
		return -1;
	}


	while ((retval=modulate(infd,outfd)))
		;


	close(outfd);
	close(infd);

	return retval;
}
