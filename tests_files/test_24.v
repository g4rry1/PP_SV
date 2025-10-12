module type_parameters #(
    parameter type T = logic [7:0],
    parameter int SIZE = 1024
) (
    input T data_in,
    output T data_out
);
endmodule