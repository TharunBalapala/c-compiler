int main() {
    int i = 0;
    int sum = 0;
    
    while (i < 5) {
        sum = sum + i;
        i = i + 1;
    }
      printf("The loop computed: %d\n", sum);
    return 0;
}
