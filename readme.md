mkdir build
cd build
cmake .. -DUSE_MODERN_CPP=ON
cmake --build .
openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c "program ./stm32_blink.elf verify reset exit"