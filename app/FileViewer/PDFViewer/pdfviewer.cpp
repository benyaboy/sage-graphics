#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <unistd.h>


#if defined(USE_POPPLER)

#if SAGE_POPPLER_VERSION == 5
// Centos 5.5
#include "glib/poppler.h"
PopplerDocument *document;
PopplerBackend backend;
PopplerPage *page;
GEnumValue *enum_value;
GError *error;
GdkPixbuf *pixbuf;
#endif

#if SAGE_POPPLER_VERSION == 12
#include "poppler/goo/GooString.h"
#include "poppler/GlobalParams.h"
#include "poppler/PDFDoc.h"
#include "poppler/UnicodeMap.h"
#include "poppler/PDFDocEncoding.h"
#include "poppler/DateInfo.h"
#include "poppler/splash/SplashBitmap.h"
#include "poppler/splash/Splash.h"
#include "poppler/SplashOutputDev.h"
// Rendering object
SplashOutputDev *splashOut;
PDFDoc *doc;
#endif

int pg_index;
double x_resolution;
double pg_w, pg_h;

#else
#include <wand/magick-wand.h>
#endif


// headers for SAGE
#include "sail.h"
#include "misc.h"

// for dxt compression
#include "libdxt.h"


using namespace std;

// if true, it will show the original image and not the dxt compressed one
#if defined(USE_POPPLER)
bool useDXT = false;      // cannot yet since poppler returns RGB instead of RGBA
#else
bool useDXT = true;
#endif


// other globals...
byte *sageBuffer = NULL;  // buffers for sage and dxt data
byte *dxt = NULL;   
byte *rgba = NULL;
unsigned int width, height;  // image size
string fileName;    
float lastX = 0;
float dist = 0;
int numImages = 0;
int firstPage = 0;


#if ! defined(USE_POPPLER)
// use ImageMagick to read all other formats
MagickBooleanType status;  
MagickWand *wand;
#endif

// sail object
sail sageInf; 
sailConfig scfg;


#define ThrowWandException(wand)		\
  {						\
    char					\
      *description;				\
						\
    ExceptionType				\
      severity;					\
						    \
    description=MagickGetException(wand,&severity);			\
    (void) fprintf(stderr,"%s %s %ld %s\n",GetMagickModule(),description); \
    description=(char *) MagickRelinquishMemory(description);		\
    exit(-1);								\
  }


// -----------------------------------------------------------------------------

void getRGBA()
{
    // get the pixels
  memset(rgba, 0, width*height*4);
#if defined(USE_POPPLER)
#if SAGE_POPPLER_VERSION == 5
  poppler_page_render_to_pixbuf (page, 0, 0, width, height, x_resolution/72.0, 0, pixbuf);
  memcpy(rgba, gdk_pixbuf_get_pixels(pixbuf), width *height * 3);
#endif

#if SAGE_POPPLER_VERSION == 12
  SplashBitmap *bitmap = splashOut->getBitmap();
  memcpy(rgba, bitmap->getDataPtr(), width *height * 3);
#endif
#else
    MagickGetImagePixels(wand, 0, 0, width, height, "RGBA", CharPixel, rgba);
#endif
    
    if (useDXT) {
	unsigned int numBytes;

	// compress into DXT
	memset(dxt, 0, width*height*4/8);
	numBytes = CompressDXT(rgba, dxt, width, height, FORMAT_DXT1, 1);
    }
}

void swapBuffer()
{
    // get buffer from SAGE and fill it with dxt data
    sageBuffer = (byte*)sageInf.getBuffer();      
    if (useDXT)
	memcpy(sageBuffer, dxt, width*height*4/8);
    else
	memcpy(sageBuffer, rgba, width*height*3);
    sageInf.swapBuffer();

    // get buffer from SAGE and fill it with dxt data
    sageBuffer = (byte*)sageInf.getBuffer();      
    if (useDXT)
	memcpy(sageBuffer, dxt, width*height*4/8);
    else
	memcpy(sageBuffer, rgba, width*height*3);
    sageInf.swapBuffer();
}


void onHomeBtn()
{
#if defined(USE_POPPLER)
#if SAGE_POPPLER_VERSION == 5
  pg_index = 0;
  page = poppler_document_get_page(document, pg_index);
#endif
#if SAGE_POPPLER_VERSION == 12
  pg_index = 1;
  doc->displayPageSlice(splashOut, pg_index, x_resolution, x_resolution, 0, gTrue, gFalse, gFalse, 0, 0, pg_w, pg_h);
#endif
#else
    MagickSetImageIndex(wand, 0);
#endif
    sage::printLog("Page %d of %d", 1, numImages);
    getRGBA();
    swapBuffer();
}


void onEndBtn()
{
#if defined(USE_POPPLER)
#if SAGE_POPPLER_VERSION == 5
  pg_index = numImages-1;
  page = poppler_document_get_page(document, pg_index);
#endif
#if SAGE_POPPLER_VERSION == 12
  pg_index = numImages;
  doc->displayPageSlice(splashOut, pg_index, x_resolution, x_resolution, 0, gTrue, gFalse, gFalse, 0, 0, pg_w, pg_h);
#endif
#else
    MagickSetImageIndex(wand, numImages-1);
#endif
    sage::printLog("Page %d of %d", numImages, numImages);
    getRGBA();
    swapBuffer();
}

void onPrevBtn()
{
#if defined(USE_POPPLER)
#if SAGE_POPPLER_VERSION == 5
  pg_index = pg_index - 1;
  if (pg_index < 0) pg_index = 0;
  page = poppler_document_get_page(document, pg_index);
    sage::printLog("Page %ld of %d", pg_index+1, numImages);
#endif
#if SAGE_POPPLER_VERSION == 12
  pg_index = pg_index - 1;
  if (pg_index < 1) pg_index = 1;
  doc->displayPageSlice(splashOut, pg_index, x_resolution, x_resolution, 0, gTrue, gFalse, gFalse, 0, 0, pg_w, pg_h);
    sage::printLog("Page %ld of %d", pg_index, numImages);
#endif
#else
    MagickPreviousImage(wand);
    sage::printLog("Page %ld of %d", MagickGetImageIndex(wand)+1, numImages);
#endif
    getRGBA();
    swapBuffer();
}


void onNextBtn()
{
#if defined(USE_POPPLER)
#if SAGE_POPPLER_VERSION == 5
  pg_index = pg_index + 1;
  if (pg_index >= numImages) pg_index = numImages-1;
  page = poppler_document_get_page(document, pg_index);
    sage::printLog("Page %ld of %d", pg_index+1, numImages);
#endif
#if SAGE_POPPLER_VERSION == 12
  pg_index = pg_index + 1;
  if (pg_index > numImages) pg_index = numImages;
  doc->displayPageSlice(splashOut, pg_index, x_resolution, x_resolution, 0, gTrue, gFalse, gFalse, 0, 0, pg_w, pg_h);
    sage::printLog("Page %ld of %d", pg_index, numImages);
#endif
#else
    MagickNextImage(wand);
    sage::printLog("Page %ld of %d", MagickGetImageIndex(wand)+1, numImages);
#endif
    getRGBA();
    swapBuffer();
}


#if defined(USE_POPPLER)
#if SAGE_POPPLER_VERSION == 5
static void
print_index (PopplerIndexIter *iter)
{
  do
    {
      PopplerAction *action;
      PopplerIndexIter *child;

      action = poppler_index_iter_get_action (iter);
      sage::printLog ("Action: %d", action->type);
      poppler_action_free (action);
      child = poppler_index_iter_get_child (iter);
      if (child)
	print_index (child);
      poppler_index_iter_free (child);
    }
  while (poppler_index_iter_next (iter));
}

static void
print_document_info (PopplerDocument *document)
{
  gchar *title, *format, *author, *subject, *keywords, *creator, *producer, *linearized;
  GTime creation_date, mod_date;
  PopplerPageLayout layout;
  PopplerPageMode mode;
  PopplerViewerPreferences view_prefs;
  PopplerFontInfo *font_info;
  PopplerFontsIter *fonts_iter;
  PopplerIndexIter *index_iter;
  GEnumValue *enum_value;

  g_object_get (document,
		"title", &title,
		"format", &format,
		"author", &author,
		"subject", &subject,
		"keywords", &keywords,
		"creation-date", &creation_date,
		"mod-date", &mod_date,
		"creator", &creator,
		"producer", &producer,	
		"linearized", &linearized,
		"page-mode", &mode,
		"page-layout", &layout,
		"viewer-preferences", &view_prefs,
		NULL);

  sage::printLog ("\t---------------------------------------------------------");
  sage::printLog ("\tDocument Metadata");
  sage::printLog ("\t---------------------------------------------------------");
  if (title)  sage::printLog   ("\ttitle:\t\t%s", title);
  if (format) sage::printLog   ("\tformat:\t\t%s", format);
  if (author) sage::printLog   ("\tauthor:\t\t%s", author);
  if (subject) sage::printLog  ("\tsubject:\t%s", subject);
  if (keywords) sage::printLog ("\tkeywords:\t%s", keywords);
  if (creator) sage::printLog ("\tcreator:\t%s", creator);
  if (producer) sage::printLog ("\tproducer:\t%s", producer);
  if (linearized) sage::printLog ("\tlinearized:\t%s", linearized);
  
  enum_value = g_enum_get_value ((GEnumClass *) g_type_class_peek (POPPLER_TYPE_PAGE_MODE), mode);
  sage::printLog ("\tpage mode:\t%s", enum_value->value_name);
  enum_value = g_enum_get_value ((GEnumClass *) g_type_class_peek (POPPLER_TYPE_PAGE_LAYOUT), layout);
  sage::printLog ("\tpage layout:\t%s", enum_value->value_name);

  sage::printLog ("\tcreation date:\t%d", creation_date);
  sage::printLog ("\tmodified date:\t%d", mod_date);

  sage::printLog ("\tfonts:");
  font_info = poppler_font_info_new (document);
  while (poppler_font_info_scan (font_info, 20, &fonts_iter)) {
    if (fonts_iter) {
      do {
        sage::printLog ("\t\t\t%s", poppler_fonts_iter_get_name (fonts_iter));
      } while (poppler_fonts_iter_next (fonts_iter));
      poppler_fonts_iter_free (fonts_iter);
    }
  }
  poppler_font_info_free (font_info);

  index_iter = poppler_index_iter_new (document);
  if (index_iter)
    {
      sage::printLog ("\tindex:");
      print_index (index_iter);
      poppler_index_iter_free (index_iter);
    }

  g_free (title);
  g_free (format);
  g_free (author);
  g_free (subject);
  g_free (keywords);
  g_free (creator);
  g_free (producer); 
  g_free (linearized);
}

#endif
#if SAGE_POPPLER_VERSION == 12
// -----------------------------------------------------------------------------
static void printInfoString(Dict *infoDict, char *key, char *text,
                            UnicodeMap *uMap) {
  Object obj;
  GooString *s1;
  GBool isUnicode;
  Unicode u;
  char buf[8];
  int i, n;

  if (infoDict->lookup(key, &obj)->isString()) {
    fputs(text, stdout);
    s1 = obj.getString();
    if ((s1->getChar(0) & 0xff) == 0xfe &&
        (s1->getChar(1) & 0xff) == 0xff) {
      isUnicode = gTrue;
      i = 2;
    } else {
      isUnicode = gFalse;
      i = 0;
    }
    while (i < obj.getString()->getLength()) {
      if (isUnicode) {
        u = ((s1->getChar(i) & 0xff) << 8) |
            (s1->getChar(i+1) & 0xff);
        i += 2;
      } else {
        u = pdfDocEncoding[s1->getChar(i) & 0xff];
        ++i;
      }
      n = uMap->mapUnicode(u, buf, sizeof(buf));
      fwrite(buf, 1, n, stdout);
    }
    fputc('\n', stdout);
  }
  obj.free();
}


static void printInfoDate(Dict *infoDict, char *key, char *text) {
  Object obj;
  char *s;
  int year, mon, day, hour, min, sec, tz_hour, tz_minute;
  char tz;
  struct tm tmStruct;
  char buf[256];

  if (infoDict->lookup(key, &obj)->isString()) {
    fputs(text, stdout);
    s = obj.getString()->getCString();
    // TODO do something with the timezone info
    if ( parseDateString( s, &year, &mon, &day, &hour, &min, &sec, &tz, &tz_hour, &tz_minute ) ) {
      tmStruct.tm_year = year - 1900;
      tmStruct.tm_mon = mon - 1;
      tmStruct.tm_mday = day;
      tmStruct.tm_hour = hour;
      tmStruct.tm_min = min;
      tmStruct.tm_sec = sec;
      tmStruct.tm_wday = -1;
      tmStruct.tm_yday = -1;
      tmStruct.tm_isdst = -1;
      // compute the tm_wday and tm_yday fields
      if (mktime(&tmStruct) != (time_t)-1 &&
          strftime(buf, sizeof(buf), "%c", &tmStruct)) {
        fputs(buf, stdout);
      } else {
        fputs(s, stdout);
      }
    } else {
      fputs(s, stdout);
    }
    fputc('\n', stdout);
  }
  obj.free();
}
#endif
#endif


// -----------------------------------------------------------------------------



int main(int argc,char **argv)
{
    unsigned int window_width=-1, window_height=-1;  // sage window size

    sage::initUtil();

    // parse command line arguments
    if (argc < 2){
	sage::printLog("PDF> pdfviewer filename [width] [height] [-show_original] [-page num]");
	return 0;
    }
    for (int argNum=2; argNum<argc; argNum++)
    {
	if (strcmp(argv[argNum], "-show_original") == 0) {
	    useDXT = false;
	}
	else if(strcmp(argv[argNum], "-page") == 0) {
	    int p = atoi(argv[argNum+1]);
	    if (p != 0)
		firstPage = p-1;
	    argNum++;
	}
	else if(atoi(argv[argNum]) != 0 && atoi(argv[argNum+1]) != 0) {
	    window_width = atoi( argv[argNum] );
	    window_height = atoi( argv[argNum+1] );
	    argNum++;  // increment because we read two args here
	}
    }


    fileName = string(argv[1]);

#if defined(USE_POPPLER)
#if SAGE_POPPLER_VERSION == 5
  g_type_init();
  sage::printLog("PDF> Poppler version %s", poppler_get_version());
  backend = poppler_get_backend ();
  enum_value = g_enum_get_value ((GEnumClass *) g_type_class_ref (POPPLER_TYPE_BACKEND), backend);
  sage::printLog ("PDF> Backend is %s", enum_value->value_name);

  error = NULL;
  const string filename = string((const char*)"file://") + string(argv[1]);
  document = poppler_document_new_from_file (filename.c_str(), NULL, &error);
  if (document == NULL) {
    sage::printLog("PDF> error for [%s]: %s", filename.c_str(), error->message);
    exit(0);
  }
  // get the number of pages
  numImages = poppler_document_get_n_pages(document);

  pg_index = 0; // index starts at 0
  page = poppler_document_get_page(document, pg_index);
  if (page == NULL) {
    sage::printLog("PDF> error for [%s]: %s", filename.c_str());
    exit(0);
  }

  // page size
  double mwidth, mheight;
  poppler_page_get_size (page, &mwidth, &mheight);
  sage::printLog ("PDF> page size: %g by %g ", mwidth, mheight);

  x_resolution = 200.0;
  pg_w = mwidth  * (x_resolution / 72.0);
  pg_h = mheight * (x_resolution / 72.0);

  if (pg_w > 4096) {
	pg_w = 4096.0;
	pg_h = 4096.0 / (mwidth/mheight);
	x_resolution = pg_w * 72.0 / mwidth;
  }
  if (pg_h > 4096) {
	pg_w = 4096.0 / (mheight/mwidth);
	pg_h = 4096.0;
	x_resolution = pg_h * 72.0 / mheight;
  }

  width  = (unsigned int)pg_w;
  height = (unsigned int)pg_h;
  sage::printLog("PDF> Crop @ %d DPI: width %d height %d", int(x_resolution), int(pg_w), int(pg_h));

  pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, width, height);
  gdk_pixbuf_fill (pixbuf, 0xffffffff);

  // print some information
  print_document_info (document); 

#endif
#if SAGE_POPPLER_VERSION == 12
  GooString *fileName;
  GooString *ownerPW, *userPW;
  Object info;
  UnicodeMap *uMap;

  // read config file
  globalParams = new GlobalParams();

  // get mapping to output encoding
  if (!(uMap = globalParams->getTextEncoding())) {
    sage::printLog("PDF> Couldn't get text encoding");
  }

  fileName = new GooString(argv[1]);
  ownerPW = NULL;
  userPW = NULL;
  doc = new PDFDoc(fileName, ownerPW, userPW, NULL);
  sage::printLog("PDF> file [%s] is OK: %d", argv[1], doc->isOk() );
  numImages = doc->getNumPages();
  sage::printLog("PDF> num pages: %d", numImages );

  SplashColor paperColor;
  paperColor[0] = 255;
  paperColor[1] = 255;
  paperColor[2] = 255;

  splashOut = new SplashOutputDev(splashModeRGB8, 3, gFalse, paperColor);
  splashOut->startDoc(doc->getXRef());

  pg_index = firstPage+1;
  double mwidth  = doc->getPageMediaWidth(pg_index);
  double mheight = doc->getPageMediaHeight(pg_index);
  sage::printLog("PDF> Media width %d height %d", int(mwidth), int(mheight));

  x_resolution = 200.0;
  pg_w = mwidth  * (x_resolution / 72.0);
  pg_h = mheight * (x_resolution / 72.0);

  if (pg_w > 4096) {
	pg_w = 4096.0;
	pg_h = 4096.0 / (mwidth/mheight);
	x_resolution = pg_w * 72.0 / mwidth;
  }
  if (pg_h > 4096) {
	pg_w = 4096.0 / (mheight/mwidth);
	pg_h = 4096.0;
	x_resolution = pg_h * 72.0 / mheight;
  }

  width  = pg_w;
  height = pg_h;
  sage::printLog("PDF> Crop @ %d DPI: width %d height %d", int(x_resolution), int(pg_w), int(pg_h));

  doc->displayPageSlice(splashOut, 1, x_resolution, x_resolution, 0, gTrue, gFalse, gFalse, 0, 0, pg_w, pg_h);

  // print doc info
  doc->getDocInfo(&info);
  if (info.isDict()) {
    printInfoString(info.getDict(), (char*)"Title",        (char*)"Title:          ", uMap);
    printInfoString(info.getDict(), (char*)"Subject",      (char*)"Subject:        ", uMap);
    printInfoString(info.getDict(), (char*)"Keywords",     (char*)"Keywords:       ", uMap);
    printInfoString(info.getDict(), (char*)"Author",       (char*)"Author:         ", uMap);
    printInfoString(info.getDict(), (char*)"Creator",      (char*)"Creator:        ", uMap);
    printInfoString(info.getDict(), (char*)"Producer",     (char*)"Producer:       ", uMap);
    printInfoDate(info.getDict(),   (char*)"CreationDate", (char*)"CreationDate:   ");
    printInfoDate(info.getDict(),   (char*)"ModDate",      (char*)"ModDate:        ");
  }
  info.free();
#endif
#else
    // read file
    wand=NewMagickWand();  

    // set the resolution (pixel density)
    MagickSetResolution(wand, 200,200);
    
    status=MagickReadImage(wand, fileName.data());
    if (status == MagickFalse)
	ThrowWandException(wand);
    numImages = MagickGetNumberImages(wand);
    MagickSetImageIndex(wand, firstPage);

    // get the image size
    width = MagickGetImageWidth(wand);
    height = MagickGetImageHeight(wand);

    // -------  TODO: need to keep track of images that have been flipped already...

    // crop the image if necessary to make sure it's a multiple of 4
    if (useDXT)
    {
	if (width%4 != 0 || height%4 != 0)
	{
	    fprintf(stderr, "\n**** Image cropped a few pixels to be a multiple of 4 for dxt");
	    width -= width%4;
	    height -= height%4;
	}
	
	// flip the image to have the correct orientation for dxt
	MagickFlipImage(wand);
    }
#endif

    // allocate buffers 
    rgba = (byte*) memalign(16, width*height*4);
    if (useDXT)
	dxt = (byte*) memalign(16, width*height*4/8);

    // get the first page
    getRGBA();

    // if the user didn't specify the window size, use the image size
    if (window_height == -1 && window_width == -1)
    {
	window_width = width;
	window_height = height;
    }
  
    // initialize SAIL

    // Search for a configuration file
    char *tmpconf = getenv("SAGE_APP_CONFIG");
    if (tmpconf) {
      sage::printLog("PDFViewer> found SAGE_APP_CONFIG variable: [%s]", tmpconf);
      scfg.init(tmpconf);
    }
    else {
      sage::printLog("PDFViewer> using default pdfviewer.conf");
      scfg.init((char*)"pdfviewer.conf");
    }

    scfg.setAppName("pdfviewer");

    scfg.resX = width;
    scfg.resY = height;

    // if it hasn't been specified by the config file, use the app-determined size
    if (scfg.winWidth == -1 || scfg.winHeight == -1) {
	scfg.winWidth = window_width;
	scfg.winHeight = window_height;
    }
    
    if (useDXT)
    {
	scfg.pixFmt = PIXFMT_DXT;
	scfg.rowOrd = BOTTOM_TO_TOP;
    }
    else
    {
	scfg.pixFmt = PIXFMT_888;
	scfg.rowOrd = TOP_TO_BOTTOM;
    }

    sageInf.init(scfg);

    // finally swap the first buffer
    swapBuffer();


    // Wait the end
    while (1)
    {
	usleep(50000);  // so that we don't keep spinning too frequently (0.5s)
	sageMessage msg;
	if (sageInf.checkMsg(msg, false) > 0) {
	    char *data = (char*) msg.getData();

	    switch (msg.getCode()) {
	    case APP_QUIT:
		// release the memory
		free(dxt);
		free(rgba);
#if ! defined(USE_POPPLER)
		DestroyMagickWand(wand);
#endif

		sageInf.shutdown();
		exit(1);
		break;

	    }   // end switch
	}  // end if
    } // end while

    return 0;
}

