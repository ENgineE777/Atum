/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.atum.engine;

import android.content.Context;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

class AtumView extends GLSurfaceView
{
    private static final String TAG = "AtumView";
    private static final boolean DEBUG = true;

    public AtumView(Context context)
    {
        super(context);

        setEGLContextClientVersion(3);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        setRenderer(new Renderer());
    }

    private static class Renderer implements GLSurfaceView.Renderer
    {
        public void onDrawFrame(GL10 gl)
        {
            AtumLib.inst.Update();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height)
        {
            AtumLib.inst.Resize(width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config)
        {
            AtumLib.inst.Init();
        }
    }

    @Override public boolean onTouchEvent(MotionEvent event)
    {
        final int pointerNumber = event.getPointerCount();
        final int[] ids = new int[pointerNumber];
        final float[] xs = new float[pointerNumber];
        final float[] ys = new float[pointerNumber];

        for (int i = 0; i < pointerNumber; i++)
        {
            ids[i] = event.getPointerId(i);
            xs[i] = event.getX(i);
            ys[i] = event.getY(i);
        }

        switch (event.getAction() & MotionEvent.ACTION_MASK)
        {
            case MotionEvent.ACTION_POINTER_DOWN:

                final int indexPointerDown = event.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                AtumLib.inst.TouchStart(event.getPointerId(indexPointerDown), (int)event.getX(indexPointerDown), (int)event.getY(indexPointerDown));

                break;

            case MotionEvent.ACTION_DOWN:

                AtumLib.inst.TouchStart(event.getPointerId(0), (int)xs[0], (int)ys[0]);

                break;

            case MotionEvent.ACTION_MOVE:

                for (int i = 0; i < pointerNumber; i++)
                {
                    AtumLib.inst.TouchUpdate(ids[i], (int)xs[i], (int)ys[i]);
                }

                break;

            case MotionEvent.ACTION_POINTER_UP:

                int indexPointUp = event.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                AtumLib.inst.TouchEnd(event.getPointerId(indexPointUp));

                break;

            case MotionEvent.ACTION_UP:

                AtumLib.inst.TouchEnd(event.getPointerId(0));

                break;

            case MotionEvent.ACTION_CANCEL:

                for (int i = 0; i < pointerNumber; i++)
                {
                    AtumLib.inst.TouchEnd(ids[i]);
                }

                break;
        }

        return true;
    }
}
