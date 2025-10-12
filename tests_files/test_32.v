module parent_named;
    child_module inst1(
        .clk(clock_signal),
        .rst(reset_signal),
        .data_in(input_data),
        .data_out(output_data)
    );
endmodule