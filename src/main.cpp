#include <iostream>

#include "MMU.hpp"

int main() {
    cout << "Loading MMU";
    MMU mmu = MMU();
    if (mmu.LoadRom()) {
        cout << "... Loaded";
    } else {
        cout << "... Error!!!";
        exit(0);
    }
    cout << endl;

    cout << "Title: " << mmu.GameTitle() << endl;
}