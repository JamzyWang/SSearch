//
// Autogenerated by Thrift Compiler (0.9.0)
//
// DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
//
var Thrift = require('thrift').Thrift;
var ttypes = module.exports = {};
Pair = module.exports.Pair = function(args) {
  this.filename = null;
  this.result = null;
  if (args) {
    if (args.filename !== undefined) {
      this.filename = args.filename;
    }
    if (args.result !== undefined) {
      this.result = args.result;
    }
  }
};
Pair.prototype = {};
Pair.prototype.read = function(input) {
  input.readStructBegin();
  while (true)
  {
    var ret = input.readFieldBegin();
    var fname = ret.fname;
    var ftype = ret.ftype;
    var fid = ret.fid;
    if (ftype == Thrift.Type.STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
      if (ftype == Thrift.Type.STRING) {
        this.filename = input.readString();
      } else {
        input.skip(ftype);
      }
      break;
      case 2:
      if (ftype == Thrift.Type.DOUBLE) {
        this.result = input.readDouble();
      } else {
        input.skip(ftype);
      }
      break;
      default:
        input.skip(ftype);
    }
    input.readFieldEnd();
  }
  input.readStructEnd();
  return;
};

Pair.prototype.write = function(output) {
  output.writeStructBegin('Pair');
  if (this.filename !== null && this.filename !== undefined) {
    output.writeFieldBegin('filename', Thrift.Type.STRING, 1);
    output.writeString(this.filename);
    output.writeFieldEnd();
  }
  if (this.result !== null && this.result !== undefined) {
    output.writeFieldBegin('result', Thrift.Type.DOUBLE, 2);
    output.writeDouble(this.result);
    output.writeFieldEnd();
  }
  output.writeFieldStop();
  output.writeStructEnd();
  return;
};
