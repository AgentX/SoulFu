// <ZZ> This file contains a bunch of common functions
//  **  make_uppercase          - Changes a string to be all uppercase letters
//  **  free_mainbuffer         - Frees up the memory used by the general purpose buffer
//  **  get_mainbuffer          - Allocates memory for a general purpose buffer
//  **  count_indentation       - Returns the number of spaces at the start of a string
//  **  datadump                - Spits out some data into a file on disk (for debuggin')

#include "soulfu.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAINBUFFERSIZE (32*MEG)         // 16 Meg for temporary buffers, 4 Meg for map, 12 Meg for room...
unsigned char* mainbuffer = NULL;       // A General purpose buffer
unsigned char* subbuffer = NULL;        // A subset of the mainbuffer
unsigned char* thirdbuffer = NULL;      // A subset of the mainbuffer
unsigned char* fourthbuffer = NULL;     // A subset of the mainbuffer
unsigned char* mapbuffer = NULL;        // A subset of the mainbuffer
unsigned char* roombuffer = NULL;       // A subset of the mainbuffer

unsigned int timer_start_time = 0;
unsigned int timer_end_time;
unsigned int timer_end_length;
void timer_start()
{
    // <ZZ> This function is for figuring out how slow things are...
    timer_start_time = SDL_GetTicks();
}

//-----------------------------------------------------------------------------------------------
void timer_end()
{
    // <ZZ> This function is for figuring out how slow things are...
    timer_end_time = SDL_GetTicks();
    timer_end_length = timer_end_time-timer_start_time;
#ifdef DEVTOOL
    log_message("INFO:   Timed function took %d msecs to run", timer_end_length);
#endif
}

//-----------------------------------------------------------------------------------------------
unsigned int main_timer_length = 0;
unsigned int main_timer_start_time;
void main_timer_start()
{
    // <ZZ> This function is for figuring out how many frames to update...
    main_timer_start_time = SDL_GetTicks();
}

//-----------------------------------------------------------------------------------------------
unsigned int main_timer_end_time;
float main_timer_fps = 0;
void main_timer_end()
{
    // <ZZ> This function is for figuring out how slow things are...
    main_timer_end_time = SDL_GetTicks();
    timer_end_length = (main_timer_end_time-main_timer_start_time);
    main_timer_length += timer_end_length;


    // Do FPS calculations
    if(timer_end_length > 0)
    {
        main_timer_fps = (((1000.0f/timer_end_length) + 0.5f) * 0.05f) + (main_timer_fps * 0.95f);
    }
}

//-----------------------------------------------------------------------------------------------
void make_uppercase(char *string)
{
    // <ZZ> This function changes all lowercase letters in string to be uppercase.
    int i;

    i = 0;
    while(string[i] != 0)
    {
        if(string[i] >= 'a' && string[i] <= 'z')  string[i] += 'A'-'a';
        i++;
    }
}

//-----------------------------------------------------------------------------------------------
void free_mainbuffer(void)
{
    // <ZZ> This function frees up the memory allocated by get_mainbuffer(), and should be
    //      called automatically atexit().
    if(mainbuffer)
    {
        free(mainbuffer);
    }
}

//-----------------------------------------------------------------------------------------------
signed char get_mainbuffer(void)
{
    // <ZZ> This function reserves memory for a general purporse buffer used throughout the
    //      program.  If it fails, it returns FALSE.  If it works, it returns TRUE and registers
    //      free_mainbuffer() to be called atexit().

    mainbuffer = malloc(MAINBUFFERSIZE);
    if(mainbuffer)
    {
        log_message("INFO:   Allocated %d bytes (%d Meg) for the mainbuffer", MAINBUFFERSIZE, (MAINBUFFERSIZE/MEG));
        subbuffer = mainbuffer + (4*MEG);
        thirdbuffer = subbuffer + (4*MEG);
        fourthbuffer = thirdbuffer + (4*MEG);
        mapbuffer = fourthbuffer + (4*MEG);
        roombuffer = mapbuffer + (4*MEG);
        atexit(free_mainbuffer);
        return TRUE;
    }
    log_message("ERROR:  Could not allocate memory for the mainbuffer");
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
int count_indentation(char *string)
{
    // <ZZ> This function returns the number of spaces at the start of string
    int count;

    count = 0;
    while(string[count] == ' ')
    {
        count++;
    }
    return count;
}

//-----------------------------------------------------------------------------------------------
void datadump(unsigned char* location, int size, signed char append)
{
    // <ZZ> This function copies the data at location to a file on disk named DATADUMP.DAT.
    //      Size is the number of bytes to write...  If append is TRUE, data is appended,
    //      otherwise the file is overwritten...
    FILE* openfile;

    if(append) openfile = fopen("DATADUMP.DAT", "ab");
    else openfile = fopen("DATADUMP.DAT", "wb");
    if(openfile)
    {
        fwrite(location, 1, size, openfile);
        fclose(openfile);
        log_message("INFO:   Wrote %d bytes to DATADUMP.DAT from location %d", size, location);
    }
    else
    {
        log_message("ERROR:  Couldn't open DATADUMP.DAT");
    }
}

void cross_product(float* A_xyz, float* B_xyz, float* C_xyz)
{
    // <ZZ> This function crosses two vectors, A and B, and gives the result in C.  C should be
    //      a different location than either A or B...
    C_xyz[0] = (A_xyz[1]*B_xyz[2]) - (A_xyz[2]*B_xyz[1]);
    C_xyz[1] = (A_xyz[2]*B_xyz[0]) - (A_xyz[0]*B_xyz[2]);
    C_xyz[2] = (A_xyz[0]*B_xyz[1]) - (A_xyz[1]*B_xyz[0]);
// !!!BAD!!!
// !!!BAD!!!  Should macroize...
// !!!BAD!!!
}

//-----------------------------------------------------------------------------------------------
#define dot_product(A, B) (A[0]*B[0] + A[1]*B[1] + A[2]*B[2])

//-----------------------------------------------------------------------------------------------
float vector_length(float* A_xyz)
{
    // <ZZ> This function returns the length of a vector.
    return ((float) sqrt(dot_product(A_xyz, A_xyz)));
// !!!BAD!!!
// !!!BAD!!!  Should macroize...
// !!!BAD!!!
}

//-----------------------------------------------------------------------------------------------
// This macro swaps the bytes in an unsigned int...  Converts between endian formats...
#define swap_32_bit_endian(A) { A = (A<<24) | ((A&0xFF00)<<8) | ((A>>8)&0xFF00) | (A>>24); }


// This macro does the same thing, but with a [4] array of unsigned chars...
unsigned char global_endian_swap;
#define swap_32_bit_endian_array(A) { global_endian_swap = A[0];  A[0] = A[3];  A[3] = global_endian_swap;  global_endian_swap = A[1];  A[1] = A[2];  A[2] = global_endian_swap; }


//-----------------------------------------------------------------------------------------------
