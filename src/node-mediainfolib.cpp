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
            string outStr = std::regex_replace(prop.Utf8Value(), std::regex("[ /()*-]"), "_");
            //Wrap regexed string for output
            Napi::String property = Napi::String::From(env, outStr);
            returnvalue.Set(property, value);
        }
    }
    return returnvalue;
}

Napi::Object inform(Napi::Env env, stream_t Stream, size_t StreamNumber, bool full)
{
    Napi::Object returnvalue = Napi::Object::New(env);

    string countStr = MI.Get(Stream, (size_t)StreamNumber, 0, Info_Text);
    int count = stoi(countStr);
    for (int i = 0; i < count; i++)
    {
        bool include = true;
        string valueStr = MI.Get(Stream, StreamNumber, i, Info_Text);
        const char inInform = MI.Get(Stream, (size_t)StreamNumber, i, Info_Options)[0];
        if (inInform == 'N' && !full)
        {
            include = false;
        }
        if (!valueStr.empty() && include)
        {
            Napi::String value = Napi::String::From(env, valueStr);
            string keyStr = MI.Get(Stream, (size_t)StreamNumber, i, Info_Name);
            string key = std::regex_replace(keyStr, std::regex("[ /()*-]"), "_");
            Napi::String property = Napi::String::From(env, key);
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
    Napi::Object general;
    Napi::Array videoArray;
    Napi::Array audioArray;
    Napi::Array textArray;
    Napi::Array otherArray;
    Napi::Array imageArray;
    Napi::Array menuArray;

    string file = info[0].As<Napi::String>().Utf8Value();

    Napi::Object inputObj = info[1].As<Napi::Object>();
    string v = MI.Option(__T("Info_Version"));

    returnObj.Set("Version", Napi::String::From(env, v));

    MI.Open(__T(file));

    //returnObj.Set("File", info[0].As<Napi::String>());

    // Input values
    Napi::Array generalProps;
    Napi::Array videoProps;
    Napi::Array audioProps;
    Napi::Array textProps;
    Napi::Array otherProps;
    Napi::Array imageProps;
    Napi::Boolean menu = Napi::Boolean::New(env, false);

    if (inputObj.HasOwnProperty("General"))
    {
        generalProps = inputObj.Get("General").As<Napi::Array>();
        general = Napi::Object::New(env);
    }
    if (inputObj.HasOwnProperty("Video"))
    {
        videoProps = inputObj.Get("Video").As<Napi::Array>();
        videoArray = Napi::Array::New(env);
    }
    if (inputObj.HasOwnProperty("Audio"))
    {
        audioProps = inputObj.Get("Audio").As<Napi::Array>();
        audioArray = Napi::Array::New(env);
    }
    if (inputObj.HasOwnProperty("Text"))
    {
        textProps = inputObj.Get("Text").As<Napi::Array>();
        textArray = Napi::Array::New(env);
    }
    if (inputObj.HasOwnProperty("Other"))
    {
        otherProps = inputObj.Get("Other").As<Napi::Array>();
        otherArray = Napi::Array::New(env);
    }
    if (inputObj.HasOwnProperty("Image"))
    {
        imageProps = inputObj.Get("Image").As<Napi::Array>();
        imageArray = Napi::Array::New(env);
    }
    if (inputObj.HasOwnProperty("Menu"))
    {
        menu = inputObj.Get("Menu").As<Napi::Boolean>();
        menuArray = Napi::Array::New(env);
    }

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
                if (generalProps)
                {
                    //Napi::Object general;
                    if (generalProps.Get((uint32_t)0).As<Napi::String>().Utf8Value() == "default")
                    {
                        general = inform(env, Stream_General, (size_t)streamNum, false);
                    }
                    else if (generalProps.Get((uint32_t)0).As<Napi::String>().Utf8Value() == "full")
                    {
                        general = inform(env, Stream_General, (size_t)streamNum, true);
                    }
                    else
                    {
                        general = processProps(env, Stream_General, (size_t)streamNum, generalProps);
                    }
                    returnObj.Set("General", general);
                }
                break;
            case Stream_Video:
                if (videoProps)
                {
                    Napi::Value element;
                    if (videoProps.Get((uint32_t)0).As<Napi::String>().Utf8Value() == "default")
                    {
                        element = inform(env, Stream_Video, (size_t)streamNum, false);
                    }
                    else if (videoProps.Get((uint32_t)0).As<Napi::String>().Utf8Value() == "full")
                    {
                        element = inform(env, Stream_Video, (size_t)streamNum, true);
                    }
                    else
                    {
                        element = processProps(env, Stream_Video, (size_t)streamNum, videoProps);
                    }
                    videoArray.Set(streamNum, element);
                }
                break;
            case Stream_Audio:
                if (audioProps)
                {
                    Napi::Value element;
                    if (audioProps.Get((uint32_t)0).As<Napi::String>().Utf8Value() == "default")
                    {
                        element = inform(env, Stream_Audio, (size_t)streamNum, false);
                    }
                    else if (audioProps.Get((uint32_t)0).As<Napi::String>().Utf8Value() == "full")
                    {
                        element = inform(env, Stream_Audio, (size_t)streamNum, true);
                    }
                    else
                    {
                        element = processProps(env, Stream_Audio, (size_t)streamNum, audioProps);
                    }
                    audioArray.Set(streamNum, element);
                }
                break;
            case Stream_Text:
                if (textProps)
                {
                    Napi::Value element;
                    if (textProps.Get((uint32_t)0).As<Napi::String>().Utf8Value() == "default")
                    {
                        element = inform(env, Stream_Text, (size_t)streamNum, false);
                    }
                    else if (textProps.Get((uint32_t)0).As<Napi::String>().Utf8Value() == "full")
                    {
                        element = inform(env, Stream_Text, (size_t)streamNum, true);
                    }
                    else
                    {
                        element = processProps(env, Stream_Text, (size_t)streamNum, textProps);
                    }
                    textArray.Set(streamNum, element);
                }
                break;
            case Stream_Other:
                if (otherProps)
                {
                    Napi::Value element;
                    if (otherProps.Get((uint32_t)0).As<Napi::String>().Utf8Value() == "default")
                    {
                        element = inform(env, Stream_Other, (size_t)streamNum, false);
                    }
                    else if (otherProps.Get((uint32_t)0).As<Napi::String>().Utf8Value() == "full")
                    {
                        element = inform(env, Stream_Other, (size_t)streamNum, true);
                    }
                    else
                    {
                        element = processProps(env, Stream_Other, (size_t)streamNum, otherProps);
                    }
                    otherArray.Set(streamNum, element);
                }
                break;
            case Stream_Image:
                if (imageProps)
                {
                    Napi::Value element;
                    if (imageProps.Get((uint32_t)0).As<Napi::String>().Utf8Value() == "default")
                    {
                        element = inform(env, Stream_Image, (size_t)streamNum, false);
                    }
                    else if (imageProps.Get((uint32_t)0).As<Napi::String>().Utf8Value() == "full")
                    {
                        element = inform(env, Stream_Image, (size_t)streamNum, true);
                    }
                    else
                    {
                        element = processProps(env, Stream_Image, (size_t)streamNum, imageProps);
                    }
                    imageArray.Set(streamNum, element);
                }
                break;
            case Stream_Menu:
                if (menu)
                {
                    Napi::Object element = inform(env, Stream_Menu, (size_t)streamNum, false);
                    string startStr = MI.Get(Stream_Menu, (size_t)streamNum, "Chapters_Pos_Begin", Info_Text, Info_Name);
                    string endStr = MI.Get(Stream_Menu, (size_t)streamNum, "Chapters_Pos_End", Info_Text, Info_Name);
                    int start = stoi(startStr);
                    int end = stoi(endStr);
                    for (int i = start; i < end; i++)
                    {
                        string value = MI.Get(Stream_Menu, (size_t)streamNum, i, Info_Text);
                        string key = MI.Get(Stream_Menu, (size_t)streamNum, i, Info_Name);
                        element.Set(key, value);
                    }
                    menuArray.Set(streamNum, element);
                }
                break;
            }
        }
    }

    MI.Close();

    if (videoProps && videoArray.GetPropertyNames().Length() > 0)
    {
        returnObj.Set("Video", videoArray);
    }

    if (audioProps && audioArray.GetPropertyNames().Length() > 0)
    {
        returnObj.Set("Audio", audioArray);
    }

    if (textProps && textArray.GetPropertyNames().Length() > 0)
    {
        returnObj.Set("Text", textArray);
    }

    if (otherProps && otherArray.GetPropertyNames().Length() > 0)
    {
        returnObj.Set("Other", otherArray);
    }

    if (imageProps && imageArray.GetPropertyNames().Length() > 0)
    {
        returnObj.Set("Image", imageArray);
    }

    if (menu && menuArray.GetPropertyNames().Length() > 0)
    {
        returnObj.Set("Menu", menuArray);
    }

    return returnObj;
}

/*Napi::Value getLocal(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    //Output object and property/value arrays
    Napi::Object returnObj = Napi::Object::New(env);
    Napi::Object general;
    Napi::Array videoArray;
    Napi::Array audioArray;
    Napi::Array textArray;
    Napi::Array otherArray;
    Napi::Array imageArray;
    Napi::Object menuObject;

    string file = info[0].As<Napi::String>().Utf8Value();

    Napi::Object inputObj = info[1].As<Napi::Object>();
    string v = MI.Option(__T("Info_Version"));
    returnObj.Set("Version", Napi::String::From(env, v));

    const char *cfile = file.c_str();
    FILE *F = fopen(cfile, "r");

    if (F == 0)
        throw Napi::Error::New(env, "could not open file");

    returnObj.Set("File", info[0].As<Napi::String>());

    unsigned char *From_Buffer = new unsigned char[7 * 188];
    size_t From_Buffer_Size;
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

    // Input values
    Napi::Array generalProps;
    Napi::Array videoProps;
    Napi::Array audioProps;
    Napi::Array textProps;
    Napi::Array otherProps;
    Napi::Array imageProps;
    Napi::Boolean menu = Napi::Boolean::New(env, false);

    if (inputObj.HasOwnProperty("General"))
    {
        generalProps = inputObj.Get("General").As<Napi::Array>();
        general = Napi::Object::New(env);
    }
    if (inputObj.HasOwnProperty("Video"))
    {
        videoProps = inputObj.Get("Video").As<Napi::Array>();
        videoArray = Napi::Array::New(env);
    }
    if (inputObj.HasOwnProperty("Audio"))
    {
        audioProps = inputObj.Get("Audio").As<Napi::Array>();
        audioArray = Napi::Array::New(env);
    }
    if (inputObj.HasOwnProperty("Text"))
    {
        textProps = inputObj.Get("Text").As<Napi::Array>();
        textArray = Napi::Array::New(env);
    }
    if (inputObj.HasOwnProperty("Other"))
    {
        otherProps = inputObj.Get("Other").As<Napi::Array>();
        otherArray = Napi::Array::New(env);
    }
    if (inputObj.HasOwnProperty("Image"))
    {
        imageProps = inputObj.Get("Image").As<Napi::Array>();
        imageArray = Napi::Array::New(env);
    }
    if (inputObj.HasOwnProperty("Menu"))
    {
        menu = inputObj.Get("Menu").As<Napi::Boolean>();
        menuObject = Napi::Object::New(env);
    }

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
                if (generalProps)
                {
                    general = processProps(env, Stream_General, (size_t)streamNum, generalProps);
                    returnObj.Set("General", general);
                }
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
            case Stream_Other:
                if (otherProps)
                {
                    Napi::Value element = processProps(env, Stream_Other, (size_t)streamNum, otherProps);
                    otherArray.Set(streamNum, element);
                }
                break;
            case Stream_Image:
                if (imageProps)
                {
                    Napi::Value element = processProps(env, Stream_Image, (size_t)streamNum, imageProps);
                    imageArray.Set(streamNum, element);
                }
                break;
            case Stream_Menu:
                if (menu)
                {
                    string startStr = MI.Get(Stream_Menu, (size_t)streamNum, "Chapters_Pos_Begin", Info_Text, Info_Name);
                    string endStr = MI.Get(Stream_Menu, (size_t)streamNum, "Chapters_Pos_End", Info_Text, Info_Name);
                    int start = stoi(startStr);
                    int end = stoi(endStr);
                    for (int i = start; i < end; i++)
                    {
                        string chapter = MI.Get(Stream_Menu, (size_t)streamNum, i, Info_Text);
                        string startTime = MI.Get(Stream_Menu, (size_t)streamNum, i, Info_Name);
                        menuObject.Set(chapter, startTime);
                    }
                }
                break;
            }
        }
    }

    MI.Close();
    fclose(F);

    if (videoProps && videoArray.GetPropertyNames().Length() > 0)
    {
        returnObj.Set("Video", videoArray);
    }

    if (audioProps && audioArray.GetPropertyNames().Length() > 0)
    {
        returnObj.Set("Audio", audioArray);
    }

    if (textProps && textArray.GetPropertyNames().Length() > 0)
    {
        returnObj.Set("Text", textArray);
    }

    if (otherProps && otherArray.GetPropertyNames().Length() > 0)
    {
        returnObj.Set("Other", otherArray);
    }

    if (imageProps && imageArray.GetPropertyNames().Length() > 0)
    {
        returnObj.Set("Image", imageArray);
    }

    if (menu && menuObject.GetPropertyNames().Length() > 0)
    {
        returnObj.Set("Menu", menuObject);
    }

    return returnObj;
}*/

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    //exports.Set(Napi::String::New(env, "get_local"),
    //            Napi::Function::New(env, getLocal));
    exports.Set(Napi::String::New(env, "get_file"),
                Napi::Function::New(env, getFile));
    return exports;
}

//NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);
