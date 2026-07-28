#include <iostream>
#include "integer.h"

static void integer_print(const char * label, Integer & integer)
{
    std::cout << label << integer.to_string_format_hex() << " = " << integer.to_string_format_bin()
    << ",  & = " << &integer << ",  get() = " << integer.get() << ",  size() = " << (int)integer.size() << std::endl;
}

int main(int argc, char * argv[])
{
    try
    {
        std::cout << "-----------------------------" << std::endl;
        Integer integer_1 = Integer("0x80"); // -128
        Integer integer_2(3);
        integer_2.set_byte(0, 0x8A); // 138
        integer_2.set_byte(1, 0x00); // +
        integer_2.set_byte(2, 0x00); // +
        integer_2.delete_empty_bytes(); // byte 2;
        Integer integer_3 = integer_1 + integer_2; // 10 (0x0A)
        Integer integer_4 = integer_1 * integer_2; // -17664 (0xBB00)
        Integer integer_5 = integer_4 * integer_1; // 2260992 (0x228000)

        integer_print("integer_1 = ", integer_1);
        integer_print("integer_2 = ", integer_2);
        integer_print("integer_3 = integer_1 + integer_2 = ", integer_3);
        integer_print("integer_4 = integer_1 * integer_2 = ", integer_4);
        integer_print("integer_5 = integer_4 * integer_1 = ", integer_5);

        std::cout << "-----------------------------" << std::endl;
        std::cout << "copy integer_5 --> integer_3" << std::endl;
        integer_3 = integer_5;
        integer_print("integer_3 = ", integer_3);
        integer_print("integer_5 = ", integer_5);

        uint8_t iters_count = 8;
        while(iters_count--)
        {
            integer_3 = integer_3 * Integer("0x00FF");
            integer_print("integer_3 = integer_3 * 0x00FF = ", integer_3);
        }

        std::cout << "-----------------------------" << std::endl;
        integer_print("integer_1 = ", integer_1);
        integer_print("integer_5 = ", integer_5);
        std::cout << "move integer_5 --> integer_1" << std::endl;
        integer_1 = static_cast<Integer&&>(integer_5);
        integer_print("integer_1 = ", integer_1);
        integer_print("integer_5 = ", integer_5);


    }
    catch(const char * info)
    {
        std::cout << "ERROR: " << info << std::endl;
    }

    return 0;
}
