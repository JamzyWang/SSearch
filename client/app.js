var fs = require('fs');

var client = require('./client');
var uuid = require('node-uuid');
var express = require('express');
var app = express();

app.use('/public', express.static(__dirname + '/public'));

app.use(express.bodyParser({
  keepExtensions: false,
  uploadDir: __dirname + '/public/uploads'
}));

app.engine('.html', require('ejs').__express);
app.set('views', __dirname + '/views');
app.set('view engine', 'html');

app.get('/', function (req, res) {
  res.render('index');
});

app.get('/query/:name', function (req, res) {
  var img_path = './public/uploads/' + req.params.name;

  fs.readFile(img_path + '.js', 'utf8', function (err, data) {
    if (err) {
      console.log(err);
      return;
    }

    res.render('result', { 'results': data });
  });
});

function query_and_redirect(img_path, name, callback) {
  client.query(img_path, function (err, data) {
    var results = {};
    results['/public/uploads/' + name] = data.slice(0, 100);	
     fs.writeFile(img_path + '.js', JSON.stringify(results), function (err) {
      if (err) throw err;

      if (callback) callback();
    });
  });
}

app.post('/sketch', function (req, res) {
  var base64Data = req.body.imgData.replace(/^data:image\/(jpeg|png);base64,/, '');
  var name = uuid.v4();
  var img_path = __dirname + '/public/uploads/' + name;

  fs.writeFile(img_path, base64Data, 'base64', function (err) {
    query_and_redirect(img_path, name, function () {            
	 res.end(name);
    });
  });
});

app.post('/upload', function (req, res) {
  var name = uuid.v4();
  var img_path = __dirname + '/public/uploads/' + name;

  fs.rename(req.files.sketch.path, img_path, function (err) {
    query_and_redirect(img_path, name, function () {
      res.redirect('/query/' + name);
    });
  });
});

app.listen(3000);
