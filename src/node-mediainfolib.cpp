#include <napi.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <regex>
#include <iostream>
#include <chrono>
#include "MediaInfoDLL/MediaInfoDLL.h"
using namespace MediaInfoDLL;
using namespace std;

//Init at top level for use in all methods
MediaInfo MI;

napi_value processProps(napi_env env, stream_t Stream, size_t StreamNumber, napi_value propertiesArray)
{
    napi_status status;
    napi_value returnvalue;
    status = napi_create_object(env, &returnvalue);

    uint32_t len;
    status = napi_get_array_length(env, propertiesArray, &len);
    assert(status == napi_ok);

    for (uint32_t i = 0; i < len; i++)
    {
        //Get Property
        napi_value prop;
        status = napi_get_element(env, propertiesArray, i, &prop);
        assert(status == napi_ok);
        //Wrap property for use with MI
        Napi::String propStr = Napi::String(env, prop);
        string proptmp = propStr.Utf8Value();
        string valueStr = MI.Get(Stream, StreamNumber, propStr.Utf8Value(), Info_Text, Info_Name);
        if (!valueStr.empty())
        {
            //Wrap value fro output
            Napi::String value = Napi::String::From(env, valueStr);

            //Remove all special characters in property to make output easier to deal with
            string outStr = std::regex_replace(propStr.Utf8Value(), std::regex("[/()*-]"), "");
            //Wrap regexed string for output
            Napi::String property = Napi::String::From(env, outStr);

            status = napi_set_property(env, returnvalue, property, value);
            assert(status == napi_ok);
        }
    }
    return returnvalue;
}

napi_value GetMediaInfo(napi_env env, napi_callback_info info)
{
    //Timing
    auto start = std::chrono::system_clock::now();

    size_t result = 0;
    size_t argc = 2;

    char file[4096];

    napi_value args[2];
    napi_status status;

    //Input object and property arrays
    napi_value inputObj; //taken from argv[1]
    napi_value generalProps;
    napi_value videoProps;
    napi_value audioProps;
    napi_value textProps;
    napi_value menuProps;

    //Output object and property/value arrays
    napi_value returnObj;
    status = napi_create_object(env, &returnObj);
    assert(status == napi_ok);
    napi_value general;
    //status = napi_create_object(env, &general);
    assert(status == napi_ok);
    napi_value videoArray;
    status = napi_create_array(env, &videoArray);
    napi_value audioArray;
    status = napi_create_array(env, &audioArray);

    status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    if (status != napi_ok)
        napi_throw_error(env, NULL, "Failed to parse arguments");

    status = napi_get_value_string_utf8(env, args[0], file, 4096, &result);
    assert(status == napi_ok);
    inputObj = args[1];

    //From: preparing an example file for reading
    FILE *F = fopen(file, "rb"); //You can use something else than a f$

    if (F == 0)
        return NULL;

    //From: preparing a memory buffer for reading
    unsigned char *From_Buffer = new unsigned char[7 * 188]; //Note: you can do your own buffer
    size_t From_Buffer_Size;                                 //The size of the read file buffer
    //From: retrieving file size
    fseeko(F, 0, SEEK_END);
    off_t F_Size = ftello(F);
    fseeko(F, 0, SEEK_SET);
    
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
            MI.Open_Buffer_Init(F_Size, ftello(F));                  //Informing MediaInfo we have seek
        }
    } while (From_Buffer_Size > 0);

    //Finalizing
    MI.Open_Buffer_Finalize(); //This is the end of the stream, MediaInfo must finish some work

    status = napi_get_named_property(env, inputObj, "General", &generalProps);
    assert(status == napi_ok);
    status = napi_get_named_property(env, inputObj, "Video", &videoProps);
    status = napi_get_named_property(env, inputObj, "Audio", &audioProps);
    status = napi_get_named_property(env, inputObj, "Text", &textProps);
    status = napi_get_named_property(env, inputObj, "Menu", &menuProps);

    for (size_t StreamKind = (size_t)Stream_General; StreamKind < Stream_Max; StreamKind++)
    {
        for (size_t StreamPos = 0; StreamPos < (size_t)MI.Count_Get((stream_t)StreamKind); StreamPos++)
        {
            //Pour chaque stream
            String stream = MI.Get((stream_t)StreamKind, StreamPos, __T("StreamKind/String"));
            String pos = MI.Get((stream_t)StreamKind, StreamPos, __T("StreamKindPos"));
            size_t streamNum = 0;
            if (!pos.empty())
            {
                streamNum = stoi(pos)-1;
            }
            switch (StreamKind)
            {
            case Stream_General:
                general = processProps(env, Stream_General, (size_t)streamNum, generalProps);
                status = napi_set_named_property(env, returnObj, "General", general);
                break;
            case Stream_Video:
                if (videoProps)
                {
                    napi_value element = processProps(env, Stream_Video, (size_t)streamNum, videoProps);
                    status = napi_set_element(env, videoArray, streamNum, element);
                }
                break;
            case Stream_Audio:
                if (audioProps)
                {
                    napi_value element = processProps(env, Stream_Audio, (size_t)streamNum, audioProps);
                    status = napi_set_element(env, audioArray, streamNum, element);
                }
                break;
            }
        }
    }
    uint32_t len;
    napi_get_array_length(env, videoArray, &len);
    assert(status == napi_ok);
    if (videoProps && len > 0)
    {
        napi_set_named_property(env, returnObj, "Video", videoArray);
        assert(status == napi_ok);
    }
    napi_get_array_length(env, audioArray, &len);
    assert(status == napi_ok);
    if (audioProps && len > 0)
    {
        napi_set_named_property(env, returnObj, "Audio", audioArray);
        assert(status == napi_ok);
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time " << diff.count() << " s\n";

    return returnObj;
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