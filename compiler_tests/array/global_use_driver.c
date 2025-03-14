int f();
void setGlobal(int);

int main() {
    setGlobal(42);
    return !(f() == 42);
}