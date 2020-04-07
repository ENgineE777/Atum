
#include "Services/Core/Core.h"
#include "Renderer/SPK_QuadRenderer.h"

void SetupExplosion(SPK::Ref<SPK::System> system)
{
	Texture* textureExplosion = core.render.LoadTexture("Spark/explosion.bmp");
	Texture* textureFlash = core.render.LoadTexture("Spark/flash.bmp");
	Texture* textureSpark1 = core.render.LoadTexture("Spark/spark1.bmp");
	Texture* textureSpark2 = core.render.LoadTexture("Spark/point.bmp");
	Texture* textureWave = core.render.LoadTexture("Spark/wave.bmp");

	///////////////
	// Renderers //
	///////////////

	float scale = 10.0f;

	// smoke renderer
	SPK::Ref<SPK::GLQuadRenderer> smokeRenderer = SPK::GLQuadRenderer::create();
	smokeRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
	smokeRenderer->setTexture(textureExplosion);
	smokeRenderer->setAtlasDimensions(2, 2); // uses 4 different patterns in the texture
	smokeRenderer->setBlendMode(SPK::BLEND_MODE_ALPHA);
	smokeRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, false);
	smokeRenderer->setScale(scale, scale);
	smokeRenderer->setShared(true);

	// flame renderer
	SPK::Ref<SPK::GLQuadRenderer> flameRenderer = SPK::GLQuadRenderer::create();
	flameRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
	flameRenderer->setTexture(textureExplosion);
	flameRenderer->setAtlasDimensions(2, 2);
	flameRenderer->setBlendMode(SPK::BLEND_MODE_ADD);
	flameRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, false);
	flameRenderer->setScale(scale, scale);
	flameRenderer->setShared(true);

	// flash renderer
	SPK::Ref<SPK::GLQuadRenderer> flashRenderer = SPK::GLQuadRenderer::create();
	flashRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
	flashRenderer->setTexture(textureFlash);
	flashRenderer->setBlendMode(SPK::BLEND_MODE_ADD);
	flashRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, false);
	flashRenderer->setScale(scale, scale);
	flashRenderer->setShared(true);

	// spark 1 renderer
	SPK::Ref<SPK::GLQuadRenderer> spark1Renderer = SPK::GLQuadRenderer::create();
	spark1Renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
	spark1Renderer->setTexture(textureSpark1);
	spark1Renderer->setBlendMode(SPK::BLEND_MODE_ADD);
	spark1Renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, false);
	spark1Renderer->setOrientation(SPK::DIRECTION_ALIGNED); // sparks are oriented function of their velocity
	spark1Renderer->setScale(0.05f * scale, 1.0f * scale); // thin rectangles
	spark1Renderer->setShared(true);

	// spark 2 renderer
	SPK::Ref<SPK::GLQuadRenderer> spark2Renderer = SPK::GLQuadRenderer::create();
	spark2Renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
	spark2Renderer->setTexture(textureSpark2);
	spark2Renderer->setBlendMode(SPK::BLEND_MODE_ADD);
	spark2Renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, false);
	spark2Renderer->setScale(scale, scale);
	spark2Renderer->setShared(true);

	// wave renderer
	SPK::Ref<SPK::GLQuadRenderer> waveRenderer = SPK::GLQuadRenderer::create();
	waveRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
	waveRenderer->setTexture(textureWave);
	waveRenderer->setBlendMode(SPK::BLEND_MODE_ALPHA);
	waveRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, false);
	waveRenderer->enableRenderingOption(SPK::RENDERING_OPTION_ALPHA_TEST, true); // uses the alpha test
	waveRenderer->setAlphaTestThreshold(0.0f);
	waveRenderer->setOrientation(SPK::FIXED_ORIENTATION); // the orientation is fixed
	waveRenderer->lookVector.set(0.0f, 1.0f, 0.0f);
	waveRenderer->upVector.set(1.0f, 0.0f, 0.0f); // we dont really care about the up axis
	waveRenderer->setScale(scale, scale);
	waveRenderer->setShared(true);

	//////////////
	// Emitters //
	//////////////

	// This zone will be used by several emitters
	SPK::Ref<SPK::Sphere> explosionSphere = SPK::Sphere::create(SPK::Vector3D(0.0f, 0.0f, 0.0f), 0.4f);

	// smoke emitter
	SPK::Ref<SPK::RandomEmitter> smokeEmitter = SPK::RandomEmitter::create();
	smokeEmitter->setZone(SPK::Sphere::create(SPK::Vector3D(0.0f, 0.0f, 0.0f), 0.6f), false);
	smokeEmitter->setTank(15);
	smokeEmitter->setFlow(-1);
	smokeEmitter->setForce(0.02f, 0.04f);

	// flame emitter
	SPK::Ref<SPK::NormalEmitter> flameEmitter = SPK::NormalEmitter::create();
	flameEmitter->setZone(explosionSphere);
	flameEmitter->setTank(15);
	flameEmitter->setFlow(-1);
	flameEmitter->setForce(0.06f, 0.1f);

	// flash emitter
	SPK::Ref<SPK::StaticEmitter> flashEmitter = SPK::StaticEmitter::create();
	flashEmitter->setZone(SPK::Sphere::create(SPK::Vector3D(0.0f, 0.0f, 0.0f), 0.1f));
	flashEmitter->setTank(3);
	flashEmitter->setFlow(-1);

	// spark 1 emitter
	SPK::Ref<SPK::NormalEmitter> spark1Emitter = SPK::NormalEmitter::create();
	spark1Emitter->setZone(explosionSphere);
	spark1Emitter->setTank(20);
	spark1Emitter->setFlow(-1);
	spark1Emitter->setForce(2.0f, 3.0f);
	spark1Emitter->setInverted(true);

	// spark 2 emitter
	SPK::Ref<SPK::NormalEmitter> spark2Emitter = SPK::NormalEmitter::create();
	spark2Emitter->setZone(explosionSphere);
	spark2Emitter->setTank(400);
	spark2Emitter->setFlow(-1);
	spark2Emitter->setForce(0.4f, 1.0f);
	spark2Emitter->setInverted(true);

	// wave emitter
	SPK::Ref<SPK::StaticEmitter> waveEmitter = SPK::StaticEmitter::create();
	waveEmitter->setZone(SPK::Point::create());
	waveEmitter->setTank(1);
	waveEmitter->setFlow(-1);

	////////////
	// Groups //
	////////////

	SPK::Ref<SPK::ColorGraphInterpolator> colorInterpolator;
	SPK::Ref<SPK::FloatGraphInterpolator> paramInterpolator;

	// smoke group
	colorInterpolator = SPK::ColorGraphInterpolator::create();
	colorInterpolator->addEntry(0.0f, 0x33333300);
	colorInterpolator->addEntry(0.4f, 0x33333366, 0x33333399);
	colorInterpolator->addEntry(0.6f, 0x33333366, 0x33333399);
	colorInterpolator->addEntry(1.0f, 0x33333300);

	SPK::Ref<SPK::Group> smokeGroup = system->createGroup(15);
	smokeGroup->setName("Smoke");
	smokeGroup->setPhysicalRadius(0.0f);
	smokeGroup->setLifeTime(2.5f, 3.0f);
	smokeGroup->setRenderer(smokeRenderer);
	smokeGroup->addEmitter(smokeEmitter);
	smokeGroup->setColorInterpolator(colorInterpolator);
	smokeGroup->setParamInterpolator(SPK::PARAM_SCALE, SPK::FloatRandomInterpolator::create(0.3f, 0.4f, 0.5f, 0.7f));
	smokeGroup->setParamInterpolator(SPK::PARAM_TEXTURE_INDEX, SPK::FloatRandomInitializer::create(0.0f, 4.0f));
	smokeGroup->setParamInterpolator(SPK::PARAM_ANGLE, SPK::FloatRandomInterpolator::create(0.0f, 3.14f * 0.5f, 0.0f, 3.14f * 0.5f));
	smokeGroup->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f, 0.05f, 0.0f)));

	// flame group
	colorInterpolator = SPK::ColorGraphInterpolator::create();
	colorInterpolator->addEntry(0.0f, 0xFF8033FF);
	colorInterpolator->addEntry(0.5f, 0x995933FF);
	colorInterpolator->addEntry(1.0f, 0x33333300);

	paramInterpolator = SPK::FloatGraphInterpolator::create();
	paramInterpolator->addEntry(0.0f, 0.125f);
	paramInterpolator->addEntry(0.02f, 0.3f, 0.4f);
	paramInterpolator->addEntry(1.0f, 0.5f, 0.7f);

	SPK::Ref<SPK::Group> flameGroup = system->createGroup(15);
	flameGroup->setName("Flame");
	flameGroup->setLifeTime(1.5f, 2.0f);
	flameGroup->setRenderer(flameRenderer);
	flameGroup->addEmitter(flameEmitter);
	flameGroup->setColorInterpolator(colorInterpolator);
	flameGroup->setParamInterpolator(SPK::PARAM_SCALE, paramInterpolator);
	flameGroup->setParamInterpolator(SPK::PARAM_TEXTURE_INDEX, SPK::FloatRandomInitializer::create(0.0f, 4.0f));
	flameGroup->setParamInterpolator(SPK::PARAM_ANGLE, SPK::FloatRandomInterpolator::create(0.0f, 3.14f * 0.5f, 0.0f, 3.14f * 0.5f));

	// flash group
	paramInterpolator = SPK::FloatGraphInterpolator::create();
	paramInterpolator->addEntry(0.0f, 0.1f);
	paramInterpolator->addEntry(0.25f, 0.5f, 1.0f);

	SPK::Ref<SPK::Group> flashGroup = system->createGroup(3);
	flashGroup->setName("Flash");
	flashGroup->setLifeTime(0.2f, 0.2f);
	flashGroup->addEmitter(flashEmitter);
	flashGroup->setRenderer(flashRenderer);
	flashGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFFFFFFFF, 0xFFFFFF00));
	flashGroup->setParamInterpolator(SPK::PARAM_SCALE, paramInterpolator);
	flashGroup->setParamInterpolator(SPK::PARAM_ANGLE, SPK::FloatRandomInitializer::create(0.0f, 2.0f * 3.14f));

	// spark 1 group
	SPK::Ref<SPK::Group> spark1Group = system->createGroup(20);
	spark1Group->setName("Spark 1");
	spark1Group->setPhysicalRadius(0.0f);
	spark1Group->setLifeTime(0.2f, 1.0f);
	spark1Group->addEmitter(spark1Emitter);
	spark1Group->setRenderer(spark1Renderer);
	spark1Group->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFFFFFFFF, 0xFFFFFF00));
	spark1Group->setParamInterpolator(SPK::PARAM_SCALE, SPK::FloatRandomInitializer::create(0.1f, 0.2f));
	spark1Group->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f, -0.75f, 0.0f)));

	// spark 2 group
	SPK::Ref<SPK::Group> spark2Group = system->createGroup(400);
	spark2Group->setName("Spark 2");
	spark2Group->setGraphicalRadius(0.01f);
	spark2Group->setLifeTime(1.0f, 3.0f);
	spark2Group->addEmitter(spark2Emitter);
	spark2Group->setRenderer(spark2Renderer);
	spark2Group->setColorInterpolator(SPK::ColorRandomInterpolator::create(0xFFFFB2FF, 0xFFFFB2FF, 0xFF4C4C00, 0xFFFF4C00));
	spark2Group->setParamInterpolator(SPK::PARAM_MASS, SPK::FloatRandomInitializer::create(0.5f, 2.5f));
	spark2Group->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f, -0.1f, 0.0f)));
	spark2Group->addModifier(SPK::Friction::create(0.4f));

	// wave group
	paramInterpolator = SPK::FloatGraphInterpolator::create();
	paramInterpolator->addEntry(0.0f, 0.0f);
	paramInterpolator->addEntry(0.2f, 0.0f);
	paramInterpolator->addEntry(1.0f, 3.0f);

	SPK::Ref<SPK::Group> waveGroup = system->createGroup(1);
	waveGroup->setName("Wave");
	waveGroup->setLifeTime(0.8f, 0.8f);
	waveGroup->addEmitter(waveEmitter);
	waveGroup->setRenderer(waveRenderer);
	waveGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFFFFFF20, 0xFFFFFF00));
	waveGroup->setParamInterpolator(SPK::PARAM_SCALE, paramInterpolator);

	//SPK::IO::XMLSaver saver;
	//saver.save("Spark/explossion.xml", system, "Spark/explossion.xml");
}

void SetupHit1(SPK::Ref<SPK::System> system)
{
	Texture* textureFlash = core.render.LoadTexture("Spark/flash.bmp");
	Texture* textureSpark1 = core.render.LoadTexture("Spark/spark1.bmp");

	///////////////
	// Renderers //
	///////////////

	float scale = 1.25f;

	// flash renderer
	SPK::Ref<SPK::GLQuadRenderer> flashRenderer = SPK::GLQuadRenderer::create();
	flashRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
	flashRenderer->setTexture(textureFlash);
	flashRenderer->setBlendMode(SPK::BLEND_MODE_ADD);
	flashRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, false);
	flashRenderer->setScale(scale, scale);
	flashRenderer->setShared(true);

	// spark 1 renderer
	SPK::Ref<SPK::GLQuadRenderer> spark1Renderer = SPK::GLQuadRenderer::create();
	spark1Renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
	spark1Renderer->setTexture(textureSpark1);
	spark1Renderer->setBlendMode(SPK::BLEND_MODE_ADD);
	spark1Renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, false);
	spark1Renderer->setOrientation(SPK::DIRECTION_ALIGNED); // sparks are oriented function of their velocity
	spark1Renderer->setScale(0.1f * scale, 1.0f * scale); // thin rectangles
	spark1Renderer->setShared(true);

	//////////////
	// Emitters //
	//////////////

	// This zone will be used by several emitters
	SPK::Ref<SPK::Sphere> explosionSphere = SPK::Sphere::create(SPK::Vector3D(0.0f, 0.0f, 0.0f), 0.4f);

	// flash emitter
	SPK::Ref<SPK::StaticEmitter> flashEmitter = SPK::StaticEmitter::create();
	flashEmitter->setZone(SPK::Sphere::create(SPK::Vector3D(0.0f, 0.0f, 0.0f), 0.1f));
	flashEmitter->setTank(3);
	flashEmitter->setFlow(-1);

	// spark 1 emitter
	SPK::Ref<SPK::NormalEmitter> spark1Emitter = SPK::NormalEmitter::create();
	spark1Emitter->setZone(explosionSphere);
	spark1Emitter->setTank(20);
	spark1Emitter->setFlow(-1);
	spark1Emitter->setForce(2.0f, 3.0f);
	spark1Emitter->setInverted(true);

	////////////
	// Groups //
	////////////

	SPK::Ref<SPK::ColorGraphInterpolator> colorInterpolator;
	SPK::Ref<SPK::FloatGraphInterpolator> paramInterpolator;

	// smoke group
	colorInterpolator = SPK::ColorGraphInterpolator::create();
	colorInterpolator->addEntry(0.0f, 0x33333300);
	colorInterpolator->addEntry(0.4f, 0x33333366, 0x33333399);
	colorInterpolator->addEntry(0.6f, 0x33333366, 0x33333399);
	colorInterpolator->addEntry(1.0f, 0x33333300);

	// flame group
	colorInterpolator = SPK::ColorGraphInterpolator::create();
	colorInterpolator->addEntry(0.0f, 0xFF8033FF);
	colorInterpolator->addEntry(0.5f, 0x995933FF);
	colorInterpolator->addEntry(1.0f, 0x33333300);

	paramInterpolator = SPK::FloatGraphInterpolator::create();
	paramInterpolator->addEntry(0.0f, 0.125f);
	paramInterpolator->addEntry(0.02f, 0.3f, 0.4f);
	paramInterpolator->addEntry(1.0f, 0.5f, 0.7f);

	// flash group
	paramInterpolator = SPK::FloatGraphInterpolator::create();
	paramInterpolator->addEntry(0.0f, 0.1f);
	paramInterpolator->addEntry(0.25f, 0.5f, 1.0f);

	SPK::Ref<SPK::Group> flashGroup = system->createGroup(3);
	flashGroup->setName("Flash");
	flashGroup->setLifeTime(0.2f, 0.2f);
	flashGroup->addEmitter(flashEmitter);
	flashGroup->setRenderer(flashRenderer);
	flashGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFFFFFFFF, 0xFFFFFF00));
	flashGroup->setParamInterpolator(SPK::PARAM_SCALE, paramInterpolator);
	flashGroup->setParamInterpolator(SPK::PARAM_ANGLE, SPK::FloatRandomInitializer::create(0.0f, 2.0f * 3.14f));

	// spark 1 group
	SPK::Ref<SPK::Group> spark1Group = system->createGroup(20);
	spark1Group->setName("Spark 1");
	spark1Group->setPhysicalRadius(0.0f);
	spark1Group->setLifeTime(0.2f, 1.0f);
	spark1Group->addEmitter(spark1Emitter);
	spark1Group->setRenderer(spark1Renderer);
	spark1Group->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFFFFFFFF, 0xFFFFFF00));
	spark1Group->setParamInterpolator(SPK::PARAM_SCALE, SPK::FloatRandomInitializer::create(0.1f, 0.2f));
	spark1Group->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f, -0.75f, 0.0f)));


	//SPK::IO::XMLSaver saver;
	//saver.save("Spark/explossion.xml", system, "Spark/explossion.xml");
}

void SetupHit2(SPK::Ref<SPK::System> system)
{
	Texture* textureFlash = core.render.LoadTexture("Spark/flash.bmp");
	Texture* textureSpark1 = core.render.LoadTexture("Spark/spark1.bmp");

	///////////////
	// Renderers //
	///////////////

	float scale = 4.0f;

	// flash renderer
	SPK::Ref<SPK::GLQuadRenderer> flashRenderer = SPK::GLQuadRenderer::create();
	flashRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
	flashRenderer->setTexture(textureFlash);
	flashRenderer->setBlendMode(SPK::BLEND_MODE_ADD);
	flashRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, false);
	flashRenderer->setScale(scale, scale);
	flashRenderer->setShared(true);

	// spark 1 renderer
	SPK::Ref<SPK::GLQuadRenderer> spark1Renderer = SPK::GLQuadRenderer::create();
	spark1Renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
	spark1Renderer->setTexture(textureSpark1);
	spark1Renderer->setBlendMode(SPK::BLEND_MODE_ADD);
	spark1Renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, false);
	spark1Renderer->setOrientation(SPK::DIRECTION_ALIGNED); // sparks are oriented function of their velocity
	spark1Renderer->setScale(0.1f * scale, 1.0f * scale); // thin rectangles
	spark1Renderer->setShared(true);

	//////////////
	// Emitters //
	//////////////

	// This zone will be used by several emitters
	SPK::Ref<SPK::Sphere> explosionSphere = SPK::Sphere::create(SPK::Vector3D(0.0f, 0.0f, 0.0f), 0.4f);

	// flash emitter
	SPK::Ref<SPK::StaticEmitter> flashEmitter = SPK::StaticEmitter::create();
	flashEmitter->setZone(SPK::Sphere::create(SPK::Vector3D(0.0f, 0.0f, 0.0f), 0.1f));
	flashEmitter->setTank(3);
	flashEmitter->setFlow(-1);

	// spark 1 emitter
	SPK::Ref<SPK::NormalEmitter> spark1Emitter = SPK::NormalEmitter::create();
	spark1Emitter->setZone(explosionSphere);
	spark1Emitter->setTank(20);
	spark1Emitter->setFlow(-1);
	spark1Emitter->setForce(2.0f, 3.0f);
	spark1Emitter->setInverted(true);

	////////////
	// Groups //
	////////////

	SPK::Ref<SPK::ColorGraphInterpolator> colorInterpolator;
	SPK::Ref<SPK::FloatGraphInterpolator> paramInterpolator;

	// smoke group
	colorInterpolator = SPK::ColorGraphInterpolator::create();
	colorInterpolator->addEntry(0.0f, 0x33333300);
	colorInterpolator->addEntry(0.4f, 0x33333366, 0x33333399);
	colorInterpolator->addEntry(0.6f, 0x33333366, 0x33333399);
	colorInterpolator->addEntry(1.0f, 0x33333300);

	// flame group
	colorInterpolator = SPK::ColorGraphInterpolator::create();
	colorInterpolator->addEntry(0.0f, 0xFF8033FF);
	colorInterpolator->addEntry(0.5f, 0x995933FF);
	colorInterpolator->addEntry(1.0f, 0x33333300);

	paramInterpolator = SPK::FloatGraphInterpolator::create();
	paramInterpolator->addEntry(0.0f, 0.125f);
	paramInterpolator->addEntry(0.02f, 0.3f, 0.4f);
	paramInterpolator->addEntry(1.0f, 0.5f, 0.7f);

	// flash group
	paramInterpolator = SPK::FloatGraphInterpolator::create();
	paramInterpolator->addEntry(0.0f, 0.1f);
	paramInterpolator->addEntry(0.25f, 0.5f, 1.0f);

	SPK::Ref<SPK::Group> flashGroup = system->createGroup(3);
	flashGroup->setName("Flash");
	flashGroup->setLifeTime(0.2f, 0.2f);
	flashGroup->addEmitter(flashEmitter);
	flashGroup->setRenderer(flashRenderer);
	flashGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFFFFFFFF, 0xFFFFFF00));
	flashGroup->setParamInterpolator(SPK::PARAM_SCALE, paramInterpolator);
	flashGroup->setParamInterpolator(SPK::PARAM_ANGLE, SPK::FloatRandomInitializer::create(0.0f, 2.0f * 3.14f));

	// spark 1 group
	SPK::Ref<SPK::Group> spark1Group = system->createGroup(20);
	spark1Group->setName("Spark 1");
	spark1Group->setPhysicalRadius(0.0f);
	spark1Group->setLifeTime(0.2f, 1.0f);
	spark1Group->addEmitter(spark1Emitter);
	spark1Group->setRenderer(spark1Renderer);
	spark1Group->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFFFFFFFF, 0xFFFFFF00));
	spark1Group->setParamInterpolator(SPK::PARAM_SCALE, SPK::FloatRandomInitializer::create(0.1f, 0.2f));
	spark1Group->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f, -0.75f, 0.0f)));

	//SPK::IO::XMLSaver saver;
	//saver.save("Spark/explossion.xml", system, "Spark/explossion.xml");
}

void SetupSmokeTrail(SPK::Ref<SPK::System> system)
{
	Texture* textureExplosion = core.render.LoadTexture("Spark/explosion.bmp");

	///////////////
	// Renderers //
	///////////////

	float scale = 1.0f;

	// smoke renderer
	SPK::Ref<SPK::GLQuadRenderer> smokeRenderer = SPK::GLQuadRenderer::create();
	smokeRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
	smokeRenderer->setTexture(textureExplosion);
	smokeRenderer->setAtlasDimensions(2, 2); // uses 4 different patterns in the texture
	smokeRenderer->setBlendMode(SPK::BLEND_MODE_ALPHA);
	smokeRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE, false);
	smokeRenderer->setScale(scale, scale);
	smokeRenderer->setShared(true);

	//////////////
	// Emitters //
	//////////////

	// smoke emitter
	SPK::Ref<SPK::RandomEmitter> smokeEmitter = SPK::RandomEmitter::create();
	smokeEmitter->setZone(SPK::Sphere::create(SPK::Vector3D(0.0f, 0.0f, 0.0f), 0.01f), false);
	smokeEmitter->setFlow(30.0f);
	smokeEmitter->setForce(0.02f, 0.04f);

	////////////
	// Groups //
	////////////

	SPK::Ref<SPK::ColorGraphInterpolator> colorInterpolator;

	// smoke group
	colorInterpolator = SPK::ColorGraphInterpolator::create();
	colorInterpolator->addEntry(0.0f, 0x33333300);
	colorInterpolator->addEntry(0.05f, 0x333333aa, 0x333333ee);
	colorInterpolator->addEntry(0.6f, 0x333333aa, 0x333333ee);
	colorInterpolator->addEntry(1.0f, 0x33333300);

	SPK::Ref<SPK::Group> smokeGroup = system->createGroup(2050);
	smokeGroup->setName("Smoke");
	smokeGroup->setPhysicalRadius(0.0f);
	smokeGroup->setLifeTime(2.0f, 2.5f);
	smokeGroup->setRenderer(smokeRenderer);
	smokeGroup->addEmitter(smokeEmitter);
	smokeGroup->setColorInterpolator(colorInterpolator);
	smokeGroup->setParamInterpolator(SPK::PARAM_SCALE, SPK::FloatRandomInterpolator::create(0.5f, 0.6f, 0.9f, 1.2f));
	smokeGroup->setParamInterpolator(SPK::PARAM_TEXTURE_INDEX, SPK::FloatRandomInitializer::create(0.0f, 4.0f));
	smokeGroup->setParamInterpolator(SPK::PARAM_ANGLE, SPK::FloatRandomInterpolator::create(0.0f, 3.14f * 0.5f, 0.0f, 3.14f * 0.5f));
	smokeGroup->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f, 0.05f, 0.0f)));
}

void Particles::Init()
{
	SPK::System::setClampStep(true, 0.1f);
	SPK::System::useAdaptiveStep(0.001f, 0.01f);
}

ParticleSystem* Particles::LoadParticle(const char* name, TaskExecutor::SingleTaskPool* taskPool, TaskExecutor::SingleTaskPool* renderPool, bool autoDelete)
{
	SPK::Ref<SPK::System> system;

	if (particles.count(name) > 0)
	{
		ParticleRef& ref = particles[name];

		ref.count++;
		system = ref.system;
	}
	else
	{
		system = SPK::System::create(false);

		ParticleRef& ref = particles[name];

		ref.count = 1;
		ref.system = system;

		if (StringUtils::IsEqual(name, "explosion"))
		{
			SetupExplosion(system);
		}
		else
		if (StringUtils::IsEqual(name, "hit1"))
		{
			SetupHit1(system);
		}
		else
		if (StringUtils::IsEqual(name, "hit2"))
		{
			SetupHit2(system);
		}
		else
		if (StringUtils::IsEqual(name, "smokeTrail"))
		{
			SetupSmokeTrail(system);
		}
	}

	ParticleSystem* instance = new ParticleSystem();
	instance->Init(system, taskPool, renderPool, autoDelete);

	return instance;
}

bool Particles::DecRef(SPK::Ref<SPK::System> system)
{
	typedef std::map<std::string, ParticleRef>::iterator it_type;

	for (it_type iterator = particles.begin(); iterator != particles.end(); iterator++)
	{
		if (iterator->second.system == system)
		{
			iterator->second.count--;

			if (iterator->second.count == 0)
			{
				particles.erase(iterator);
				return true;
			}

			return false;
		}
	}

	return true;
}

void SetupExplotion(SPK::Ref<SPK::System> system)
{

}