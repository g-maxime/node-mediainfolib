const cardinfo = require('./build/Debug/mediainfolib');

module.exports.getMediaInfo = (file) => {
    return cardinfo.get_media_info(file);
}

module.exports.getBigMediaInfo = (file) => {
    return cardinfo.get_big_media_info(file);
}


module.exports.PLAYBACK = 0;
module.exports.CAPTURE = 1;