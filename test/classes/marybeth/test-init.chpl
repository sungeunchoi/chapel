
class TestRandom {

  const timeofday = 4531;
  const seed = if (timeofday%2) then (timeofday) else (timeofday + 1);

  def showseed() {
    writeln(seed);
  }
}

var randlist = TestRandom();

randlist.showseed();
