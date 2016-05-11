set(CATCH_SOURCES
    catch.hpp
    catch_main.cpp
)

add_executable(parsenumber_unittests
    parsenumber.cpp
    ${CATCH_SOURCES}
)
target_link_libraries(parsenumber_unittests ${GCOV_LIB})
add_test(parsenumber_unittests parsenumber_unittests)
