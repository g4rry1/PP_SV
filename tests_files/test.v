// Модуль, реализующий простой сумматор
module simple_adder (
  input  logic a, // Входной бит 1
  input  logic b, // Входной бит 2
  output logic sum, // Результат сложения
  output logic carry // Перенос
);

  // Логика суммирования
  assign {carry, sum} = a + b; // Арифметическое сложение с переносом

endmodule