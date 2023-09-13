#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

int main() {
    // declaring variables
    int integer_;
    unsigned short unsigned_short_;
    signed long int signed_long_int_;
    float float_;
    double double_;

    // assigning values (defining)
    integer_ = INT_MAX;
    unsigned_short_ = USHRT_MAX;
    float_ = FLT_MAX;
    signed_long_int_ = LONG_MAX;
    double_ = DBL_MAX;

    /*
     * Printing their characteristics
     */
    fprintf(stdout, "Sizes of types (in bytes):\nint = %lu\n"
                    "unsigned short int = %lu\n"
                    "signed long = %lu\n"
                    "float = %lu\n"
                    "double = %lu\n",
            sizeof(integer_), sizeof(unsigned_short_), sizeof(signed_long_int_), sizeof(float_), sizeof(double_));
    fprintf(stdout, "Maximum values:\nint = %d\n"
                    "unsigned short int = %hu\n"
                    "signed long = %ld\n"
                    "float = %f\n"
                    "double = %lf",
            integer_, unsigned_short_, signed_long_int_, float_, double_);

    return EXIT_SUCCESS;
}
