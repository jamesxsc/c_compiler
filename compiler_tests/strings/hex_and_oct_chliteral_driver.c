//
// Created by James on 13/03/2025.
//

char hex();
char oct();

int main() {
    return '\x1a' != hex() || '\032' != oct();
}
