#include "DeckLinkAPI.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// headers for SAGE
#include "sail.h"
#include "misc.h"


void	print_input_modes (IDeckLink* deckLink);
void	print_output_modes (IDeckLink* deckLink);

void	print_input_capabilities (IDeckLink* deckLink);
void	print_output_capabilities (IDeckLink* deckLink);

void	print_capabilities();
void	capture(int device, int mode, int connection);

sail sageInf; // sail object
void *PixelFrame;

// 1080i
int winWidth = 1920;
int winHeight = 1080;

// 720p
//int winWidth = 1280;
//int winHeight = 720;

// PAL
//int winWidth = 720;
//int winHeight = 576;

// NTSC
//int winWidth = 720;
//int winHeight = 486;



int main (int argc, char** argv)
{
    int done = 0;
    

	if (argc == 1) {
		fprintf(stderr, "Decklink Capture Program\n");
	}
	else {
		if ( (argc==2) && (!strcmp(argv[1],"-h")) ) {
			fprintf(stderr, "Usage> %s \n", argv[0]);
			fprintf(stderr, "\t\t\t -l\tlist capabilities\n");
			fprintf(stderr, "\t\t\t -i input mode\tSelect a capture settings\n");
		}
		if ( (argc==2) && (!strcmp(argv[1],"-l")) ) {
			print_capabilities();
		}
		if ( (argc==4) && (!strcmp(argv[1],"-i")) ) {

		int card = 0;
		int mode = atoi(argv[2]);
		int connexion = atoi(argv[3]);

                // SAGE setup
            sailConfig cfg;
            cfg.init("decklinkcapture.conf");   // every app has a config file named "appName.conf"
            std::cout << "SAIL configuration was initialized by decklinkcapture.conf" << std::endl;
	 
            cfg.setAppName("decklinkcapture");
            cfg.rank = 0;
            cfg.resX = winWidth;
            cfg.resY = winHeight;
            cfg.winWidth = winWidth;
            cfg.winHeight = winHeight;

            sageRect renderImageMap;
            renderImageMap.left = 0.0;
            renderImageMap.right = 1.0;
            renderImageMap.bottom = 0.0;
            renderImageMap.top = 1.0;

            cfg.imageMap = renderImageMap;
            cfg.pixFmt = PIXFMT_YUV;
            cfg.rowOrd = TOP_TO_BOTTOM;
            cfg.master = true;
	 		 
            sageInf.init(cfg);
            std::cout << "sail initialized " << std::endl;
            
                // Capture setup
		capture(card, mode, connexion);
            //capture(0, 3, 1); // 0: card0, 3: 1080i 50, 1: HDMI
            //capture(0, 5, 1); // HD 1080i 59.94
            //capture(0, 4, 2); // HD 1080i 59.94 Component
            //capture(0, 5, 1); // HD 1080i 60
            //capture(0, 2, 1); // PAL
            //capture(0, 0, 1); // NTSC
 
                // Just wait and process messages
            while (!done) {
                sageMessage msg;
                
                if (sageInf.checkMsg(msg, false) > 0) {
                    switch (msg.getCode()) {
                        case APP_QUIT : {
                            exit(0);
                            break;
                        }
                    }
                }   
                sleep(1);
            }
            
		}
	}

	return 0;
}

class VideoDelegate : public IDeckLinkInputCallback
{
private:
	int32_t mRefCount;
	double  lastTime;
    int     framecount;
    
public:
    VideoDelegate () {
         framecount = 0;
    };
						
	virtual HRESULT		QueryInterface (REFIID iid, LPVOID *ppv)
        {};
	virtual ULONG		AddRef (void) {
        return mRefCount++;
    };
	virtual ULONG		Release (void) {
        int32_t		newRefValue;
        
        newRefValue = mRefCount--;
        if (newRefValue == 0)
        {
            delete this;
            return 0;
        }        
        return newRefValue;
    };

	virtual HRESULT	VideoInputFrameArrived (IDeckLinkVideoInputFrame* arrivedFrame,
                                                IDeckLinkAudioInputPacket*);
        virtual HRESULT VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags)
        {};

};

HRESULT	VideoDelegate::VideoInputFrameArrived (IDeckLinkVideoInputFrame* arrivedFrame,
                                               IDeckLinkAudioInputPacket*)
{
		BMDTimeValue		frameTime, frameDuration;
		int					hours, minutes, seconds, frames;
		HRESULT				theResult;
		
		arrivedFrame->GetStreamTime(&frameTime, &frameDuration, 600);
		
		hours = (frameTime / (600 * 60*60));
		minutes = (frameTime / (600 * 60)) % 60;
		seconds = (frameTime / 600) % 60;
		frames = (frameTime / 6) % 100;
		//fprintf(stderr, "frame> %02d:%02d:%02d:%02d", hours, minutes, seconds, frames);
		//fprintf(stderr, "\t %dx%d\r", arrivedFrame->GetWidth(), arrivedFrame->GetHeight() );

        arrivedFrame->GetBytes(&PixelFrame);
        unsigned char *rgbBuffer = (unsigned char *)sageInf.getBuffer();
        memcpy(rgbBuffer, PixelFrame, arrivedFrame->GetWidth()*arrivedFrame->GetHeight()*2);
        sageInf.swapBuffer();
    
#if 0
        char fn[128];
        memset(fn, 0, 128);
        sprintf(fn, "frame%04d.yuv", framecount);
        FILE *f=fopen(fn, "w+");
        fwrite(PixelFrame, 1, 1920*1080*2, f);
        fclose(f);
#endif   

        framecount++;
        
        return S_OK;

}


void capture(int device, int mode, int connection)
{
    int dnum, mnum, cnum;
 	int	itemCount;
   
	IDeckLinkIterator *deckLinkIterator;
	IDeckLink         *deckLink;
	HRESULT            result;

	IDeckLinkInput*               deckLinkInput = NULL;
	IDeckLinkDisplayModeIterator* displayModeIterator = NULL;
	IDeckLinkDisplayMode*         displayMode = NULL;
	IDeckLinkConfiguration       *deckLinkConfiguration = NULL;
    VideoDelegate                *delegate;

//     PixelFrame = (void*)malloc(1920*1080*2);
//     memset(PixelFrame, 0, 1920*1080*2);;
    

    fprintf(stderr, "Starting> Capture on device %d, mode %d, connection %d\n", device, mode, connection);
    
        // Create an IDeckLinkIterator object to enumerate all DeckLink cards in the system
	deckLinkIterator = CreateDeckLinkIteratorInstance();
	if (deckLinkIterator == NULL)
	{
		fprintf(stderr, "A DeckLink iterator could not be created (DeckLink drivers may not be installed).\n");
		return;
	}

    dnum = 0;
    deckLink = NULL;
    
	while (deckLinkIterator->Next(&deckLink) == S_OK)
	{
        if (device != dnum) {
            dnum++;
                // Release the IDeckLink instance when we've finished with it to prevent leaks
            deckLink->Release();
            continue;
        }
        dnum++;
        
		const char *deviceNameString = NULL;
		
            // *** Print the model name of the DeckLink card
		result = deckLink->GetModelName(&deviceNameString);
		if (result == S_OK)
		{
			char deviceName[64];	
			fprintf(stderr, "Using device [%s]\n", deviceNameString);

                // Query the DeckLink for its configuration interface
            result = deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&deckLinkInput);
            if (result != S_OK)
            {
                fprintf(stderr, "Could not obtain the IDeckLinkInput interface - result = %08x\n", result);
                return;
            }

                // Obtain an IDeckLinkDisplayModeIterator to enumerate the display modes supported on input
            result = deckLinkInput->GetDisplayModeIterator(&displayModeIterator);
            if (result != S_OK)
            {
                fprintf(stderr, "Could not obtain the video input display mode iterator - result = %08x\n", result);
                return;
            }

            mnum = 0;
            while (displayModeIterator->Next(&displayMode) == S_OK)
            {
                if (mode != mnum) {
                    mnum++;
                        // Release the IDeckLinkDisplayMode object to prevent a leak
                    displayMode->Release();
                    continue;
                }
                mnum++;                
     
                const char *displayModeString = NULL;
                
                result = displayMode->GetName(&displayModeString);
                if (result == S_OK)
                {
                    BMDPixelFormat pf = bmdFormat8BitYUV;

                    fprintf(stderr, "Stopping previous streams, if needed\n");
                    deckLinkInput->StopStreams();

                    const char *displayModeString = NULL;		
                    displayMode->GetName(&displayModeString);
                    fprintf(stderr, "Enable video input: %s\n", displayModeString);

                    deckLinkInput->EnableVideoInput(displayMode->GetDisplayMode(), pf, 0);



                    // Query the DeckLink for its configuration interface
                    result = deckLinkInput->QueryInterface(IID_IDeckLinkConfiguration, (void**)&deckLinkConfiguration);
                    if (result != S_OK)
                    {
                        fprintf(stderr, "Could not obtain the IDeckLinkConfiguration interface: %08x\n", result);
                        return;
                    }
                    BMDVideoConnection conn;
                    switch (connection) {
                        case 0:
                            conn = bmdVideoConnectionSDI;
                            break;
                        case 1:
                            conn = bmdVideoConnectionHDMI;
                            break;
                        case 2:
                            conn = bmdVideoConnectionComponent;
                            break;
                        case 3:
                            conn = bmdVideoConnectionComposite;
                            break;
                        case 4:
                            conn = bmdVideoConnectionSVideo;
                            break;
                        case 5:
                            conn = bmdVideoConnectionOpticalSDI;
                            break;
                        default:
                            break;
                    }
                    
                    if (deckLinkConfiguration->SetVideoInputFormat(conn) == S_OK) {
                        fprintf(stderr, "Input set to: %d\n", connection);
                    }
                    
                    delegate = new VideoDelegate();
                    deckLinkInput->SetCallback(delegate);
                    
                    fprintf(stderr, "Start capture\n", connection);
                    deckLinkInput->StartStreams();

               }

            }
	
		}
	}


        // Release the IDeckLink instance when we've finished with it to prevent leaks
    if (deckLink) deckLink->Release();
        // Ensure that the interfaces we obtained are released to prevent a memory leak
	if (displayModeIterator != NULL)
		displayModeIterator->Release();

   return;
}
    

void print_capabilities()
{
	IDeckLinkIterator* deckLinkIterator;
	IDeckLink*         deckLink;
	int                numDevices = 0;
	HRESULT            result;
	
        // Create an IDeckLinkIterator object to enumerate all DeckLink cards in the system
	deckLinkIterator = CreateDeckLinkIteratorInstance();
	if (deckLinkIterator == NULL)
	{
		fprintf(stderr, "A DeckLink iterator could not be created (DeckLink drivers may not be installed).\n");
		return;
	}
	
        // Enumerate all cards in this system
	printf("Device(s):\n");
	while (deckLinkIterator->Next(&deckLink) == S_OK)
	{
		const char *		deviceNameString = NULL;
		
            // *** Print the model name of the DeckLink card
		result = deckLink->GetModelName(&deviceNameString);
		if (result == S_OK)
		{
			char			deviceName[64];
			
			printf("\t%3d: =============== %s ===============\n", numDevices, deviceNameString);
		}
		
            // ** List the video output display modes supported by the card
		print_input_modes(deckLink);
		
            // ** List the input capabilities of the card
		print_input_capabilities(deckLink);
		
            // Release the IDeckLink instance when we've finished with it to prevent leaks
		deckLink->Release();

            // Increment the total number of DeckLink cards found
		numDevices++;
	}
	
	
        // If no DeckLink cards were found in the system, inform the user
	if (numDevices == 0)
		printf("No Blackmagic Design devices were found.\n");
	printf("\n");
}



void print_input_modes (IDeckLink* deckLink)
{
	IDeckLinkInput*				deckLinkInput = NULL;
	IDeckLinkDisplayModeIterator*		displayModeIterator = NULL;
	IDeckLinkDisplayMode*			displayMode = NULL;
	HRESULT					result;	
	int mm = 0;
	
        // Query the DeckLink for its configuration interface
	result = deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&deckLinkInput);
	if (result != S_OK)
	{
		fprintf(stderr, "Could not obtain the IDeckLinkInput interface - result = %08x\n", result);
		goto bail;
	}
	
        // Obtain an IDeckLinkDisplayModeIterator to enumerate the display modes supported on input
	result = deckLinkInput->GetDisplayModeIterator(&displayModeIterator);
	if (result != S_OK)
	{
		fprintf(stderr, "Could not obtain the video input display mode iterator - result = %08x\n", result);
		goto bail;
	}
	
        // List all supported output display modes
	printf("Supported video input display modes:\n");
	while (displayModeIterator->Next(&displayMode) == S_OK)
	{
		const char *			displayModeString = NULL;
		
		result = displayMode->GetName(&displayModeString);
		if (result == S_OK)
		{
			char		modeName[64];
			int		modeWidth;
			int		modeHeight;
			BMDTimeValue	frameRateDuration;
			BMDTimeScale	frameRateScale;
			
			
                // Obtain the display mode's properties
			modeWidth = displayMode->GetWidth();
			modeHeight = displayMode->GetHeight();
			displayMode->GetFrameRate(&frameRateDuration, &frameRateScale);
			printf("\t%3d) %-20s \t %d x %d \t %g FPS\n", mm, displayModeString, modeWidth, modeHeight, (double)frameRateScale / (double)frameRateDuration);
			mm++;
		}
		
            // Release the IDeckLinkDisplayMode object to prevent a leak
		displayMode->Release();
	}
	
	printf("\n");
	
  bail:
        // Ensure that the interfaces we obtained are released to prevent a memory leak
	if (displayModeIterator != NULL)
		displayModeIterator->Release();
	
	if (deckLinkInput != NULL)
		deckLinkInput->Release();
}


void	print_output_modes (IDeckLink* deckLink)
{
	IDeckLinkOutput*					deckLinkOutput = NULL;
	IDeckLinkDisplayModeIterator*		displayModeIterator = NULL;
	IDeckLinkDisplayMode*				displayMode = NULL;
	HRESULT								result;	
	
        // Query the DeckLink for its configuration interface
	result = deckLink->QueryInterface(IID_IDeckLinkOutput, (void**)&deckLinkOutput);
	if (result != S_OK)
	{
		fprintf(stderr, "Could not obtain the IDeckLinkOutput interface - result = %08x\n", result);
		goto bail;
	}
	
        // Obtain an IDeckLinkDisplayModeIterator to enumerate the display modes supported on output
	result = deckLinkOutput->GetDisplayModeIterator(&displayModeIterator);
	if (result != S_OK)
	{
		fprintf(stderr, "Could not obtain the video output display mode iterator - result = %08x\n", result);
		goto bail;
	}
	
        // List all supported output display modes
	printf("Supported video output display modes:\n");
	while (displayModeIterator->Next(&displayMode) == S_OK)
	{
		const char *			displayModeString = NULL;
		
		result = displayMode->GetName(&displayModeString);
		if (result == S_OK)
		{
			char			modeName[64];
			int				modeWidth;
			int				modeHeight;
			BMDTimeValue	frameRateDuration;
			BMDTimeScale	frameRateScale;
			
			
                // Obtain the display mode's properties
			modeWidth = displayMode->GetWidth();
			modeHeight = displayMode->GetHeight();
			displayMode->GetFrameRate(&frameRateDuration, &frameRateScale);
			printf("\t%-20s \t %d x %d \t %g FPS\n", displayModeString, modeWidth, modeHeight, (double)frameRateScale / (double)frameRateDuration);
		}
		
            // Release the IDeckLinkDisplayMode object to prevent a leak
		displayMode->Release();
	}
	
	printf("\n");
	
  bail:
        // Ensure that the interfaces we obtained are released to prevent a memory leak
	if (displayModeIterator != NULL)
		displayModeIterator->Release();
	
	if (deckLinkOutput != NULL)
		deckLinkOutput->Release();
}

void	print_input_capabilities (IDeckLink* deckLink)
{
	IDeckLinkConfiguration*		deckLinkConfiguration = NULL;
	IDeckLinkConfiguration*		deckLinkValidator = NULL;
	int							itemCount;
	HRESULT						result;	
	int mm = 0;
	
        // Query the DeckLink for its configuration interface
	result = deckLink->QueryInterface(IID_IDeckLinkConfiguration, (void**)&deckLinkConfiguration);
	if (result != S_OK)
	{
		fprintf(stderr, "Could not obtain the IDeckLinkConfiguration interface - result = %08x\n", result);
		goto bail;
	}
	
        // Obtain a validator object from the IDeckLinkConfiguration interface.
        // The validator object implements IDeckLinkConfiguration, however, all configuration changes are ignored
        // and will not take effect.  However, you can use the returned result code from the validator object
        // to determine whether the card supports a particular configuration.
	
	result = deckLinkConfiguration->GetConfigurationValidator(&deckLinkValidator);
	if (result != S_OK)
	{
		fprintf(stderr, "Could not obtain the configuration validator interface - result = %08x\n", result);
		goto bail;
	}
	
        // Use the validator object to determine which video input connections are available
	printf("Supported video input connections:\n  ");
	itemCount = 0;
	if (deckLinkValidator->SetVideoInputFormat(bmdVideoConnectionSDI) == S_OK)
	{
		printf("\t%3d) SDI\n", itemCount);
	}
    itemCount++;
	if (deckLinkValidator->SetVideoInputFormat(bmdVideoConnectionHDMI) == S_OK)
	{
		printf("\t%3d) HDMI\n", itemCount);
	}
    itemCount++;
	if (deckLinkValidator->SetVideoInputFormat(bmdVideoConnectionComponent) == S_OK)
	{
		printf("\t%3d) Component\n", itemCount);
	}
    itemCount++;
	if (deckLinkValidator->SetVideoInputFormat(bmdVideoConnectionComposite) == S_OK)
	{
		printf("\t%3d) Composite\n", itemCount);
	}
    itemCount++;
	if (deckLinkValidator->SetVideoInputFormat(bmdVideoConnectionSVideo) == S_OK)
	{
		printf("\t%3d) S-Video\n", itemCount);
	}
    itemCount++;
	if (deckLinkValidator->SetVideoInputFormat(bmdVideoConnectionOpticalSDI) == S_OK)
	{
		printf("\t%3d) Optical SDI\n", itemCount);
	}
	
	printf("\n");
	
  bail:
	if (deckLinkValidator != NULL)
		deckLinkValidator->Release();
	
	if (deckLinkConfiguration != NULL)
		deckLinkConfiguration->Release();
}

void	print_output_capabilities (IDeckLink* deckLink)
{
	IDeckLinkConfiguration*		deckLinkConfiguration = NULL;
	IDeckLinkConfiguration*		deckLinkValidator = NULL;
	int							itemCount;
	HRESULT						result;	
	
	// Query the DeckLink for its configuration interface
	result = deckLink->QueryInterface(IID_IDeckLinkConfiguration, (void**)&deckLinkConfiguration);
	if (result != S_OK)
	{
		fprintf(stderr, "Could not obtain the IDeckLinkConfiguration interface - result = %08x\n", result);
		goto bail;
	}
	
	// Obtain a validator object from the IDeckLinkConfiguration interface.
	// The validator object implements IDeckLinkConfiguration, however, all configuration changes are ignored
	// and will not take effect.  However, you can use the returned result code from the validator object
	// to determine whether the card supports a particular configuration.
	
	result = deckLinkConfiguration->GetConfigurationValidator(&deckLinkValidator);
	if (result != S_OK)
	{
		fprintf(stderr, "Could not obtain the configuration validator interface - result = %08x\n", result);
		goto bail;
	}
	
	// Use the validator object to determine which video output connections are available
	printf("Supported video output connections:\n  ");
	itemCount = 0;
	if (deckLinkValidator->SetVideoOutputFormat(bmdVideoConnectionSDI) == S_OK)
	{
		if (itemCount++ > 0)
			printf(", ");
		printf("SDI");
	}
	if (deckLinkValidator->SetVideoOutputFormat(bmdVideoConnectionHDMI) == S_OK)
	{
		if (itemCount++ > 0)
			printf(", ");
		printf("HDMI");
	}
	if (deckLinkValidator->SetVideoOutputFormat(bmdVideoConnectionComponent) == S_OK)
	{
		if (itemCount++ > 0)
			printf(", ");
		printf("Component");
	}
	if (deckLinkValidator->SetVideoOutputFormat(bmdVideoConnectionComposite) == S_OK)
	{
		if (itemCount++ > 0)
			printf(", ");
		printf("Composite");
	}
	if (deckLinkValidator->SetVideoOutputFormat(bmdVideoConnectionSVideo) == S_OK)
	{
		if (itemCount++ > 0)
			printf(", ");
		printf("S-Video");
	}
	if (deckLinkValidator->SetVideoOutputFormat(bmdVideoConnectionOpticalSDI) == S_OK)
	{
		if (itemCount++ > 0)
			printf(", ");
		printf("Optical SDI");
	}
	
	printf("\n\n");
	
bail:
	if (deckLinkValidator != NULL)
		deckLinkValidator->Release();
	
	if (deckLinkConfiguration != NULL)
		deckLinkConfiguration->Release();
}


