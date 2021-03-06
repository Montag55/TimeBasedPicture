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
    let path = args[0];
    let start = args[1];
    let end = args[2];
    let width = args[3];
    let height = args[4];
    Timeline.init(path, start, end, width, height);
});

ipcRenderer.on('addedTransformation', (event, arg) => {
    Transformations.addId(arg);
});

function requestConnect(segmentId, transformationId) {
    ipcRenderer.send('connect', JSON.stringify([segmentId, transformationId]));
}

function requestManipulateTransformation() {
    ipcRenderer.send('manipulateTransformation', arguments)
}

function requestAddTransformation() {
    ipcRenderer.send('addTransformation', arguments)
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
        local_i: segment.local_i,
        enableMask: segment.enableMask
    };
    ipcRenderer.send('manipulateSegment', JSON.stringify(data));
    ipcRenderer.send('update', '');
}

function requestFull() {
    ipcRenderer.send('resizeFull');
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
