
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

using namespace GE;
using namespace GE::States;
using namespace GE::Rendering;
using namespace GE::Audio;

class StateSampleThreads
{
public:
   static GEThreadFunction(LoadSamplesThread);
   static GEThreadFunction(MusicTimerThread);
};

class StateSampleCallbacks
{
public:
   static void SampleLoaded(unsigned int TotalSamples, unsigned int Loaded, void* Data);
   static void TimerTick(const MSTimePosition& TimePosition, void* Data);
   static void PlayNote(unsigned int Instrument, const MSNote& Note, void* Data);
   static void ReleaseNote(unsigned int Instrument, const MSNote& Note, void* Data);
   static void Damper(unsigned int Instrument, bool DamperState, void* Data);
};

class StateSample : public State
{
private:
   // sprites
   Sprite* cSpriteUnused;
   Sprite* cSpriteKeyWhite;
   Sprite* cSpriteKeyBlack;
   Sprite* cSpriteKeyPress;  
   Sprite* cSpriteKeyPressBack;
   Sprite* cSpritePedalOn;
   Sprite* cSpritePedalOff;
   Sprite* cSpritePedal;
   Sprite* cSpriteLoading;
   
   // labels
   Label* cTextModus;
   Label* cTextLoading;
   Label* cTextPlaying;
   
   // textures
   struct
   {
      enum {KeyWhite, KeyBlack, KeyPress, KeyPressBack, PedalOn, PedalOff, Loading};      
   } 
   Textures;
   
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
   StateSample(RenderSystem* Render, AudioSystem* Audio, void* GlobalData);
   
   virtual void internalInit();
   virtual void update(float DeltaTime);
   virtual void render();
   virtual void release();

   virtual void pause();
   virtual void resume();
   
   void updateSamplesLoaded(unsigned int TotalSamples, unsigned int Loaded);

   MCSoundGenAudio* getSoundGen();
   
   void setIntensity(unsigned char Pitch, unsigned char Intensity);
   void setDamper(bool On);
   
   virtual void inputTouchBegin(int ID, const Vector2& Point);
};

#endif
