#include <kot/math.h>

uint64_t DivideRoundUp(uint64_t value, uint64_t divider){
    uint64_t returnValue = value / divider;
    if((value % divider) != 0){
        returnValue++;
    }
    return returnValue;
}

int8_t sgn(int64_t value) {
    if (value < 0) return -1;
    if (value > 0) return 1;
    return 0;
}

int64_t abs(int64_t value) {
    return value * ((value > 0) - (value < 0));
}

double fabs(double x) {
    if(x < 0){
        return -x;
    }else{
        return x;
    }
}

int64_t min(int64_t a, int64_t b) {
    return (a > b) ? b : a;
}

int64_t max(int64_t a, int64_t b) {
    return (a > b) ? a : b;
}

uint64_t divideRoundUp(uint64_t value, uint64_t divider) {
    uint64_t returnValue = value / divider;
    if((value % divider) != 0){
        returnValue++;
    }
    return returnValue;
}

int exponentInt(const int base, int n) {
    int i, p = base;
    for (i = 1; i < n; ++i){
        p *= base;
    }        
    return p;
}

double pow(double x, double y) {
    double result = 1.0;

    while (y != 0){
        if(y < 0){
            x = 1 / x;
            y = -y;
        }
        if((int)y % 2 == 1){
            result *= x;
        }
        x *= x;
        y /= 2;
    }

    return result;
}

double sqrt(double x) {
    if(x < 0){
        return 0;
    }

    double result = x / 2;
    double lastResult = 0;
    double epsilon = 0.000001;

    while(result != lastResult){
        lastResult = result;
        result = (result + x / result) / 2;
        if(result * result == x){
            break;
        }
        if(fabs(result - lastResult) < epsilon){
            break;
        }
    }

    return result;
}

double cos(double x) {
    double sum = 1.0;
    double term = 1.0;
    double sign = -1.0;
    int n = 2;
    
    while(fabs(term) > fabs(sum)*1e-6){
        term *= x*x/(n*(n-1));
        sum += sign*term;
        sign = -sign;
        n += 2;
    }
    
    return sum;
}

double sin(double x) {
    double sin = 0.0;
    int i, j;
    for(i = 0; i < 3; i++){
        double power = 1.0;
        int factorial = 1;
        for(j = 1; j <= 2*i+1; j++){
            power *= x;
            factorial *= j;
        }
        if(i % 2 == 0){
            sin += power / factorial;
        }else{
            sin -= power / factorial;
        }
    }
    return sin;
}