var thrift = require('thrift');
var ImageServer = require('./gen-nodejs/ImageServer');
var ttypes = require('./gen-nodejs/ImageServer_types');

var connection = thrift.createConnection('localhost', 9090);

var client = thrift.createClient(ImageServer, connection);

connection.on('error', function (err) {
  console.log(err);
});

/*
client.reload_func("calc_distance_bar", function (err, data) {
  if (err) {
    console.log(err);
  }
});
*/

exports.query = function (path, callback) {
  client.query(path, function (err, data) {
    callback(err, data);
  });
};
