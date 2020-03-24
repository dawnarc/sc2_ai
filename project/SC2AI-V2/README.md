# SC2 Navigation AI upgrade

Thinking 1:  
1. Split whole scene into many small cube box using a custom Actor;
2. Compute which small box that each Character is inside when Character moved;
3. Create Six Point (hexagon) around Character to check the number of pawns that around current Character.
4. When controlled role moving, calculate count of the near acotrs around the Six Point which around controlled role: check which small cube box that Six Point belonged, then get actors data from this cube box which precomputed.

Thinking 2:  
1. Split whole scene into many small hexagon box using a custom Actor;
2. Compute which small hexagon box that each Character is inside when Character moved;
3. When controlled role search next point to move, check which hexagon box neared controlled role that is empty, then move into this hexagon box.
