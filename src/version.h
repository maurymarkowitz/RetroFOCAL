//
//  version.h
//  retrofocal
//
//  Created by Maury Markowitz on 2023-03-30.
//  Copyright © 2023 Maury Markowitz. All rights reserved.
//

#ifndef version_h
#define version_h

#include <stdio.h>
#include <string.h>

struct VERSION {
    int major;
    int minor;
    int build;
};

static struct VERSION getVersion() {
    struct VERSION v = {1, 1, 0};
    return v;
}

static void getVersionString(char * vers) {
    struct VERSION v = getVersion();
    char major[10];
    char minor[10];
    char build[20];
    
    sprintf(major, "%i.", v.major);
    sprintf(minor, "%i.", v.minor);
    sprintf(build, "%i", v.build);
    
    strcpy(vers, major);
    strcat(vers, minor);
    strcat(vers, build);
}

#endif /* version_h */
