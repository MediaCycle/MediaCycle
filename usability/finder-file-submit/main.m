//
//  main.m
//  cd to ...
//
//  Created by James Tuley on 2/16/07.
//  Copyright Jay Tuley 2007. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "Finder.h"

#include <curl/curl.h>

NSString* getPathToFrontFinderWindow(){

    FinderApplication* finder = [SBApplication applicationWithBundleIdentifier:@"com.apple.Finder"];
    FinderFinderWindow* frontWindow =[[finder windows]  objectAtIndex:0];
    FinderItem* target =  [frontWindow.properties objectForKey:@"target"] ;

    //NSURL* url =[NSURL URLWithString:target.URL];
    NSString * path = 0;// =target.displayedName;

    SBElementArray * selection = [[finder selection] get];
    NSArray * items = [selection arrayByApplyingSelector:@selector(URL)];
//    if(!items)
//        NSLog(@"no items");
//    else
//        NSLog(@"got items");
    for (NSString * item in items) {
        NSURL * url = [NSURL URLWithString:item];
        path = [url path];
//        NSLog(@"selected item url: %@", url);
    }

    if(path){
        NSString* command;

        command = @"localhost:4040";
        command = [command stringByAppendingString:@"/team=1"];
        command = [command stringByAppendingString:@"/file="];
        command = [command stringByAppendingString:path];
        //command = [command stringByAppendingString:@"040"];
        //command = [command stringByAppendingString:@"/segstart=43826"];
        //command = [command stringByAppendingString:@"/segstop=43826"];

//        NSLog(@"command: %@", command);

        // Put this inside a thread, blocks the plugin if the server can't be reached
        CURL *curl;
        CURLcode res;

        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, (const char*)[command UTF8String]);

            /* Perform the request, res will get the return code */
            res = curl_easy_perform(curl);
            /* Check for errors */
            if(res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));

            /* always cleanup */
            curl_easy_cleanup(curl);
        }
    }
    return path;
}

int main(int argc, char *argv[])
{
    id pool = [[NSAutoreleasePool alloc] init];

    NSString* path;
    @try{
        path = getPathToFrontFinderWindow();
    }@catch(id ex){
        path =[@"~/Desktop" stringByExpandingTildeInPath];
    }

    printf("path %s",[path UTF8String]);

    [pool release];
    return 0;
}



