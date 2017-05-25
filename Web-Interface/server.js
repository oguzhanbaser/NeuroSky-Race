var express = require('express');
var app = express();
var http = require('http').Server(app);
var socket = require('socket.io')(http);
var fs = require('fs');
var path = require('path');

var SerialPort1 = require("serialport");
var SerialPort2 = require("serialport");
var port = 3000;

var portName1 = "COM5";
var portName2 = "COM13";

app.use(express.static(path.join(__dirname)));

app.get('/', function(req ,res) {
	res.sendFile (__dirname + '/layouts/layout-1.html');
});

var data1 = {"signal": 0, "attention": 0};
var data2 = {"signal": 0, "attention": 0};
var isOpened1 = false, isOpened2 = false;

try{
    serialPort1 = new SerialPort1(portName1, {
        baudrate: 57600,
        parser: SerialPort1.parsers.readline('\r\n')
    });
    isOpened1 = true;
}catch(err){
    console.log(err);
}

try{
    serialPort2 = new SerialPort2(portName2, {
        baudrate: 57600,
        parser: SerialPort2.parsers.readline('\r\n')
    });
    isOpened2 = true;
}catch(err){
    console.log(err);
}

if(isOpened1)
{
    serialPort1.on('open',function(error) {
        if(error)
        {
            console.log("Error");
            return;
        }
        console.log('Port open');
    });

    serialPort1.on('data', function(p_data, err){

        var s_data = p_data.split("|");

        if(s_data[0] == "#")
        {
            data1["signal"] = 100 - s_data[1];
            data1["attention"] = s_data[2];
        }

    });
}

if(isOpened2)
{
    serialPort2.on('open',function(error) {
        if(error)
        {
            console.log("Error");
            return;
        }
        console.log('Port open');
    });

    serialPort2.on('data', function(p_data, err){

        var s_data = p_data.split("|");

        if(s_data[0] == "#")
        {
            data2["signal"] = 100 - s_data[1];
            data2["attention"] = s_data[2];
        }

    });
}

socket.on('connection', function(p_socket){
    console.log("Biri baglandi");

    var sendInterval = setInterval(function(){
        p_socket.emit('player1', data1);
        p_socket.emit('player2', data2);
    }, 1000);

    p_socket.on('close', function(){
        clearInterval(sendInterval);
    });
});

http.listen(port,function(){
	console.log("Listeining: ", port);
});
