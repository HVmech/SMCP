1. mkdir build
2. cd build
3. 1. cmake ..
3. 1. 1. -DPLATFORM=mcu или host
3. 1. 2. -DCMAKE_BUILD_TYPE=Debug или Release
3. 1. 3. -DUSE_MODERN_CPP=ON или OFF
4. cmake --build .
5. 1. openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c "program platforms/mcu/bare/SMCP.elf verify reset exit"
5. 2. ctest --test-dir ./platforms/host -V