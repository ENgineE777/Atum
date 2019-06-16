
package com.atum.engine;

import android.app.Activity;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import java.io.File;

import static android.view.View.SYSTEM_UI_FLAG_FULLSCREEN;
import static android.view.View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
import static java.security.AccessController.getContext;

public class AtumActivity extends Activity
{
    AtumView main_view;

    @Override protected void onCreate(Bundle icicle)
    {
        System.loadLibrary("atumlib");
        AtumLib.inst = new AtumLib();

        super.onCreate(icicle);

        AtumLib.inst.SetAssetManager(this.getAssets());

        main_view = new AtumView(getApplication());
        main_view.setPreserveEGLContextOnPause(true);

        setContentView(main_view);
    }

    @Override public void onBackPressed()
    {
        moveTaskToBack(true);
    }

    @Override protected void onPause()
    {
        AtumLib.inst.OnPause();
        super.onPause();
        main_view.onPause();
    }

    @Override protected void onResume()
    {
        AtumLib.inst.OnResume();
        super.onResume();
        main_view.onResume();
    }

    @Override protected void onDestroy()
    {
        AtumLib.inst.OnDestroy();
        super.onDestroy();
    }
}
