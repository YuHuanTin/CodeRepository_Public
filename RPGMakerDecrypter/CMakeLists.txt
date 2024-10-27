
add_executable(RPGMakerDecrypter main.cpp DecryptAlgo.hpp)
target_compile_options(RPGMakerDecrypter PRIVATE /fsanitize=address /Zi)
target_compile_options(RPGMakerDecrypter PRIVATE /utf-8)
