#include "../include/annotator.h"
#include "../include/verilog_token.h"
#include <vector>

class TokenUnwrapper {
public:
    static void unwrapper_token(vector<my_token>& tokens);

    static void need_a_transfer(vector<my_token>& tokens);
    static void tabulating(vector<my_token>& tokens);

private:
    static int tab;
};
