process.env['ELECTRON_DISABLE_SECURITY_WARNINGS'] = 'true';

const wrapper = require('../build/Release/wrapper.node')
const IPCStream = require('electron-ipc-stream')
const fs = require('fs')


//console.log(`\t > wrapper init:     \t ${wrapper.init("videos/bootraffa/bootraffa%04d.jpg")}`)
console.log(`\n`)

const {app, BrowserWindow} = require('electron')
const {ipcMain} = require('electron')

let win;
let ipcs;
let tempfile;
let path

function createWindow () {
  tmpfile = '/tmp/mainfile'
  // Create the browser window.
  win = new BrowserWindow({width: 800, height: 1000})
  // win.setMenu(null)
   // and load the index.html of the app.
  win.loadURL(`file://${__dirname}/index.html`)

  ipcs = new IPCStream('any-arbitrary-channel-name', win)
}

ipcMain.on("addInterpretation", (event, arg) => {
  let arr = [];
  Object.keys(arg).forEach(function (key) {
    arr.push(arg[key]);
  });

  console.log(arr);

  let id = wrapper.add_interpretation.apply(null, arr);

  event.sender.send('addedInterpretation', id)
  //console.log(id);
})

ipcMain.on("manipulateInterpretation", (event, arg) => {
  let arr = [];
  Object.keys(arg).forEach(function (key) {
    arr.push(arg[key]);
  });
  console.log(arr);

  wrapper.manipulate_interpretation.apply(null, arr);

})

ipcMain.on("getProgress", (event, arg) => {
  let progress = wrapper.get_segment_progress(arg);
  event.sender.send('progress', JSON.stringify({progress: progress, id: arg}))
})

ipcMain.on("init", (event, arg) => {
  let res = wrapper.init(`${arg}frame%01d.jpg`)
  event.sender.send('init', arg + ' ' + parseInt(res.split(' ')[1]));
})

ipcMain.on('addSegment', (event, arg) => {
  let id = wrapper.add_segment(0, 5, 1.0, 1.0)
  event.sender.send('newSegment', id)
})

ipcMain.on('removeSegment', (event, arg) => {
  wrapper.delete_segment(parseInt(arg))
})

ipcMain.on('manipulateSegment', (event, arg) => {
  let data = JSON.parse(arg)
  console.log(data)
  let t = process.hrtime()
  wrapper.manipulate_segment(data.id, data.start, data.end, data.local_i, data.global_i, true)
})


ipcMain.on('update', (event, arg) => {

  event.sender.send('update', '')
})

ipcMain.on('connect', (event, arg) => {

  console.log(JSON.parse(arg));
  wrapper.connect.apply(null, JSON.parse(arg));
})

app.on('ready', createWindow)

global.shared = {data: []}

ipcMain.on('load-result', (event, arg) => {
  let t = process.hrtime()
  console.log(t)
  global.shared.random = wrapper.base_get_result_image() //addon.giveRandomArrayBuffer()

  t = process.hrtime(t)
  console.log(t)
  event.sender.send('load-result', t[0] + ',' + t[1])
})

ipcMain.on('add_segm', (event, arg) => {
  var start=Math.floor((Math.random() * 330) + 0)
  var end = (Math.floor((Math.random() * (330-start)) + start))
  global.shared.random = wrapper.add_segment(start, end, 1.0, 1.0)

})
