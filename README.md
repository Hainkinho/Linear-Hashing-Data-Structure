This was a second semester university project. We have to choose a data strcuture. I choosed the linked list data structure, because I want to challenge myselve. In the beginning the project was really hard. My problem was, that I started coding although I did't understand the data structure very good. After I figured out how to draw it by hand, the development was very straight forward. 


# Terminal Commands

### main.cpp:
reset&& g++ -Wall -Wextra -O3 -std=c++17 -pedantic-errors main.cpp && ./a.out

reset&& g++ -Wall -Wextra -O3 -std=c++17 -pedantic-errors main.cpp && valgrind ./a.out

### simpleTest.cpp:
(-DSIZE=... gibt die Bucket Size an!)
* unsigned:
reset && g++ -Wall -Wextra -O3 --std=c++17 --pedantic-errors -DPH2 -DETYPE=unsigned -DSIZE=5 simpletest.cpp -o simpletest && ./simpleTest

* string:
reset && g++ -Wall -Wextra -O3 --std=c++17 --pedantic-errors -DPH2 -DETYPE=std::string -DSIZE=5 simpletest.cpp -o simpletest && ./simpleTest

* Person:
reset && g++ -Wall -Wextra -O3 --std=c++17 --pedantic-errors -DPH2 -DETYPE=Person -DSIZE=5 simpletest.cpp -o simpletest && ./simpleTest

reset && g++ -Wall -Wextra -O3 --std=c++17 --pedantic-errors -DPH2 -DETYPE=unsigned -DSIZE=5 simpletest.cpp -o simpletest && ./simpleTest < test.txt


### bTest.cpp:
reset && g++ -Wall -Wextra -Og -g -std=c++17 -pedantic-errors -pthread -DPH2 btest.cpp -o btest && ./btest
