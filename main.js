const cardinfo = require('./build/Release/mediainfolib');

let defaultFormat = {
    General: ['UniqueID', 'Format', 'Format_Version', 'FileSize/String',
        'Duration/String', 'OverallBitRate_Mode', 'OverallBitRate/String',
        'Encoded_Date', 'Encoded_Application/String', 'Encoded_Library/String',
        'OriginalSourceMedium'],
    Video: ['ID/String', 'OriginalSourceMedium_ID/String', 'Format', 'Format_Version', 'Format_Profile', 'Format_Settings_BVOP', 'Format_Settings_Matrix',
        'Format_Settings_GOP', 'CodecID', 'CodecID/Info', 'Duration', 'BitRate_Mode', 'BitRate', 'BitRate_Maximum/String',
        'Width', 'Height', 'DisplayAspectRatio', 'FrameRate_Mode', 'FrameRate', 'Standard', 'ColorSpace', 'ChromaSubsampling',
        'BitDepth', 'ScanType', 'ScanOrder', 'Compression_Mode', 'Bits-(Pixel*Frame)', 'TimeCode_FirstFrame',
        'TimeCode_Source', 'StreamSize/String', 'Language', 'Default', 'Forced', 'colour_primaries', 'transfer_characteristics',
        'matrix_coefficients'],
    Audio: ['ID/String', 'OriginalSourceMedium_ID/String', 'Format', 'Format/Info', 'Format_Settings_ModeExtension',
        'Format_Settings_Endianness', 'CodecID/String', 'Duration/String', 'BitRate_Mode', 'BitRate/String',
        'Channel(s)', 'ChannelPositions', 'SamplingRate/String', 'BitDepth', 'FrameRate/String', 'Compression_Mode/String',
        'StreamSize/String', 'Title', 'Language/String', 'Default', 'Forced','Encoded_Library/String',]
};

module.exports.getMediaInfo = (file, format = defaultFormat) => {
    return cardinfo.get_media_info(file, format);
}
