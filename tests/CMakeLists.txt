find_package(Catch2 REQUIRED)

add_executable(test_Function test_Function.cpp)
target_link_libraries(test_Function PRIVATE Catch2::Catch2WithMain)
add_test(NAME FunctionTest COMMAND test_Function)

add_executable(test_Lifetime test_Lifetime.cpp)
target_link_libraries(test_Lifetime PRIVATE Catch2::Catch2WithMain)
add_test(NAME LifetimeTest COMMAND test_Lifetime)

add_executable(test_Overloaded test_Overloaded.cpp)
target_link_libraries(test_Overloaded PRIVATE Catch2::Catch2WithMain)
add_test(NAME OverloadedTest COMMAND test_Overloaded)

add_executable(test_RedirectCout test_RedirectCout.cpp)
target_link_libraries(test_RedirectCout PRIVATE Catch2::Catch2WithMain)
add_test(NAME RedirectCoutTest COMMAND test_RedirectCout)

add_executable(test_SmallStorage test_SmallStorage.cpp)
target_link_libraries(test_SmallStorage PRIVATE Catch2::Catch2WithMain)
add_test(NAME SmallStorageTest COMMAND test_SmallStorage)
