WMCS006-ACG-PN-Quads

This is a PN-Quads Tesselation program build as part of the Advanced Computer Graphics course at the RUG.
It uses the tesselation shaders to implement PN-Quads.
It inputs a 4 vertex quad patch into the TCS Shader, which computes all the 16 geometry + 9 normal control points. 
The TES then evaluates them all using bezier functions and their derivaties to compute analytical normals.
These can be enabled using a check box.
The sigma value for the fake normal field approximation can be tweaked using a slider.