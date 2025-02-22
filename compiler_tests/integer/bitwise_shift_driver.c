/* Created by James Conway on 22/02/2025. */

int left_shift(int x, int n);
int right_shift(int x, int n);

int main() {
    /* Test cases for left shift */
    /* 5 (101 in binary) << 2 = 20 (10100 in binary) */
    if (left_shift(5, 2) != 20) return 1;

    /* Test cases for right shift */
    /* 20 (10100 in binary) >> 2 = 5 (101 in binary) */
    if (right_shift(20, 2) != 5) return 1;

    /* All tests passed */
    return 0;
}
