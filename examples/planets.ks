#!/usr/bin/env ks
""" planets.ks - basic solar system model of planets


@author: Cade Brown <cade@kscript.org>
"""

# OpenGL bindings
import gl

# NumeriX, for specific datatypes
import nx
import m

import time

# audio/video loaders
import av

# Create a basic window from GLFW (part of the 'gl' package)
window = gl.glfw.Window("basic", (980, 768))

# Create a 'gl.Shader' object. This is how we tell the computer to render 
#   objects. The constructor takes the vertex shader source, then the fragment
#   shader source.
#
# SEE: GLSL (OpenGL Shading Language) https://www.khronos.org/opengl/wiki/OpenGL_Shading_Language
shader = gl.Shader("""#version 330 core
layout(location = 0) in vec3 vXYZ;
layout(location = 1) in vec2 vUV;

out vec4 fXYZ;
out vec2 fUV;

// Projection-View-Model matrix
uniform mat4 uPVM;

void main() {
    fXYZ = uPVM * vec4(vXYZ.x, vXYZ.y, vXYZ.z, 1.0);
    fUV = vUV;
    gl_Position = fXYZ;
}
""", """#version 330 core
layout(location = 0) out vec4 oDiffuse;

in vec4 fXYZ;
in vec2 fUV;

void main() {
    float dist = distance(vec3(0, 0, 2), fXYZ.xyz);
    oDiffuse = vec4(1.0, 1.0, 1.0, 1.0) * (0.2 + 0.8 / (1 + dist * dist));
}
""")


# Make an icosphere with 'n' subdivisions
# Returns (vao, numtris)
func make_icosphere(n=0) {

    # Golden ratio, used for initial points
    t = (1 + m.sqrt(5)) / 2
    
    # Vertex points
    # x, y, z
    verts = nx.float([
        [-1, +t, 0],
        [+1, +t, 0],
        [-1, -t, 0],
        [+1, -t, 0],

        [0, -1, +t],
        [0, +1, +t],
        [0, -1, -t],
        [0, +1, -t],

        [+t, 0, -1],
        [+t, 0, +1],
        [-t, 0, -1],
        [-t, 0, +1],
    ])

    # Face indices
    # a, b, c
    idxs = nx.u32([
        [0, 11, 5],
        [0, 5, 1],
        [0, 1, 7],
        [0, 7, 10],
        [0, 10, 11],

        [1, 5, 9],
        [5, 11, 4],
        [11, 10, 2],
        [10, 7, 6],
        [7, 1, 8],
        
        [3, 9, 4],
        [3, 4, 2],
        [3, 2, 6],
        [3, 6, 8],
        [3, 8, 9],
        
        [4, 9, 5],
        [2, 4, 11],
        [6, 2, 10],
        [8, 6, 7],
        [9, 8, 1],
    ])

    # Create a 'gl.VAO' object. This is a state binding, which will capture the
    #   buffers and atttributes we create within it
    res = gl.VAO()
    res.bind()

    # Create vertex and index data
    vbo = gl.VBO(verts)
    ebo = gl.EBO(idxs)

    # vec3 xyz;
    res.attrib(0, 3, gl.FLOAT, gl.FALSE, 3 * nx.float.size, 0)
    res.enableAttrib(0)

    # vec2 uv;
    #tri.attrib(1, 2, gl.FLOAT, gl.FALSE, 5 * nx.float.size, 3 * nx.float.size)
    #tri.enableAttrib(1)

    # We are done, so unbind it. When we re-bind it, the VBO and EBO are loaded as the current
    #   state
    res.unbind()

    ret (res, idxs.shape[0])
}


func make_quad() {
    
    # Vertex points
    # x, y, z
    verts = nx.float([
        [-1, -1, 0],
        [-1, +1, 0],
        [+1, -1, 0],
        [+1, +1, 0],
    ])

    # Face indices
    # a, b, c
    idxs = nx.u32([
        [0, 2, 1],
        [1, 3, 2],
    ])

    # Create a 'gl.VAO' object. This is a state binding, which will capture the
    #   buffers and atttributes we create within it
    res = gl.VAO()
    res.bind()

    # Create vertex and index data
    vbo = gl.VBO(verts)
    ebo = gl.EBO(idxs)

    # vec3 xyz;
    res.attrib(0, 3, gl.FLOAT, gl.FALSE, 3 * nx.float.size, 0)
    res.enableAttrib(0)

    # We are done, so unbind it. When we re-bind it, the VBO and EBO are loaded as the current
    #   state
    res.unbind()

    ret (res, idxs.shape[0])
}

#(sphere, sphere_nt) = make_quad()
(sphere, sphere_nt) = make_icosphere()



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

    # Set background color
    gl.clearColor(0.1, 0.1, 0.1)


    # Wireframe mode
    #gl.polygonMode(gl.FRONT_AND_BACK, gl.LINE)

    # Enable depth testing
    gl.enable(gl.DEPTH_TEST)


    ## Render Scene ##

    uV = gl.lookat((0, m.sin(time.time()), 5), (0, 0, 0))
    uP = gl.perspective(m.rad(90), w / h)

    # Use the shader program
    shader.use()

    # Bind and set the shader uniform
    #tex0.bind(0)
    #shader.uniform('uTex0', 0)


    # Bind the VAO 
    sphere.bind()

    # Model matrix
    uM = gl.translate(0, 0, 0)

    # Update shader
    shader.uniform('uPVM', uP @ uV @ uM)

    # Now, draw all the elements of the VAO
    # '3' is the number of datapoints, and each triangle takes 3
    gl.drawElements(gl.TRIANGLES, 3 * sphere_nt, gl.UNSIGNED_INT, 0)

    # Done with the VAO
    sphere.unbind()


    ## Finalize frame ##

    # Poll events
    gl.glfw.poll()

    # Swap buffers
    window.swap()
}

