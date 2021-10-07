//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// Simple_Texture2D.c
//
//    This is a simple example that draws a quad with a 2D
//    texture image. The purpose of this example is to demonstrate 
//    the basics of 2D texturing
//
#include <stdlib.h>
#include "esUtil.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include "esUtil.h"

#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>


EGLStreamKHR stream;

typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // Attribute locations
   GLint  positionLoc;
   GLint  texCoordLoc;

   // Sampler location
   GLint samplerLoc;

   // Texture handle
   GLuint textureId;

} UserData;

///
// Create a simple 2x2 texture image with four different colors
//
GLuint CreateSimpleTexture2D( )
{
   // Texture object handle
   GLuint textureId;
   
   // 2x2 Image, 3 bytes per pixel (R, G, B)
   GLubyte pixels[4 * 3] =
   {  
      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0  // Yellow
   };

   // Use tightly packed data
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

   // Generate a texture object
   glGenTextures ( 1, &textureId );

   // Bind the texture object
   glBindTexture (GL_TEXTURE_EXTERNAL_OES, textureId);
   // glBindTexture ( GL_TEXTURE_2D, textureId );

   // Load the texture
   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

   // Set the filtering mode
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

   return textureId;

}


///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
   esContext->userData = malloc(sizeof(UserData));	
   UserData *userData = esContext->userData;
   GLbyte vShaderStr[] =  
      "attribute vec4 a_position;   \n"
      "attribute vec2 a_texCoord;   \n"
      "varying vec2 v_texCoord;     \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = a_position; \n"
      "   v_texCoord = a_texCoord;  \n"
      "}                            \n";
   
   GLbyte fShaderStr[] =  
      "precision mediump float;                            \n"
      "varying vec2 v_texCoord;                            \n"
      "uniform sampler2D s_texture;                        \n"
      "void main()                                         \n"
      "{                                                   \n"
      "  gl_FragColor = texture2D( s_texture, v_texCoord );\n"
      "}                                                   \n";

   // Load the shaders and get a linked program object
   userData->programObject = esLoadProgram ( vShaderStr, fShaderStr );

   // Get the attribute locations
   userData->positionLoc = glGetAttribLocation ( userData->programObject, "a_position" );
   userData->texCoordLoc = glGetAttribLocation ( userData->programObject, "a_texCoord" );
   
   // Get the sampler location
   userData->samplerLoc = glGetUniformLocation ( userData->programObject, "s_texture" );

   // Load the texture
   userData->textureId = CreateSimpleTexture2D ();
   glBindTexture (GL_TEXTURE_EXTERNAL_OES, userData->textureId);

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
   return GL_TRUE;
}

///
// Cleanup
//
void ShutDown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   // Delete texture object
   glDeleteTextures ( 1, &userData->textureId );

   // Delete program object
   glDeleteProgram ( userData->programObject );
	
   free(esContext->userData);
}


int connection_handler(int connection_fd, EGLNativeFileDescriptorKHR fd)
{
    int i;

    if (ancil_send_fd(connection_fd, fd)) {
        perror("ancil_send_fd");
        exit(1);
    } else {
       printf("Sent evfd %d\n", fd);
    }
    
    return 0;
}

void ESUTIL_API thisMainLoop ( ESContext *esContext )
{
    struct timeval t1, t2;
    struct timezone tz;
    float deltatime;
    float totaltime = 0.0f;
    unsigned int frames = 0;

    gettimeofday ( &t1 , &tz );

    EGLint state = 0;
    PFNEGLSTREAMCONSUMERACQUIREKHRPROC eglStreamConsumerAcquireKHR = (PFNEGLSTREAMCONSUMERACQUIREKHRPROC)eglGetProcAddress("eglStreamConsumerAcquireKHR");
    PFNEGLSTREAMCONSUMERRELEASEKHRPROC eglStreamConsumerReleaseKHR = (PFNEGLSTREAMCONSUMERRELEASEKHRPROC)eglGetProcAddress("eglStreamConsumerReleaseKHR");

    while(1)
    {

      PFNEGLQUERYSTREAMKHRPROC eglQueryStreamKHR = (PFNEGLQUERYSTREAMKHRPROC)eglGetProcAddress("eglQueryStreamKHR");
      if (!eglStreamConsumerAcquireKHR(esContext->eglDisplay, stream)) {
         printf ("No image\n");
      } else {
         printf ("Valid \n");
         eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
         if (!eglStreamConsumerReleaseKHR(esContext->eglDisplay, stream)) {
            printf ("Release frame failed.\n");
         } else {
            printf ("Release frame.\n");
         }
                
      }

       state = 0;
       eglQueryStreamKHR(esContext->eglDisplay, stream, EGL_STREAM_STATE_KHR, &state);

       /* Check why acquire failed */
       switch (state)
       {
       case EGL_STREAM_STATE_DISCONNECTED_KHR:
           printf("Lost connection.\n");
           break;
       case EGL_BAD_STATE_KHR:
           printf("Bad state.\n");
           break;
       case EGL_STREAM_STATE_EMPTY_KHR:
            printf("Empty.\n");
           break;
       case EGL_STREAM_STATE_CONNECTING_KHR:
            printf("Connecting.\n");
           break;
       case EGL_STREAM_STATE_NEW_FRAME_AVAILABLE_KHR:
            printf("New frame.\n");
           break;
       case EGL_STREAM_STATE_OLD_FRAME_AVAILABLE_KHR:
            printf("Old frame.\n");
           break;
       default:
           printf("Unexpected stream state: %04x.\n", state);
       }


    }
}


#define TEXTURES_NUM 1

int main ( int argc, char *argv[] )
{
   ESContext esContext;
   UserData  userData;

   EGLBoolean eglStatus = EGL_TRUE;

   static const EGLint streamAttrFIFOMode[] = { EGL_STREAM_FIFO_LENGTH_KHR, 5, EGL_SUPPORT_REUSE_NV, EGL_FALSE, EGL_NONE };

   EGLNativeFileDescriptorKHR fd;
   char *socket_name = "Xeventfd_socket";

    struct sockaddr_un address;
    int socket_fd, connection_fd;
    socklen_t address_length  = sizeof(address);

    socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(socket_fd < 0) {
        printf("socket() failed\n");
        return 1;
    }

    int enable = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    memset(&address, 0, sizeof(struct sockaddr_un));
    address.sun_family = AF_UNIX;

    snprintf(address.sun_path,sizeof(address.sun_path), "%s", socket_name);
    address.sun_path[0] = '\0';

    if (bind(socket_fd, (struct sockaddr *) &address, sizeof(struct sockaddr_un)) != 0) {
       fprintf(stderr,"bind() failed: %s\n", strerror(errno));
       return 1;
    }

    if (listen(socket_fd, 5) != 0) {
        fprintf(stderr,"listen() failed: %s\n", strerror(errno));
        return 1;
    }

   esInitContext ( &esContext );
   esContext.userData = &userData;

   esCreateWindow ( &esContext, "Simple Texture 2D", 320, 240, ES_WINDOW_RGB );

   if ( !Init ( &esContext ) )
      return 0;

   glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   PFNEGLCREATESTREAMKHRPROC eglCreateStreamKHR = (PFNEGLCREATESTREAMKHRPROC)eglGetProcAddress("eglCreateStreamKHR");
   stream = eglCreateStreamKHR(esContext.eglDisplay, streamAttrFIFOMode);
   if (stream == EGL_NO_STREAM_KHR) {
     printf("Could not create EGL stream.\n");
     eglStatus = EGL_FALSE;
   }

   eglStatus = 0;
   PFNEGLQUERYSTREAMKHRPROC eglQueryStreamKHR = (PFNEGLQUERYSTREAMKHRPROC)eglGetProcAddress("eglQueryStreamKHR");
   eglQueryStreamKHR(esContext.eglDisplay, stream, EGL_STREAM_STATE_KHR, &eglStatus);
   printf("EGLStatus: %d %d\n", eglStatus, EGL_STREAM_STATE_CREATED_KHR);

   PFNEGLGETSTREAMFILEDESCRIPTORKHRPROC eglGetStreamFileDescriptorKHR = (PFNEGLGETSTREAMFILEDESCRIPTORKHRPROC)eglGetProcAddress("eglGetStreamFileDescriptorKHR");
   fd = eglGetStreamFileDescriptorKHR(esContext.eglDisplay, stream);
   if (fd == EGL_NO_FILE_DESCRIPTOR_KHR) {
     printf("Could not get file descriptor.\n");
     eglStatus = EGL_FALSE;
   }
   printf("File descriptor: %d\n", fd);

   PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALKHRPROC eglStreamConsumerGLTextureExternalKHR = (PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALKHRPROC)eglGetProcAddress("eglStreamConsumerGLTextureExternalKHR");
   if (!eglStreamConsumerGLTextureExternalKHR(esContext.eglDisplay, stream)) {
     printf("Could not bind texture.\n");
     eglStatus = EGL_FALSE;
   }

    printf ("Waiting for client...\n");
    if ((connection_fd = accept(socket_fd, (struct sockaddr *) &address, &address_length)) > -1) {
        connection_handler(connection_fd, fd);
        close(connection_fd);
    } else {
        fprintf(stderr,"accept() failed: %s\n", strerror(errno));
        close(socket_fd);
    }

   thisMainLoop ( &esContext );

   ShutDown ( &esContext );

   return eglStatus;
}
