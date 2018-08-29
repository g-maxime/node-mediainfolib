const mediaInfo = require('./main.js');

//Local example
var info = mediaInfo.get('Example.ogg');
console.log(info[0]);

//Remote example
info = mediaInfo.get('http://download.blender.org/peach/bigbuckbunny_movies/BigBuckBunny_320x180.mp4');
console.log(info[0]);
