(* syn_hier = "hard" *)
module complete_example #(
    parameter type DATA_TYPE = logic [31:0],
    parameter int FIFO_DEPTH = 256,
    parameter real CLOCK_FREQ = 100.0e6,
    parameter string MODULE_NAME = "fifo_controller"
) (
    // Тактовые и сбросовые сигналы
    input logic clk,
    input logic rst_n,
    
    // Интерфейс данных
    input DATA_TYPE data_in,
    output DATA_TYPE data_out,
    
    // Управляющие сигналы
    input logic wr_en,
    input logic rd_en,
    output logic full,
    output logic empty,
    
    // Статусные сигналы
    output logic [7:0] status,
    
    // Виртуальный интерфейс для тестирования
    virtual debug_if debug_intf
);

    localparam ADDR_WIDTH = $clog2(FIFO_DEPTH);
    
    // Внутренние сигналы
    logic [ADDR_WIDTH-1:0] wr_ptr;
    logic [ADDR_WIDTH-1:0] rd_ptr;
    
    // содержимое модуля
    
endmodule