"use strict";
/*
 * Respond to commands over a websocket to relay UDP commands to a local program
 */

var socketio = require('socket.io');
var io;

var dgram = require('dgram');

exports.listen = function(server) {
	io = socketio.listen(server);
	io.set('log level 1');

	io.sockets.on('connection', function(socket) {
		handleCommand(socket);
	});
};

var reply = "";

function handleCommand(socket) {
	// Pased string of comamnd to relay
	socket.on('daUdpCommand', function(data) {
		console.log('daUdpCommand command: ' + data);

		// Info for connecting to the local process via UDP
		var PORT = 12345;
		var HOST = '127.0.0.1';
		var buffer = new Buffer(data);

		var client = dgram.createSocket('udp4');
		let timer;

		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
			if (err)
				throw err;
			timer = setTimeout(() => {
				socket.emit('error', "noConnection");
			}, 1000);
		});

		client.on('listening', function (data) {
			var address = client.address();
			console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});

		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
			clearTimeout(timer);
			socket.emit('error', "noError");
			console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);
			reply = message.toString('utf8')
			let index = reply.indexOf(" ");
			// split only the first blank
			let firstArray = reply.substr(0,index);
			let secondArray = reply.substr(index+1);

			if(firstArray === "tempo"){
				socket.emit('tempo', secondArray);
			}
			else if(firstArray === "volume"){
				socket.emit('volume', secondArray);
			}
			else if(firstArray === "system"){
				socket.emit('system', secondArray);
			}
			else if(firstArray === "mode"){
				socket.emit('mode', secondArray);
			}
			else{
				socket.emit('commandReply', reply);
			}
			client.close();
		});

		client.on("UDP Client: close", function() {
			console.log("closed");
		});
		client.on("UDP Client: error", function(err) {
			console.log("error: ",err);
		});
	});
};