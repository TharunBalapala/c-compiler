int main() {
    int _a = 5; // Syntax error if _a is not identifier? Actually lexer allows _. Let's do missing semicolon.
    int x = 10;
    int y = x + z; // Semantic error: z is undeclared
    return 0;
}
