
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  Sample Application
//
//  Arturo Cepeda
//
////////////////////////////////////////////////////////////////////////

#ifndef _STATESAMPLE_H_
#define _STATESAMPLE_H_

#include "States/GEState.h"
#include "Core/GEThreads.h"
#include "modus.h"

#define LOWEST_NOTE     21
#define SCORES          2

#ifdef __APPLE__
#include "mxsoundgenopenal.h"
typedef MCSoundGenOpenAL MCSoundGenNative;
#else
#include "mxsoundgenopensl.h"
typedef MCSoundGenOpenSL MCSoundGenNative;
#endif

class GEStateSampleThreads
{
public:
   static GEThreadFunction(LoadSamplesThread);
   static GEThreadFunction(MusicTimerThread);
};

class GEStateSampleCallbacks
{
public:
   static void SampleLoaded(unsigned int TotalSamples, unsigned int Loaded, void* Data);
   static void TimerTick(const MSTimePosition& TimePosition, void* Data);
   static void PlayNote(unsigned int Instrument, const MSNote& Note, void* Data);
   static void ReleaseNote(unsigned int Instrument, const MSNote& Note, void* Data);
   static void Damper(unsigned int Instrument, bool DamperState, void* Data);
};

class GEStateSample : public GEState
{
private:
   // sprites
   GESprite* cSpriteUnused;
   GESprite* cSpriteKeyWhite;
   GESprite* cSpriteKeyBlack;
   GESprite* cSpriteKeyPress;  
   GESprite* cSpriteKeyPressBack;
   GESprite* cSpritePedalOn;
   GESprite* cSpritePedalOff;
   GESprite* cSpritePedal;
   GESprite* cSpriteLoading;
   
   // labels
   GELabel* cTextModus;
   GELabel* cTextLoading;
   GELabel* cTextPlaying;
   
   // textures
   struct
   {
      enum {KeyWhite, KeyBlack, KeyPress, KeyPressBack, PedalOn, PedalOff, Loading};      
   } 
   Textures;
   
   // sound system
   GEAudio* cAudio;
   
   // Modus objects
   MCInstrument* mPiano;
   MCScore mScore[SCORES];
   MCSoundGenAudio* mSoundGen;
   
   // sample-pack
   unsigned int iTotalSamples;
   unsigned int iSamplesLoaded;
   
   // piano status
   unsigned char iIntensity[88];
   bool bDamper;
   int iCurrentScore;
   
   // music threads
   GEThread pMusicTimerThread;
   GEThread pLoadSamples;
   
public:
   GEStateSample(GERendering* Render, GEAudio* Audio, void* GlobalData);
   
   void internalInit();
   void update(float DeltaTime);
   void render();
   void release();
   
   void updateSamplesLoaded(unsigned int TotalSamples, unsigned int Loaded);

   MCSoundGenAudio* getSoundGen();
   
   void setIntensity(unsigned char Pitch, unsigned char Intensity);
   void setDamper(bool On);
   
   void inputTouchBegin(int ID, const GEVector2& Point);
};

#endif
