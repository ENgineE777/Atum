
#import "AtumViewController.h"

#include "Services/Core/Core.h"
#include "Services/Scene/Scene.h"
#include "Services/Scene/ExecuteLevels.h"
#include "SceneObjects/2D/Sprite.h"

extern Scene* hack_scene;
TaskExecutor::SingleTaskPool* renderTaskPool;
Scene scene;

class Renderer : public Object
{
public:
    
    Scene scene;
    bool inited = false;
    
    void Init()
    {
        if (inited)
        {
            return;
        }
        
        hack_scene = &scene;
        
        scene.Init();
        scene.Load("Projects/SunnyLand/SunnyLand.scn");
        scene.Play();
        
        inited = true;
    }
    
    void Draw(float dt)
    {
        render.DebugPrintText(10.0f, COLOR_GREEN, "%i", core.GetFPS());
        
        render.GetDevice()->Clear(true, COLOR_GRAY, true, 1.0f);
        
        render.ExecutePool(ExecuteLevels::Camera, dt);
        render.ExecutePool(ExecuteLevels::Prepare, dt);
        render.ExecutePool(ExecuteLevels::Geometry, dt);
        render.ExecutePool(ExecuteLevels::DebugGeometry, dt);
        render.ExecutePool(ExecuteLevels::Sprites, dt);
        render.ExecutePool(ExecuteLevels::PostProcess, dt);
        render.ExecutePool(ExecuteLevels::GUI, dt);
        render.ExecutePool(ExecuteLevels::Debug, dt);
    }
};

Renderer renderer;

@interface AtumViewController()

@end

@implementation AtumViewController
{
    
}

GLKView* view;

- (void)viewDidLoad
{
    [super viewDidLoad];

    view = [[GLKView alloc] initWithFrame: self.view.bounds];
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    [EAGLContext setCurrentContext:view.context];
    
    self.preferredFramesPerSecond = 60;
    self.view = view;

    
    core.Init(nullptr);
    
    Sprite::Init();
    
    scripts.Start();
    
    render.AddExecutedLevelPool(1);
    
    renderTaskPool = render.AddTaskPool();
    renderTaskPool->AddTask(1, &renderer, (Object::Delegate)&Renderer::Draw);
    
    renderer.Init();
}

-(void)glkView:(GLKView*)view drawInRect:(CGRect)rect
{
    float koef = [UIScreen mainScreen].nativeScale;
    render.GetDevice()->SetVideoMode(rect.size.width * koef, rect.size.height * koef, nullptr);

    core.CountDeltaTime();
    
    renderer.scene.Execute(core.GetDeltaTime());
    
    core.Update();
}

#define IOS_MAX_TOUCHES_COUNT 10

UITouch* ids[IOS_MAX_TOUCHES_COUNT] = {0};

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    float koef = [UIScreen mainScreen].nativeScale;
    
    for (UITouch *touch in touches)
    {
        int index = -1;
        
        for (int j = 0; j < IOS_MAX_TOUCHES_COUNT; j++)
        {
            if (ids[j] == 0)
            {
                ids[j] = touch;
                index = j;
                break;
            }
        }
        
        controls.TouchStart(index,
                            [touch locationInView: [touch view]].x * koef,
                            [touch locationInView: [touch view]].y * koef);
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    float koef = [UIScreen mainScreen].nativeScale;
    
    for (UITouch *touch in touches)
    {
        int index = -1;
        
        for (int j = 0; j < IOS_MAX_TOUCHES_COUNT; j++)
        {
            if (ids[j] == touch)
            {
                index = j;
                break;
            }
        }
        
        controls.TouchUpdate(index,
                             [touch locationInView: [touch view]].x * koef,
                             [touch locationInView: [touch view]].y * koef);

    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
    {
        int index = -1;
        
        for (int j = 0; j < IOS_MAX_TOUCHES_COUNT; j++)
        {
            if (ids[j] == touch)
            {
                ids[j] = 0;
                index = j;
                break;
            }
        }
        
        controls.TouchEnd(index);
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch *touch in touches)
    {
        int index = -1;
        
        for (int j = 0; j < IOS_MAX_TOUCHES_COUNT; j++)
        {
            if (ids[j] == touch)
            {
                ids[j] = 0;
                index = j;
                break;
            }
        }
        
        controls.TouchEnd(index);
    }
}

@end

