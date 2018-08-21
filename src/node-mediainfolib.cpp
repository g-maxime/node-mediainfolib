#include <node/node_api.h>
#include <iostream>
#include <chrono>
#include "MediaInfoDLL/MediaInfoDLL.h"
using namespace MediaInfoDLL;
using namespace std;


napi_value GetMediaInfo(napi_env env, napi_callback_info info)
{
    napi_value returnobj;
    auto start = std::chrono::system_clock::now();
    //From: preparing an example file for reading
    FILE *F = fopen("bigfile.flac", "rb"); //You can use something else than a f$
    if (F == 0)
        return NULL;
    //From: preparing a memory buffer for reading
    unsigned char *From_Buffer = new unsigned char[7 * 188]; //Note: you can do your own buffer
    size_t From_Buffer_Size;                                 //The size of the read file buffer
    //From: retrieving file size
    fseeko(F, 0, SEEK_END);
    off_t F_Size = ftello(F);
    fseeko(F, 0, SEEK_SET);
    //Initializing MediaInfo
    MediaInfo MI;
    //Preparing to fill MediaInfo with a buffer
    MI.Open_Buffer_Init(F_Size, 0);
    //The parsing loop
    do
    {
        //Reading data somewhere, do what you want for this.
        From_Buffer_Size = fread(From_Buffer, 1, 7 * 188, F);
        //Sending the buffer to MediaInfo
        size_t Status = MI.Open_Buffer_Continue(From_Buffer, From_Buffer_Size);
        if (Status & 0x08) //Bit3=Finished
            break;
        //Testing if there is a MediaInfo request to go elsewhere
        if (MI.Open_Buffer_Continue_GoTo_Get() != (MediaInfo_int64u)-1)
        {
            fseeko(F, MI.Open_Buffer_Continue_GoTo_Get(), SEEK_SET); //Position the file
            MI.Open_Buffer_Init(F_Size, ftello(F)); //Informing MediaInfo we have seek
        }
    } while (From_Buffer_Size > 0);
    //Finalizing
    MI.Open_Buffer_Finalize(); //This is the end of the stream, MediaInfo must finnish some work
    //Get() example
    //String To_Display=MI.Get(Stream_General, 0, __T("Format"));
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time " << diff.count() << " s\n";
    String To_Display = MI.Inform();

#ifdef _UNICODE
    std::wcout << To_Display;
#else
    std::cout << To_Display;
#endif
    return returnobj;
}

napi_value Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_value fn;

    status = napi_create_function(env, NULL, 0, GetMediaInfo, NULL, &fn);
    if (status != napi_ok)
    {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }

    status = napi_set_named_property(env, exports, "get_media_info", fn);
    if (status != napi_ok)
    {
        napi_throw_error(env, NULL, "Unable to populate exports");
    }
    
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)