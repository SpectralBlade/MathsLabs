#include <stdio.h>
#include <stdlib.h>

#define INT_BITS (sizeof(int) * 8)
#define SIGN_BIT (1U << (INT_BITS - 1))
#define BASE_MASK 0xFFFFFFFFxFFFFFFFFULL

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

//Receive sign of BigInt
int getNumberSign(RealNumber *number) {
    if (number == NULL) {
        return 2;
    }
    return (number->firstDigit & SIGN_BIT) ? 1 : 0;
}

//Compare abs'es of two BigInts
int compareAbsolute(RealNumber *a, RealNumber *b) {
    if (a == NULL || b == NULL) {
        return 0;
    }

    unsigned int lenA = a->digits[0];
    unsigned int lenB = b->digits[0];

    if (lenA > lenB) return 1;
    if (lenA < lenB) return -1;

    for (int i = (int)lenA; i >= 1; i--) {
        if (a->digits[i] > b->digits[i]) return 1;
        if (a->digits[i] < b->digits[i]) return -1;
    }

    return 0;
}

//Assign value to RealNumber struct (user input from console)
RealNumber *inputNumber(RealNumber *number) {
    if (number == NULL) {
        return NULL;
    }

    unsigned int totalLen;
    printf("Enter total number of digits (including the first/most significant one): ");
    if (scanf("%u", &totalLen) != 1 || totalLen == 0) {
        return NULL;
    }

    printf("Enter the most significant digit (as a positive integer): ");
    unsigned int mostSignificant;
    scanf("%u", &mostSignificant);
    number->firstDigit = (int)mostSignificant;

    if (totalLen > 1) {
        number->digits = (unsigned int*)malloc(totalLen * sizeof(unsigned int));
        if (number->digits == NULL) {
            return NULL;
        }

        number->digits[0] = totalLen - 1;

        printf("Enter the remaining %u digits in Little Endian order (from least to most significant):\n", totalLen - 1);
        for (unsigned int i = 1; i < totalLen; i++) {
            scanf("%u", &number->digits[i]);
        }
    } else {
        number->digits = NULL;
    }

    int sign;
    printf("Enter sign ('0' for '+', '1' for '-'): ");
    scanf("%d", &sign);
    setNumberSign(number, sign);

    return number;
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

//Trim leading zeroes in BigInt
int normalizeNumber(RealNumber *number) {
    if (number == NULL) {
        return 1;
    }
    if (number->digits == NULL) {
        return 1;
    }
    unsigned int len = number->digits[0];
    if (len <= 1) {
        return 0;
    }
    int len2 = len;
    while (len2 > 1 && number->digits[len2] == 0) {
        len2--;
    }
    number->digits[0] = (unsigned int)len2;
    if (len2 == 1 && number->digits[1] == 0) {
        number->firstDigit = 0;
    }
    return 0;
}

//Sum of two abs'es
void addAbsolute(RealNumber *a, RealNumber *b) {
    unsigned int lenA = a->digits[0];
    unsigned int lenB = b->digits[0];
    unsigned int maxLen = (lenA > lenB) ? lenA : lenB;

    unsigned int *d = (unsigned int *)realloc(a->digits, (maxLen + 2) * sizeof(unsigned int));
    if (d == NULL) {
        return;
    }
    a->digits = d;

    unsigned long long carry = 0;
    for (unsigned int i = 1; i <= maxLen || carry; i++) {
        unsigned long long valB = (i <= lenB) ? b->digits[i] : 0;
        unsigned long long valA = (i <= lenA) ? a->digits[i] : 0;

        unsigned long long sum = valA + valB + carry;
        a->digits[i] = (unsigned int)(sum & 0xFFFFFFFF);
        carry = sum >> 32;

        if (i > lenA) a->digits[0] = i;
    }
}

//Substraction of two abs'es
void subAbsolute(RealNumber *a, RealNumber *b) {
    unsigned int lenA = a->digits[0];
    unsigned int lenB = b->digits[0];
    long long borrow = 0;

    for (unsigned int i = 1; i <= lenA; i++) {
        long long valB = (i <= lenB) ? b->digits[i] : 0;
        long long diff = (long long)a->digits[i] - valB - borrow;

        if (diff < 0) {
            diff += 4294967296ULL;
            borrow = 1;
        } else {
            borrow = 0;
        }
        a->digits[i] = (unsigned int)diff;
    }
    normalizeNumber(a);
}

//Sum of two BigInts with result in first number
void sumNumberWithoutCopy(RealNumber *number1, RealNumber *number2) {
    if (!number1 || !number2) return;

    int sign1 = getNumberSign(number1);
    int sign2 = getNumberSign(number2);

    if (sign1 == sign2) {
        addAbsolute(number1, number2);
    } else {
        int cmp = compareAbsolute(number1, number2);

        if (cmp >= 0) {
            subAbsolute(number1, number2);
        } else {
            RealNumber *tempB = copyNumber(number2);
            subAbsolute(tempB, number1);

            free(number1->digits);
            number1->digits = tempB->digits;
            number1->firstDigit = tempB->firstDigit;
            setNumberSign(number1, sign2);

            tempB->digits = NULL;
            deinitNumber(tempB);
        }
    }
}

//Sum of two BigInts with result in the new BigInt
RealNumber *sumNumberWithCopy(RealNumber *number1, RealNumber *number2) {
    if (number1 == NULL || number2 == NULL) {
        return NULL;
    }

    RealNumber *result = copyNumber(number1);
    if (result == NULL) {
        return NULL;
    }

    sumNumberWithoutCopy(result, number2);
    return result;
}

//Substraction of two BigInts with result in the first number
void subNumberWithoutCopy(RealNumber *number1, RealNumber *number2) {
    if (number1 == NULL || number2 == NULL) {
        return;
    }

    int originalSign2 = getNumberSign(number2);
    setNumberSign(number2, originalSign2 == 0 ? 1 : 0);

    sumNumberWithoutCopy(number1, number2);
    setNumberSign(number2, originalSign2);
}

//Substraction of two BigInts with result in the new BigInt
RealNumber *subNumberWithCopy(RealNumber *number1, RealNumber *number2) {
    if (number1 == NULL || number2 == NULL) {
        return NULL;
    }

    RealNumber *result = copyNumber(number1);
    if (result == NULL) {
        return NULL;
    }

    subNumberWithoutCopy(result, number2);
    return result;
}

//Multiply of two BigInts with result in the first number
void multiplyNumberWithoutCopy(RealNumber *number1, RealNumber *number2) {
    if (number1 == NULL || number2 == NULL) {
        return;
    }

    int sign1 = getNumberSign(number1);
    int sign2 = getNumberSign(number2);
    int resultSign = (sign1 == sign2) ? 0 : 1;


    unsigned int len1 = (number1->digits ? number1->digits[0] : 0) + 1;
    unsigned int len2 = (number2->digits ? number2->digits[0] : 0) + 1;

    unsigned int *A = (unsigned int*)malloc(len1 * sizeof(unsigned int));
    unsigned int *B = (unsigned int*)malloc(len2 * sizeof(unsigned int));

    for(int i = 0; i < len1 - 1; i++) A[i] = number1->digits[i+1];
    A[len1-1] = number1->firstDigit & ~SIGN_BIT;

    for(int i = 0; i < len2 - 1; i++) B[i] = number2->digits[i+1];
    B[len2-1] = number2->firstDigit & ~SIGN_BIT;

    unsigned int resLen = len1 + len2;
    unsigned int *resDigits = (unsigned int*)calloc(resLen, sizeof(unsigned int));

    for (unsigned int i = 0; i < len1; i++) {
        unsigned long long carry = 0;
        for (unsigned int j = 0; j < len2; j++) {
            unsigned long long cur = resDigits[i + j] +
                                     A[i] * 1ULL * B[j] + carry;
            resDigits[i + j] = (unsigned int)(cur & 0xFFFFFFFF);
            carry = cur >> 32;
        }
        resDigits[i + len2] += (unsigned int)carry;
    }

    int actualLen = resLen;
    while (actualLen > 1 && resDigits[actualLen - 1] == 0) actualLen--;

    number1->firstDigit = (int)resDigits[actualLen - 1];
    setNumberSign(number1, resultSign);

    if (actualLen > 1) {
        unsigned int *newDigits = (unsigned int*)realloc(number1->digits, actualLen * sizeof(unsigned int));
        if (newDigits) {
            number1->digits = newDigits;
            number1->digits[0] = actualLen - 1;
            for (int i = 1; i < actualLen; i++) {
                number1->digits[i] = resDigits[i - 1];
            }
        }
    } else {
        free(number1->digits);
        number1->digits = NULL;
    }
    free(A); free(B); free(resDigits);
}

//Multiply two BigInts with result in the new BigInt
RealNumber* multiplyNumberWithCopy(RealNumber *number1, RealNumber *number2) {
    if (number1 == NULL || number2 == NULL) {
        return NULL;
    }

    RealNumber* result = copyNumber(number1);
    if (result == NULL) {
        return NULL;
    }

    multiplyNumberWithCopy(result, number2);
    return result;
}

unsigned int LoWord(unsigned int value) {
    return value & (1 << (sizeof(unsigned int) << 2)) - 1;
}

unsigned int HiWord(unsigned int value) {
    return value >> (sizeof(unsigned int) << 2);
}