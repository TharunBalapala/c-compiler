// A comprehensive test file for the miniature C compiler
int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

int main() {
    int i = 0;
    int sum = 0;
    int user_input;
    
    // Loop and Arithmetic Test
    while (j < 5) {
        sum = sum + i;
        i = i + 1;
    }
    printf("The loop computed sum: %d\n", sum);

    // input/output Test
    printf("Enter a number to double: ");
    scanf("%d", &user_input);

    //function calling
    int doubled = multiply(user_input, 2);
    int final_result = add(sum, doubled);
    
    printf("Your number doubled is: %d\n", doubled);
    printf("Sum + Doubled = %d\n", final_result);

    return 0;
}
