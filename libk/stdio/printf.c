#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <kernel/arch.h>

int isdigit(char c)
{
    if ((c>='0') && (c<='9')) return 1;
    return 0;
}

static inline uint32_t divmod_u64(uint64_t *num, uint32_t base) {
    uint64_t n = *num;
    uint64_t result = 0;
    uint64_t b = 1;

    // Shift b until it is just less than or equal to n
    while (base <= (n >> 1)) {
        base <<= 1;
        b <<= 1;
    }

    while (b != 0) {
        if (n >= base) {
            n -= base;
            result += b;
        }
        base >>= 1;
        b >>= 1;
    }

    *num = result;
    return (uint32_t)n; // remainder
}

char* __int_str(intmax_t value, char b[], int base, bool plusSignIfNeeded,
                bool spaceSignIfNeeded, int paddingNo, bool justify, bool zeroPad) {

    char digits[17] = "0123456789ABCDEF";
    char buf[65]; // enough for binary 64-bit plus null
    int i = 0;
    bool negative = false;
    uint64_t n;

    if (value < 0) {
        negative = true;
        n = (uint64_t)(-value);
    } else {
        n = (uint64_t)value;
    }

    // Convert number to string in reverse order
    do {
        uint32_t rem = divmod_u64(&n, base);
        buf[i++] = digits[rem];
    } while (n);

    if (negative) {
        buf[i++] = '-';
    } else if (plusSignIfNeeded) {
        buf[i++] = '+';
    } else if (spaceSignIfNeeded) {
        buf[i++] = ' ';
    }

    // Reverse the string into output buffer
    int j;
    for (j = 0; j < i; ++j) {
        b[j] = buf[i - j - 1];
    }
    b[i] = '\0';

    // Optional padding (simplified)
    int pad = paddingNo - i;
    if (pad > 0) {
        if (justify) {
            for (int k = 0; k < pad; ++k) {
                b[i + k] = zeroPad ? '0' : ' ';
            }
            b[i + pad] = '\0';
        } else {
            // left pad
            char tmp[256];
            for (int k = 0; k < pad; ++k) {
                tmp[k] = zeroPad ? '0' : ' ';
            }
            strcpy(tmp + pad, b);
            strcpy(b, tmp);
        }
    }

    return b;
}


void displayCharacter(char c, int* a) {
    putc(c);
    *a += 1;
}

void displayString(char* c, int* a) {
    for (int i = 0; c[i]; ++i) {
        displayCharacter(c[i], a);
    }
}

int vprintf (const char* format, va_list list)
{
    int chars        = 0;
    char intStrBuffer[256] = {0};
    
    for (int i = 0; format[i]; ++i) {
        
        char specifier   = '\0';
        char length      = '\0';
        
        int  lengthSpec  = 0; 
        int  precSpec    = 0;
        bool leftJustify = false;
        bool zeroPad     = false;
        bool spaceNoSign = false;
        bool altForm     = false;
        bool plusSign    = false;
        bool emode       = false;
        int  expo        = 0;
        
        if (format[i] == '%') {
            ++i;
            
            bool extBreak = false;
            while (1) {
            
                switch (format[i]) {
                    case '-':
                        leftJustify = true;
                        ++i;
                        break;
                        
                    case '+':
                        plusSign = true;
                        ++i;
                        break;
                        
                    case '#':
                        altForm = true;
                        ++i;
                        break;
                        
                    case ' ':
                        spaceNoSign = true;
                        ++i;
                        break;
                        
                    case '0':
                        zeroPad = true;
                        ++i;
                        break;
                        
                    default:
                        extBreak = true;
                        break;
                }
                
                if (extBreak) break;
            }
            
            while (isdigit(format[i])) {
                lengthSpec *= 10;
                lengthSpec += format[i] - 48;
                ++i;
            }
            
            if (format[i] == '*') {
                lengthSpec = va_arg(list, int);
                ++i;
            }
            
            if (format[i] == '.') {
                ++i;
                while (isdigit(format[i])) {
                    precSpec *= 10;
                    precSpec += format[i] - 48;
                    ++i;
                }
                
                if (format[i] == '*') {
                    precSpec = va_arg(list, int);
                    ++i;
                }
            } else {
                precSpec = 6;
            }
            
            if (format[i] == 'h' || format[i] == 'l' || format[i] == 'j' ||
                   format[i] == 'z' || format[i] == 't' || format[i] == 'L') {
                length = format[i];
                ++i;
                if (format[i] == 'h') {
                    length = 'H';
                } else if (format[i] == 'l') {
                    length = 'q';
                    ++i;
                }
            }
            specifier = format[i];
            
            memset(intStrBuffer, 0, 256);
            
            int base = 10;
            if (specifier == 'o') {
                base = 8;
                specifier = 'u';
                if (altForm) {
                    displayString("0", &chars);
                }
            }
            if (specifier == 'p') {
                base = 16;
                length = 'z';
                specifier = 'u';
            }
            switch (specifier) {
                case 'X':
                    base = 16;
                case 'x':
                    base = base == 10 ? 17 : base;
                    if (altForm) {
                        displayString("0x", &chars);
                    }
                    
                case 'u':
                {
                    switch (length) {
                        case 0:
                        {
                            uint32_t integer = va_arg(list, uint32_t);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 'H':
                        {
                            unsigned char integer = (unsigned char) va_arg(list, uint32_t);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 'h':
                        {
                            uint16_t integer = va_arg(list, uint32_t);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 'l':
                        {
                            uint64_t integer = va_arg(list, uint64_t);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 'q':
                        {
                            uint64_t integer = va_arg(list, uint64_t);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 'j':
                        {
                            uintmax_t integer = va_arg(list, uintmax_t);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 'z':
                        {
                            size_t integer = va_arg(list, size_t);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 't':
                        {
                            uint32_t integer = va_arg(list, uint32_t);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }
                    
                case 'd':
                case 'i':
                {
                    switch (length) {
                    case 0:
                    {
                        int integer = va_arg(list, int);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 'H':
                    {
                        signed char integer = (signed char) va_arg(list, int);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 'h':
                    {
                        short int integer = va_arg(list, int);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 'l':
                    {
                        long integer = va_arg(list, long);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 'q':
                    {
                        long long integer = va_arg(list, long long);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 'j':
                    {
                        intmax_t integer = va_arg(list, intmax_t);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 'z':
                    {
                        size_t integer = va_arg(list, size_t);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 't':
                    {
                        uint32_t integer = va_arg(list, uint32_t);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    default:
                        break;
                    }
                    break;
                }
                    
                case 'c':
                {
                        displayCharacter(va_arg(list, int), &chars);
                    
                    break;
                }
                    
                case 's':
                {
                    displayString(va_arg(list, char*), &chars);
                    break;
                }
                    
                case 'n':
                {
                    switch (length) {
                        case 'H':
                            *(va_arg(list, signed char*)) = chars;
                            break;
                        case 'h':
                            *(va_arg(list, short int*)) = chars;
                            break;
                            
                        case 0: {
                            int* a = va_arg(list, int*);
                            *a = chars;
                            break;
                        }
                            
                        case 'l':
                            *(va_arg(list, long*)) = chars;
                            break;
                        case 'q':
                            *(va_arg(list, long long*)) = chars;
                            break;
                        case 'j':
                            *(va_arg(list, intmax_t*)) = chars;
                            break;
                        case 'z':
                            *(va_arg(list, size_t*)) = chars;
                            break;
                        case 't':
                            *(va_arg(list, uint32_t*)) = chars;
                            break;
                        default:
                            break;
                    }
                    break;
                }
                   
                case 'e':
                case 'E':
                    emode = true;
                    
                case 'f':
                case 'F':
                case 'g':
                case 'G':
                {
                    double floating = va_arg(list, double);
                    
                    while (emode && floating >= 10) {
                        floating /= 10;
                        ++expo;
                    }
                    
                    int form = lengthSpec - precSpec - expo - (precSpec || altForm ? 1 : 0);
                    if (emode) {
                        form -= 4;      // 'e+00'
                    }
                    if (form < 0) {
                        form = 0;
                    }
                    
                    __int_str(floating, intStrBuffer, base, plusSign, spaceNoSign, form, \
                              leftJustify, zeroPad);
                    
                    displayString(intStrBuffer, &chars);
                    
                    floating -= (int) floating;
                    
                    for (int i = 0; i < precSpec; ++i) {
                        floating *= 10;
                    }
                    intmax_t decPlaces = (intmax_t) (floating + 0.5);
                    
                    if (precSpec) {
                        displayCharacter('.', &chars);
                        __int_str(decPlaces, intStrBuffer, 10, false, false, 0, false, false);
                        intStrBuffer[precSpec] = 0;
                        displayString(intStrBuffer, &chars);
                    } else if (altForm) {
                        displayCharacter('.', &chars);
                    }
                    
                    break;
                }
                    
                    
                case 'a':
                case 'A':
                    //ACK! Hexadecimal floating points...
                    break;
                
                default:
                    break;
            }
            
            if (specifier == 'e') {
                displayString("e+", &chars);
            } else if (specifier == 'E') {
                displayString("E+", &chars);
            }
            
            if (specifier == 'e' || specifier == 'E') {
                __int_str(expo, intStrBuffer, 10, false, false, 2, false, true);
                displayString(intStrBuffer, &chars);
            }
            
        } else {
            displayCharacter(format[i], &chars);
        }
    }
    
    return chars;
}

__attribute__ ((format (printf, 1, 2))) int printf (const char* format, ...)
{
    va_list list;
    va_start (list, format);
    int i = vprintf (format, list);
    va_end (list);
    return i;
    
}