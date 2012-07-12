/*
 * FILE:    video_decompress.c
 * AUTHORS: Martin Benes     <martinbenesh@gmail.com>
 *          Lukas Hejtmanek  <xhejtman@ics.muni.cz>
 *          Petr Holub       <hopet@ics.muni.cz>
 *          Milos Liska      <xliska@fi.muni.cz>
 *          Jiri Matela      <matela@ics.muni.cz>
 *          Dalibor Matura   <255899@mail.muni.cz>
 *          Ian Wesley-Smith <iwsmith@cct.lsu.edu>
 *
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
 *      This product includes software developed by CESNET z.s.p.o.
 * 
 * 4. Neither the name of the CESNET nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING,
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#include "config_unix.h"
#include "config_win32.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "vo_postprocess.h"
#include "vo_postprocess/3d-interlaced.h"
#include "vo_postprocess/split.h"
#include "vo_postprocess/scale.h"
#include "vo_postprocess/double-framerate.h"

struct vo_postprocess_t {
        const char * name;
        vo_postprocess_init_t init;
        vo_postprocess_reconfigure_t reconfigure;
        vo_postprocess_getf_t getf;
        vo_postprocess_get_out_desc_t get_out_desc;
        vo_postprocess_get_supported_codecs_t get_supported_codecs;
        vo_postprocess_t vo_postprocess;
        vo_postprocess_done_t done;
};

struct vo_postprocess_state {
        struct vo_postprocess_t *handle;
        void *state;
};

struct vo_postprocess_t vo_postprocess_modules[] = {
        {"3d-interlaced", interlaced_3d_init, interlaced_3d_postprocess_reconfigure, 
                        interlaced_3d_getf, interlaced_3d_get_out_desc,
                        interlaced_3d_get_supported_codecs,
                        interlaced_3d_postprocess, interlaced_3d_done },
        {"split", split_init, split_postprocess_reconfigure, 
                        split_getf, split_get_out_desc,
                        split_get_supported_codecs,
                        split_postprocess, split_done },
        {"double-framerate", df_init, df_reconfigure, 
                        df_getf, df_get_out_desc,
                        df_get_supported_codecs,
                        df_postprocess, df_done },
#ifdef HAVE_LINUX
        {"scale", scale_init, scale_reconfigure, 
                        scale_getf, scale_get_out_desc,
                        scale_get_supported_codecs,
                        scale_postprocess, scale_done },
#endif /* HAVE_LINUX */
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};

void show_vo_postprocess_help()
{
        int i;
        printf("Possible postprocess modules:\n");
        for(i = 0; vo_postprocess_modules[i].name != NULL; ++i)
                printf("\t%s\n", vo_postprocess_modules[i].name);
}

struct vo_postprocess_state *vo_postprocess_init(char *config_string)
{
        struct vo_postprocess_state *s;
        char *vo_postprocess_options = NULL;
        
        if(!config_string) 
                return NULL;
        
        if(strcmp(config_string, "help") == 0)
        {
                show_vo_postprocess_help();
                return NULL;
        }
        
        s = (struct vo_postprocess_state *) malloc(sizeof(struct vo_postprocess_state));
        s->handle = NULL;
        int i;
        for(i = 0; vo_postprocess_modules[i].name != NULL; ++i) {
                if(strncasecmp(config_string, vo_postprocess_modules[i].name,
                                strlen(vo_postprocess_modules[i].name)) == 0) {
                        s->handle = &vo_postprocess_modules[i];
                        if(config_string[strlen(vo_postprocess_modules[i].name)] == ':') 
                                        vo_postprocess_options = config_string +
                                                strlen(vo_postprocess_modules[i].name) + 1;
                }
        }
        if(!s->handle) {
                fprintf(stderr, "Unknown postprocess module: %s\n", config_string);
                free(s);
                return NULL;
        }
        s->state = s->handle->init(vo_postprocess_options);
        if(!s->state) {
                fprintf(stderr, "Postprocessing initialization failed: %s\n", config_string);
                free(s);
                return NULL;
        }
        return s;
}

int vo_postprocess_reconfigure(struct vo_postprocess_state *s,
                struct video_desc desc)
{
        if(s) {
                return s->handle->reconfigure(s->state, desc);
        } else {
                return FALSE;
        }
}

struct video_frame * vo_postprocess_getf(struct vo_postprocess_state *s)
{
        if(s) {
                return s->handle->getf(s->state);
        } else {
                return NULL;
        }
}

void vo_postprocess(struct vo_postprocess_state *s, struct video_frame *in,
                struct video_frame *out, int req_pitch)
{
        if(s)
                s->handle->vo_postprocess(s->state, in, out, req_pitch);
}

void vo_postprocess_done(struct vo_postprocess_state *s)
{
        if(s) s->handle->done(s->state);
}

void vo_postprocess_get_out_desc(struct vo_postprocess_state *s, struct video_desc *out, int *display_mode, int *out_frames_count)
{
        if(s) s->handle->get_out_desc(s->state, out, display_mode, out_frames_count);
}

void vo_postprocess_get_supported_codecs(struct vo_postprocess_state *s, codec_t ** supported_codecs, int *count)
{
        if(s) s->handle->get_supported_codecs(supported_codecs, count);
}
