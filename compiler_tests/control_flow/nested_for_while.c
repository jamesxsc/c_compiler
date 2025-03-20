int f() {
    int i = 0;
    int j = 0;
    int sum = 0;

    i = 0;
    while (i < 5) {
        j = 0;
        while (j < 5) {
            if (i == 2 && j == 3) {
                break;
            }
            if ((i + j) % 2 == 0) {
                sum += i + j;
            }
            j++;
        }
        i++;
    }

    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            if (j == 3) {
                continue;
            }
            sum += i * j;
            if (sum > 299) {
                break;
            }
        }
    }

    return sum;
}
