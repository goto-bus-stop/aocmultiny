const net = require('net')
const split = require('split')

const players = {}
const sessions = {}

let enumSessionsId = 100
const enumSessionResponses = {}

class Session {
  constructor (id, hostId) {
    this.id = id
    this.hostId = hostId
    this.players = []
  }

  getPlayer (id) {
    return this.players.find((player) => player.id === id)
  }

  get host () {
    return this.getPlayer(this.hostId)
  }

  createPlayer (id, socket) {
    const player = new Player(id, socket)
    this.players.forEach((other) => {
      other.sendNewPlayer(player)
      player.sendNewPlayer(other)
      if (other.sdp) {
        player.sendPlayerSdp(other)
      }
    })
    this.players.push(player)
    console.log('new player', this.id, id)
    return player
  }

  removePlayer (player) {
    this.players = this.players.filter((other) => other.id !== player.id)
  }
}

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
  let session
  let player
  socket
    .pipe(split('\n'))
    .on('data', (line) => {
      console.log('<< <' + (player ? player.id : 'unk') + '  ' + JSON.stringify(line))

      if (/^create /.test(line)) {
        const match = /^create session:(.*?),id:(\d+)\s*$/.exec(line)
        const sid = match[1]
        const pid = parseInt(match[2], 10)
        if (!sessions[sid]) {
          console.log('new session', sid)
          sessions[sid] = new Session(sid, pid)
        }
        session = sessions[sid]
        player = session.createPlayer(pid, socket)
        players[pid] = player
      } else if (/^sdp /.test(line)) {
        const match = /^sdp to:(\d+),sdp:(.*?)\s*$/.exec(line)
        const toId = parseInt(match[1], 10)
        const sdp = match[2]
        player.sdps[toId] = sdp

        const target = session.getPlayer(toId)
        if (target) target.sendPlayerSdp(player)
      } else if (/^relay /.test(line)) {
        const match = /^relay to:(\d+),message:(.*?)\s*$/.exec(line)
        const toId = parseInt(match[1], 10)
        const message = match[2]
        const target = players[toId]
        if (target) target.sendRelayed(message)
      } else if (/^enums /.test(line)) {
        enumSessionsId++
        const target = sessions[line.substr(6)]
        if (target) {
          enumSessionResponses[enumSessionsId] = socket
          target.host.send('enums ' + enumSessionsId)
        }
      } else if (/^enumr /.test(line)) {
        const match = /^enumr id:(\d+),reply:(.*?)\s*$/.exec(line)
        const responseTo = match[1]
        const response = match[2]
        const target = enumSessionResponses[responseTo]
        if (target) {
          console.log('relaying')
          target.write('enumr ' + response + '\n')
          delete enumSessionResponses[responseTo]
        }
      }
    })
    .on('end', () => {
      if (session) {
        if (player) {
          session.removePlayer(player)
          delete players[player.id]
        }
        if (session.players.length === 0) {
          delete sessions[session.id]
        }
      }
    })
})

server.listen(7788)
