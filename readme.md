1. mkdir build
2. cd build
3. 1. cmake .. -DPLATFORM=host -DUSE_MODERN_CPP=ON
3. 2. cmake .. -DPLATFORM=mcu -DUSE_MODERN_CPP=ON
4. cmake --build .
5. 1. openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c "program platforms/mcu/bare/SMCP.elf verify reset exit"
5. 2. ./platforms/host/run_all_tests