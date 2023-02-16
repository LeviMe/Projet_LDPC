transcript on
if {[file exists rtl_work]} {
	vdel -lib rtl_work -all
}
vlib rtl_work
vmap work rtl_work

vcom -2008 -work work {/home/levi/intelFPGA_lite/21.1/LDPC_0/encoder2.vhd}

vcom -2008 -work work {/home/levi/intelFPGA_lite/21.1/LDPC_0/LDPC_tb_1.vhd}

vsim -t 1ps -L altera -L lpm -L sgate -L altera_mf -L altera_lnsim -L cycloneive -L rtl_work -L work -voptargs="+acc"  LDPC_tb_1

add wave *
view structure
view signals
run -all
