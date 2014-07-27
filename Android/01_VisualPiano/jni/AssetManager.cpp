
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  Sample Application
//
//  Arturo Cepeda
//
////////////////////////////////////////////////////////////////////////

#include <android/asset_manager_jni.h>
#include "Core/GEDevice.h"
#include "mxsoundgenopensl.h"

using namespace GE::Core;

static AAssetManager* NativeAssetManager = 0;

extern "C"
{
   JNIEXPORT void JNICALL Java_com_Modus_VisualPiano_ModusLib_CreateAssetManager(JNIEnv* env, jclass clazz, jobject assetManager)
   {
      NativeAssetManager = AAssetManager_fromJava(env, assetManager);
   }
};

unsigned int Device::getFileLength(const char* Filename)
{
    AAsset* aAsset = AAssetManager_open(NativeAssetManager, Filename, AASSET_MODE_UNKNOWN);
    
    if(aAsset)
    {
        int iReadBytes = AAsset_getLength(aAsset);
        AAsset_close(aAsset);
        return iReadBytes;
    }

    return 0;
}

unsigned int Device::readFile(const char* Filename, unsigned char* ReadBuffer, unsigned int BufferSize)
{
    AAsset* aAsset = AAssetManager_open(NativeAssetManager, Filename, AASSET_MODE_UNKNOWN);
    
    if(aAsset)
    {
        int iReadBytes = AAsset_read(aAsset, ReadBuffer, BufferSize);
        AAsset_close(aAsset);
        return iReadBytes;
    }

    return 0;
}
