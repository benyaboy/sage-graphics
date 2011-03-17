/**************************************************************************
    sagepdf - a PDF viewer for SAGE tiled displays.
    Copyright (C) 2011  Christoph Willing
    
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
**************************************************************************/


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include <libgen.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib/poppler.h>
#include <math.h>

#define SAGEPDF_DEF_SCALEFACTOR	(4)

int npages;
int firstPage = 0;
PopplerDocument *document;
PopplerPage *page;
double	width, height;
cairo_t *cr, *sage_cr;
cairo_surface_t *surface, *sage_surface;
int scaleFactor;

#ifndef NOSAGE
// headers for SAGE
#include "sail.h"
#include "misc.h"
sail sageInf; // sail object

// for dxt compression
#include "libdxt.h"

using namespace std;
bool useDXT = true;

byte *sageBuffer = NULL;  // buffers for sage and dxt data
byte *dxt = NULL;
byte *rgba = NULL;

float lastX = 0;
float lastY = 0;
float dist = 0;

GdkWindow *local_window = NULL;
#endif

float local_startX = 0;
float local_startY = 0;
float local_dist = 0;
bool button1_is_pressed = false;


void
show_page()
{
	/* Clear window */
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_fill(cr);


	cairo_set_source_surface(cr, surface, 0, 0);
	cairo_translate(cr, 0, 0);
	cairo_scale(cr, 1, 1);
	poppler_page_render(page, cr);

	cairo_set_operator(cr, CAIRO_OPERATOR_DEST_OVER);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);


#ifndef NOSAGE
	if( useDXT )
	{
		int cairo_height, cairo_width, cairo_rowstride;
		unsigned int numBytes, *src;
		unsigned char *cairo_data, *dst;

		sage_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width*scaleFactor, height*scaleFactor);
		sage_cr = cairo_create(sage_surface);
		cairo_save (sage_cr);
		/* Flip */
		cairo_translate(sage_cr, 0, height*scaleFactor);
		cairo_scale(sage_cr, scaleFactor, -scaleFactor);
		poppler_page_render (page, sage_cr);
		cairo_restore (sage_cr);

		cairo_set_operator(sage_cr, CAIRO_OPERATOR_DEST_OVER);
		cairo_set_source_rgba(sage_cr, 1, 1, 1, 1);
		cairo_paint(sage_cr);

		/* We'd like to apply the sage_surface data directly to the
		*  DXT compression engine but it is in the wrong colour space.
		*  Therefore we extract it manually to the rgba buffer.
		*/
		cairo_height = cairo_image_surface_get_height (sage_surface);
		cairo_width = cairo_image_surface_get_width (sage_surface);
		cairo_rowstride = cairo_image_surface_get_stride (sage_surface);
		cairo_data = cairo_image_surface_get_data (sage_surface);
		for (int y = 0; y < cairo_height; y++)
		{
			src = (unsigned int *) (cairo_data + y * cairo_rowstride);
			dst = rgba + y * cairo_rowstride;
			for (int x = 0; x < cairo_width; x++) 
			{
				dst[0] = (*src >> 16) & 0xff;
				dst[1] = (*src >> 8) & 0xff; 
				dst[2] = (*src >> 0) & 0xff;
				dst[3] = (*src >> 24) & 0xff;
				dst += 4;
				src++;
			}
		}

		numBytes = CompressDXT(rgba, dxt, scaleFactor * width, scaleFactor * height, FORMAT_DXT1, 1);
		sageBuffer = (byte*)sageInf.getBuffer();
		memcpy(sageBuffer, dxt, scaleFactor * scaleFactor * width*height*4/8);
		sageInf.swapBuffer();

		cairo_destroy(sage_cr);
	}
	else
	{
		sage_surface = cairo_image_surface_create_for_data ((unsigned char*)sageInf.getBuffer(),
					     CAIRO_FORMAT_ARGB32, width * scaleFactor, height * scaleFactor,
					     cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width * scaleFactor));
		sage_cr = cairo_create(sage_surface);

		/* Clear SAGE window */
		cairo_rectangle(sage_cr, 0, 0, width * scaleFactor, height * scaleFactor);
		cairo_stroke_preserve(sage_cr);
		cairo_set_source_rgb(sage_cr, 1, 1, 1);
		cairo_fill(sage_cr);

		cairo_set_source_surface(sage_cr, sage_surface, 0, 0);
		cairo_scale(sage_cr, scaleFactor, scaleFactor);
		poppler_page_render(page, sage_cr);
		cairo_set_operator(sage_cr, CAIRO_OPERATOR_DEST_OVER);
		cairo_set_source_rgb(sage_cr, 1, 1, 1);
		cairo_paint(sage_cr);

		sageInf.swapBuffer();
		cairo_destroy(sage_cr);
	}
#endif

}

void
show_absolute_page(int requested_page)
{
	if( requested_page < 0 )
		requested_page = 0;
	if( requested_page >= npages )
		requested_page = npages - 1;

	/* Don't render again if we're already at that page */
	if( poppler_page_get_index(page) == requested_page )
		return;

	page = poppler_document_get_page(document, requested_page);
	show_page();
}

void
show_next_page(int next)
{
	show_absolute_page(poppler_page_get_index(page) + next);
/*
	int ncurrent = poppler_page_get_index(page) + next;
	if( (ncurrent > -1) && (ncurrent < npages) )
		page = poppler_document_get_page(document, ncurrent);
	show_page();
*/
}

gint
check_sage_messages(gpointer data)
{
#ifndef NOSAGE
	sageMessage msg;
	if (sageInf.checkMsg(msg, false) > 0) {
		char *data = (char*) msg.getData();

		switch (msg.getCode()) {
			case APP_QUIT : {
				sageInf.shutdown();
				gtk_main_quit();
				break;

			case EVT_CLICK:
				// Click event x and y location normalized to size of window
				float clickX, clickY;

				// Click device Id, button Id, and is down flag
				int clickDeviceId, clickButtonId, clickIsDown, clickEvent;

				// Parse message
				sscanf(data, 
				       "%d %f %f %d %d %d", 
				       &clickDeviceId, &clickX, &clickY, 
				       &clickButtonId, &clickIsDown, &clickEvent);
				//printf("Pointer click at %f,%f\n", clickX, clickY);

				// record the click position so we know how far we moved
				if (clickIsDown && clickEvent == EVT_PAN) {
				    lastX = clickX;
				    lastY = clickY;
				}

				if( clickIsDown )
				{
					//printf("Pointer click by button %d\n", clickButtonId);

					cr = gdk_cairo_create(local_window);
					if( clickButtonId == 1 )
						show_next_page(1);
					else
						show_next_page(-1);
				}

				break;

			case EVT_PAN: 
				// Pan event properties
				int panDeviceId;

				// Pan event x and y location and change in x, y and z direction
				// normalized to size of window
				float startX, startY, panDX, panDY, panDZ;
				sscanf(data, 
					"%d %f %f %f %f %f", 
					&panDeviceId, &startX, &startY, &panDX, &panDY, &panDZ);

				// keep track of distance
				dist += panDX;

				// we started a new drag
				if (lastX != startX) {
					lastX = startX;
					dist = 0;
				}
				else if( fabs(dist) > 0.07 ) { // if we dragged more than a certain distance, change a page
					cr = gdk_cairo_create(local_window);
					if (dist > 0)
						show_next_page(1);
					else
						show_next_page(-1);

					// reset the counter
					lastX = startX;
					dist = 0;
				}
				break;

			}
		}      
	}
#endif
	return TRUE;
}

static gboolean
on_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
	float local_lastX = event->x;
	local_dist = local_lastX - local_startX; 

	if( button1_is_pressed )
		if( fabs(local_dist / width) > 0.07 )
		{
			cr = gdk_cairo_create(widget->window);
			local_startX = local_lastX;
			if( local_dist > 0 )
				show_next_page(1);
			else
				show_next_page(-1);
		}

	return true;
}

static gboolean
on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	cr = gdk_cairo_create(widget->window);
	button1_is_pressed = false;
	switch(event->button)
	{
		case 1:
			button1_is_pressed = true;
			local_startX = event->x;
			show_next_page(1);
			break;
		case 3:
			show_next_page(-1);
			break;
		default:
			// printf("Unhandled button press: %d\n", event->button);
			return false;
			break;
	}
	return true;
}

static gboolean
on_key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	cr = gdk_cairo_create(widget->window);
	switch(event->keyval)
	{
		case GDK_Home:
			show_absolute_page(0);
			break;
		case GDK_End:
			show_absolute_page(npages);
			break;
		case GDK_Page_Up:
		case GDK_Left:
			show_next_page(-1);
			break;
		case GDK_Page_Down:
		case GDK_Right:
			show_next_page(1);
			break;
		case GDK_Escape:
			printf("Quit\n");
			gtk_main_quit();
			break;
		default:
			// printf("XXXXXXXXXX Unhandled key: %d\n", event->keyval);
			return false;
			break;
	}
	return true;
}

static gboolean
on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cr = gdk_cairo_create(widget->window);
	show_page();

	return FALSE;
}


int main(int argc, char *argv[])
{
	unsigned int window_width=-1, window_height=-1;  // sage window size
	GtkWidget *window;

	char *filename;
	char pathbuf[PATH_MAX];
	char uri[256];
	char *dirc, *basec, *dname, *bname;
	int     i=0;

	gtk_init(&argc, &argv);
	scaleFactor = SAGEPDF_DEF_SCALEFACTOR;
#ifndef NOSAGE
	sage::initUtil();
#endif

	// parse command line arguments
	if( argc < 2 )
	{
#ifndef NOSAGE
		sage::printLog("PDF> sagepdf filename [width] [height] [-show_original] [-page num] [-scale num]");
#else
		fprintf(stderr, "PDF> sagepdf filename [width] [height] [-show_original] [-page num] [-scale num]\n");
#endif
		exit(1);
	}
	for (int argNum=2; argNum<argc; argNum++)
	{
		if( strcmp(argv[argNum], "-page") == 0 )
		{
			int p = atoi(argv[argNum+1]);
			if( p > 0 )
				firstPage = p - 1;
			if( p < 0 )
				firstPage = 0;
			argNum++;
		}
#ifndef NOSAGE
		else if (strcmp(argv[argNum], "-show_original") == 0)
		{
			useDXT = false;
		}
#endif
		else if( strcmp(argv[argNum], "-scale") == 0 )
		{
			scaleFactor = atoi(argv[argNum+1]);
			if( scaleFactor < 1 )
				scaleFactor = 1;
			argNum++;
		}
		else if(atoi(argv[argNum]) != 0 && atoi(argv[argNum+1]) != 0)
		{
			window_width = atoi( argv[argNum] );
			window_height = atoi( argv[argNum+1] );
			argNum++;
		}
	}

	/* Check that the filename is an absolute pathname */
	filename = argv[1];
	dirc = strdup(filename);
	basec = strdup(filename);
	dname = dirname(dirc);
	bname = basename(basec);

	if( dname[0] != '/' )
	{
		fprintf(stderr, "Need absolute path for PDF file\n");

		/* We could try to construct a path -
		 * assume the path givern is relative to here
		 */
		if( ! getcwd(pathbuf, PATH_MAX) )
			exit(1);
		sprintf(pathbuf + strlen(pathbuf), "/\0");
		strcat(pathbuf, filename);
		fprintf(stderr, "Lets try %s\n", pathbuf);
	}
	else
	{
		sprintf(pathbuf, "%s\0", filename);
	}
	/* Can we read it ? */
	if( access(pathbuf, R_OK ) )
	{
		fprintf(stderr, "Can't access %s - exiting now ...\n", filename);
		exit(2);
	}

	sprintf(uri, "file://%s\0", pathbuf);
	document = poppler_document_new_from_file(uri, NULL, NULL);
	npages = poppler_document_get_n_pages(document);

	if( npages <= firstPage )
		firstPage = npages - 1;
	page = poppler_document_get_page(document, firstPage);
	poppler_page_get_size(page, &width, &height);
	printf("Page %d has size %fx%f\n", firstPage, width, height);


#ifndef NOSAGE
	if (useDXT)
	{
		if ((int)width % 4 != 0 || (int)height % 4 != 0)
		{
			fprintf(stderr, "\n**** Image cropped a few pixels to be a multiple of 4 for dxt");
			width -= (int)width % 4; width = (int)width * 1.0;
			height -= (int)height % 4; height = (int)height * 1.0;
		}

		// allocate buffers 
		rgba = (byte*) memalign(16, scaleFactor * scaleFactor * width*height*4);
		dxt = (byte*) memalign(16, scaleFactor * scaleFactor * width*height*4/8);
	}

	// SAGE setup
	sailConfig cfg;
	cfg.init("sagepdf.conf");   // every app has a config file named "appName.conf"
	std::cout << "SAIL configuration was initialized by sagepdf.conf" << std::endl;

	cfg.setAppName("sagepdf");
	cfg.rank = 0;
	cfg.resX = width * scaleFactor;
	cfg.resY = height * scaleFactor;
	cfg.winWidth = width * scaleFactor;
	cfg.winHeight = height * scaleFactor;

	sageRect renderImageMap;
	renderImageMap.left = 0.0;
	renderImageMap.right = 1.0;
	renderImageMap.bottom = 0.0;
	renderImageMap.top = 1.0;

	cfg.imageMap = renderImageMap;
	if( useDXT )
	{
		cfg.pixFmt = PIXFMT_DXT;
		cfg.rowOrd = BOTTOM_TO_TOP;
	}
	else
	{
		cfg.pixFmt = PIXFMT_8888_INV;
		cfg.rowOrd = TOP_TO_BOTTOM;
	}
	cfg.master = TRUE;

	sageInf.init(cfg);
	std::cout << "sail initialized " << std::endl;
#endif

	//printf("Using page size %fx%f\n", width, height);
	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), width, height); 
	gtk_widget_set_app_paintable(window, TRUE);


	g_signal_connect(window, "expose-event", G_CALLBACK (on_expose_event), NULL);
	g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect(window, "key_release_event", G_CALLBACK (on_key_release_event), NULL);
	g_signal_connect(window, "button_press_event", G_CALLBACK (on_button_press_event), NULL);
	g_signal_connect(window, "motion_notify_event", G_CALLBACK (on_motion_notify_event), NULL);
	gtk_widget_set_events (window,	GDK_KEY_RELEASE |
					GDK_BUTTON_PRESS_MASK |
					GDK_BUTTON_MOTION_MASK |
					GDK_POINTER_MOTION_HINT_MASK );

	gtk_widget_show_all(window);
#ifndef NOSAGE
	local_window = window->window;
#endif

	g_timeout_add (200, check_sage_messages, NULL);

	gtk_main();
	sageInf.shutdown();
	cairo_surface_destroy(surface);

	return 0;
}

