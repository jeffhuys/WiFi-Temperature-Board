var PORT = 33333;
var HOST = '0.0.0.0';

var dgram = require('dgram');
var server = dgram.createSocket('udp4');

var express = require('express');
var cors = require('cors');
var app = express();

var latestTemperature = -1;

app.use(cors());
app.get('/temperature', function (req, res) {
  res.send({ latestTemperature: latestTemperature })
})
app.listen(3000, function() {
	console.log('CORS-enabled web server listening on port 3000');
})


server.on('listening', function () {
    var address = server.address();
    console.log('UDP Server listening on ' + address.address + ":" + address.port);
});

server.on('message', function (message, remote) {
    console.log(remote.address + ':' + remote.port +' - ' + message);
    latestTemperature = parseFloat(message);
});

server.bind(PORT, HOST);