import ase
from ase.calculators.lj import LennardJones

def getValues(position):

    # Create positions array
    array = []
    for i in range(len(position)//3):
        line = [0, 0, 0]
        line[0] = position[3*i]
        line[1] = position[3*i+1]
        line[2] = position[3*i+2]
        array.append(line)

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

# This  comes in handy when something is broken w the C API and you need to troubleshoot
if __name__ == '__main__':
    print(getValues([1,1,1,2,2,2,3,3,3,4,4,4,5,5,5]))