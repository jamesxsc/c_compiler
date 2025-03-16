int* f();

int main() {
    int* result = f();
    return !(result[0] == 10 && result[1] == 20 && result[2] == 30);
}