from ase.io import read
import os

def get_state_information(filename: string):
    try:
        our_atoms = read(filename)
    except:
        print("Error: File not found in", os.getcwd())

    return our_atoms.get_positions(), our_atoms.get_masses(), our_atoms.get_atomic_numbers()
