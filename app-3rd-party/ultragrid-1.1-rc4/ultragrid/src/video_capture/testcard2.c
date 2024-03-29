/*
 * FILE:   testcard2.c
 * AUTHOR: Colin Perkins <csp@csperkins.org
 *         Alvaro Saurin <saurin@dcs.gla.ac.uk>
 *         Martin Benes     <martinbenesh@gmail.com>
 *         Lukas Hejtmanek  <xhejtman@ics.muni.cz>
 *         Petr Holub       <hopet@ics.muni.cz>
 *         Milos Liska      <xliska@fi.muni.cz>
 *         Jiri Matela      <matela@ics.muni.cz>
 *         Dalibor Matura   <255899@mail.muni.cz>
 *         Ian Wesley-Smith <iwsmith@cct.lsu.edu>
 *
 * Copyright (c) 2005-2006 University of Glasgow
 * Copyright (c) 2005-2010 CESNET z.s.p.o.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, is permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 * 
 *      This product includes software developed by the University of Southern
 *      California Information Sciences Institute. This product also includes
 *      software developed by CESNET z.s.p.o.
 * 
 * 4. Neither the name of the University, Institute, CESNET nor the names of
 *    its contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "config.h"
#include "config_unix.h"
#include "config_win32.h"
#include "host.h"
 
#ifdef HAVE_SDL

#include "debug.h"
#include "tv.h"
#include "video_codec.h"
#include "video_capture.h"
#include "video_capture/testcard2.h"
#include "compat/platform_semaphore.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#ifdef HAVE_LIBSDL_TTF
#include <SDL/SDL_ttf.h>
#endif
#include "audio/audio.h"
#include <pthread.h>
#include <time.h>
#include <limits.h>

#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_BPS 2
#define AUDIO_CHANNELS 2
#define BUFFER_SEC 1
#define AUDIO_BUFFER_SIZE (AUDIO_SAMPLE_RATE * AUDIO_BPS * \
                AUDIO_CHANNELS * BUFFER_SEC)

void * vidcap_testcard2_thread(void *args);
void rgb2yuv422(unsigned char *in, unsigned int width, unsigned int height);
unsigned char *tov210(unsigned char *in, unsigned int width, unsigned int align_x,
                      unsigned int height, double bpp);
void toR10k(unsigned char *in, unsigned int width, unsigned int height);

struct testcard_state2 {
        int count;
        int size;
        int pan;
        SDL_Surface *surface;
        char *data;
        struct timeval t0;
        struct video_frame *frame;
        struct tile *tile;
        struct audio_frame audio;
        int aligned_x;
        struct timeval start_time;
        int play_audio_frame;
        
        double audio_remained,
                seconds_tone_played;
        struct timeval last_audio_time;
        char *audio_tone, *audio_silence;
        unsigned int grab_audio:1;
        sem_t semaphore;
        
        pthread_t thread_id;
};

extern const int rect_colors[];

#define COL_NUM 6

static int configure_audio(struct testcard_state2 *s)
{
        int i;
        
        s->audio_silence = calloc(1, AUDIO_BUFFER_SIZE /* 1 sec */);
        
        s->audio_tone = calloc(1, AUDIO_BUFFER_SIZE /* 1 sec */);
        short int * data = (short int *) s->audio_tone;
        for( i=0; i < AUDIO_BUFFER_SIZE/2; i+=2 )
        {
                data[i] = data[i+1] = (float) sin( ((double)i/(double)200) * M_PI * 2. ) * SHRT_MAX;
        }

        
        s->audio.bps = AUDIO_BPS;
        s->audio.ch_count = AUDIO_CHANNELS;
        s->audio.sample_rate = AUDIO_SAMPLE_RATE;
        
        printf("[testcard2] playing audio\n");
        
        return 0;
}

void *vidcap_testcard2_init(char *fmt, unsigned int flags)
{
        struct testcard_state2 *s;
        const char *strip_fmt = NULL;
        unsigned int i, j;
        unsigned int rect_size = COL_NUM;
        codec_t codec=0;

        if (fmt == NULL || strcmp(fmt, "help") == 0) {
                printf("testcard2 options:\n");
                printf("\t-t testcard2:<width>:<height>:<fps>:<codec>\n");
                show_codec_help("testcard");
                return NULL;
        }

        s = calloc(1, sizeof(struct testcard_state2));
        if (!s)
                return NULL;

        char *tmp;

        s->frame = vf_alloc(1);
        s->tile = vf_get_tile(s->frame, 0);
        
        tmp = strtok(fmt, ":");
        if (!tmp) {
                fprintf(stderr, "Wrong format for testcard '%s'\n", fmt);
                free(s);
                return NULL;
        }
        s->tile->width = atoi(tmp);
        if(s->tile->width % 2 != 0) {
                fprintf(stderr, "Width must be multiple of 2.\n");
                free(s);
                return NULL;
        }
        tmp = strtok(NULL, ":");
        if (!tmp) {
                fprintf(stderr, "Wrong format for testcard '%s'\n", fmt);
                free(s);
                return NULL;
        }
        s->tile->height = atoi(tmp);
        tmp = strtok(NULL, ":");
        if (!tmp) {
                free(s);
                fprintf(stderr, "Wrong format for testcard '%s'\n", fmt);
                return NULL;
        }

        s->frame->fps = atof(tmp);

        tmp = strtok(NULL, ":");
        if (!tmp) {
                free(s);
                fprintf(stderr, "Wrong format for testcard '%s'\n", fmt);
                return NULL;
        }

        int h_align = 0;
        double bpp = 0;

        for (i = 0; codec_info[i].name != NULL; i++) {
                if (strcmp(tmp, codec_info[i].name) == 0) {
                        h_align = codec_info[i].h_align;
                        bpp = codec_info[i].bpp;
                        codec = codec_info[i].codec;
                        break;
                }
        }

        s->frame->color_spec = codec;

        if(bpp == 0) {
                fprintf(stderr, "Unknown codec '%s'\n", tmp);
                return NULL;
        }

        s->aligned_x = s->tile->width;
        if (h_align) {
                s->aligned_x = (s->aligned_x + h_align - 1) / h_align * h_align;
        }

        rect_size = (s->tile->width + rect_size - 1) / rect_size;

        s->frame->interlacing = PROGRESSIVE;
        s->size = s->aligned_x * s->tile->height * bpp;

        {
                SDL_Rect r;
                int col_num = 0;
                s->surface =
                    SDL_CreateRGBSurface(SDL_SWSURFACE, s->aligned_x, s->tile->height * 2,
                                         32, 0xff, 0xff00, 0xff0000,
                                         0xff000000);
                for (j = 0; j < s->tile->height; j += rect_size) {
                        if (j == rect_size * 2) {
                                r.w = s->tile->width;
                                r.h = rect_size / 4;
                                r.x = 0;
                                r.y = j;
                                SDL_FillRect(s->surface, &r, 0xffffffff);
                                r.y = j + rect_size * 3 / 4;
                                SDL_FillRect(s->surface, &r, 0);
                        }
                        for (i = 0; i < s->tile->width; i += rect_size) {
                                r.w = rect_size;
                                r.h = rect_size;
                                r.x = i;
                                r.y = j;
                                printf("Fill rect at %d,%d\n", r.x, r.y);
                                //if (j != rect_size * 2) {
                                        SDL_FillRect(s->surface, &r,
                                                     rect_colors[col_num]);
                                        col_num = (col_num + 1) % COL_NUM;
                                /*} else {
                                        r.h = rect_size / 2;
                                        r.y += rect_size / 4;
                                        SDL_FillRect(s->surface, &r, grey);
                                        grey += 0x00010101 * (255 / COL_NUM);
                                }*/
                        }
                }
        }

        tmp = strtok(NULL, ":");
        if (tmp) {
                if (tmp[0] == 'p') {
                        s->pan = 48;
                } else if (tmp[0] == 's') {
                        strip_fmt = tmp;
                }
        }
        
        if(flags & VIDCAP_FLAG_AUDIO_EMBEDDED) {
                s->grab_audio = TRUE;
                if(configure_audio(s) != 0) {
                        s->grab_audio = FALSE;
                        fprintf(stderr, "[testcard2] Disabling audio output. "
                                        "\n");
                }
        } else {
                s->grab_audio = FALSE;
        }

        s->count = 0;
        s->audio_remained = 0.0;
        s->seconds_tone_played = 0.0;
        s->play_audio_frame = FALSE;

        platform_sem_init(&s->semaphore, 0, 0);
        printf("Testcard capture set to %dx%d, bpp %f\n", s->tile->width, s->tile->height, bpp);

        s->tile->data_len = s->size;

        if(flags & VIDCAP_FLAG_AUDIO_EMBEDDED) {
                s->grab_audio = TRUE;
                if(configure_audio(s) != 0) {
                        s->grab_audio = FALSE;
                        fprintf(stderr, "[testcard] Disabling audio output. "
                                        "SDL-mixer missing, running on Mac or other problem.");
                }
        } else {
                s->grab_audio = FALSE;
        }
        
        if(!s->grab_audio) {
                s->audio_tone = NULL;
                s->audio_silence = NULL;
        }
        
        gettimeofday(&s->start_time, NULL);
        
        pthread_create(&s->thread_id, NULL, vidcap_testcard2_thread, s);

        return s;
}

void vidcap_testcard2_finish(void *state)
{
        UNUSED(state);
}

void vidcap_testcard2_done(void *state)
{
        struct testcard_state2 *s = state;

        free(s->data);
        
        free(s->audio_tone);
        free(s->audio_silence);
        free(s);
}

void * vidcap_testcard2_thread(void *arg)
{
        struct testcard_state2 *s;
        s = (struct testcard_state2 *)arg;
        struct timeval curr_time;
        struct timeval next_frame_time;
        SDL_Surface *copy;
        SDL_Surface *old = NULL;
        unsigned int seed = time(NULL);
        int prev_x1 = rand_r(&seed) % (s->tile->width - 300);
        int prev_y1 = rand_r(&seed) % (s->tile->height - 300);
        int down1 = rand_r(&seed) % 2, right1 = rand_r(&seed) % 2;
        int prev_x2 = rand_r(&seed) % (s->tile->width - 100);
        int prev_y2 = rand_r(&seed) % (s->tile->height - 100);
        int down2 = rand_r(&seed) % 2, right2 = rand_r(&seed) % 2;
        
        int stat_count_prev = 0;
        
        gettimeofday(&s->last_audio_time, NULL);
        
#ifdef HAVE_LIBSDL_TTF
        SDL_Surface *text;
        SDL_Color col = { 0, 0, 0, 0 };
        TTF_Font * font;
        
        if(TTF_Init() == -1)
        {
          fprintf(stderr, "Unable to initialize SDL_ttf: %s\n",
            TTF_GetError());
          exit(128);
        }
        
        font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf", 108);
        if(!font) {
                font = TTF_OpenFont("/usr/share/fonts/truetype/DejaVuSansMono.ttf", 108);
        }
        if(!font) {
                fprintf(stderr, "Unable to load any usable font (last font tryied: %s)!\n", TTF_GetError());
                exit(128);
        }

#endif
        
        while(!should_exit)
        {
                SDL_Rect r;
                copy = SDL_ConvertSurface(s->surface, s->surface->format, SDL_SWSURFACE);
                
                r.w = 300;
                r.h = 300;
                r.x = prev_x1 + (right1 ? 1 : -1) * 4;
                r.y = prev_y1 + (down1 ? 1 : -1) * 4;
                if(r.x < 0) right1 = 1;
                if(r.y < 0) down1 = 1;
                if((unsigned int) r.x + r.w > s->tile->width) right1 = 0;
                if((unsigned int) r.y + r.h > s->tile->height) down1 = 0;
                prev_x1 = r.x;
                prev_y1 = r.y;
                
                SDL_FillRect(copy, &r, 0x00000000);
                
                r.w = 100;
                r.h = 100;
                r.x = prev_x2 + (right2 ? 1 : -1) * 12;
                r.y = prev_y2 + (down2 ? 1 : -1) * 9;
                if(r.x < 0) right2 = 1;
                if(r.y < 0) down2 = 1;
                if((unsigned int) r.x + r.w > s->tile->width) right2 = 0;
                if((unsigned int) r.y + r.h > s->tile->height) down2 = 0;
                prev_x2 = r.x;
                prev_y2 = r.y;
                
                SDL_FillRect(copy, &r, 0xffff00aa);
                
                r.w = s->tile->width;
                r.h = 150;
                r.x = 0;
                r.y = s->tile->height - r.h - 30;
                SDL_FillRect(copy, &r, 0xffffffff);
                
#ifdef HAVE_LIBSDL_TTF                
                char frames[20];
                double since_start = tv_diff(next_frame_time, s->start_time);
                snprintf(frames, 20, "%02d:%02d:%02d %3d", (int) since_start / 3600 ,
                                (int) since_start / 60 % 60,
                                (int) since_start % 60,
                                 s->count % (int)s->frame->fps);
                text = TTF_RenderText_Solid(font,
                        frames, col);
#endif
                SDL_Rect src_rect;
                src_rect.x=0;
                src_rect.y=0;
                
#ifdef HAVE_LIBSDL_TTF
                r.y += (r.h - text->h) / 2;
                r.x = (s->tile->width - src_rect.w) / 2;
                src_rect.w=text->w;
                src_rect.h=text->h;
                SDL_BlitSurface(text,  &src_rect,  copy, &r);
                SDL_FreeSurface(text);
#endif
                            
                if (s->frame->color_spec == UYVY || s->frame->color_spec == v210 || s->frame->color_spec == Vuy2) {
                        rgb2yuv422((unsigned char *) copy->pixels, s->aligned_x,
                                   s->tile->height);
                }

                if (s->frame->color_spec == v210) {
                        copy->pixels =
                            (char *)tov210((unsigned char *) copy->pixels, s->aligned_x,
                                           s->aligned_x, s->tile->height, get_bpp(s->frame->color_spec));
                }

                if (s->frame->color_spec == R10k) {
                        toR10k((unsigned char *) copy->pixels, s->tile->width, s->tile->height);
                }
                
                s->tile->data = copy->pixels;

                SDL_FreeSurface(old);
                old = copy;
                
                int since_start_usec;
next_frame:
                since_start_usec = (s->count) * (1000000 / s->frame->fps);
                next_frame_time = s->start_time;
                tv_add_usec(&next_frame_time, since_start_usec);
                
                gettimeofday(&curr_time, NULL);
                
                if(tv_gt(next_frame_time, curr_time)) {
                        int sleep_time = tv_diff_usec(next_frame_time, curr_time);
                        usleep(sleep_time);
                } else {
                        if((++s->count) % ((int) s->frame->fps * 5) == 0) {
                                s->play_audio_frame = TRUE;
                        }
                        ++stat_count_prev;
                        goto next_frame;
                }
                
                if((++s->count) % ((int) s->frame->fps * 5) == 0) {
                        s->play_audio_frame = TRUE;
                }
                platform_sem_post(&s->semaphore);
                
                
                double seconds = tv_diff(curr_time, s->t0);
                if (seconds >= 5) {
                        float fps = (s->count - stat_count_prev) / seconds;
                        fprintf(stderr, "[testcard2] %d frames in %g seconds = %g FPS\n",
                                (s->count - stat_count_prev), seconds, fps);
                        s->t0 = curr_time;
                        stat_count_prev = s->count;
                }
        }

        return NULL;
}

static void grab_audio(struct testcard_state2 *s)
{
        struct timeval curr_time;
        gettimeofday(&curr_time, NULL);
        
        double seconds = tv_diff(curr_time, s->last_audio_time);
        if(s->play_audio_frame) {
                s->seconds_tone_played = 0.0;
                s->play_audio_frame = FALSE;
        }
        
        s->audio.data_len = ((int)((seconds + s->audio_remained) * AUDIO_SAMPLE_RATE));
        if(s->seconds_tone_played < 1.0) {
                s->audio.data = s->audio_tone;
                s->audio.data_len = s->audio.data_len / 400 * 400;
        } else {
                s->audio.data = s->audio_silence;
        }
        
        s->seconds_tone_played += (double) s->audio.data_len / AUDIO_SAMPLE_RATE;
        
        s->audio_remained = (seconds + s->audio_remained) * AUDIO_SAMPLE_RATE - s->audio.data_len;
        s->audio_remained /= AUDIO_SAMPLE_RATE;
        s->audio.data_len *= AUDIO_CHANNELS * AUDIO_BPS;
        
        s->last_audio_time = curr_time;
}

struct video_frame *vidcap_testcard2_grab(void *arg, struct audio_frame **audio)
{
        struct testcard_state2 *s;

        s = (struct testcard_state2 *)arg;

        platform_sem_wait(&s->semaphore);
        
        *audio = NULL;
        if(s->grab_audio){
                grab_audio(s);
                if(s->audio.data_len)
                        *audio = &s->audio;
                else
                        *audio = NULL;
         }
        
        return s->frame;
}

struct vidcap_type *vidcap_testcard2_probe(void)
{
        struct vidcap_type *vt;

        vt = (struct vidcap_type *)malloc(sizeof(struct vidcap_type));
        if (vt != NULL) {
                vt->id = VIDCAP_TESTCARD2_ID;
                vt->name = "testcard2";
                vt->description = "Video testcard 2";
        }
        return vt;
}

#endif
