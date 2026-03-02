#include <stdio.h>
#include <stdlib.h>

#define INT_BITS (sizeof(int) * 8)
#define SIGN_BIT (1U << (INT_BITS - 1));

typedef struct RealNumber {
    int firstDigit;
    unsigned int *digits;
} RealNumber;

//Initialization of BigInt
RealNumber *initNumber() {
    RealNumber *number = (RealNumber*)malloc(sizeof(RealNumber));
    if (number == NULL) {
        return NULL;
    }
    number->firstDigit = 0;
    number->digits = NULL;
    return number;
}

// Deinitialization of BigInt
void deinitNumber(RealNumber* number) {
    if (number == NULL) {
        return;
    }
    free(number->digits);
    free(number);
}

//Assign sign to BigInt
void setNumberSign(RealNumber *number, unsigned int sign) {
    if (number == NULL) {
        return;
    }
    if (sign == 0) {
        number->firstDigit |= SIGN_BIT;
    }
    else if (sign == 1) {
        number->firstDigit &= ~SIGN_BIT;
    }
}

//Assign value to RealNumber struct (user input from console)
RealNumber *inputNumber(RealNumber *number) {
    if (number == NULL) {
        return NULL;
    }
    unsigned int len;
    printf("Enter number of digits in your BigInt: ");
    scanf("%u", &len);

    if (len <= 0) {
        return NULL;
    }
    printf("Enter first digit (without sign): ");
    scanf("%u", &number->firstDigit);
    if (len > 1) {
        number->digits = (unsigned int*)malloc(len * sizeof(unsigned int));
        if (number->digits == NULL) {
            return NULL;
        }
        printf("Enter digits of your BigInt (from first to last,"
               "without the first one: ");
        for (int i = 0; i < len; i++) {
            scanf("%u", &number->digits[i]);
        }
    }
    int sign;
    printf("Enter sign ('0' for '+', '1' for '-'): ");
    scanf("%d", &sign);
    setNumberSign(number, sign);
    return 0;
}

//Print bits of BigInt
void printNumberBits(unsigned int number) {
    int step = sizeof(number) * CHAR_BIT - 1;
    for (int n = step; n >= 0; n--) {
        printf("%u", (number >> n) & 1);
        if (n != 0 && n % 8 == 0) {
            printf(" ");
        }
    }
}

//Make copy of BigInt
RealNumber *copyNumber(RealNumber *number) {
    if (number == NULL) {
        return NULL;
    }
    RealNumber *copy = initNumber();
    if (copy == NULL) {
        return NULL;
    }
    copy->firstDigit = number->firstDigit;
    if (number->digits) {
        unsigned int len = number->digits[0] + 1;
        copy->digits = (unsigned int*)malloc(len * sizeof(unsigned int));
        if (copy->digits == NULL) {
            free(copy->digits);
            free(copy);
            return NULL;
        }
        for (int i = 1; i < len; i++) {
            copy->digits[i] = number->digits[i];
        }
    } else {
        copy->digits = NULL;
    }
    return copy;
}

//Sum of two BigInts with result in first number
int sumNumberWithoutCopy(RealNumber *number1, RealNumber *number2) {
    if (number1 == NULL || number2 == NULL) {
        return 1;
    }
    if (number1->digits == NULL || number2->digits == NULL) {
        return 1;
    }
}


unsigned int LoWord(unsigned int value) {
    return value & (1 << (sizeof(unsigned int) << 2)) - 1;
}

unsigned int HiWord(unsigned int value) {
    return value >> (sizeof(unsigned int) << 2);
}