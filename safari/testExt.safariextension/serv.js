var port = 8888;
var http = require('http');
http.createServer(function (req, res) {
  var data = "<!DOCTYPE html><html><head><title>Hello World</title></head><body>Hello World! respond by nodejs server.</body></html>";
  res.writeHead(200, {'Content-Type': 'text/html','Content-Length':data.length});
  res.write(data);
  res.end();
}).listen(port);
console.log ('port listen at :' + port);
