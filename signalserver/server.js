'use strict'
const net = require('net')
const split = require('split')

const players = {}
const sessions = {}

let nextPlayerId = 1

let enumSessionsId = 100
const enumSessionResponses = {}

class Player {
  constructor (id, socket) {
    this.id = id
    this.socket = socket
    this.sdps = {}
  }

  send (msg) {
    console.log('>> @' + this.id + '  ' + JSON.stringify(msg))
    this.socket.write(msg + '\n')
  }

  sendNewPlayer (other) {
    this.send('player id:' + other.id)
  }

  sendPlayerSdp (other) {
    if (other.sdps[this.id]) {
      this.send('sdp player:' + other.id + ',sdp:' + other.sdps[this.id])
    }
  }

  sendRelayed (message) {
    this.send('relayed message:' + message)
  }
}

const server = net.createServer((socket) => {
  const playerId = nextPlayerId++
  const player = new Player(playerId, socket)
  let host
  let hostingSession
  players[playerId] = player
  socket
    .pipe(split('\n'))
    .on('data', (line) => {
      console.log('<< ~' + (player ? player.id : 'unk') + '  ' + JSON.stringify(line))

      if (/^host /.test(line)) {
        const sid = line.substr(5)
        if (!sessions[sid]) {
          console.log('new session', sid)
          sessions[sid] = player
          host = player
          hostingSession = sid
        }
      } else if (/^join /.test(line)) {
        const sid = line.substr(5)
        if (sessions[sid]) {
          host = sessions[sid]
          host.send(`join ${player.id}`)
          player.send(`join ${host.id}`)
        }
      } else if (/^sdp /.test(line)) {
        const match = /^sdp to:(\d+),sdp:(.*?)\s*$/.exec(line)
        const toId = parseInt(match[1], 10)
        const sdp = match[2]

        const target = player === host ? players[toId] : host
        if (target) target.send(`sdp player:${player.id},sdp:${sdp}`)
      } else if (/^enums /.test(line)) {
        enumSessionsId++
        const target = sessions[line.substr(6)]
        if (target) {
          enumSessionResponses[enumSessionsId] = player
          target.send('enums ' + enumSessionsId)
        }
      } else if (/^enumr /.test(line)) {
        const match = /^enumr id:(\d+),reply:(.*?)\s*$/.exec(line)
        const responseTo = match[1]
        const response = match[2]
        const target = enumSessionResponses[responseTo]
        if (target) {
          console.log('relaying')
          target.send('enumr ' + response)
          delete enumSessionResponses[responseTo]
        }
      } else if (/^send /.test(line)) {
        if (host === player) {
          const match = /^send to:(\d+),message:(.*?)\s*$/.exec(line)
          const target = players[match[1]]
          if (target) {
            target.send(match[2])
          }
        } else {
          const message = line.substr(5)
          host.send(message)
        }
      }
    })
    .on('end', () => {
      delete players[player.id]
      if (hostingSession) {
        delete sessions[hostingSession]
      }
    })
})

server.listen(7788)
