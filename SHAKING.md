### ITEMS FOR TESTING

1. Damping
  A) Damp forces: IN PROGRESS
  B) Damp acceleration: SEE A)
  C) Damp velocity: IN PROGRESS
  D) Damp change in position: FAIL

2. Change Haptic Movement
  A) Increase stiffness: NOT YET TESTED
  B) Reduce movement of device due to forces: NOT YET TESTED

3. Prevent Rapid Oscillations
  IDEAS:
    - Save last position change/velocity/acceleration for each particle.
    - If it switches to another direction too quickly, greatly reduce magnitude.
    - Try threshold damping the acceleration? It's rapid acceleration that's
    - causing the shaking.

### TEST CASES FOR Damping

Having V: 0.99 causes it to be quite jerky in rearranging into optimal structure.

F: 0.01, A: 1.0
  - If velocity ~ 1 (>0.99) will randomly cluster and float off from center.

F: 0.01, V: 1.0
  - Same with acceleration - becomes very slow if below 0.8 or so.

### CURRENT OPTIMAL

6/25/19: F: 0.01, V: 0.99, A: 0.99
6/26/19: F: 0.20, V: 0.80, A: 0.99
