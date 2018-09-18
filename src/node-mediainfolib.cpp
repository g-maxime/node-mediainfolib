#include <napi.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <regex>
#include <iostream>
#include <MediaInfoDLL/MediaInfoDLL.h>
using namespace MediaInfoDLL;
using namespace std;

MediaInfo MI;

Napi::Object processProps(Napi::Env env, stream_t Stream, size_t StreamNumber, Napi::Array propertiesArray)
{
    Napi::Object returnvalue = Napi::Object::New(env);

    uint32_t len = propertiesArray.Length();

    for (uint32_t i = 0; i < len; i++)
    {
        Napi::String prop = propertiesArray.Get(i).As<Napi::String>();
        string valueStr = MI.Get(Stream, StreamNumber, prop.Utf8Value(), Info_Text, Info_Name);
        if (!valueStr.empty())
        {
            Napi::String value = Napi::String::From(env, valueStr);

            // Replace all special characters in property to make output easier to deal with
            string outStr = std::regex_replace(prop.Utf8Value(), std::regex("[/()*-]"), "_");
            //Wrap regexed string for output
            Napi::String property = Napi::String::From(env, outStr);
            returnvalue.Set(property, value);
        }
    }
    return returnvalue;
}

Napi::Value getFile(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    
    //Output object and property/value arrays
    Napi::Object returnObj = Napi::Object::New(env);
    
    Napi::Object general = Napi::Object::New(env);
    Napi::Array videoArray = Napi::Array::New(env);
    Napi::Array audioArray = Napi::Array::New(env);
    Napi::Array textArray = Napi::Array::New(env);
    
    string file = info[0].As<Napi::String>().Utf8Value();
    
    Napi::Object inputObj = info[1].As<Napi::Object>();
    string v = MI.Option(__T("Info_Version"));
    returnObj.Set("Version", Napi::String::From(env, v));

    MI.Open(__T(file));

    returnObj.Set("File", info[0].As<Napi::String>());

    Napi::Array generalProps = inputObj.Get("General").As<Napi::Array>();
    Napi::Array videoProps = inputObj.Get("Video").As<Napi::Array>();
    Napi::Array audioProps = inputObj.Get("Audio").As<Napi::Array>();
    Napi::Array textProps = inputObj.Get("Text").As<Napi::Array>();

    for (size_t StreamKind = (size_t)Stream_General; StreamKind < Stream_Max; StreamKind++)
    {
        for (size_t StreamPos = 0; StreamPos < (size_t)MI.Count_Get((stream_t)StreamKind); StreamPos++)
        {
            String stream = MI.Get((stream_t)StreamKind, StreamPos, __T("StreamKind/String"));
            String pos = MI.Get((stream_t)StreamKind, StreamPos, __T("StreamKindPos"));
            size_t streamNum = 0;
            if (!pos.empty())
            {
                streamNum = stoi(pos) - 1;
            }
            switch (StreamKind)
            {
            case Stream_General:
                general = processProps(env, Stream_General, (size_t)streamNum, generalProps);
                returnObj.Set("General", general);
                break;
            case Stream_Video:
                if (videoProps)
                {
                    Napi::Value element = processProps(env, Stream_Video, (size_t)streamNum, videoProps);
                    videoArray.Set(streamNum, element);
                }
                break;
            case Stream_Audio:
                if (audioProps)
                {
                    Napi::Value element = processProps(env, Stream_Audio, (size_t)streamNum, audioProps);
                    audioArray.Set(streamNum, element);
                }
                break;
            case Stream_Text:
                if (textProps)
                {
                    Napi::Value element = processProps(env, Stream_Text, (size_t)streamNum, textProps);
                    textArray.Set(streamNum, element);
                }
                break;
            }
        }
    }

    MI.Close();
    
    uint32_t len = videoArray.Length();
    
    if (videoProps && len > 0)
    {
        returnObj.Set("Video", videoArray);
    }
    len = audioArray.Length();
    
    if (audioProps && len > 0)
    {
        returnObj.Set("Audio", audioArray);
    }
    len = textArray.Length();
    
    if (textProps && len > 0)
    {
        returnObj.Set("Text", textArray);
    }
    
    return returnObj;
}

Napi::Value getLocal(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    
    //Output object and property/value arrays
    Napi::Object returnObj = Napi::Object::New(env);
    
    Napi::Object general = Napi::Object::New(env);
    Napi::Array videoArray = Napi::Array::New(env);
    Napi::Array audioArray = Napi::Array::New(env);
    Napi::Array textArray = Napi::Array::New(env);
    
    string file = info[0].As<Napi::String>().Utf8Value();
    const char * cfile = file.c_str();
    Napi::Object inputObj = info[1].As<Napi::Object>();
    string v = MI.Option(__T("Info_Version"));
    returnObj.Set("Version", Napi::String::From(env, v));
    //From: preparing an example file for reading
    FILE *F = fopen(cfile, "r"); //You can use something else than a f$

    if (F == 0)
        throw Napi::Error::New(env, "could not open file");

    returnObj.Set("File", info[0].As<Napi::String>());
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


    Napi::Array generalProps = inputObj.Get("General").As<Napi::Array>();
    Napi::Array videoProps = inputObj.Get("Video").As<Napi::Array>();
    Napi::Array audioProps = inputObj.Get("Audio").As<Napi::Array>();
    Napi::Array textProps = inputObj.Get("Text").As<Napi::Array>();

    for (size_t StreamKind = (size_t)Stream_General; StreamKind < Stream_Max; StreamKind++)
    {
        for (size_t StreamPos = 0; StreamPos < (size_t)MI.Count_Get((stream_t)StreamKind); StreamPos++)
        {
            String stream = MI.Get((stream_t)StreamKind, StreamPos, __T("StreamKind/String"));
            String pos = MI.Get((stream_t)StreamKind, StreamPos, __T("StreamKindPos"));
            size_t streamNum = 0;
            if (!pos.empty())
            {
                streamNum = stoi(pos) - 1;
            }
            switch (StreamKind)
            {
            case Stream_General:
                general = processProps(env, Stream_General, (size_t)streamNum, generalProps);
                returnObj.Set("General", general);
                break;
            case Stream_Video:
                if (videoProps)
                {
                    Napi::Value element = processProps(env, Stream_Video, (size_t)streamNum, videoProps);
                    videoArray.Set(streamNum, element);
                }
                break;
            case Stream_Audio:
                if (audioProps)
                {
                    Napi::Value element = processProps(env, Stream_Audio, (size_t)streamNum, audioProps);
                    audioArray.Set(streamNum, element);
                }
                break;
            case Stream_Text:
                if (textProps)
                {
                    Napi::Value element = processProps(env, Stream_Text, (size_t)streamNum, textProps);
                    textArray.Set(streamNum, element);
                }
                break;
            }
        }
    }

    MI.Close();
    fclose(F);
    
    uint32_t len = videoArray.Length();
    
    if (videoProps && len > 0)
    {
        returnObj.Set("Video", videoArray);
    }
    len = audioArray.Length();
    
    if (audioProps && len > 0)
    {
        returnObj.Set("Audio", audioArray);
    }
    len = textArray.Length();
    
    if (textProps && len > 0)
    {
        returnObj.Set("Text", textArray);
    }
    
    return returnObj;
}

/*napi_value Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_value fn;

    status = napi_create_function(env, NULL, 0, GetLocal, NULL, &fn);
    if (status != napi_ok)
    {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }

    status = napi_set_named_property(env, exports, "get_local", fn);
    if (status != napi_ok)
    {
        napi_throw_error(env, NULL, "Unable to populate exports");
    }

    status = napi_create_function(env, NULL, 0, Get, NULL, &fn);
    if (status != napi_ok)
    {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }

    status = napi_set_named_property(env, exports, "get", fn);
    if (status != napi_ok)
    {
        napi_throw_error(env, NULL, "Unable to populate exports");
    }
    return exports;
}*/

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "get_local"),
        Napi::Function::New(env, getLocal));
    exports.Set(Napi::String::New(env, "get_file"),
        Napi::Function::New(env, getFile));
    return exports;
}

//NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);
