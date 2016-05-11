/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

// test enum class
enum class Test {First, Second, Third};

int main()
{
    // test initializers
    int i {1};
    double d {2.0};

    // test initializer list
    int a[] {1, 2, 3, 4};

    // test nullptr
    int *pi {nullptr};

    pi = &i;

    std::cout << d << a[1] << *pi << std::endl;

    // test enum class
    Test e {Test::Second};

    if (e != Test::Third) {
        std::cout << "enum class works\n";
    }
}
