cmake_minimum_required(VERSION 3.4.1)

set( atum_DIR ../../../../../../../Atum/ )
set( physx_DIR ../../../../../../../Libraries/PhysX )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall --include-directory=${atum_DIR}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pack-struct -std=c++11 -DPLATFORM_ANDROID=1 -DNDEBUG=1 -fno-rtti -fno-exceptions -Wall --include-directory=${atum_DIR} --include-directory=${physx_DIR}/Include")
set(OPENGL_LIB GLESv3)

add_library(gles3jni SHARED
            ${atum_DIR}/Platform/Android/gles3jni.cpp
            ${atum_DIR}/Platform/Android/android_fopen.cpp
            ${atum_DIR}/SceneAssets/Animator/DataTrack/ColorDataTrack.cpp
            ${atum_DIR}/SceneAssets/Animator/DataTrack/FloatDataTrack.cpp
            ${atum_DIR}/SceneAssets/Animator/DataTrack/Transform2DDataTrack.cpp
            ${atum_DIR}/SceneAssets/Animator/DataTrack/TransformDataTrack.cpp
            ${atum_DIR}/SceneAssets/Animator/TrackPlayers/BoxPlayer.cpp
            ${atum_DIR}/SceneAssets/Animator/TrackPlayers/CameraPlayer.cpp
            ${atum_DIR}/SceneAssets/Animator/TrackPlayers/SpritePlayer.cpp
            ${atum_DIR}/SceneAssets/Animator/TrackPlayers/TrackPlayer.cpp
            ${atum_DIR}/SceneAssets/Animator/Animator.cpp
            ${atum_DIR}/SceneAssets/Animator/AnimatorObject.cpp
            ${atum_DIR}/SceneAssets/Sprite.cpp
            ${atum_DIR}/SceneAssets/SpriteAsset.cpp
            ${atum_DIR}/SceneAssets/SpriteGraphAsset.cpp
            ${atum_DIR}/SceneObjects/GUI/GUIWidget.cpp
            ${atum_DIR}/SceneObjects/GUI/TestGUI.cpp
            ${atum_DIR}/SceneObjects/PathFinder/AStar.cpp
            ${atum_DIR}/SceneObjects/PathFinder/PathFinder.cpp
            ${atum_DIR}/SceneObjects/CharacterCamera2D.cpp
            ${atum_DIR}/SceneObjects/correction.cpp
            ${atum_DIR}/SceneObjects/hovertank.cpp
            ${atum_DIR}/SceneObjects/model.cpp
            ${atum_DIR}/SceneObjects/PhysBox.cpp
            ${atum_DIR}/SceneObjects/plane.cpp
            ${atum_DIR}/SceneObjects/programs.cpp
            ${atum_DIR}/SceneObjects/scene2.cpp
            ${atum_DIR}/SceneObjects/SimpleCharacter2D.cpp
            ${atum_DIR}/SceneObjects/SimpleClient.cpp
            ${atum_DIR}/SceneObjects/SimpleServer.cpp
            ${atum_DIR}/SceneObjects/SpriteObject.cpp
            ${atum_DIR}/SceneObjects/tank.cpp
            ${atum_DIR}/SceneObjects/tankclient.cpp
            ${atum_DIR}/SceneObjects/TDPrt.cpp
            ${atum_DIR}/SceneObjects/terrain.cpp
            ${atum_DIR}/SceneObjects/VirtualJoystick.cpp
            ${atum_DIR}/Support/json/block_allocator.cpp
            ${atum_DIR}/Support/json/json.cpp
            ${atum_DIR}/Support/json/JSONReader.cpp
            ${atum_DIR}/Support/json/JSONWriter.cpp
            ${atum_DIR}/Support/StringUtils.cpp
            ${atum_DIR}/Support/MetaData/MetaData.cpp
            ${atum_DIR}/Support/Timer.cpp
            ${atum_DIR}/Support/ThreadExecutor.cpp
            ${atum_DIR}/Services/Controls/Controls.cpp
            ${atum_DIR}/Services/Core/Core.cpp
            ${atum_DIR}/Services/Font/Font.cpp
            ${atum_DIR}/Services/Font/FontRes.cpp
            ${atum_DIR}/Services/Font/Fonts.cpp
            ${atum_DIR}/Services/Network/Network.cpp
            ${atum_DIR}/Services/Physics/PhysController.cpp
            ${atum_DIR}/Services/Physics/PhysHeightmap.cpp
            ${atum_DIR}/Services/Physics/Physics.cpp
            ${atum_DIR}/Services/Physics/PhysObject.cpp
            ${atum_DIR}/Services/Physics/PhysScene.cpp
            ${atum_DIR}/Services/Render/Debug/DebugBoxes.cpp
            ${atum_DIR}/Services/Render/Debug/DebugFont.cpp
            ${atum_DIR}/Services/Render/Debug/DebugLines.cpp
            ${atum_DIR}/Services/Render/Debug/DebugPrograms.cpp
            ${atum_DIR}/Services/Render/Debug/DebugSpheres.cpp
            ${atum_DIR}/Services/Render/Debug/DebugSprites.cpp
            ${atum_DIR}/Services/Render/Debug/DebugTriangles.cpp
            ${atum_DIR}/Services/Render/GLES3/DeviceGLES.cpp
            ${atum_DIR}/Services/Render/GLES3/GeometryBufferGLES.cpp
            ${atum_DIR}/Services/Render/GLES3/ShaderGLES.cpp
            ${atum_DIR}/Services/Render/GLES3/TextureGLES.cpp
            ${atum_DIR}/Services/Render/GLES3/VertexDeclGLES.cpp
            ${atum_DIR}/Services/Render/Program.cpp
            ${atum_DIR}/Services/Render/Render.cpp
            ${atum_DIR}/Services/Render/Texture.cpp
            ${atum_DIR}/Services/Scene/Scene.cpp
            ${atum_DIR}/Services/Scene/SceneAsset.cpp
            ${atum_DIR}/Services/Scene/SceneObject.cpp
            ${atum_DIR}/Services/TaskExecutor/TaskExecutor.cpp)

# Include libraries needed for gles3jni lib
target_link_libraries(gles3jni
            ${OPENGL_LIB}
            F:/android16/libPhysX3.a
            F:/android16/libPhysX3CharacterKinematic.a
            F:/android16/libPhysX3Common.a
            F:/android16/libPhysX3Extensions.a
            F:/android16/libPxFoundation.a
            F:/android16/libPxTask.a
            F:/android16/libSceneQuery.a
            F:/android16/libSimulationController.a
            android
            EGL
            log
            m)
