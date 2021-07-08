import ase
import ase.io
#import numpy
from ase.calculators.lj import LennardJones

def getValues(position):

    # Create positions array
    m=len(position)//3
    n=3
    array = [[0 for j in range(n)] for i in range(m)]
    for i in range(m):
        for j in range(n):
            array[i][j] = 0
    for i in range(len(position)//3):
        array[i][0] = position[3*i]
        array[i][1] = position[3*i+1]
        array[i][2] = position[3*i+2]

    # troubleshooting
    #print(array)

    # Set up calculator
    calc = LennardJones()
    # load atoms object
    p = ase.Atoms('Pt' + str(len(position)//3), positions=array)

    # Set Calculator
    p.set_calculator(calc)

    # Forces are the first 3N values of the list array2, and the potential energy is the final value
    forces = p.get_forces()

    array2 = []
    for i in range(len(position)//3):
        for j in range(3):
            array2.append(forces[i][j])

    array2.append(p.get_potential_energy())
    #array2.append(p.get_potential_energy())
    return array2


if __name__ == '__main__':
    getValues([1,1,1,2,2,2,3,3,3,4,4,4,5,5,5])
