#!/usr/bin/env ks
""" graph.ks - Generating 3D surfaces

@author: Cade Brown <cade@kscript.org>
"""

# OpenGL bindings
import gl

import nx
import m
import time

# Create a basic window from GLFW (part of the 'gl' package)
window = gl.glfw.Window("graph", (1200, 960))

# Create a 'gl.Shader' object. This is how we tell the computer to render 
#   objects. The constructor takes the vertex shader source, then the fragment
#   shader source.
#
# SEE: GLSL (OpenGL Shading Language) https://www.khronos.org/opengl/wiki/OpenGL_Shading_Language
shader = gl.Shader("""#version 330 core
layout (location = 0) in vec3 vXYZ;
layout (location = 1) in vec3 vN;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec4 vRGBA;

out vec4 fXYZ;
out vec3 fN;
out vec2 fUV;
out vec4 fRGBA;

uniform mat4 uPV;
uniform mat4 uM;

void main() {
    fXYZ = uM * vec4(vXYZ, 1.0);
    fN = (uM * vec4(vN, 0.0)).xyz;
    fUV = vUV;
    fRGBA = vRGBA;

    gl_Position = uPV * fXYZ;
}
""", """#version 330 core
layout(location = 0) out vec4 oDiffuse;

in vec4 fXYZ;
in vec3 fN;
in vec2 fUV;
in vec4 fRGBA;

void main() {
    vec3 ldir = normalize(vec3(1, -1, 0));
    float diff = max(0, -dot(ldir, normalize(fN)));

    oDiffuse = fRGBA * (0.2 + 0.8 * diff);
}
""")


v = gl.VAO()
v.bind()

N = 64
sz = 50

# Create a VBO describing the vertex data
data = nx.zeros((N * N, 3 + 3 + 2 + 4), nx.float)
ntri = (N - 1) * N * 2
idxs = nx.zeros((ntri, 3), nx.u32)


for i in range(N) {
    for j in range(N) {
        # (x, z) forms the complex plane
        x = sz * (i / (N-1) - .5) 
        z = sz * (j / (N-1) - .5)
        
        # now, calculate the result
        y = m.zeta(x + z * 1i)

        # absolute value
        y_abs = abs(y)
        # argument, 0 to 2*pi
        y_arg = m.atan2(y.re, y.im)
       
        # now, convert to color
        t = y_arg / (2 * m.pi)
        (r, g, b) = (t, 0, 1 - t)
        data[i * N + j] = [x, y_abs, z, 0, 0, 0, 0, 0, r, g, b, 1]
    }
}

pos = data[..., 0:3]
normals = data[..., 3:6]

# Adds a triangle with vertices a, b, and c
func addtri(a, b, c) {
    idxs[ct.val] = [a, b, c]
    ct.val += 1
    
    cn = nx.cross(pos[b] - pos[a], pos[c] - pos[b])
    cn /= nx.la.norm(cn.T.T)

    normals[a] += cn
    normals[b] += cn
    normals[c] += cn
}

ct = object()
ct.val = 0
for i in range(N - 1) {
    for j in range(N - 1) {
        # Quad
        (a, b, c, d) = (
            i * N + j, 
            i * N + j + 1, 
            i * N + j + N,
            i * N + j + N + 1,
        )
        
        addtri(a, b, c)
        addtri(b, d, c)
    }
}

# Now, normalize all the vertices
for i in range(N * N) {
    normals[i] /= nx.la.norm(normals[i].T.T)
}

vbo = gl.VBO(data as nx.float)

# Create an EBO describing the triangles
ebo = gl.EBO(idxs as nx.u32)

# Describe the 3 vertex attributes
v.attrib(0, 3, gl.FLOAT, gl.FALSE, 12 * nx.float.size, 0)
v.attrib(1, 3, gl.FLOAT, gl.FALSE, 12 * nx.float.size, 3 * nx.float.size)
v.attrib(2, 2, gl.FLOAT, gl.FALSE, 12 * nx.float.size, 6 * nx.float.size)
v.attrib(3, 4, gl.FLOAT, gl.FALSE, 12 * nx.float.size, 8 * nx.float.size)

# Unbind the vertex
v.unbind()



# While it should stay rendering
while window {

    ## Logic ##


    ## Render Setup ##
    
    # Query the size of the window
    (w, h) = window.size

    # Draw over the entire window
    gl.viewport(0, 0, w, h)

    # Clear the screen
    gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT)

    # Enable depth testing
    gl.enable(gl.DEPTH_TEST)


    # Set background color
    gl.clear_color(0.1, 0.1, 0.1)

    # Wireframe mode
    #gl.polygon_mode(gl.FRONT_AND_BACK, gl.LINE)

    uP = gl.perspective(m.rad(60), w / h)
    uV = gl.lookat((20, 20, 10), (0, 0, 0))


    ## Render Scene ##

    # Use the shader program
    shader.use()

    # Go ahead and set the uniform
    shader.uniform('uPV', uP @ uV)

    # First, set the transformation matrix
    T = nx.la.diag([1, 1, 1, 1])
    shader.uniform('uM', T)

    # Bind the VAO for the corresponding mesh
    v.bind()

    # Draw the correct number of datapoints
    gl.draw_elements(gl.TRIANGLES, ntri * 3, gl.UNSIGNED_INT)

    # Done with the VAO
    v.unbind()


    ## Finalize frame ##

    # Poll events
    gl.glfw.poll()

    # Swap buffers
    window.swap()
}
