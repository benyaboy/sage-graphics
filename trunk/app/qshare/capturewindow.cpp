#include "capturewindow.h"

#if defined(__linux__)

#include <X11/Xutil.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xfixes.h>

struct x11_grab
{
    int frame_size;          /* Size in bytes of a grabbed frame */
    int height;              /* Height of the grab frame */
    int width;               /* Width of the grab frame */
    int x_off;               /* Horizontal top-left corner coordinate */
    int y_off;               /* Vertical top-left corner coordinate */

    Display *dpy;            /* X11 display from which x11grab grabs frames */
    XImage *image;           /* X11 image holding the grab */
    int use_shm;             /* !0 when using XShm extension */
    XShmSegmentInfo shminfo; /* When using XShm, keeps track of XShm infos */
    int nomouse;
};

struct x11_grab *x11grab;

#endif

#if defined(WIN32)
HWND    hDesktopWnd;
HDC     hDesktopDC;
HDC     hCaptureDC;
HBITMAP hCaptureBitmap;
void*	pBits = NULL;
#endif

// headers for SAGE
#include "sail.h"
#include "misc.h"

// Compression
#include "libdxt.h"

// headers for SAGE
GLubyte *rgbBuffer = NULL;
GLubyte *dxtBuffer = NULL;
sail sageInf;
sailConfig scfg;


CaptureWindow::CaptureWindow(QWidget *parent) :
        QMainWindow(parent)
{
    dxt_aInitialize();

    // Build the UI
    ui.setupUi(this);
    ui.statusBar->showMessage(QString("Desktop capture"), 0);

	// Disable DXT for now
	ui.checkBox->setEnabled(false);

    // Get dimensions
    QDesktopWidget *desktop = QApplication::desktop();
    WW = desktop->width();
    HH = desktop->height(); 
    sage::printLog("QSHARE> Desktop width %d height %d", WW, HH);

    // Set private variable
    fps = 25;
    started = false;
    dxt = false;
    count = 0;
    showcursor = true;

#if defined(__APPLE__)
    /////////////////////////
    // Get the OpenGL context
    CGLPixelFormatObj pixelFormatObj ;
    GLint numPixelFormats ;
    CGDirectDisplayID displayId = CGMainDisplayID();
    CGRect dRect = CGDisplayBounds( displayId );
    WW = dRect.size.width;
    HH = dRect.size.height;
    CGOpenGLDisplayMask displayMask = CGDisplayIDToOpenGLDisplayMask(displayId);

    CGLPixelFormatAttribute attribs[] =
    {
        (CGLPixelFormatAttribute)kCGLPFAFullScreen,
        (CGLPixelFormatAttribute)kCGLPFADisplayMask,
        (CGLPixelFormatAttribute)displayMask,
        (CGLPixelFormatAttribute)0
    };
    CGLChoosePixelFormat( attribs, &pixelFormatObj, &numPixelFormats );
    CGLCreateContext( pixelFormatObj, NULL, &glContextObj ) ;
    CGLDestroyPixelFormat( pixelFormatObj ) ;
    CGLSetCurrentContext( glContextObj ) ;
    glReadBuffer(GL_FRONT);
    CGLSetFullScreen( glContextObj ) ;///UUUUUUUUUUnbelievable
    CGLSetCurrentContext( NULL );
    qDebug() << "W " << WW << "  H" << HH;
    /////////////////////////
#endif

#if defined(__linux__)
    //
    // Initialize x11 frame grabber
    //
    Display *dpy;
    XImage *image;
    int use_shm;

    x11grab = (struct x11_grab*)malloc(sizeof(struct x11_grab));
    memset(x11grab, 0, sizeof(struct x11_grab));

    dpy = XOpenDisplay(0);
    if(!dpy) {
        sage::printLog("QSHARE> Could not open X display");
    }
 
    use_shm = XShmQueryExtension(dpy);
    sage::printLog("QSHARE> shared memory extension %s found", use_shm ? "" : "not");

    if(use_shm) {
        int scr = XDefaultScreen(dpy);
        image = XShmCreateImage(dpy,
                                DefaultVisual(dpy, scr),
                                DefaultDepth(dpy, scr),
                                ZPixmap,
                                NULL,
                                &x11grab->shminfo,
                                WW, HH);
	sage::printLog("QSHARE> Image: widht %d  height %d bytes_per_line %d, depth %d", image->width,image->height,image->bytes_per_line, DefaultDepth(dpy, scr));
        x11grab->shminfo.shmid = shmget(IPC_PRIVATE,
                                        image->bytes_per_line * image->height,
                                        IPC_CREAT|0777);
        if (x11grab->shminfo.shmid == -1) {
            sage::printLog("QSHARE> Fatal: Can't get shared memory!");
        }
        x11grab->shminfo.shmaddr = image->data = (char*)shmat(x11grab->shminfo.shmid, 0, 0);
        x11grab->shminfo.readOnly = False;

        if (!XShmAttach(dpy, &x11grab->shminfo)) {
            sage::printLog("QSHARE> Fatal: Failed to attach shared memory!");
        }
    } else {
        image = XGetImage(dpy, RootWindow(dpy, DefaultScreen(dpy)),
                          0, 0, WW, HH,
                          AllPlanes, ZPixmap);
    }

    switch (image->bits_per_pixel) {
    case 8:
        sage::printLog("QSHARE> 8 bit palette");
        //input_pixfmt = PIX_FMT_PAL8;
        break;
    case 16:
        if (       image->red_mask   == 0xf800 &&
                   image->green_mask == 0x07e0 &&
                   image->blue_mask  == 0x001f ) {
            sage::printLog("QSHARE> 16 bit RGB565");
            //input_pixfmt = PIX_FMT_RGB565;
        } else if (image->red_mask   == 0x7c00 &&
                   image->green_mask == 0x03e0 &&
                   image->blue_mask  == 0x001f ) {
            sage::printLog("QSHARE> 16 bit RGB555");
            //input_pixfmt = PIX_FMT_RGB555;
        } else {
            sage::printLog("QSHARE> RGB ordering at image depth %i not supported ... aborting\n", image->bits_per_pixel);
            sage::printLog("QSHARE> color masks: r 0x%.6lx g 0x%.6lx b 0x%.6lx\n", image->red_mask, image->green_mask, image->blue_mask);
        }
        break;
    case 24:
        if (        image->red_mask   == 0xff0000 &&
                    image->green_mask == 0x00ff00 &&
                    image->blue_mask  == 0x0000ff ) {
            sage::printLog("QSHARE> 24 bit BGR24");
            //input_pixfmt = PIX_FMT_BGR24;
        } else if ( image->red_mask   == 0x0000ff &&
                    image->green_mask == 0x00ff00 &&
                    image->blue_mask  == 0xff0000 ) {
	    sage::printLog("QSHARE> 24 bit RGB24");
            //input_pixfmt = PIX_FMT_RGB24;
       } else {
            sage::printLog("QSHARE> rgb ordering at image depth %i not supported ... aborting", image->bits_per_pixel);
	    sage::printLog("QSHARE> color masks: r 0x%.6lx g 0x%.6lx b 0x%.6lx", image->red_mask, image->green_mask, image->blue_mask);
        }
        break;
    case 32:
	sage::printLog("QSHARE> 32 bit RGB32");
	//input_pixfmt = PIX_FMT_RGB32;
        break;
    default:
        sage::printLog("QSHARE> image depth %i not supported ... aborting", image->bits_per_pixel);
    }
    x11grab->nomouse = 1;
    x11grab->frame_size = WW * HH * image->bits_per_pixel/8;
    x11grab->dpy = dpy;
    x11grab->width = WW;
    x11grab->height = HH;
    x11grab->x_off = 0;
    x11grab->y_off = 0;
    x11grab->image = image;
    x11grab->use_shm = use_shm;   
#endif

#if defined(WIN32)
    BITMAPINFO	bmpInfo;
    ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
    bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biBitCount=24;//BITSPERPIXEL;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    bmpInfo.bmiHeader.biWidth=WW;
    bmpInfo.bmiHeader.biHeight=HH;
    bmpInfo.bmiHeader.biPlanes=1;
    bmpInfo.bmiHeader.biSizeImage=abs(bmpInfo.bmiHeader.biHeight)*bmpInfo.bmiHeader.biWidth*bmpInfo.bmiHeader.biBitCount/8;

    hDesktopWnd = GetDesktopWindow();
    hDesktopDC = GetDC(hDesktopWnd);
    hCaptureDC = CreateCompatibleDC(hDesktopDC);
    hCaptureBitmap = CreateDIBSection(hDesktopDC,&bmpInfo,DIB_RGB_COLORS,&pBits,NULL,0);
    SelectObject(hCaptureDC,hCaptureBitmap);
#endif

    // Enables buttons
    ui.pushButton->setEnabled(true);
    ui.pushButton_2->setEnabled(false);
    ui.pushButton_3->setEnabled(false);

    // Load the pointer
    cursor_icon = new QImage("arrow2.png");

    // Load SAIL config
    scfg.init((char*)"qshare.conf");
    scfg.setAppName((char*)"qshare");

    fsip = QString(scfg.fsIP);
    qDebug() << "FSIP " << fsip;
    ui.lineEdit->setText(fsip);

    // Timer setup
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
}

void CaptureWindow::closeEvent(QCloseEvent *event)
{
#if defined(__linux__)
    //
    // Close x11 frame grabber
    //
		
    // Detach cleanly from shared mem
    // if (x11grab && x11grab->use_shm) {
    // 	XShmDetach(x11grab->dpy, &x11grab->shminfo);
    // 	shmdt(x11grab->shminfo.shmaddr);
    // 	shmctl(x11grab->shminfo.shmid, IPC_RMID, NULL);
    // }
		
    // Destroy X11 image
    // if (x11grab && x11grab->image) {
    // 	XDestroyImage(x11grab->image);
    // 	x11grab->image = NULL;
    // }
		
    // Free X11 display
    // if (x11grab)
    // 	XCloseDisplay(x11grab->dpy);
#endif
    onStop();
    event->accept();
}

void CaptureWindow::frameRate(int f)
{
    fps = f;
    qDebug() << "framerate " << fps;
    timer->setInterval(1000/fps);
}


void CaptureWindow::fsIP(QString f)
{
    fsip = f;
    qDebug() << "FSIP changed" << fsip;
}

void CaptureWindow::update()
{
    if (started) {

        // Capture the desktop pixels
        capture((char*)rgbBuffer,0,0,WW,HH);

        if (showcursor) {
	    // Get mouse position
            QPoint pt = QCursor::pos();

	    // Flip the Y value
            int adjust = HH - pt.y();
	    if (adjust < 0) adjust = 0;
            pt.setY(adjust);
	    
	    // Compute boundaries
            int startx = MAX(0,pt.x());
            // int starty = MAX(0, pt.y()-cursor_icon->height());
            int starty = pt.y()-cursor_icon->height();
	    
	    // Compose desktop with cursor
            QImage resultImage = QImage((uchar*)rgbBuffer, WW, HH, QImage::Format_RGB888);
            QPainter painter(&resultImage);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            painter.drawImage(startx, starty, *cursor_icon);
            painter.end();
	    // rgbBuffer is filled after that point
        }

        if (dxt)
        {
            CompressDXT(rgbBuffer, dxtBuffer, WW, HH, FORMAT_DXT1, 1);
            sageInf.swapBuffer();
            dxtBuffer = (GLubyte *)sageInf.getBuffer();
        }
        else
        {
            sageInf.swapBuffer();
            rgbBuffer = (GLubyte *)sageInf.getBuffer();
        }

        sageMessage msg;
        if (sageInf.checkMsg(msg, false) > 0) {
            switch (msg.getCode()) {
            case APP_QUIT:
		exit(1);
                break;
            }
        }


        QString str;
        double nowt = dxt_aTime();
        //qDebug() << "now " << nowt << "   startt" << startt;
        double dfps = 1.0 / (nowt - startt);
        str = QString("%1 ms / %2 fps/ # %3 / DXT %4 / %5x%6").arg(nowt-startt,5,'f',1).arg(dfps,5,'f',1).arg(count++).arg(dxt).arg(WW).arg(HH);
        ui.statusBar->showMessage( str, 0 );

#if 1
        if (dfps < fps) {
            timer->start( MAX(0, (2*(1000/fps)-(1000/dfps) ) ) );
        }
        else
            timer->start(1000/fps);
#else
        timer->start(0);
#endif
        startt = nowt;
    }
}

void CaptureWindow::capture(char* m_pFrameRGB,int x,int y,int cx,int cy)
{
#if defined(__APPLE__)
    CGLSetCurrentContext( glContextObj ) ;
    //CGLSetFullScreen( glContextObj ) ;///UUUUUUUUUUnbelievable
    //glReadBuffer(GL_FRONT);

    if (dxt)
        glReadPixels(x,y,cx,cy,GL_RGBA,GL_UNSIGNED_BYTE,m_pFrameRGB);
    else
        glReadPixels(x,y,cx,cy,GL_RGB,GL_UNSIGNED_BYTE,m_pFrameRGB);

    CGLSetCurrentContext( NULL );
#endif

#if defined(__linux__)
    if (x11grab->use_shm) {
	if (!XShmGetImage(x11grab->dpy, RootWindow(x11grab->dpy, DefaultScreen(x11grab->dpy)),
			  x11grab->image, 0, 0, AllPlanes)) {
	    sage::printLog("QSHARE> XShmGetImage() failed");
	}
	else {
	    // Upside-down and RGBA-to-RGB conversion
	    for (int i = 0 ; i < cy; i++) {
		for (int j = 0 ; j < cx; j++) {
		    m_pFrameRGB [ (i*cx+j) * 3 + 0 ] = x11grab->image->data [ ((cy-i-1)*cx+j) * 4 + 2 ];
		    m_pFrameRGB [ (i*cx+j) * 3 + 1 ] = x11grab->image->data [ ((cy-i-1)*cx+j) * 4 + 1 ];
		    m_pFrameRGB [ (i*cx+j) * 3 + 2 ] = x11grab->image->data [ ((cy-i-1)*cx+j) * 4 + 0 ];
		}
	    }
	}
    } 
#endif

#if defined(WIN32)
	BitBlt(hCaptureDC,0,0,WW,HH,hDesktopDC,0,0,SRCCOPY|CAPTUREBLT);
	memcpy(m_pFrameRGB, pBits, WW*HH*3);
#endif
}

void CaptureWindow::onStart()
{
    if (! started) {
        ui.pushButton->setEnabled(false);
        ui.pushButton_2->setEnabled(true);
        ui.pushButton_3->setEnabled(true);
        ui.lineEdit->setEnabled(false);
        ui.spinBox->setEnabled(true);
        ui.checkBox->setEnabled(false);

        qDebug() << "On Start";

        sageRect ishareImageMap;
        ishareImageMap.left = 0.0;
        ishareImageMap.right = 1.0;
        ishareImageMap.bottom = 0.0;
        ishareImageMap.top = 1.0;

        scfg.rank = 0;
        scfg.resX = WW;
        scfg.resY = HH;
        scfg.winWidth  = WW;
        scfg.winHeight = HH;
        scfg.imageMap = ishareImageMap;
	
	// Copy back the text box into the SAGE variable
	memset(scfg.fsIP, 0, SAGE_IP_LEN);
	strncpy(scfg.fsIP, fsip.toAscii().constData(), SAGE_IP_LEN);

        if (dxt)
            scfg.pixFmt = PIXFMT_DXT;
        else
#if defined(WIN32)
			scfg.pixFmt = PIXFMT_888_INV; // for some reasons, win32 is BGR
#else
			scfg.pixFmt = PIXFMT_888;
#endif

        scfg.rowOrd = BOTTOM_TO_TOP;
        scfg.master = true;

        sageInf.init(scfg);

        if (dxt) {
            dxtBuffer = (GLubyte *)sageInf.getBuffer();
            if (rgbBuffer) delete [] rgbBuffer;
            rgbBuffer = (byte*)memalign(16, WW*HH*4);
            memset(rgbBuffer, 0,  WW*HH*4);
        }
        else {
            rgbBuffer = (GLubyte *)sageInf.getBuffer();
        }


        started = true;
        timer->setSingleShot(true);
        timer->start(1000/fps);
        //timer->start();

        startt = dxt_aTime();
    }
}

void CaptureWindow::compression(int c)
{
    dxt = c;
}

void CaptureWindow::cursor(int c)
{
    showcursor = c;
}

void CaptureWindow::onStop()
{
    qDebug() << "On Stop";
    started = false;
    timer->stop();
    ui.pushButton->setEnabled(true);
    ui.pushButton_2->setEnabled(false);
    ui.pushButton_3->setEnabled(false);
    ui.lineEdit->setEnabled(true);
    ui.spinBox->setEnabled(true);
    ui.checkBox->setEnabled(true);
    sageInf.shutdown();
}

void CaptureWindow::onPause()
{
    if (started) {
        qDebug() << "Pause";
        started = false;
        timer->stop();
    }
    else {
        qDebug() << "UnPause";
        started = true;
        timer->start(1000/fps);
    }
}


/////////////////////////////////////////////////////////////////////////

