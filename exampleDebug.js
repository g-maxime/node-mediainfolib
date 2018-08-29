const mediaInfo = require('./mainDebug.js');

//var info = mediaInfo.getMediaInfo('Example.ogg');
var info = mediaInfo.get('http://download.blender.org/peach/bigbuckbunny_movies/BigBuckBunny_320x180.mp4');

console.log(info);
