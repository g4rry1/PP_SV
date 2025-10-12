typedef enum {READ, WRITE} cmd_t;
typedef struct {
    logic [31:0] addr;
    logic [63:0] data;
} transaction_t;

module custom_types(
    input cmd_t command,
    output transaction_t result
);
endmodule