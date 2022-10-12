@echo off
nrfjprog --eraseall
nrfjprog -f NRF52 --program 20221011_1857_bl653_x_debug.hex

@echo Done!
nrfjprog --reset
pause
