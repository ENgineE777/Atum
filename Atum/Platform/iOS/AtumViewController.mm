
#import "AtumViewController.h"

#include "Platform/Common/AtumRunner.h"

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
    self.view.multipleTouchEnabled = YES;

    runner.Init();
}

-(void)glkView:(GLKView*)view drawInRect:(CGRect)rect
{
    float koef = [UIScreen mainScreen].nativeScale;
    
    runner.Resize(rect.size.width * koef, rect.size.height * koef);
    
    runner.Execute();
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
        
        core.controls.TouchStart(index,
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
        
        core.controls.TouchUpdate(index,
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
        
        core.controls.TouchEnd(index);
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
        
        core.controls.TouchEnd(index);
    }
}

@end

