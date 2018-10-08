
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

        main_view.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                // Hide the nav bar and status bar
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN);
        setContentView(main_view);
    }

    @Override protected void onPause()
    {
        super.onPause();
        main_view.onPause();
    }

    @Override protected void onResume()
    {
        super.onResume();
        main_view.onResume();
    }
}
