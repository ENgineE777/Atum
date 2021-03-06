cmake_minimum_required(VERSION 3.4.1)

set( atum_DIR ../../../../../../../Atum/ )
set( physx_DIR ../../../../../../../Libraries/PhysX )
set( physx_lib_DIR ../../../../../../../Atum/Libraries/PhysX/Lib/android64/release )
set( as_DIR ../../../../../../../Libraries/Angelscript )
set( vorbis_DIR ../../../../../../../Atum/Services/Sound/vorbis )
set( ogg_DIR ../../../../../../../Atum/Services/Sound/ogg )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall --include-directory=${atum_DIR} --include-directory=${vorbis_DIR}/include --include-directory=${vorbis_DIR}/lib --include-directory=${ogg_DIR}/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -fno-pack-struct -std=c++11 -DPLATFORM_ANDROID=1 -DAS_NO_COMPILER -DAS_NO_EXCEPTIONS -DAS_MAX_PORTABILITY -fno-rtti -fno-exceptions -Wall --include-directory=${atum_DIR} --include-directory=${physx_DIR}/Include --include-directory=${as_DIR}/Include --include-directory=${vorbis_DIR}/include --include-directory=${ogg_DIR}/include" )
set(OPENGL_LIB GLESv3)

link_directories( ${physx_lib_DIR} )
set(PHYSX_LINK_LIBS libphysx.so)

add_library(atumlib SHARED
            ${as_DIR}/source/as_atomic.cpp
            ${as_DIR}/source/as_builder.cpp
            ${as_DIR}/source/as_bytecode.cpp
            ${as_DIR}/source/as_callfunc.cpp
            ${as_DIR}/source/as_callfunc_arm.cpp
            ${as_DIR}/source/as_configgroup.cpp
            ${as_DIR}/source/as_compiler.cpp
            ${as_DIR}/source/as_configgroup.cpp
            ${as_DIR}/source/as_gc.cpp
            ${as_DIR}/source/as_generic.cpp
            ${as_DIR}/source/as_context.cpp
            ${as_DIR}/source/as_datatype.cpp
            ${as_DIR}/source/as_globalproperty.cpp
            ${as_DIR}/source/as_memory.cpp
            ${as_DIR}/source/as_module.cpp
            ${as_DIR}/source/as_objecttype.cpp
            ${as_DIR}/source/as_outputbuffer.cpp
            ${as_DIR}/source/as_parser.cpp
            ${as_DIR}/source/as_restore.cpp
            ${as_DIR}/source/as_scriptcode.cpp
            ${as_DIR}/source/as_scriptengine.cpp
            ${as_DIR}/source/as_scriptfunction.cpp
            ${as_DIR}/source/as_scriptnode.cpp
            ${as_DIR}/source/as_scriptobject.cpp
            ${as_DIR}/source/as_string.cpp
            ${as_DIR}/source/as_string_util.cpp
            ${as_DIR}/source/as_thread.cpp
            ${as_DIR}/source/as_tokenizer.cpp
            ${as_DIR}/source/as_typeinfo.cpp
            ${as_DIR}/source/as_variablescope.cpp
            ${atum_DIR}/Platform/Android/AtumLib.cpp
            ${atum_DIR}/Platform/Common/AtumRunner.cpp
            ${atum_DIR}/SceneObjects/2D/Camera2D.cpp
            ${atum_DIR}/SceneObjects/2D/Phys2DComp.cpp
            ${atum_DIR}/SceneObjects/2D/Sprite.cpp
            ${atum_DIR}/SceneObjects/2D/SpriteAsset.cpp
            ${atum_DIR}/SceneObjects/2D/SpriteGraphAsset.cpp
            ${atum_DIR}/SceneObjects/2D/SpriteGraphInst.cpp
            ${atum_DIR}/SceneObjects/2D/SpriteInst.cpp
            ${atum_DIR}/SceneObjects/2D/SpritesLayerAsset.cpp
            ${atum_DIR}/SceneObjects/2D/SpritesLayerInst.cpp
            ${atum_DIR}/SceneObjects/2D/SpriteTileAsset.cpp
            ${atum_DIR}/SceneObjects/2D/SpriteTileInst.cpp
            ${atum_DIR}/SceneObjects/2D/Track2DComp.cpp
            ${atum_DIR}/SceneObjects/2D/Trigger2D.cpp
            ${atum_DIR}/SceneObjects/3D/PhysBox.cpp
            ${atum_DIR}/SceneObjects/3D/terrain.cpp
            ${atum_DIR}/SceneObjects/Common/Animator/DataTrack/ColorDataTrack.cpp
            ${atum_DIR}/SceneObjects/Common/Animator/DataTrack/FloatDataTrack.cpp
            ${atum_DIR}/SceneObjects/Common/Animator/DataTrack/Transform2DDataTrack.cpp
            ${atum_DIR}/SceneObjects/Common/Animator/DataTrack/TransformDataTrack.cpp
            ${atum_DIR}/SceneObjects/Common/Animator/TrackPlayers/BoxPlayer.cpp
            ${atum_DIR}/SceneObjects/Common/Animator/TrackPlayers/CameraPlayer.cpp
            ${atum_DIR}/SceneObjects/Common/Animator/TrackPlayers/SpritePlayer.cpp
            ${atum_DIR}/SceneObjects/Common/Animator/TrackPlayers/TrackPlayer.cpp
            ${atum_DIR}/SceneObjects/Common/Animator/Animator.cpp
            ${atum_DIR}/SceneObjects/Common/Animator/AnimatorObject.cpp
            ${atum_DIR}/SceneObjects/Common/SceneScriptAsset.cpp
            ${atum_DIR}/SceneObjects/Common/SceneScriptInst.cpp
            ${atum_DIR}/SceneObjects/Common/ScriptMetaDataAsset.cpp
            ${atum_DIR}/SceneObjects/Common/ScriptMetaDataComp.cpp
            ${atum_DIR}/SceneObjects/Common/VirtualJoystick.cpp
            ${atum_DIR}/SceneObjects/Common/MusicPlayer.cpp
            ${atum_DIR}/SceneObjects/Other/Demo/correction.cpp
            ${atum_DIR}/SceneObjects/Other/Demo/model.cpp
            ${atum_DIR}/SceneObjects/Other/Demo/plane.cpp
            ${atum_DIR}/SceneObjects/Other/Demo/programs.cpp
            ${atum_DIR}/SceneObjects/Other/Demo/SimpleCharacter2D.cpp
            ${atum_DIR}/SceneObjects/Other/Demo/SimpleClient.cpp
            ${atum_DIR}/SceneObjects/Other/Demo/SimpleServer.cpp
            ${atum_DIR}/SceneObjects/Other/Demo/tank.cpp
            ${atum_DIR}/SceneObjects/Other/Demo/tankclient.cpp
            ${atum_DIR}/SceneObjects/UI/UIButtonAsset.cpp
            ${atum_DIR}/SceneObjects/UI/UIContainerAsset.cpp
            ${atum_DIR}/SceneObjects/UI/UIImageAsset.cpp
            ${atum_DIR}/SceneObjects/UI/UILabelAsset.cpp
            ${atum_DIR}/SceneObjects/UI/UIViewAsset.cpp
            ${atum_DIR}/SceneObjects/UI/UIViewInstanceAsset.cpp
            ${atum_DIR}/SceneObjects/UI/UIWidgetAsset.cpp
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
            ${atum_DIR}/Services/File/Files.cpp
            ${atum_DIR}/Services/File/FileInMemory.cpp
            ${atum_DIR}/Services/Network/Network.cpp
            ${atum_DIR}/Services/Physic/PhysController.cpp
            ${atum_DIR}/Services/Physic/PhysHeightmap.cpp
            ${atum_DIR}/Services/Physic/Physics.cpp
            ${atum_DIR}/Services/Physic/PhysObjectBase.cpp
            ${atum_DIR}/Services/Physic/PhysObject.cpp
            ${atum_DIR}/Services/Physic/PhysScene.cpp
            ${atum_DIR}/Services/Render/Debug/DebugBoxes.cpp
            ${atum_DIR}/Services/Render/Debug/DebugFont.cpp
            ${atum_DIR}/Services/Render/Debug/DebugLines.cpp
            ${atum_DIR}/Services/Render/Debug/DebugPrograms.cpp
            ${atum_DIR}/Services/Render/Debug/DebugSpheres.cpp
            ${atum_DIR}/Services/Render/Debug/DebugSprites.cpp
            ${atum_DIR}/Services/Render/Debug/DebugTriangles.cpp
            ${atum_DIR}/Services/Render/Debug/DebugTriangles2D.cpp
            ${atum_DIR}/Services/Render/GLES3/DeviceGLES.cpp
            ${atum_DIR}/Services/Render/GLES3/DataBufferGLES.cpp
            ${atum_DIR}/Services/Render/GLES3/ShaderGLES.cpp
            ${atum_DIR}/Services/Render/GLES3/TextureGLES.cpp
            ${atum_DIR}/Services/Render/GLES3/VertexDeclGLES.cpp
            ${atum_DIR}/Services/Render/Program.cpp
            ${atum_DIR}/Services/Render/Render.cpp
            ${atum_DIR}/Services/Render/Texture.cpp
            ${atum_DIR}/Services/Scene/Scene.cpp
            ${atum_DIR}/Services/Scene/SceneAsset.cpp
            ${atum_DIR}/Services/Scene/SceneManager.cpp
            ${atum_DIR}/Services/Scene/SceneObject.cpp
            ${atum_DIR}/Services/Scene/SceneObjectComp.cpp
            ${atum_DIR}/Services/Script/Libs/scriptarray.cpp
            ${atum_DIR}/Services/Script/Libs/scriptdictionary.cpp
            ${atum_DIR}/Services/Script/Libs/scripthandle.cpp
            ${atum_DIR}/Services/Script/Libs/scriptmath.cpp
            ${atum_DIR}/Services/Script/Libs/scriptstdstring.cpp
            ${atum_DIR}/Services/Script/Libs/scriptstdstring_utils.cpp
            ${atum_DIR}/Services/Script/ScriptContext.cpp
            ${atum_DIR}/Services/Script/ScriptCore.cpp
            ${atum_DIR}/Services/Script/Scripts.cpp
            ${ogg_DIR}/src/bitwise.c
            ${ogg_DIR}/src/framing.c
            ${vorbis_DIR}/lib/analysis.c
            ${vorbis_DIR}/lib/bitrate.c
            ${vorbis_DIR}/lib/block.c
            ${vorbis_DIR}/lib/codebook.c
            ${vorbis_DIR}/lib/envelope.c
            ${vorbis_DIR}/lib/floor0.c
            ${vorbis_DIR}/lib/floor1.c
            ${vorbis_DIR}/lib/info.c
            ${vorbis_DIR}/lib/lookup.c
            ${vorbis_DIR}/lib/lpc.c
            ${vorbis_DIR}/lib/lsp.c
            ${vorbis_DIR}/lib/mapping0.c
            ${vorbis_DIR}/lib/mdct.c
            ${vorbis_DIR}/lib/psy.c
            ${vorbis_DIR}/lib/registry.c
            ${vorbis_DIR}/lib/res0.c
            ${vorbis_DIR}/lib/sharedbook.c
            ${vorbis_DIR}/lib/smallft.c
            ${vorbis_DIR}/lib/synthesis.c
            ${vorbis_DIR}/lib/vorbisenc.c
            ${vorbis_DIR}/lib/vorbisfile.c
            ${vorbis_DIR}/lib/window.c
            ${atum_DIR}/Services/Sound/DecodedBuffer.cpp
            ${atum_DIR}/Services/Sound/SoundBase.cpp
            ${atum_DIR}/Services/Sound/SoundInstance.cpp
            ${atum_DIR}/Services/Sound/Sounds.cpp
            ${atum_DIR}/Services/Sound/SoundStream.cpp
            ${atum_DIR}/Services/TaskExecutor/TaskExecutor.cpp)

# Include libraries needed for atumlib lib

target_link_libraries(atumlib
            ${OPENGL_LIB}
            ${PHYSX_LINK_LIBS}
            android
            EGL
            OpenSLES
            log
            m)
