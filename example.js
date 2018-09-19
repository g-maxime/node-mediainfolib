const mediaInfo = require('./main.js');

//Local example
//var info = mediaInfo.get('Example.ogg');
//console.log(info[0]);

//Remote example
info = mediaInfo.get('http://download.blender.org/peach/bigbuckbunny_movies/BigBuckBunny_320x180.mp4');
console.log(info);

//info = mediaInfo.get('/home/dominic/Downloads/audiobooks/BaudolinoUnabridged_ep5.m4b');
//console.log(info);

//info = mediaInfo.get('/storage/HomeVideos/tape01.mkv');
//console.log(info);

//info = mediaInfo.get('/storage/music/classical/Igor\ Stravinsky/Best\ Of\ Stravinsky/04\ Igor\ Stravinsky\ -\ Pulcinella\,\ suite\ for\ orchestra\;\ Andantino.wav')
//console.log(info);