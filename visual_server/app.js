var express = require('express');
var app = express();
var server = require('http').createServer(app);
var io = require('socket.io')(server);
var mqtt = require('mqtt');
var mqttInfo = require('./secret/mqtt-info.json');
const { Observable, Subject, ReplaySubject, from, of, interval } = require('rxjs');
const { map, take, filter, switchMap } = require('rxjs/operators');

app.use(express.static(__dirname + '/node_modules'));
app.use(express.static(__dirname + '/public' ));
app.get('/', function(req, res,next) {
    res.sendFile(__dirname + '/index.html');
});

io.on('connection', function(client) {
    client.on('join', function(data) {
        console.log(data);
    });

    client.on('mouse-click',
        function(index) {
            var data = testdata[index % testdata.length];
            var message = JSON.stringify(data);
            io.sockets.emit('mesh-data', message);
        }
    );
});



var options = {
    host: 'broker.hivemq.com',
    port: 1883,
    username: mqttInfo.username,
    password: mqttInfo.password
}

var mqttClient  = mqtt.connect(options)

mqttClient.on('connect', function () {
    mqttClient.subscribe('btt_mesh_gateway/data', function (err) {
        if (!err) {
            console.log("successfully subscribed to topic");
        } else {
            console.log("failed to subscribe to topic");
        }
    })
})

const parseMessage = (message) => {
    let json = "{\"";
    const msg = message.split('[');
    const body = msg[0];
    const node = msg[1];

    json += node + "\": ["

    routes = body.split('/');
    routes.forEach((route, index) => {
        info = route.split(',');
        json += "{\"n\":" + info[0];
        json += ",\"r\":" + info[1];
        json += "}";

        if (index != routes.length - 1) {
            json += ",";
        }
    })

    json += "]}";

    return json;
}

mqttClient.on('message', function (topic, message) {
    var parsedJson = parseMessage(message.toString());
    console.log(parsedJson)
    io.sockets.emit('mesh-data', parsedJson.toString());
})

// {"2": [{"n":1,"r":-44},{"n":255,"r":0},{"n":3,"r":-13}]},
// 255,0/2,-37/3,-35/4,-47[1

server.listen(14200);
