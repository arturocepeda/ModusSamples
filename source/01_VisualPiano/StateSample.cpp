
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  Sample Application
//
//  Arturo Cepeda
//
////////////////////////////////////////////////////////////////////////


#include "StateSample.h"
#include "Core/GEUtils.h"
#include "Content/GEContentData.h"
#include "Core/GEDevice.h"
#include <iostream>
#include <sstream>


using namespace GE::Core;
using namespace GE::States;
using namespace GE::Content;


//
//  global data
//
MCTimer mTimer;
GEMutex pTimerMutex;
bool bSamplesLoaded = false;
bool bThreadEnd = false;


//
//  StateSampleThreads
//
GEThreadFunction(StateSampleThreads::LoadSamplesThread)
{
   StateSample* cState = (StateSample*)pData;
   MCSoundGenAudio* mSoundGen = cState->getSoundGen();

   ContentData cSamplePackData;
   Device::readContentFile(ContentType::GenericBinaryData, "instruments", "Piano", "msp", &cSamplePackData);
   std::istringstream fStream(std::string(cSamplePackData.getData(), cSamplePackData.getDataSize()));
   mSoundGen->loadSamplePack(fStream, StateSampleCallbacks::SampleLoaded, pData);
   
   GEMutexInit(pTimerMutex);
   mTimer.start();
   bSamplesLoaded = true;
   
   return 0;
}

GEThreadFunction(StateSampleThreads::MusicTimerThread)
{
   while(!bThreadEnd)
   {
      GEMutexLock(pTimerMutex);
      mTimer.update();
      GEMutexUnlock(pTimerMutex);
   }
   
   return 0;
}


//
//  StateSampleCallbacks
//
void StateSampleCallbacks::SampleLoaded(unsigned int TotalSamples, unsigned int Loaded, void* Data)
{
   StateSample* cState = (StateSample*)Data;
   cState->updateSamplesLoaded(TotalSamples, Loaded);
}

void StateSampleCallbacks::TimerTick(const MSTimePosition& TimePosition, void* Data)
{
   MCInstrument* mPiano = (MCInstrument*)Data;
   mPiano->update(TimePosition);
}

void StateSampleCallbacks::PlayNote(unsigned int Instrument, const MSNote& Note, void* Data)
{
   StateSample* cState = (StateSample*)Data;   
   cState->setIntensity(Note.Pitch, Note.Intensity);
}

void StateSampleCallbacks::ReleaseNote(unsigned int Instrument, const MSNote& Note, void* Data)
{
   StateSample* cState = (StateSample*)Data;
   cState->setIntensity(Note.Pitch, 0);
}

void StateSampleCallbacks::Damper(unsigned int Instrument, bool DamperState, void* Data)
{
   StateSample* cState = (StateSample*)Data;
   cState->setDamper(DamperState);
}


//
//  StateSample
//
StateSample::StateSample(RenderSystem* Render, AudioSystem* Audio, void* GlobalData)
    : State(Render, Audio, GlobalData)
{
   // initialize piano status
   memset(iIntensity, 0, 88);
   bDamper = false;
}

void StateSample::internalInit()
{   
   cRender->setBackgroundColor(Color(0.8f, 0.8f, 1.0f));
   cRender->set2D();
   
   iTotalSamples = 0;
   iSamplesLoaded = 0;

   // textures
   cRender->loadTexture(Textures.KeyWhite, "pianokey_white", "png");
   cRender->loadTexture(Textures.KeyBlack, "pianokey_black", "png");
   cRender->loadTexture(Textures.KeyPress, "pianokey_press", "png");
   cRender->loadTexture(Textures.KeyPressBack, "pianokey_press_back", "png");
   cRender->loadTexture(Textures.PedalOn, "pianopedal_on", "png");
   cRender->loadTexture(Textures.PedalOff, "pianopedal_off", "png");
   cRender->loadTexture(Textures.Loading, "note", "png");
   
   // sprites
   cRender->createSprite(&cSpriteUnused);
   cRender->createSprite(&cSpriteKeyWhite);
   cSpriteKeyWhite->setTexture(cRender->getTexture(Textures.KeyWhite));
   cRender->createSprite(&cSpriteKeyBlack);
   cSpriteKeyBlack->setTexture(cRender->getTexture(Textures.KeyBlack));
   cRender->createSprite(&cSpriteKeyPress);
   cSpriteKeyPress->setTexture(cRender->getTexture(Textures.KeyPress));
   cRender->createSprite(&cSpriteKeyPressBack);
   cSpriteKeyPressBack->setTexture(cRender->getTexture(Textures.KeyPressBack));
   cRender->createSprite(&cSpritePedalOn);
   cSpritePedalOn->setTexture(cRender->getTexture(Textures.PedalOn));
   cRender->createSprite(&cSpritePedalOff);
   cSpritePedalOff->setTexture(cRender->getTexture(Textures.PedalOff));
   cSpritePedal = cSpritePedalOff;
   cRender->createSprite(&cSpriteLoading);
   cSpriteLoading->setTexture(cRender->getTexture(Textures.Loading));

   // labels
   cRender->defineFont(0, "Ostrich", 24.0f);
   cRender->createLabel(&cTextModus, 0, Alignment::TopLeft, Vector2(0.035f, 0.035f), "");
   cTextModus->setHorizontalSpacing(0.15f);
   cTextModus->setColor(Color(0.2f, 0.2f, 0.2f));
   cRender->createLabel(&cTextLoading, 0, Alignment::TopLeft, Vector2(0.035f, 0.035f), "");
   cTextLoading->setHorizontalSpacing(0.15f);
   cTextLoading->setColor(Color(0.2f, 0.2f, 0.2f));
   cRender->createLabel(&cTextPlaying, 0, Alignment::TopLeft, Vector2(0.035f, 0.035f),
      "Playing! Touch the screen to change the piece...");
   cTextPlaying->setHorizontalSpacing(0.15f);
   cTextPlaying->setColor(Color(0.2f, 0.2f, 0.2f));
   
   // setup sizes and positions
   cSpriteKeyWhite->setSize(Vector2(0.24f, 0.24f));
   cSpriteKeyBlack->setSize(Vector2(0.24f, 0.24f));
   cSpriteKeyPress->setSize(Vector2(0.24f, 0.24f));
   cSpriteKeyPressBack->setSize(Vector2(0.24f, 0.24f));
   cSpritePedalOn->setSize(Vector2(0.24f, 0.24f));
   cSpritePedalOff->setSize(Vector2(0.24f, 0.24f));
   cSpriteLoading->setSize(Vector2(0.125f, 0.125f));
   
   cTextModus->setPosition(-0.7f, 0.4f);
   cTextLoading->setPosition(-0.7f, 0.1f);
   cTextPlaying->setPosition(-0.7f, 0.1f);
   
   // Modus objects
   MSRange mPianoRange(21, 108);
   mPiano = new MCInstrument(1, mPianoRange, mPianoRange.getSize());   
   mSoundGen = new MCSoundGenNative(1, mPianoRange.getSize(), false);
   mPiano->setSoundGen(mSoundGen);
   
   ContentData cScript;
   Device::readContentFile(ContentType::GenericTextData, "scripts", "score.piano.chopin", "txt", &cScript);
   mScore[0].loadScriptFromString(cScript.getData());
   cScript.unload();

   Device::readContentFile(ContentType::GenericTextData, "scripts", "score.piano.chopin2", "txt", &cScript);
   mScore[1].loadScriptFromString(cScript.getData());
   mScore[1].displace(1);
   cScript.unload();
   
   iCurrentScore = 0;
   mPiano->setScore(&mScore[iCurrentScore]);
   
   mTimer.setCallbackTick(StateSampleCallbacks::TimerTick, mPiano);
   
   // set instrument callbacks
   mPiano->setCallbackPlay(StateSampleCallbacks::PlayNote, this);
   mPiano->setCallbackRelease(StateSampleCallbacks::ReleaseNote, this);
   mPiano->setCallbackDamper(StateSampleCallbacks::Damper, this);
   
   // create music threads
   GEThreadCreate(pLoadSamples, StateSampleThreads::LoadSamplesThread, this);
   GEThreadCreate(pMusicTimerThread, StateSampleThreads::MusicTimerThread, 0);
}

void StateSample::release()
{
   // wait until the music timer thread finishes
   bThreadEnd = true;
   GEThreadWait(pMusicTimerThread);
   GEMutexDestroy(pTimerMutex);
   
   // release Modus objects
   delete mSoundGen;
   delete mPiano;
   
   // release sprites
   delete cSpriteUnused;
   delete cSpriteKeyWhite;
   delete cSpriteKeyBlack;
   delete cSpriteKeyPress;
   delete cSpriteKeyPressBack;
   delete cSpritePedalOn;
   delete cSpritePedalOff;
   delete cSpriteLoading;
   
   // release labels
   delete cTextModus;
   delete cTextLoading;
   delete cTextPlaying;
}

void StateSample::update(float DeltaTime)
{
}

void StateSample::pause()
{
   if(!bSamplesLoaded)
      return;

   mSoundGen->setDamper(false);
   mSoundGen->releaseAllNotes();

   GEMutexLock(pTimerMutex);
   mTimer.stop();
   GEMutexUnlock(pTimerMutex);

   // make sure that all the currently sounding notes are faded-out
   for(uint i = 0; i < M_TICKS_PER_BEAT; i++)
      mSoundGen->update();
}

void StateSample::resume()
{
   if(!bSamplesLoaded)
      return;

   mSoundGen->setDamper(bDamper);

   GEMutexLock(pTimerMutex);
   mTimer.start();
   GEMutexUnlock(pTimerMutex);
}

void StateSample::updateSamplesLoaded(unsigned int TotalSamples, unsigned int Loaded)
{
   iTotalSamples = TotalSamples;
   iSamplesLoaded = Loaded;
}

MCSoundGenAudio* StateSample::getSoundGen()
{
   return mSoundGen;
}

void StateSample::setIntensity(unsigned char Pitch, unsigned char Intensity)
{
   iIntensity[Pitch - LOWEST_NOTE] = Intensity;
}

void StateSample::setDamper(bool On)
{
   bDamper = On;
   cSpritePedal = bDamper? cSpritePedalOn: cSpritePedalOff;
}

void StateSample::render()
{
   // labels
   cRender->useShaderProgram((unsigned int)ShaderPrograms::Text);
   cTextModus->setText("Modus");
   cRender->renderLabel(cTextModus);
   cTextModus->move(0.0f, -0.05f);
   cTextModus->setText("C++ Music Library");
   cRender->renderLabel(cTextModus);
   cTextModus->move(0.0f, -0.05f);
   cTextModus->setText("Sample Application");
   cRender->renderLabel(cTextModus);
   cTextModus->move(0.0f, 0.1f);
  
   if(bSamplesLoaded)
   {
      cRender->renderLabel(cTextPlaying);
   }
   else 
   {
      int iPercentage = std::min(1 + (int)(((float)iSamplesLoaded / iTotalSamples) * 100), 100);

      char sLoadingText[64];
      sprintf(sLoadingText, "Loading audio samples: %d%%", iPercentage);      
      cTextLoading->setText(sLoadingText);
      cRender->renderLabel(cTextLoading);
      
      float fPosY = 0.0f;
      float fPosX = -0.675f;
      cRender->useShaderProgram((unsigned int)ShaderPrograms::HUD);
      
      for(unsigned int i = 1; i <= iPercentage / 5; i++)
      {
         cSpriteLoading->setPosition(fPosX, fPosY);
         cRender->renderSprite(cSpriteLoading);
         fPosX += 0.0675f;
      }
      
      return;
   }
   
   cRender->useShaderProgram((unsigned int)ShaderPrograms::HUD);

   // piano keyboard
   float fPosY = -0.2f;
   float fKeyPosX;
   float fWhiteWidth = 0.03f;
   float fFirstKeyPosX = -fWhiteWidth * 26 + (fWhiteWidth / 2);  // number of white keys: 52, 52/2=26

   int iNote;
   int iOctave;
   int iOffset;
   int i;
   
   // white keys
   for(i = 0; i < 88; i++)
   {
      if(!MCNotes::isNatural(i + LOWEST_NOTE))
         continue;
      
      iNote = (i + LOWEST_NOTE) % 12;
      iOctave = (i + LOWEST_NOTE) / 12 - 2;  // pitch: [21, 108] --> octave: [0, 7]
      iOffset = 2 + (iOctave * 7);           // initial offset 2: the first C is the 3rd key
      
      switch(iNote)
      {
         case D:
            iOffset += 1;
            break;
         case E:
            iOffset += 2;
            break;
         case F:
            iOffset += 3;
            break;
         case G:
            iOffset += 4;
            break;
         case A:
            iOffset += 5;
            break;
         case B:
            iOffset += 6;
            break;
      }
      
      fKeyPosX = fFirstKeyPosX + (fWhiteWidth * iOffset);
      cSpriteKeyWhite->setPosition(fKeyPosX, fPosY);
      cRender->renderSprite(cSpriteKeyWhite);
      
      if(iIntensity[i] > 0)
      {
         cSpriteKeyPress->setPosition(fKeyPosX - 0.002f, fPosY);
         cSpriteKeyPress->setOpacity(iIntensity[i] / 127.0f);
         cRender->renderSprite(cSpriteKeyPress);
      }
   }   
   
   // black keys
   fPosY += 0.027f;
   fFirstKeyPosX = -fWhiteWidth * 26;
   
   for(i = 0; i < 88; i++)
   {
      if(MCNotes::isNatural(i + LOWEST_NOTE))
         continue;
      
      iNote = (i + LOWEST_NOTE) % 12;
      iOctave = (i + LOWEST_NOTE) / 12 - 2;  // pitch: [21, 108] --> octave: [0, 7]
      iOffset = 2 + (iOctave * 7);           // initial offset 2: the first C is the 3rd key
      
      switch(iNote)
      {
         case Cs:
            iOffset += 1;
            break;
         case Ds:
            iOffset += 2;
            break;
         case Fs:
            iOffset += 4;
            break;
         case Gs:
            iOffset += 5;
            break;
         case As:
            iOffset += 6;
            break;
      }
      
      fKeyPosX = fFirstKeyPosX + (fWhiteWidth * iOffset);
      cSpriteKeyBlack->setPosition(fKeyPosX, fPosY);
      cRender->renderSprite(cSpriteKeyBlack);
      
      if(iIntensity[i] > 0)
      {
         cSpriteKeyPressBack->setPosition(fKeyPosX, fPosY + 0.025f);
         cRender->renderSprite(cSpriteKeyPressBack);
         
         cSpriteKeyPress->setPosition(cSpriteKeyPressBack->getPosition());
         cSpriteKeyPress->setOpacity(iIntensity[i] / 127.0f);
         cRender->renderSprite(cSpriteKeyPress);
      }
   }
   
   // piano pedal
   cSpritePedal->setPosition(0.0f, fPosY - 0.114f);
   cRender->renderSprite(cSpritePedal);
}

void StateSample::inputTouchBegin(int ID, const Vector2& Point)
{
   if(!bSamplesLoaded)
      return;
   
   iCurrentScore++;
   
   if(iCurrentScore == SCORES)
      iCurrentScore = 0;

   mPiano->setDamper(false);
   mPiano->releaseAll();
   mPiano->setScore(&mScore[iCurrentScore]);

   GEMutexLock(pTimerMutex);
   mTimer.reset();
   mTimer.start();
   GEMutexUnlock(pTimerMutex);
}
