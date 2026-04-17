int add(int a, int b) {
    return a + b;
}
int multiply(int a, int b) {
    return a * b;
}

int main() {
    int i = 0;
    int sum1 = 0;
    int input;
    
    //loop testing
    while (i < 5) {
        sum1 = sum1 + i;
        i = i + 1;
    }
    printf("While loop computed sum: %d\n", sum1);

    int sum2 = 0;
    int k;
    for (k = 0; k < 5; k = k + 1) {
        sum2 = sum2 + k;
    }
    printf("For loop computed sum: %d\n", sum2);

    // input/output Test
    printf("Enter a number to double: ");
    scanf("%d", &input);

    //function calling
    int doubled = multiply(input, 2);
    int final_result = add(sum1, doubled);
    
    printf("Your number doubled is: %d\n", doubled);
    printf("Sum + Doubled = %d\n", final_result);

    return 0;
}
