//
// Created by James Conway on 21/03/2025.
//

int f(int arr[5][5]);

int main() {
    int arr[5][5] = {{1, 2, 3, 4, 5},
                     {6, 7, 8, 9, 10},
                     {11, 12, 13, 14, 15},
                     {16, 17, 18, 19, 20},
                     {21, 22, 23, 24, 25}};
    return f(arr)!=20;
}
