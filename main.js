const cardinfo = require('./build/Release/mediainfolib');

let defaultGeneralProps = ['UniqueID', 'Format', 'Format_Version', 'Format_Profile', 'CodecID/String', 'FileSize/String',
    'Duration/String', 'OverallBitRate_Mode', 'OverallBitRate/String', 'Movie', 'Track', 'Performer', 'Composer', 'Recorder_date',
    'Recorded_Date', 'Encoded_Date', 'Tagged_Date', 'Encoded_Application/String', 'Encoded_Library/String',
    'OriginalSourceMedium'];

let defaultVideoProps = ['ID', 'OriginalSourceMedium_ID/String', 'Format', 'Format_Version', 'Format_Profile', 'Format_Settings_BVOP', 'Format_Settings_Matrix',
    'Format_Settings_GOP', 'CodecID', 'CodecID/Info', 'Duration', 'BitRate_Mode', 'BitRate', 'BitRate_Maximum/String',
    'Width', 'Height', 'DisplayAspectRatio', 'FrameRate_Mode', 'FrameRate', 'Standard', 'ColorSpace', 'ChromaSubsampling',
    'BitDepth', 'ScanType', 'ScanOrder', 'Compression_Mode', 'Bits-(Pixel*Frame)', 'TimeCode_FirstFrame',
    'TimeCode_Source', 'StreamSize/String', 'Language', 'Default', 'Forced', 'colour_primaries', 'transfer_characteristics',
    'matrix_coefficients'];

let defaultAudioProps = ['ID', 'OriginalSourceMedium_ID/String', 'Format', 'Format/Info', 'Format_Settings_ModeExtension',
    'Format_Settings_Endianness', 'CodecID/String', 'Duration/String', 'BitRate_Mode', 'BitRate/String',
    'Channel(s)', 'ChannelPositions', 'SamplingRate/String', 'BitDepth', 'FrameRate/String', 'Compression_Mode/String',
    'StreamSize/String', 'Title', 'Language/String', 'Default', 'Forced', 'Encoded_Library/String',];

let defaultTextProps = ['ID', 'OriginalSourceMedium_ID/String', 'Format', 'CodecID', 'CodecID/Info', 'Duration/String', 'BitRate/String',
'ElementCount', 'StreamSize/String', 'Language/String', 'Default', 'Forced'];

let defaultProperties = {
    General: defaultGeneralProps,
    Video: defaultVideoProps,
    Audio: defaultAudioProps,
    Text: defaultTextProps
};

module.exports.getLocal = (file, properties = defaultProperties) => {
    
    var infos = [];
    if (file.constructor === Array) {

        file.forEach((item) => {
            var info = cardinfo.get_local(item, properties);
            infos.push(info);
        });
    } else infos.push(cardinfo.get_local(file, properties));

    return infos;
}

module.exports.get = (file, properties = defaultProperties) => {
    var infos = [];
    if (file.constructor === Array) {

        file.forEach((item) => {
            var info = cardinfo.get(item, properties);
            infos.push(info);
        });
    } else infos.push(cardinfo.get(file, properties));

    return infos;
}

