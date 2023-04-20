

add_executable(testLog testLog.cc)
target_link_libraries(testLog base)


add_executable(testCopyable testCopyable.cc)

add_executable(testTimestamp testTimestamp.cc)
target_link_libraries(testTimestamp base)