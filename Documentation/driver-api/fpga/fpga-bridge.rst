FPGA Bridge
===========

API to implement a new FPGA bridge
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* struct fpga_bridge — The FPGA Bridge structure
* struct fpga_bridge_ops — Low level Bridge driver ops
* devm_fpga_bridge_create() — Allocate and init a bridge struct
* fpga_bridge_register() — Register a bridge
* fpga_bridge_unregister() — Unregister a bridge

.. kernel-doc:: include/linux/fpga/fpga-bridge.h
   :functions: fpga_bridge

.. kernel-doc:: include/linux/fpga/fpga-bridge.h
   :functions: fpga_bridge_ops

.. kernel-doc:: drivers/fpga/fpga-bridge.c
   :functions: devm_fpga_bridge_create

.. kernel-doc:: drivers/fpga/fpga-bridge.c
   :functions: fpga_bridge_register

.. kernel-doc:: drivers/fpga/fpga-bridge.c
   :functions: fpga_bridge_unregister
