
#import "AppDelegate.h"
#import "AtumViewController.h"
#include "Services/Core/Core.h"

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    AtumViewController* controller = [[AtumViewController alloc] init];
    
    self.window.rootViewController = controller;
    [self.window makeKeyAndVisible];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    core.OnPause();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    core.OnResume();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    core.Release();
}

@end
