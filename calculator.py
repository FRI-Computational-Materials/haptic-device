import ase
import ase.io
import tsase
import amp
import numpy

def getValues(position):

    # Create positions array
    array = numpy.zeros((len(position)/3,3))
    for i in range(len(position)/3):
        array[i][0] = position[3*i]
        array[i][1] = position[3*i+1]
        array[i][2] = position[3*i+2]

    # troubleshooting
    #print array

    # Set up calculator
    calc = amp.Amp.load('calc.amp')
    # load con file    
    p = ase.Atoms('Pt' + str(len(position)/3), positions=array)

    # Set Calculator   
    p.set_calculator(calc)

    # Forces are the first 3N values of the list array2, and the potential energy is the final value
    forces = p.get_forces()

    array2 = []
    for i in range(len(position)/3):
        for j in range(3):
            array2.append(forces[i][j])

    array2.append(p.get_potential_energy())
    #array2.append(p.get_potential_energy())
    return array2
