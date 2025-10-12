module localparam_module #(
    parameter WIDTH = 8
) (
    input logic [WIDTH-1:0] a
);
    localparam ADDR_WIDTH = $clog2(WIDTH);
    logic [ADDR_WIDTH-1:0] addr;
endmodule