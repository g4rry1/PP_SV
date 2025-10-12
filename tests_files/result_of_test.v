 module very_long_module_name_that_describes_its_function # ( parameter integer WIDTH = 32 , parameter integer DEPTH = 1024 , parameter string TYPE = "MEMORY_BLOCK"
) (
    input wire  clock_signal ,
    input wire  reset_signal_active_low ,
    input wire [ WIDTH - 1 : 0 ]  input_data_bus ,
    output reg [ WIDTH - 1 : 0 ] output_data_bus , inout wire [ 15 : 0 ]  bidirectional_bus ,
    input my_package :: complex_type configuration_input ,
    output my_package :: status_type status_output
);
endmodule