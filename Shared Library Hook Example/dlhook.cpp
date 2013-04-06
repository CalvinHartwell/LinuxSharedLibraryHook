/*
   Description: Shared Library Method Hooking under Unix  
   Credits: Paul Hunkin and the others of ClusterGL (2012)
            Modified and commented by Calvin Hartwell (2013)
 
   Compiled using GCC/G++:
        g++ -shared -fpic dlhook.cpp -o dlhook.so
 
   To load the hook into a process use LD_PRELOAD:
        LD_PRELOAD=./dlhook.so ./process_to_hook
*/
 
 
#include <stdio.h> // Required for sprintf and printf, maybe other functions
#include <dlfcn.h> // Require for module operations
 
// Used to store the location of the original dlsym function
static void* (*o_dlsym) ( void *handle, const char *name ) = 0;
 
// Used to determine the library name/version for the current OS.
// The library contains the originally dlsym function, which we
// Need to use to avoid breaking the program (to call original functions)
void find_dlsym(){
        char buf[32];
        int maxver = 40;
 
        //Works on Ubuntu
        for(int a=0;a<maxver;a++) {
                sprintf(buf, "GLIBC_2.%d", a);
                o_dlsym = (void*(*)(void *handle, const char *name)) dlvsym(RTLD_NEXT,"dlsym", buf);
                if(o_dlsym){
                        return;
                }
        }
 
        //Works on Debian
        for(int a=0;a<maxver;a++){
                for(int b=0;b<maxver;b++){
                        sprintf(buf, "GLIBC_2.%d.%d", a, b);
                        o_dlsym = (void*(*)(void *handle, const char *name)) dlvsym(RTLD_NEXT,"dlsym", buf);
 
                        if(o_dlsym){
                        return;
                        }
                }
        }
}
 
// Our exported function, which is used
extern "C" void *dlsym(void *handle, const char *name){
        // Display the module name (we can see here, what the program is trying to load
        printf("name: %s \n", name);
       
        // If you wish to hook functions, you return your own methods as
        // opposed to those given by dlsym.
 
        // For OpenGL, custom glXGetProcAddress / glXGetProcAddressARB
        // Methods could be used.
 
        // The shared library which house dlsym differs from OS and version,
        // we attempt to find and locate it in order to use the original function
        if(!o_dlsym){
                find_dlsym();
        }
       
        // make sure we return the result from the ORIGINAL dlsym function
        // as it would otherwise break the application
        return (*o_dlsym)( handle,name );
}