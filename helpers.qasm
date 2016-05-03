.const MODEW_32_BIT, 0
.const MODEW_16_BIT_OFFSET_0, 2
.const MODEW_16_BIT_OFFSET_1, 3
.const MODEW_8_BIT_OFFSET_0, 4
.const MODEW_8_BIT_OFFSET_1, 5
.const MODEW_8_BIT_OFFSET_2, 6
.const MODEW_8_BIT_OFFSET_3, 7
.const SIZE_8_BIT, 0
.const SIZE_16_BIT, 1
.const SIZE_32_BIT, 2
.const IS_HORIZ, 1
.const NOT_HORIZ, 0
.const IS_VERT, 1
.const NOT_VERT, 0
.const IS_LANED, 1
.const NOT_LANED, 0

# MUTEX_ACQUIRE
# ~~~~~~~~~~~~~~~~~~~
.macro MUTEX_ACQUIRE
  read mutex
.endm

# MUTEX_RELEASE
# ~~~~~~~~~~~~~~~~~~~
.macro MUTEX_RELEASE
  mov mutex, -
.endm

# VPM_WRITE_SETUP
# ~~~~~~~~~~~~~~~~~~~~~
# Sets up things so writes go into the small VPM data cache.
# Once the datas been written (by outputting repeatedly to the VPM_WRITE_FIFO
# register rb48), you then call VPM_WRITE_SETUP to configure the main
# memory destination and writing pattern.
# Arguments:
#  STRIDE: 0-64 - How much to increment the ADDR after each write.
#  HORIZ: 0 or 1 - Whether the layout is horizontal (1) or vertical (0).
#  LANED: 0 or 1 - Whether the layout is laned (1) or packed (0).
#  SIZE: 0, 1, 2 - The data unit size, 8-bit (0), 16-bit(1), or 32-bit (2).
#  ADDR: 0-255 - Packed address, meaning depends on exact unit size and mode.
# See http://www.broadcom.com/docs/support/videocore/VideoCoreIV-AG100-R.pdf page 57
.const VPM_WRITE_SETUP_ID_SHIFT, 30
.const VPM_WRITE_SETUP_STRIDE_SHIFT, 12
.const VPM_WRITE_SETUP_HORIZ_SHIFT, 11
.const VPM_WRITE_SETUP_LANED_SHIFT, 10
.const VPM_WRITE_SETUP_SIZE_SHIFT, 8
.const VPM_WRITE_SETUP_ADDR_SHIFT, 0
.func VPM_WRITE_SETUP_VALUE(STRIDE, HORIZ, LANED, SIZE, ADDR)
  (0 << VPM_WRITE_SETUP_ID_SHIFT) | (STRIDE << VPM_WRITE_SETUP_STRIDE_SHIFT) | (HORIZ << VPM_WRITE_SETUP_HORIZ_SHIFT) | (LANED << VPM_WRITE_SETUP_LANED_SHIFT) | (SIZE << VPM_WRITE_SETUP_SIZE_SHIFT) | (ADDR << VPM_WRITE_SETUP_ADDR_SHIFT)
.endf
.macro VPM_WRITE_SETUP, STRIDE, HORIZ, LANED, SIZE, ADDR
  mov vw_setup, VPM_WRITE_SETUP_VALUE(STRIDE, HORIZ, LANED, SIZE, ADDR)
.endm


# VPM_READ_SETUP
# ~~~~~~~~~~~~~~~~~~~~
# Controls how values are read from the VPM data cache into the QPU.
# Arguments:
#  NUM: 0-16 - How many elements to read at a time.
#  STRIDE: 0-64 - The amount to increment the address by after each read.
#  HORIZ: 0 or 1 - Whether the layour is horizontal (1) or vertical (0).
#  LANED: 0 or 1 - Whether the layout is laned (1) or packed (0).
#  SIZE: 0, 1, 2 - The data unit size, 8-bit (0), 16-bit(1), or 32-bit (2).
#  ADDR: 0-255 - Packed address, meaning depends on exact unit size and mode.
# See http://www.broadcom.com/docs/support/videocore/VideoCoreIV-AG100-R.pdf page 58
.const VPM_READ_SETUP_ID_SHIFT, 30
.const VPM_READ_SETUP_NUM_SHIFT, 20
.const VPM_READ_SETUP_STRIDE_SHIFT, 12
.const VPM_READ_SETUP_HORIZ_SHIFT, 11
.const VPM_READ_SETUP_LANED_SHIFT, 10
.const VPM_READ_SETUP_SIZE_SHIFT, 8
.const VPM_READ_SETUP_ADDR_SHIFT, 0
.func VPM_READ_SETUP_VALUE(NUM, STRIDE, HORIZ, LANED, SIZE, ADDR)
  (0 << VPM_WRITE_SETUP_ID_SHIFT) | (NUM << VPM_READ_SETUP_NUM_SHIFT) | (STRIDE << VPM_READ_SETUP_STRIDE_SHIFT) | (HORIZ << VPM_READ_SETUP_HORIZ_SHIFT) | (LANED << VPM_READ_SETUP_LANED_SHIFT) | (SIZE << VPM_READ_SETUP_SIZE_SHIFT) | (ADDR << VPM_READ_SETUP_ADDR_SHIFT)
.endf
.macro VPM_READ_SETUP, NUM, STRIDE, HORIZ, LANED, SIZE, ADDR
  mov vr_setup, VPM_READ_SETUP_VALUE(NUM, STRIDE, HORIZ, LANED, SIZE, ADDR)
.endm

# VPM_STORE_SETUP
# ~~~~~~~~~~~~~~~~~~~
# Configures the DMA controller to transfer data from the VPM cache to main memory.
# Once the setups been done, you then need to call VPM_STORE_START to kick
# off the transfer.
# Arguments:
#  UNITS: 0-128 - Number of rows of 2D block in memory.
#  DEPTH: 0-128 - How long each row is (in bytes?).
#  HORIZ: 0 or 1 - Whether the layout is horizontal (1) or vertical (0).
#  ADDRY: The Y coordinate of the address in the VPM space to start from.
#  ADDRX: The X coordinate of the address in the VPM space to start from.
#  MODEW: 0-7 : 0 is 32-bit, 2-3 is 16-bit with offset, 4-7 is 8-bit with offset.
# See http://www.broadcom.com/docs/support/videocore/VideoCoreIV-AG100-R.pdf page 58
.const VPM_STORE_SETUP_ID_SHIFT, 30
.const VPM_STORE_SETUP_UNITS_SHIFT, 23
.const VPM_STORE_SETUP_DEPTH_SHIFT, 16
.const VPM_STORE_SETUP_HORIZ_SHIFT, 14
.const VPM_STORE_SETUP_ADDRY_SHIFT, 7
.const VPM_STORE_SETUP_ADDRX_SHIFT, 3
.const VPM_STORE_SETUP_MODEW_SHIFT, 0
.func VPM_STORE_SETUP_VALUE(UNITS, DEPTH, HORIZ, ADDRY, ADDRX, MODEW)
  (2 << VPM_WRITE_SETUP_ID_SHIFT) | (UNITS << VPM_STORE_SETUP_UNITS_SHIFT) | (DEPTH << VPM_STORE_SETUP_DEPTH_SHIFT) | (HORIZ << VPM_STORE_SETUP_HORIZ_SHIFT) | (ADDRY << VPM_STORE_SETUP_ADDRY_SHIFT) | (ADDRX << VPM_STORE_SETUP_ADDRX_SHIFT) | (MODEW << VPM_STORE_SETUP_MODEW_SHIFT)
.endf
.macro VPM_STORE_SETUP, UNITS, DEPTH, HORIZ, ADDRY, ADDRX, MODEW
  mov vw_setup, VPM_STORE_SETUP_VALUE(UNITS, DEPTH, HORIZ, ADDRY, ADDRX, MODEW)
.endm

# VPM_STORE_START
# ~~~~~~~~~~~~~~~~~~~
# Kicks off the transfer of data from the local VPM data cache to main memory.
# It will use the settings from VPM_STORE_SETUP to control the copy process.
.macro VPM_STORE_START, ADDR, OFFSET
  add vw_addr, ADDR, OFFSET
.endm

# VPM_STORE_WAIT
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Pause until the previous DMA store operation has finished.
.macro VPM_STORE_WAIT
  read vw_wait
.endm

# VPM_LOAD_SETUP
# ~~~~~~~~~~~~~~~~~~
# Initializes the settings for transfering data from main memory into the VPM cache.
# Arguments:
#  MODEW: 0-7 : 0 is 32-bit, 2-3 is 16-bit with offset, 4-7 is 8-bit with offset.
#  MPITCH: 0-15: The amount to increment the memory pointer between rows, calculated as 8*2^MPITCH bytes.
#  ROWLEN: 0-15: The number of elements in each row in main memory.
#  NROWS: 0-15: How many rows to read from memory.
#  VPITCH: 0-15: How much to increment the VPM address by after each row is loaded.
#  VERT: 0 or 1 - Whether the layout is vertical (1) or horizontal (0). Be careful, this is inverted compared to normal.
#  ADDRY: 0-64 - The Y coordinate of the address in the VPM space to start loading into.
#  ADDRX: 0-16 - The X coordinate of the address in the VPM space to start loading into.
.const VPM_LOAD_SETUP_ID_SHIFT, 31
.const VPM_LOAD_SETUP_MODEW_SHIFT, 28
.const VPM_LOAD_SETUP_MPITCH_SHIFT, 24
.const VPM_LOAD_SETUP_ROWLEN_SHIFT, 20
.const VPM_LOAD_SETUP_NROWS_SHIFT, 16
.const VPM_LOAD_SETUP_VPITCH_SHIFT, 12
.const VPM_LOAD_SETUP_VERT_SHIFT, 11
.const VPM_LOAD_SETUP_ADDRY_SHIFT, 4
.const VPM_LOAD_SETUP_ADDRX_SHIFT, 0
.func VPM_LOAD_SETUP_VALUE(MODEW, MPITCH, ROWLEN, NROWS, VPITCH, VERT, ADDRY, ADDRX)
  (1 << VPM_LOAD_SETUP_ID_SHIFT) | (MODEW << VPM_LOAD_SETUP_MODEW_SHIFT) | (MPITCH << VPM_LOAD_SETUP_MPITCH_SHIFT) | (ROWLEN << VPM_LOAD_SETUP_ROWLEN_SHIFT) | (NROWS << VPM_LOAD_SETUP_NROWS_SHIFT) | (VPITCH << VPM_LOAD_SETUP_VPITCH_SHIFT) | (VERT << VPM_LOAD_SETUP_VERT_SHIFT) | (ADDRY << VPM_LOAD_SETUP_ADDRY_SHIFT) | (ADDRX << VPM_STORE_SETUP_ADDRX_SHIFT)
.endf
.macro VPM_LOAD_SETUP, MODEW, MPITCH, ROWLEN, NROWS, VPITCH, VERT, ADDRY, ADDRX
  mov vr_setup, VPM_LOAD_SETUP_VALUE(MODEW, MPITCH, ROWLEN, NROWS, VPITCH, VERT, ADDRY, ADDRX)
.endm

# VPM_LOAD_START
# ~~~~~~~~~~~~~~~~~~~
# Kicks off the transfer of data from main memory to the local VPM data cache.
# It will use the settings from VPM_LOAD_SETUP to control the copy process.
.macro VPM_LOAD_START, ADDR, OFFSET
  add vr_addr, ADDR, OFFSET
.endm

# VPM_LOAD_WAIT
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Pause until the previous DMA load operation has finished.
.macro VPM_LOAD_WAIT
  read vr_wait
.endm

# END_PROGRAM
# ~~~~~~~~~~~
# Triggers a host interrupt to transfer control back to the main CPU.
.macro END_PROGRAM_HARD
  mov irq, 1
  nop; nop; thrend
  nop
  nop
.endm

.macro END_PROGRAM_SOFT
  nop; nop; thrend
  nop
  nop
.endm
