package my_pkg;
    typedef logic [7:0] byte_t;
endpackage

module package_types(
    input my_pkg::byte_t data_in,
    output my_pkg::byte_t data_out
);
endmodule