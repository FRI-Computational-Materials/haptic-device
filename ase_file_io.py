from ase.io import read
import os

def get_state_information(filename):
    """
    About num_atoms :
    It is pretty complicated to pass a tuple of tuples back, so I will save that for a future issue.
    For now, we want to let the C++ program know how many atoms we have, so that it can split the tuple we give back
    to do this, we will just return the number of atoms at the beginning of the list
    """
    try:
        our_atoms = read(filename)
    except:
        print("Error: File not found in", os.getcwd())

    return_vals = [len(our_atoms)]
    return_vals = [*position for position in our_atoms.get_positions()]
    return_vals += our_atoms.get_masses()
    return_vals += our_atoms.get_atomic_numbers()

    return return_vals

if __name__ == '__main__':
    print("This file is used for reading in files using ASE. It is not intended for individual use.")
