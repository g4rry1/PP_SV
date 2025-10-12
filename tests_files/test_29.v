module parameter_attributes #(
    (* doc = "Data width" *) parameter WIDTH = 8
) (
    (* keep *) input logic [WIDTH-1:0] data
);
endmodule