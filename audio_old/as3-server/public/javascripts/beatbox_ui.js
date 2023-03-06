"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();

let tempo = 120;
let volume = 80;
let mode = 1;

let connectionLost = 0;
let connectionLostNodeJs = 0;

let nodeJSConnected = true;
let beatBoxConnected = true;

const volumeInput = document.getElementById("volumeid");
const tempoInput = document.getElementById("tempoid");
const currentDrumMode = document.getElementById("modeid");

const errorText1 = document.getElementById("error-text");
const errorText2 = document.getElementById("error-text2");

const statusText = document.getElementById("status");

$(document).ready(function() {
	$('#modeNone').click(function(){
		setModeViaUDP(0);
	});

	$('#modeRock1').click(function(){
		setModeViaUDP(1);
	});

	$('#modeRock2').click(function(){
		setModeViaUDP(2);
	});

	$('#volumeDown').click(function(){
		setVolumeViaUDP(volume-5);
	});
	$('#volumeUp').click(function(){
		setVolumeViaUDP(volume+5);
	});

	$('#tempoDown').click(function(){
		setTempoViaUDP(tempo-5);
	});
	$('#tempoUp').click(function(){
		setTempoViaUDP(tempo+5);
	});

	$('#stop').click(function(){
		sendCommandViaUDP("stop");
	});

	$('#playHiHat').click(function(){
		sendCommandViaUDP("play 1");
	});

	$('#playSnare').click(function(){
		sendCommandViaUDP("play 2");
	});

	$('#playBase').click(function(){
		sendCommandViaUDP("play 0");
	});
});

setInterval(()=>{
	setSystemViaUDP();
	ErrorCheck();
},1000);

setInterval(()=>{
	setVolumeViaUDP(-1000);
	setTempoViaUDP(-1000);
	setModeViaUDP(-1000);
},250);

function ErrorCheck(){
	NodeJsResCheck();
	socket.on('error', (data)=>{
		if(data === "noConnection"){
			beatBoxConnected = false;
		}
		else if(data ==="noError"){
			beatBoxConnected = true;
			errorText1.innerText = "";
		}
	})
	if(beatBoxConnected){
		connectionLost = 0;
		errorText1.innerText = "";
	}
	else{
		connectionLost+=1;
		errorText1.innerText = "Connection Lost to Beat Box server, last active: "+connectionLost+"s ago";
	}

	if(connectionLost < 1 && connectionLostNodeJs < 1){
		$('#error-box').hide();
	}
	else{
		$('#error-box').show();
	}
}

function NodeJsResCheck(){
	socket.on('disconnect', () =>{
		nodeJSConnected = false;
	});
	socket.on('connect', () =>{
		nodeJSConnected = true;
	})

	if(nodeJSConnected){
		connectionLostNodeJs = 0;
		errorText2.innerText = "";
	}
	else{
		connectionLostNodeJs+=1;
		errorText2.innerText = "Connection Lost to Node.js server, last active: "+connectionLostNodeJs+"s ago";
	}
}

function sendCommandViaUDP(message) {
	socket.emit('daUdpCommand', message);

};

function setSystemViaUDP(){

	socket.emit('daUdpCommand', "system ");
	socket.on('system', (data)=>{
		// console.log(data);
		// $('#status').text("Device up for: "+ data);
		statusText.innerText = "Device up for: "+ data;
	})
}

function setVolumeViaUDP(value){
	socket.emit('daUdpCommand', "volume "+ value);
	setVolume()
}

function setVolume(){
	socket.on('volume', (data)=>{
		volume =  Number(data);
		volumeInput.value = volume;
	})
}

function setTempoViaUDP(value){
	socket.emit('daUdpCommand', "tempo "+ value);
	setTempo();
}

function setTempo(){
	socket.on('tempo', (data)=>{
		tempo =  Number(data);
		tempoInput.value = tempo;
	})
}

function setModeViaUDP(value){
	socket.emit('daUdpCommand', "mode "+ value);
	setMode();
}

function setMode(){
	socket.on('mode', (data)=>{
		mode =  Number(data);
	})

	if(mode === 0){
		currentDrumMode.innerHTML = "None"
	}
	else if(mode === 1){
		currentDrumMode.innerHTML = "Rock #1"
	}
	else if(mode ===2){
		currentDrumMode.innerHTML = "Rock #2077"
	}
}