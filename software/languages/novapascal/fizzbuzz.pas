program FizzBuzz;

var
  I: Byte;

begin
  I := 1;
  while I <= 100 do
  begin
    if I mod 15 = 0 then
      writeln('FizzBuzz')
    else if I mod 3 = 0 then
      writeln('Fizz')
    else if I mod 5 = 0 then
      writeln('Buzz')
    else
      writeln(I);
    I := I + 1
  end
end.
