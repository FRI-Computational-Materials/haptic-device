### ITEMS FOR TESTING

1. Damping
  A) Damp forces: FAIL
  B) Damp acceleration: FAIL
  C) Damp velocity: FAIL
  D) Damp change in position: FAIL

2. Change Haptic Movement
  A) Increase stiffness: FAIL
  B) Reduce movement of device due to forces: FAIL

3. Prevent Rapid Oscillations
  IDEAS:
    - Save last position change/velocity/acceleration for each particle.
    - If it switches to another direction too quickly, greatly reduce magnitude.
    - Try threshold damping the acceleration? It's rapid acceleration that's
    - causing the shaking.
