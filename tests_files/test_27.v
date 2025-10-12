module generate_module #(
    parameter IMPLEMENTATION = 0
) (
    input logic clk,
    output logic out
);
    generate
        if (IMPLEMENTATION == 0) begin : impl0
            assign out = 1'b0;
        end else begin : impl1
            assign out = 1'b1;
        end
    endgenerate
endmodule