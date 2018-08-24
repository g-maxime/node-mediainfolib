# node-mediainfolib

A [N-API](https://nodejs.org/api/n-api.html) module interface to MediaInfoLib. Very much a WIP.<br>

# Usage

Node 8 or greater must be installed together with the MediaInfoLib header files.
Install and use as follows:<br> 
`sudo apt-get install libmediainfo-dev` (Debian/Ubuntu only).<br>
`git clone https://github.com/dmooney65/node-mediainfolib.git`<br>
`cd node-mediainfolib`<br>
`npm install`<br>

See `example.js` for usage example. Just run `node example.js`.

The first parameter is a file name. It presently supports local files only. The second parameter is optional. It needs to be JavaScript Object containing valid `Stream` names for MediaInfoLib and an array of valid parameters for that stream. Currently only `General`, `Video` and `Audio` are supported. Default values are present in `main.js`.

Output is a JavaScript Object, with all special characters from the input object stripped, and return values added. Running `node example.js` should produce something like the following:<br>
`{ General: { Format: 'OGG', FileSizeString: '4.74 KiB', OverallBitRate_Mode: 'VBR' }, Audio: [ {  IDString: '18077 (0x469D)', Format: 'Vorbis', BitRate_Mode: 'VBR', BitRateString: '128 Kbps', Channels: '2', SamplingRateString: '44.1 KHz', Compression_ModeString: 'Lossy', Encoded_LibraryString: 'libVorbis 1.0.1 (CVS) (UTC 2003-03-08)' } ] }`
