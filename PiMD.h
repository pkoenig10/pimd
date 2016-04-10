#ifndef PiMD
#define PiMD

/*
 * Initial interface for use  NOTHING FINAL
 * Design considerations: OOP? < will make some things easier
 */

/*
 * void init_gpu(): 
 * returns a object to refer to the gpu
 */
void init_gpu ();
/*
 * Load_mem (); 
 * takes in a pointer to memory and size and loads to memory.
 * Also takes in a gpu handler if not oop
 * will lock memory 
 */
void load_mem ();
/*
 * Store_mem ();
 * Takes in a ptr and a handle and moves bytes 
 * from gpu to host ptr.
 * Releases memory
 */
void store_mem();

#endif
