var express = require('express');
var app = express();
var http = require('http').Server(app);
var socket = require('socket.io')(http);
var fs = require('fs');
var path = require('path');

const SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline');
const port = new SerialPort("COM16", { baudRate: 9600, autoOpen: false });
const parser = new Readline();
var webPort = 3000;

app.use(express.static(path.join(__dirname)));

app.get('/', function(req ,res) {
	res.sendFile (__dirname + '/layouts/layout-1.html');
});

var data1 = {"signal": 0, "attention": 0};
var data2 = {"signal": 0, "attention": 0};

port.open(function (err) {
    if (err) {
      return console.log('Error opening port: ', err.message)
    }
  
    // Because there's no callback to write, write errors will be emitted on the port:
    port.write('main screen turn on')
  })

try{
    port.pipe(parser);

    parser.on('data', function(p_data){
        var s_data = p_data.split("|");
        console.log(s_data);
        var user = s_data[1];

        if(s_data[0] == "#")
        {
            if(user == 1)
            {
                data1["signal"] = 100 - s_data[2];
                data1["attention"] = s_data[3];
            }else if(user == 2)
            {
                data2["signal"] = 100 - s_data[2];
                data2["attention"] = s_data[3];
            }
        }
    });

    isOpened1 = true;
}catch(err){
    //console.log(err);
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

http.listen(webPort,function(){
	console.log("Listeining: ", webPort);
});
