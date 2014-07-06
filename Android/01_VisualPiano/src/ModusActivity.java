
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  Sample Application
//
//  Arturo Cepeda
//
////////////////////////////////////////////////////////////////////////

package com.Modus.VisualPiano;

import android.util.Log;
import android.os.Bundle;
import android.app.Activity;
import android.view.View;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.view.WindowManager;
import android.view.View.OnTouchListener;
import android.content.res.AssetManager;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import java.io.File;

public class ModusActivity extends Activity implements SensorEventListener
{
   private static final boolean UseAccelerometer = false;

   @Override
   protected void onCreate(Bundle savedInstanceState)
   {
      super.onCreate(savedInstanceState);
      CreateMainView();
      CreateAssetManager();

      if(UseAccelerometer)
         InitializeAccelerometer();
   }

   private void CreateMainView()
   {
      view = new ModusView(getApplication());
      setContentView(view);
   }

   private void CreateAssetManager()
   {
      assetManager = getAssets();			
      ModusLib.CreateAssetManager(assetManager);
   }

   static AssetManager assetManager;
   ModusView view;
   
   private void InitializeAccelerometer()
   {
      mSensorManager = (SensorManager)getSystemService(Context.SENSOR_SERVICE);
      mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
      mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_FASTEST);
   }
   
   private SensorManager mSensorManager;
   private Sensor mAccelerometer;

   @Override
   protected void onPause()
   {
      super.onPause();
      view.onPause();

      if(UseAccelerometer)
         mSensorManager.unregisterListener(this);
   }

   @Override
   protected void onResume()
   {
      super.onResume();
      view.onResume();

      if(UseAccelerometer)
         mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
   }

   @Override
   public boolean onKeyDown(final int keyCode, KeyEvent event)
   {
      if(!keyPressed[keyCode])
      {
         keyPressed[keyCode] = true;
         ModusLib.InputButtonDown(keyCode);
      }

      return super.onKeyDown(keyCode, event);
   }

   private boolean keyPressed[] = new boolean[256];

   @Override
   public boolean onKeyUp(final int keyCode, KeyEvent event)
   {
      if(keyPressed[keyCode])
      {
         keyPressed[keyCode] = false;
         ModusLib.InputButtonUp(keyCode);
      }

      return super.onKeyUp(keyCode, event);
   }

   @Override
   public void onAccuracyChanged(Sensor sensor, int accuracy) 
   {
   }

   @Override
   public final void onSensorChanged(SensorEvent event)
   {
      ModusLib.UpdateAccelerometerStatus(event.values[0], event.values[1], event.values[2]);
   }

   public ModusActivity()
   {
      ModusLib.LoadSharedLibraries();		
   }	
}
