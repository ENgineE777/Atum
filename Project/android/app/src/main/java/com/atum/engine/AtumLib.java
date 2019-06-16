
package com.atum.engine;

import android.content.res.AssetManager;
import android.util.Log;

public class AtumLib
{
     public static AtumLib inst;

     public native void Init();
     public native void Resize(int width, int height);
     public native void Update();
     public native void SetAssetManager(AssetManager manager);
     public native void TouchStart(int index, int x, int y);
     public native void TouchUpdate(int index, int x, int y);
     public native void TouchEnd(int index);
     public native void OnPause();
     public native void OnResume();
     public native void OnDestroy();
}
