
package com.atum.engine;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.WindowManager;

import java.io.File;

public class GLES3JNIActivity extends Activity
{
    GLES3JNIView mView;

    @Override protected void onCreate(Bundle icicle)
    {
        System.loadLibrary("gles3jni");
        GLES3JNILib.inst = new GLES3JNILib();

        super.onCreate(icicle);

        GLES3JNILib.inst.SetAssetManager(this.getAssets());

        mView = new GLES3JNIView(getApplication());
        mView.setPreserveEGLContextOnPause(true);
        setContentView(mView);
    }

    @Override protected void onPause()
    {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume()
    {
        super.onResume();
        mView.onResume();
    }

    @Override public boolean onTouchEvent(MotionEvent event)
    {
        int actionCode = event.getActionMasked();
        int index = event.getActionIndex();

        switch (actionCode)
        {
            case MotionEvent.ACTION_POINTER_DOWN:
            case MotionEvent.ACTION_DOWN:
                GLES3JNILib.inst.TouchStart(index, (int)(event.getX(index)), (int)(event.getY(index)));
                break;
            case MotionEvent.ACTION_MOVE:
                for (int i = 0; i < event.getPointerCount(); i++)
                {
                    GLES3JNILib.inst.TouchUpdate(event.getPointerId(i), (int)(event.getX(i)), (int)(event.getY(i)));
                }
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
            case MotionEvent.ACTION_OUTSIDE:
            case MotionEvent.ACTION_POINTER_UP:
                GLES3JNILib.inst.TouchEnd(index);
                break;
        }

        return true;
    }
}
