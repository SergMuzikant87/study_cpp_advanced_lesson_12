#include "integer.h"
#include <iostream>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

Integer::Integer(uint8_t size) : __bytes(nullptr), __size(size)
{
    if((__size == 0) || (__size >= 128))
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    __bytes = new uint8_t[__size];
    for(uint8_t index = 0; index < __size; index++)
    {
        __bytes[index] = 0;
    }
}

Integer::Integer(const std::string & text)
{

    if(text.length() < 3)
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    if((text[0] != '0') || ((text[1] != 'x') && (text[1] != 'b')))
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    try
    {
        switch(text[1])
        {
        case 'x':
        {
            __size = ((((uint8_t)text.length()) - 2) + 1) >> 1;
            if((__size == 0) || (__size >= 128))
            {
                throw (const char *)__PRETTY_FUNCTION__;
            }
            __bytes = new uint8_t[__size];

            for(uint8_t index = 0, symbol_index = text.length() - 1; index < __size; index++)
            {
                __bytes[index] = char_to_hex_tetrada(text[symbol_index]);
                symbol_index--;
                __bytes[index] |= ((symbol_index >= 2) ? (char_to_hex_tetrada(text[symbol_index]) << 4) : ( (((uint8_t)0) - ((__bytes[index] & 0x08) >> 3)) & 0xF0 ));
                symbol_index--;
            }

            break;
        }
        case 'b':
        default :
        {
            __size = (uint8_t)(((((uint16_t)text.length()) - 2) + 7) >> 3);
            if((__size == 0) || (__size >= 128))
            {
                throw (const char *)__PRETTY_FUNCTION__;
            }
            __bytes = new uint8_t[__size];
            __bytes[__size - 1] = 0x00;

            for(uint16_t symbol_index = text.length() - 1, bit_num = 0; symbol_index >= 2; symbol_index--, bit_num++)
            {
                uint8_t byte_num = (uint8_t)((bit_num & (1 << 3)) >> 3);
                uint8_t bit_status =  (uint8_t)(((char)text[symbol_index]) - '0');
                if(bit_status & (~1))
                {
                    throw (const char *)__PRETTY_FUNCTION__;
                }

                __bytes[byte_num] &= ~(1 << (bit_num & 7));
                __bytes[byte_num] |= (bit_status << (bit_num & 7));
            }
        }
        }

        delete_empty_bytes();
    }
    catch(const char * error_text)
    {
        delete [] __bytes;
        __bytes = nullptr;
        __size = 0;
        throw error_text;
    }
}

Integer::~Integer(void)
{
    delete [] __bytes;
}

Integer::Integer(const Integer & original) : Integer()
{
    if(this == &original)
    {
        return;
    }

    if(original.__size == 0)
    {
        return;
    }

    __size = original.__size;
    __bytes = new uint8_t[__size];
    for(uint8_t index = 0; index < __size; index++)
    {
        __bytes[index] = original.__bytes[index];
    }
}

Integer::Integer(Integer && original) : Integer()
{
    if(this == &original)
    {
        return;
    }

    __size = original.__size;
    __bytes = original.__bytes;
    original.__size = 0;
    original.__bytes = nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Integer & Integer::operator= (const Integer & original)
{
    if(this == &original)
    {
        return *this;
    }

    uint8_t * bytes_copy = nullptr;

    if(original.__size != 0)
    {
        bytes_copy = new uint8_t[original.__size];
        for(uint8_t index = 0; index < original.__size; index++)
        {
            bytes_copy[index] = original.__bytes[index];
        }
    }

    delete __bytes;
    __bytes = bytes_copy;
    __size = original.__size;

    return *this;
}

Integer & Integer::operator= (Integer && original)
{
    if(this == &original)
    {
        return *this;
    }

    __size = original.__size;
    __bytes = original.__bytes;
    original.__size = 0;
    original.__bytes = nullptr;

    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Integer Integer::operator+ (const Integer & integer_2)
{

    if(!is_consenst())
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    if(!integer_2.is_consenst())
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    Integer result((__size >= integer_2.__size) ? __size : integer_2.__size);

    const uint8_t SIGN_BIT_NUM = 7;
    uint8_t carry = 0;
    uint8_t operand_1 = 0x00;
    uint8_t operand_2 = 0x00;
    for(uint8_t index = 0; index < result.__size; index++)
    {
        operand_1 = (index < __size) ? __bytes[index] : ((uint8_t)0) - ((uint8_t)((__bytes[__size - 1] & (1 << SIGN_BIT_NUM)) >> SIGN_BIT_NUM));
        operand_2 = (index < integer_2.__size) ? integer_2.__bytes[index] : ((uint8_t)0) - ((uint8_t)((integer_2.__bytes[integer_2.__size - 1] & (1 << SIGN_BIT_NUM)) >> SIGN_BIT_NUM));
        result.__bytes[index] = operand_1 + operand_2 + (carry != 0);
        carry = (result.__bytes[index] < ((operand_1 >= operand_2) ? operand_1 : operand_2));
    }

    if
        (
            ((operand_1 & (1 << SIGN_BIT_NUM)) && (operand_2 & (1 << SIGN_BIT_NUM)) && (!(result.__bytes[result.__size - 1] & (1 << SIGN_BIT_NUM))))
            ||
            ((!(operand_1 & (1 << SIGN_BIT_NUM))) && (!(operand_2 & (1 << SIGN_BIT_NUM))) && (result.__bytes[result.__size - 1] & (1 << SIGN_BIT_NUM)))
            )
    {
        uint8_t high_byte = ((uint8_t)(result.__bytes[result.__size - 1] >> SIGN_BIT_NUM)) - ((uint8_t)1);
        result.high_byte_push(high_byte);
    }

    result.delete_empty_bytes();

    return result;
}

Integer Integer::operator* (const Integer & integer_2)
{
    if(!is_consenst())
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    if(!integer_2.is_consenst())
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    Integer result(__size + integer_2.__size);
    const uint8_t SIGN_BIT_NUM = 7;

    for(uint8_t index_1 = 0; index_1 < result.__size; index_1++)
    {
        uint16_t addiv_16 = 0;
        for(uint8_t index_2 = 0; index_2 < result.__size; index_2++)
        {
            if((index_1 + index_2) < result.__size)
            {
                uint16_t operand_1 = (index_1 < __size) ? (uint16_t)__bytes[index_1] : (uint16_t) ((((uint8_t)0) - ((uint8_t)((__bytes[__size - 1] & (1 << SIGN_BIT_NUM)) >> SIGN_BIT_NUM))) & 0xFF);
                uint16_t operand_2 = (index_2 < integer_2.__size) ? (uint16_t)integer_2.__bytes[index_2] : (uint16_t) ((((uint8_t)0) - ((uint8_t)((integer_2.__bytes[integer_2.__size - 1] & (1 << SIGN_BIT_NUM)) >> SIGN_BIT_NUM))) & 0xFF);
                addiv_16 = ((uint16_t)result.__bytes[index_1 + index_2]) + (operand_1 * operand_2) + (addiv_16 >> 8);
                result.__bytes[index_1 + index_2] = ((uint8_t)(addiv_16 & 0x00FF));
            }
        }
    }

    result.delete_empty_bytes();
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t Integer::size(void) const
{
    if(!is_consenst())
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    return __size;
}

uint8_t Integer::get_byte(uint8_t index) const
{
    if(!is_consenst())
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    if(index >= __size)
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    return __bytes[index];
}

void Integer::set_byte(uint8_t index, uint8_t value)
{
    if(!is_consenst())
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    if(index >= __size)
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    __bytes[index] = value;
}

void * Integer::get(void)
{
    return (void *)__bytes;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string Integer::to_string_format_hex(void)
{
    if(!is_consenst())
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    std::string text = "0x";
    uint8_t cur_byte_index = __size;
    while(cur_byte_index--)
    {
        text += hex_tetrada_to_char(__bytes[cur_byte_index] >> 4);
        text += hex_tetrada_to_char(__bytes[cur_byte_index] & 0x0F);
    }
    return text;
}

std::string Integer::to_string_format_bin(void)
{
    if(!is_consenst())
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    std::string text = "0b";
    uint8_t cur_byte_index = __size;
    while(cur_byte_index--)
    {
        uint8_t bit_num = 8;
        while(bit_num--)
        {
            text += '0' + (char)(((uint8_t)(__bytes[cur_byte_index] & (1 << bit_num))) >> bit_num);
        }
    }
    return text;
}

char Integer::hex_tetrada_to_char(uint8_t hex_tetrada)
{
    if(hex_tetrada > 0x0F)
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    if(hex_tetrada >= 0x0A)
    {
        return 'A' + ((char)(hex_tetrada - 0x0A));
    }

    return '0' + ((char)hex_tetrada);
}

uint8_t Integer::char_to_hex_tetrada(char symbol)
{
    if((symbol >= '0') && (symbol <= '9'))
    {
        return (uint8_t)(symbol - '0');
    }

    if((symbol >= 'A') && (symbol <= 'F'))
    {
        return 0x0A + ((uint8_t)(symbol - 'A'));
    }

    if((symbol >= 'a') && (symbol <= 'f'))
    {
        return 0x0A + ((uint8_t)(symbol - 'a'));
    }

    throw (const char *)__PRETTY_FUNCTION__;
}

void Integer::high_byte_push(uint8_t value)
{
    if(!is_consenst())
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    uint8_t * bytes_copy = new uint8_t[__size + 1];
    for(uint8_t index = 0; index < __size; index++)
    {
        bytes_copy[index] = __bytes[index];
    }
    bytes_copy[__size++] = value;
    delete [] __bytes;
    __bytes = bytes_copy;
}

uint8_t Integer::high_byte_pop(void)
{
    if(!is_consenst())
    {
        throw (const char *)__PRETTY_FUNCTION__;
    }

    uint8_t elem = __bytes[--__size];
    uint8_t * bytes_copy = (__size) ? new uint8_t[__size] : nullptr;
    for(uint8_t index = 0; index < __size; index++)
    {
        bytes_copy[index] = __bytes[index];
    }
    delete [] __bytes;
    __bytes = bytes_copy;
    return elem;
}

bool Integer::is_consenst(void) const
{
    return (__size) && (__size < 128) && (__bytes);
}

void Integer::delete_empty_bytes(void)
{
    if((__bytes[__size - 1] != 0x00) && (__bytes[__size - 1] != 0xFF))
    {
        return;
    }

    const uint8_t SIGN_MASK = (1 << 7);
    uint8_t compare = __bytes[__size - 1];
    while(__size >= 2)
    {
        if(__bytes[__size - 1] != compare)
        {
            break;
        }

        if((__bytes[__size - 1] & SIGN_MASK) != (__bytes[__size - 2] & SIGN_MASK))
        {
            break;
        }

        high_byte_pop();
    }
}
