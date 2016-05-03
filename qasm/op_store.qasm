.include "pimd.qinc"

vpm_write

mov.setf -, qpu_num
brr.allz -, r:master
nop
nop
nop

slave:
  brr -, r:end
  read unif             # Discard store address
  srel -, sem_master    # Signal to master
  sacq -, sem_slave     # Wait for master

master:
  # Wait for slaves
  .rep i, 7
    sacq -, sem_master
  .endr

  vdw_store unif
  
  # Signal to slaves
  .rep i, 7
    srel -, sem_slave
  .endr

end:
