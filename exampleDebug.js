const mediaInfo = require('./mainDebug.js');

// Get params for first playback device
console.log(mediaInfo.getMediaInfo('20180818165955_rec.flac'));

console.log(mediaInfo.getBigMediaInfo('20180818165955_rec.flac'));

