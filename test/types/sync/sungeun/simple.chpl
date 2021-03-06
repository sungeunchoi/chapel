config type myType = int;
config const initval: myType = 71:myType;

var sx: sync myType;
var x: myType;
var y = 2*initval;

sync {
  begin ref(x) {
    x = sx;
    if sx.isFull then writeln("invalid state (full)");
    if x != initval then writeln("incorrect value: ", x);
  }

  sx = initval;
}
if sx.isFull then writeln("invalid state (full)");

begin {
  sx = y;
  if sx.readXX() != 2*initval then writeln("incorrect value: ", sx.readXX());
}

x = sx;
if sx.isFull then writeln("invalid state (full)");
if x != 2*initval then writeln("incorrect value: ", x);

writeln("DONE");
