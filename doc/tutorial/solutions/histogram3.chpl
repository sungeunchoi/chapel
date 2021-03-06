//
// Histogram Solution 3
//
// Compute the histogram of an array of random numbers.
//

// import standard modules to generate random number and use timers
use Random, Time;

// configuration constants
config const printRandomNumbers: bool = true, // print random numbers to screen
             printHistogram: bool = true,     // print histogram to screen
             numNumbers: int = 8,             // number of random numbers
             numBuckets: int = 10,            // number of histogram buckets
             numThreads: int = 4;             // number of threads to use

// seed the random stream with something reproducable?
config const useRandomSeed = true,
             seed  = if useRandomSeed then SeedGenerator.clockMS else 314159265,
             seed2 = if useRandomSeed then SeedGenerator.clockMS else 161803398;

// global variables
var X, X2: [1..numNumbers] real, // arrays of random numbers
    Y: [1..numBuckets] int,      // histogram
    timer: Timer;                // computation timer

// output startup message
writeln("Running Histogram Example");
writeln(" Number of Random Numbers = ", format("########", numNumbers));
writeln(" Number of Buckets        = ", format("########", numBuckets));
writeln();

// fill arrays with random numbers (using standard Random module)
fillRandom(X, seed);
fillRandom(X2, seed2);
X = (X+X2)/2;

// output arrays of random numbers as averages
if printRandomNumbers then
  writeln("Random Averages\n\n", X, "\n");

// compute histogram
timer.start();
computeHistogram(X, Y);
timer.stop();

// verify number of items in histogram is equal to number of random
// numbers and output timing results
if + reduce Y != numNumbers then
  halt("Number of items in histogram does not match number of random numbers");
writeln("Histogram computed in ", timer.elapsed(), " seconds\n");

// output histogram
if printHistogram then
  outputHistogram(Y);

def computeHistogram(X: [] real, Y: [] int) {
  var lock$: sync bool;
  coforall t in 1..numThreads {
    var low = 1+(t-1)*numNumbers/numThreads;
    var high = if t == numThreads then numNumbers else t*numNumbers/numThreads;
    var myY: [1..numBuckets] int;
    for x in X(low..high) do
      myY(1 + (x / (1.0 / numBuckets)): int) += 1;
    lock$ = true;
    Y += myY;
    lock$;
  }
}

// outputHistogram: output histogram array
def outputHistogram(Y: [] int) {
  var bucketMax = max reduce Y;
  var rowSize = ceil(bucketMax:real/10):int;
  var numRows = ceil(bucketMax:real/rowSize):int;
  for i in 1..numRows by -1 {
    write(" ");
    for j in 1..numBuckets do
      write(if Y(j)/rowSize >= i then "X" else " ");
    writeln();
  }
  write("+");
  for j in 1..numBuckets do
    write("-");
  writeln("+");
  if rowSize > 1 then
    writeln(" Each 'X' can represent up to ", rowSize, " numbers.");
  writeln(" Raw Data");
  for j in 1..numBuckets {
    writeln("  ",
            format("%0.2f", (j-1)*(1.0/numBuckets)),
            " - ",
            format("%0.2f", j*(1.0/numBuckets)),
            ": ",
            Y(j));
  }
}
