#include "camera.h"
# include <cstdlib>
# include <fstream>
# include <math.h>
# include <string.h>
# include <direct.h>
#include <stdio.h>

std::vector<unsigned> camera::m_vCameraIDs;

std::string GetLastErrorAsString(DWORD errorMessageID)
{
    //Get the error message, if any.
    if(errorMessageID == 0)
        return "No error message has been recorded";

    LPSTR messageBuffer = NULL;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}


const char *camera::dllname="g1ccd.dll";

camera::camera(unsigned (* selectCameraCallback)(std::vector<unsigned>&))
{
    handle=NULL;
    this->last=0;
    LastIndex=0;
    TakeVideo=0;
    VideoPath= new char[256];
    Widh =656;
    Lengh = 494;
    Ex=0;
    Yp=0;
    DWORD lastError=0;
    std::string errStr;
    if (InitFunctions(lastError)==1){
        camera::m_vCameraIDs.clear();
        camera::m_vCameraIDs.push_back(10);
        camera::m_vCameraIDs.push_back(100);
        void (__cdecl * EnumCamerasCallBack)(unsigned) = nullptr;
        EnumCamerasCallBack = [](unsigned id)
        {
            camera::m_vCameraIDs.push_back(id);
        };
        f_Enum(EnumCamerasCallBack);
        if ( camera::m_vCameraIDs.size() == 1 )
        {
            Iden = camera::m_vCameraIDs[0];
        }
        else if ( selectCameraCallback && camera::m_vCameraIDs.size() )
        {
            Iden = selectCameraCallback(camera::m_vCameraIDs);
        }
        else if (camera::m_vCameraIDs.size() )
        {
            Iden = camera::m_vCameraIDs[0];
        }
        else
        {
            const char* chr = "No Camera found.";
            throw chr;
        }
        handle = Initi(Iden); //get handle
        if ((int64_t)handle != -1){
            Fan(handle,1); //turn on fan
        } else{
            const char* chr = "Camera not found.";
            throw chr;
        }
    }
    else{
        errStr=GetLastErrorAsString(lastError);
        char* chr = strdup(errStr.c_str());
        throw chr;
        free(chr);
    }
}

camera::~camera(){
    delete VideoPath;
    Fan(handle,0);
    Rele(handle);
    FreeLibrary(LoadMe);
}

int camera::InitFunctions(DWORD &errCo) //load library and associate defined function with address space in dll
{
    //SetDllDirectoryA("C:/Users/FZU/Desktop/Testing/");
    LoadMe = LoadLibraryA("g1ccd.dll");
    errCo = GetLastError();
    if (LoadMe != 0){
        f_Enum = (Enumerate)GetProcAddress(LoadMe, "Enumerate");
        Initi = (Initialize)GetProcAddress(LoadMe,"Initialize");
        Rele = (Releaze)GetProcAddress(LoadMe,"Release");
        Fan = (SetFan)GetProcAddress(LoadMe,"SetFan");
        GetImageExp = (GetImageExposure)GetProcAddress(LoadMe,"GetImageExposure");
        getIm = (GetImage)GetProcAddress(LoadMe,"GetImage");
        typedef BOOLEAN (__cdecl *GetImage)(CCamera *PCamera, int x, int y, int w, int d, unsigned BufferLen, ADDRESS BufferAdr);
        return 1;
    } //if LoadMe
    else
    {
        return 0;
    }
}

void camera::takeFrame(double expo, void* Pimg, unsigned BuffLen, double* Ptime)
{
    double diff1;
    clock_t t1; //only for timing purpose
    std::fstream ImgFile;
    std::fstream timeFile;
    char filename[256];
    char filename2[256];
    unsigned short * tmp;
    tmp = static_cast<unsigned short*> (Pimg);
    succ = 0;
    succ = GetImageExp(handle, expo, 0,Ex,Yp,Widh,Lengh,BuffLen,Pimg); //fills Buffer with raw image dat
    t1=clock();
    diff1=(((double)t1-(double)last)/CLOCKS_PER_SEC);
    if (succ!=0){
        if (this->last==0){
            *Ptime=0; //returns 0 if it is the first image
            this->last=t1;
        }
        else{
            *Ptime = diff1; //return time since last image has been initialised
            last=t1;
        }
        if (TakeVideo !=0){
            sprintf_s(filename,"%s\\frame%06i.rid",VideoPath, LastIndex);
            ImgFile.open(filename, std::ios::out | std::ios::binary);
            ImgFile.seekp(0);
            ImgFile.write((char*)Pimg, BuffLen);
            ImgFile.close();
            sprintf_s(filename2,"%s\\time.txt",VideoPath);
            timeFile.open(filename2,std::ios_base::app);
            timeFile << *Ptime << "\n";
            timeFile.close();
            LastIndex++;
        }
        }
        return;
}

void camera::setVideoPath(char *path){
    sprintf_s(VideoPath,256,"%s",path);
}

void camera::resetVideoCounters()
{
    LastIndex=0;
    last=0;
}

void camera::videoOnOff(int video){
    TakeVideo=video;
}
