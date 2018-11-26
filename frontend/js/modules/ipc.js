const remote = require('electron').remote
const currentWindow = remote.getCurrentWindow();
const {ipcRenderer} = require('electron')
const IPCStream = require('electron-ipc-stream')
const fs = require('fs')
let ipcs = new IPCStream('any-arbitrary-channel-name')
let startTime = 0;

ipcs.on('data', function (data) {
    console.log('done') // => {name: 'JP'}
})

ipcRenderer.on('newSegment', (event, arg) => {
    Segments.addSegment(arg);
});

ipcRenderer.on('update', (event, arg) => {

    
});

ipcRenderer.on('init', (event, arg) => {
    let args = arg.split(' ');
    Timeline.init(args[0], args[1]);
});

ipcRenderer.on('addedInterpretation', (event, arg) => {
    Interpretations.addId(arg);
});

function requestConnect(segmentId, interpretationId) {
    ipcRenderer.send('connect', JSON.stringify([segmentId, interpretationId]));
}

function requestManipulateInterpretation() {
    ipcRenderer.send('manipulateInterpretation', arguments)
}

function requestAddInterpretation() {
    ipcRenderer.send('addInterpretation', arguments)
}

function requestAdd() {
    startTime = Date.now();
    ipcRenderer.send('addSegment', '');
}

function requestRemove(id) {
    console.log(id, 'request')

    ipcRenderer.send('removeSegment', id);
}

function requestUpdate(segment) {
    startTime = Date.now();
    let data =  {
        id: segment.id,
        start: segment.start,
        end: segment.end,
        global_i: segment.global_i,
        local_i: segment.local_i
    };
    ipcRenderer.send('manipulateSegment', JSON.stringify(data));
    ipcRenderer.send('update', '');
}

function requestInit(path) {
    ipcRenderer.send('init', path);
}

ipcRenderer.on('progress', (event, arg) => {
    let data = JSON.parse(arg);
    Segments.updateProgress(data);
});

function requestProgress(id) {
    ipcRenderer.send('getProgress', id);
}