import os, os.path, sys, wx, stat, shutil
import subprocess as sp
import traceback as tb

opj = os.path.join

global FILES_DIR, THUMB_DIR

MPLAYER_DIR = "mplayer"  # change if needed
CONVERT_DIR = "convert"  # imagemagick's convert

wx.InitAllImageHandlers()



def __GetThumbName(fullPath, fileType):
    fileSize = os.stat(fullPath).st_size
    (root, ext) = os.path.splitext( os.path.basename(fullPath) )
    thumbName = root+str(fileSize)+ext        #construct the thumb name

    # video files have .jpg tacked onto the end
    thumbName += ".jpg"
            
    return opj( THUMB_DIR, thumbName )



    ### change the permission of the settings file to allow everyone to write to it
def __SetWritePermissions(*files):
    for filePath in files:
        try:
            flags = stat.S_IWUSR | stat.S_IRUSR | stat.S_IWGRP | stat.S_IRGRP | stat.S_IROTH
            os.chmod(filePath, flags)
        except:
            print str(sys.exc_info()[0])+" "+str(sys.exc_info()[1])+" "+str(sys.exc_info()[2])



def makeVideoThumb(fullPath, thumbPath):
    print "\n", 80*"-", "\ncreating video thumbnail with command: "
    # must create two frames because of mplayer bug
    createCmd = MPLAYER_DIR+" -vo jpeg -quiet -frames 2 -ss 5 "+ fullPath
    print createCmd
    try:
        sp.check_call(createCmd.split())
    except:
        print "".join(tb.format_exception(sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2]))
        
    # delete one of them
    try:
        os.remove("00000001.jpg")
    except:
        print "".join(tb.format_exception(sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2])) 

    # resize it to 300x300
    try:
        im = wx.Image("00000002.jpg")  # read the original image
        if im.Ok():  #the image may be corrupted...
            im.Rescale(300, 300)        # resize it
            im.SaveFile("00000002.jpg", wx.BITMAP_TYPE_JPEG)    # save it back to a file
    except: 
        print "".join(tb.format_exception(sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2]))
        
    # rename the right one
    try:
        shutil.move("00000002.jpg", thumbPath)
    except:
        print "".join(tb.format_exception(sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2])) 
    


def makePDFThumb(fullPath, thumbPath):
    print "\n", 80*"-", "\ncreating PDF thumbnail with command: "
    createCmd = CONVERT_DIR+" -thumbnail 300x300 "+fullPath+"[0] "+thumbPath
    print createCmd
    try:
        sp.check_call(createCmd.split())
    except:
        print "".join(tb.format_exception(sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2]))
         


def makeImageThumb(fullPath, thumbPath):
    def __GetWxBitmapType(name):
        ext = os.path.splitext(name)[1]
        ext = ext.lower()
        if ext==".jpg": return wx.BITMAP_TYPE_JPEG
        elif ext==".png": return wx.BITMAP_TYPE_PNG
        elif ext==".bmp": return wx.BITMAP_TYPE_BMP
        elif ext==".gif": return wx.BITMAP_TYPE_GIF
        elif ext==".pcx": return wx.BITMAP_TYPE_PCX
        elif ext==".pnm": return wx.BITMAP_TYPE_PNM
        elif ext==".tif": return wx.BITMAP_TYPE_TIF
        elif ext==".tiff": return wx.BITMAP_TYPE_TIF
        elif ext==".xpm": return wx.BITMAP_TYPE_XPM
        elif ext==".ico": return wx.BITMAP_TYPE_ICO
        elif ext==".cur": return wx.BITMAP_TYPE_CUR
        else:
            return False

    try:
        wxImageType = __GetWxBitmapType(os.path.basename(fullPath))
        if wxImageType:     # some types just cant be saved by wxPython
            # write the thumb but name it with a filename and filesize so that they are unique
            #thumbPath = __GetThumbName(fullPath, "image")           
            im = wx.Image(fullPath)  # read the original image

            if im.Ok():  #the image may be corrupted...
                im.Rescale(300, 300)        # resize it
                im.SaveFile(thumbPath, wx.BITMAP_TYPE_JPEG)    # save it back to a file
    except:
        print "".join(tb.format_exception(sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2]))



def makeThumbnail(fullPath, fileType, thumbPath):
    if not os.path.exists(thumbPath):
        if fileType == "image":
            makeImageThumb(fullPath, thumbPath)
        elif fileType == "video":
            makeVideoThumb(fullPath, thumbPath)
        elif fileType == "pdf":
            makePDFThumb(fullPath, thumbPath)
        else:
            print "\nERROR:Don't know how to make thumbnail for:", fileType

        if os.path.exists(thumbPath):
            __SetWritePermissions(thumbPath)
            

def main():
    for d in os.listdir(FILES_DIR):
        if d=="image" or d=="video" or d=="pdf":
            for root, dirs, files in os.walk(opj(FILES_DIR, d)):
                for f in files:
                    fullPath = opj(root, f)
                    thumbPath = __GetThumbName(fullPath, d)
                    if not os.path.exists(thumbPath):
                        makeThumbnail(fullPath, d, thumbPath)


if __name__ == '__main__':
    import sys, os
    global FILES_DIR, THUMB_DIR

    if len(sys.argv) < 2:
        print "USAGE: python makeThumbs.py path_to_file_library"
    else:
        FILES_DIR = sys.argv[1]
        if os.path.exists(FILES_DIR):
            THUMB_DIR = opj(FILES_DIR, "thumbnails")
            main()
        else:
            print "Given directory doesn't exist... exiting"
    
