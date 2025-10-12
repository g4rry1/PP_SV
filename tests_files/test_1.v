// Модуль, реализующий простой сумматор
module      simple_adder ( // комментарий
input  logic   a, // комментарий
     input logic                b, 
output    logic sum,    output logic carry
  )    ; 

  // Логика суммирования
  assign {carry, sum} = a + b; // Арифметическое сложение с переносом

endmodule