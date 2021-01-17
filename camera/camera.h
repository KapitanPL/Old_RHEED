#ifndef CAMERA_H
#define CAMERA_H

# include <windows.h>
#include <iostream> //basic io
# include <time.h>
#include <vector>

typedef unsigned char  BOOLEAN;
typedef void *         ADDRESS;
//cardinal is unsigned

//types definition for comunication with dll
struct CCamera; //Handle to denote Camera

typedef void (__cdecl *Enumerate)( void (__cdecl callbackProc(unsigned) ) );
typedef CCamera * (__cdecl *Initialize)(unsigned id); //Initialise Camera and return handle
typedef void (__cdecl *Releaze)(CCamera *PCamera); //Disconect Camera
typedef void (__cdecl *SetFan)(CCamera *PCamera, BOOLEAN OnOff); //Sets the cooling fan on or off.
typedef BOOLEAN (__cdecl *GetImageExposure)(CCamera *PCamera, double ExpTime, BOOLEAN UseShutter, int x, int y, int w, int d, unsigned BufferLen, ADDRESS BufferAdr); //function that asks camera to take a picture
typedef BOOLEAN (__cdecl *GetImage)(CCamera *PCamera, int x, int y, int w, int d, unsigned BufferLen, ADDRESS BufferAdr); //function that asks camera to take a picture without exposure time


class camera
{
public:
    camera( unsigned (* selectCameraCallback)(std::vector<unsigned>&) = nullptr);
    ~camera();
    void takeFrame(double expo, void* Pimg, unsigned BuffLen, double* Ptime);
    void setVideoPath(char *path);
    void resetVideoCounters();
    void videoOnOff(int video);
    int Widh; //chip width in pixel
    int Lengh; //chip length  in pixel
    static std::vector<unsigned> m_vCameraIDs;
private:
    //pointers to functions and dlls
    Enumerate f_Enum = nullptr;
    Initialize Initi = nullptr;
    Releaze Rele = nullptr;
    SetFan Fan = nullptr;
    GetImageExposure GetImageExp = nullptr;
    GetImage getIm = nullptr;
    HINSTANCE LoadMe;
    //priate variables
    CCamera *handle = nullptr;
    BOOLEAN succ;
    BOOLEAN OnOff;
    clock_t last;
    int LastIndex;
    char DirectoryPath[300];
    int TakeVideo;
    char *VideoPath;
    static const char* dllname;
    unsigned Iden = 3160; //Camera ID for not to scan through devices
    int Ex; //start point
    int Yp;
    int InitFunctions(DWORD &errCo);
};

#endif // CAMERA_H
