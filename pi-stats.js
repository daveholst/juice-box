var osu = require('node-os-utils')

var cpu = osu.cpu

cpu.usage()
  .then(info => {
    console.log(info)
  })

  var drive = osu.drive

drive.info()
  .then(info => {
    console.log(info)
  })

  var mem = osu.mem

mem.info()
  .then(info => {
    console.log(info)
  })