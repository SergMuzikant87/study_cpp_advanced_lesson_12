#pragma once
#include <string>
#include <cstdint>

class Integer final
{
private:
    uint8_t * __bytes;
    uint8_t __size;

public:
    Integer(const std::string & text);
    ~Integer(void);
    Integer(const Integer & original);
    Integer(Integer && original);

    Integer & operator= (const Integer & original);
    Integer & operator= (Integer && original);

    Integer operator+ (const Integer & integer_2);
    Integer operator* (const Integer & integer_2);

    std::string to_string_format_hex(void);
    std::string to_string_format_bin(void);

    uint8_t size(void) const;
    uint8_t get_byte(uint8_t index) const;
    void set_byte(uint8_t index, uint8_t value);
    void * get(void);

public:
    Integer(uint8_t size = 1);

private:
    static char hex_tetrada_to_char(uint8_t tetrada);
    static uint8_t char_to_hex_tetrada(char symbol);

    void high_byte_push(uint8_t value);
    uint8_t high_byte_pop(void);
    bool is_consenst(void) const;
public:
    void delete_empty_bytes(void);
};
