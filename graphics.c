/*
	beatfreq - FFT-based Beat frequency calculator

    Copyright (C) 2003-2010  Nigel D. Stepp

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

	This program is DUAL LICENSED under the MIT License, which means you may
	choose the license terms appropriate for your use. See COPYING-GPL and
	COPYING-MIT for details.

    Nigel Stepp <stepp@atistar.net>

    $Id: graphics.c 925 2016-04-07 16:42:02Z stepp $
*/

#include <sys/types.h>
#include <ggi/ggi.h>
#include <math.h>
#include "global.h"
#include "window.h"
#include "graphics.h"
#include "list.h"

/*
 * This initializes a GGI visual.  Most of this is
 * boilerplate.
 */
visual_t *init_visual(int x, int y)
{
	visual_t *visual;
	//ggi_pixel white;
	ggi_pixel black;
	//ggi_graphtype type;
	ggi_color pal[65536];
	ggi_color map[65536];
	ggi_mode mode = { /* This will cause the default mode to be set */
		1,                      /* 1 frame [???] */
		{x,y},    /* Default size */
		{GGI_AUTO,GGI_AUTO},    /* Virtual */
		{0,0},                  /* size in mm don't care */
		GT_AUTO,               /* Mode */
		{GGI_AUTO,GGI_AUTO}     /* Font size */
	};

	if (ggiInit() != 0) {
		fprintf(stderr, "unable to initialize LibGGI, exiting.\n");
		exit(1);
	}

	visual = malloc(sizeof(visual_t));

	visual->vis=ggiOpen(NULL);
        if (visual->vis == NULL) {
                fprintf(stderr,"unable to open default visual, exiting.\n");
                ggiExit();
                exit(1);
        }

        ggiSetFlags(visual->vis, GGIFLAG_ASYNC);
        
        ggiCheckMode(visual->vis,&mode);
        if (ggiSetMode(visual->vis,&mode)) {
                fprintf(stderr,"Can't set mode\n");
                ggiClose(visual->vis);
                ggiExit();
                return NULL;
        }

        //type=mode.graphtype;
        visual->vx=mode.virt.x;    visual->vy=mode.virt.y;
        visual->sx=mode.visible.x; visual->sy=mode.visible.y;
        visual->depth=GT_DEPTH(mode.graphtype);

        if (GT_SCHEME(mode.graphtype) == GT_PALETTE) {
                ggiSetColorfulPalette(visual->vis);
                ggiGetPalette(visual->vis, 0, 1<<visual->depth, pal);
        }

        /* Find the colors "white" and "black".
        */
        map[0].r=0xFFFF;
        map[0].g=0xFFFF;
        map[0].b=0xFFFF;
        
        //white=ggiMapColor(visual->vis, &map[0]);

        map[0].r= map[0].g= map[0].b= 0x0;
        black=ggiMapColor(visual->vis, &map[0]);

       /* Set the drawing color to black and clear the screen.
           (The screen is cleared to black at setmode time, but the
           palette change above might have ruined that)
        */
        ggiSetGCForeground(visual->vis, black);
        ggiFillscreen(visual->vis);

        ggiSetGCBackground(visual->vis, black);

	return(visual);
}

/*
 * Initialize the spectrograph visualization
 */
vis_scope_t *visualize_sgraph_init(visual_t *visual,size_t data_size, data_t ymax)
{
	vis_scope_t *scope_data = malloc(sizeof(vis_scope_t));
	ggi_color map;

	if(visual->sx > data_size) {
		scope_data->x_stretch_factor = 1;
	} else {
		scope_data->x_stretch_factor = (float)(visual->sx) / (float)data_size;
	}

	scope_data->dy = ymax/visual->sy;

	map.r=0;
	map.g=0;
	map.b=0;
	scope_data->black=ggiMapColor(visual->vis, &map);

	map.r=0;
	map.g=0xFFFF;
	map.b=0;

	scope_data->fgcolor=ggiMapColor(visual->vis, &map);

	map.r=0xFFFF;
	map.g=0;
	map.b=0;

	scope_data->fgcolor2=ggiMapColor(visual->vis, &map);

	scope_data->lastx = 0;
	scope_data->lasty = visual->sy;

	return scope_data;

}

/*
 * Initialize a simple oscilloscope visualization
 */
vis_scope_t *visualize_scope_init(visual_t *visual, size_t data_size, data_t ymax)
{
	vis_scope_t *scope_data = malloc(sizeof(vis_scope_t));
	ggi_color map;

	scope_data->x_stretch_factor = (float)(visual->sx) / (float)data_size;
	scope_data->dy = (2*ymax)/visual->sy;

	map.r=0;
	map.g=0;
	map.b=0;
	scope_data->black=ggiMapColor(visual->vis, &map);


	map.r=0;
	map.g=0xFFFF;
	map.b=0;

	scope_data->fgcolor=ggiMapColor(visual->vis, &map);

	return scope_data;
}


/*
 * This is the main visualization, which shows peaks correlating to beat
 * frequencies.
 */
void visualize_sgraph_step(visual_t *visual, vis_scope_t *scope_data, data_t *data, maxima_list_t *peak_info, size_t data_size)
{
	int cur_idx,last_idx,x,y,step;
	char val[8];
	maxima_list_t *p = peak_info;

	// Fill the window with black
	ggiSetGCForeground(visual->vis, scope_data->black);
	ggiFillscreen(visual->vis);

	// We are drawing with green
	ggiSetGCForeground(visual->vis,scope_data->fgcolor);
	cur_idx = 0;
	last_idx = -1;
	
	for(x=0;x<visual->sx && cur_idx<data_size;x++) {
			
		// map the data point onto the ggi coordinate frame
		y=(-1)*(data[cur_idx]/scope_data->dy) + (visual->sy - 1);
		if(y > visual->sy) {
			y = visual->sy;
		} else if (y < 0 ) {
			y = 0;
		}

		// print bpm label if this point is a peak
		if( p && (p->index <= cur_idx) && (p->index > last_idx)) {
			snprintf(val,8,"%.2f",(float)(p->index) * BEAT_FREQ_BAND * 60);
			ggiSetGCForeground(visual->vis,scope_data->fgcolor2);
			ggiPuts(visual->vis,x+5,y-2,val);
			ggiSetGCForeground(visual->vis,scope_data->fgcolor);

			// Go to next peak
			p = p->next;
		}						

		// Plot the actual line
		ggiDrawLine(visual->vis,scope_data->lastx,scope_data->lasty,x,y);
		scope_data->lastx = x;
		scope_data->lasty = y;
		last_idx = cur_idx;

		// do x-stretching/compressing since the display size is
		// probably not the data size
		if(scope_data->x_stretch_factor >= 1) {
			cur_idx++;
		} else {
			step = 1;
			while(1 - (step * scope_data->x_stretch_factor) > 0) {
				cur_idx++;
				step++;
			}
		}
	}
	ggiFlush(visual->vis);
	scope_data->lastx = 0;
	scope_data->lasty = visual->sy;
}


/*
 * Draw a simple oscilloscope
 */
void visualize_scope_step(visual_t *visual, vis_scope_t *scope_data, data_t *data, size_t data_size)
{
	int cur_idx,x,y,step;

	// Clear display
    ggiSetGCForeground(visual->vis, scope_data->black);
    ggiFillscreen(visual->vis);

	// Draw a horizontal line at the 0 point
	ggiSetGCForeground(visual->vis,scope_data->fgcolor);
	for(x=0;x<visual->sx;x++) {
		ggiDrawPixel(visual->vis,x,(visual->sy / 2));
	}

	cur_idx = 0;
	for(x=0;x<visual->sx && cur_idx<data_size;x++) {
		
		// Map the point onto the GGI coordinate frame
		y=(-1)*(data[cur_idx]/scope_data->dy) + (visual->sy / 2);
		if(y > visual->sy) {
			y = visual->sy;
		} else if (y < 0 ) {
			y = 0;
		}

		// Plot the point
		ggiDrawPixel(visual->vis,x,y);

		// Squeeze the data onto our display
		step = 1;
		while(1 - (step * scope_data->x_stretch_factor) > 0) {
			cur_idx++;
			step++;
		}
	}
	ggiFlush(visual->vis);
}

// This doesn't seem to be working too well
void visualize_window(visual_t *visual, window_t *window, size_t window_size, size_t data_size)
{
	ggi_pixel color;
	int idx,x,y;
	float dx,dy,xmax,ymax;

	if(visual->sx > window_size) {
		dx=1;
	} else {
		dx=(float)window_size/(float)visual->sx;
	}

	if(visual->sy > data_size) {
		dy=1;
	} else {
		dy=(float)data_size/(float)visual->sy;
	}

	xmax = (float)window_size / dx;
	ymax = (float)data_size / dy;

	for(y=0;y<ymax;y++) {
		for(x=0;x<xmax;x++) {
			idx=(int)((y*dy*window_size) + x*dx);
			if(window->data[idx] > (1<<visual->depth)) {
				color=1<<visual->depth;
			} else {
				color=(ggi_pixel)(floor(window->data[idx]));
			}
			ggiSetGCForeground(visual->vis,color);
			ggiDrawPixel(visual->vis,x,y);
		}
	}
	ggiFlush(visual->vis);
}

/* must have normalized window */
void visualize_beat_graph(visual_t *visual, window_t *beat_window, size_t window_size, size_t data_size)
{
	int x,y,idx,xmax,ymax;
	float dx,dy;
	data_t maxval=0.0,dc;

	if(visual->sx > window_size) {
		dx=1;
	} else {
		dx=(float)window_size/(float)visual->sx;
	}

	if(visual->sy > data_size) {
		dy=1;
	} else {
		dy=(float)data_size/(float)visual->sy;
	}

	xmax = window_size / dx;
	ymax = data_size / dy;

	//find max value in window
	for(x=0;x<(window_size*data_size);x++) {
		if(beat_window->data[x] > maxval) {
			maxval = beat_window->data[x];
		}
	}

	dc = maxval/(1<<visual->depth);

	for(x=0;x<xmax;x++) {
		for(y=0;y<ymax;y++) {
			//Plotting point at (x,y).  Map to matrix
			idx=(int)((y*dy*window_size) + x*dx);
			if(idx >= (window_size*data_size) ) {
				printf("Ack!!! (x=%d,y=%d,dx=%f,dy=%f,idx=%d)\n",x,y,dx,dy,idx);
				ggiSetGCForeground(visual->vis,(ggi_pixel)0);
			} else {
				//map intensity to color (0 - 1<<depth)
				ggiSetGCForeground(visual->vis,(ggi_pixel)(beat_window->data[idx]/dc));
			}
			ggiDrawPixel(visual->vis,x,y);
		}
	}
	ggiFlush(visual->vis);
}


