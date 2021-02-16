#!/usr/bin/env ks
""" tri.ks - basic triangle hello world for OpenGL

This hello world uses modern OpenGL (specifically, 3.3), and so it will be
  fairly fast on all hardware. OpenGL 3.3 is a good standard to program for,
  as the more efficient routines are available yet it is widely supported

OpenGL uses a global state, which is admittedly not-so-great design. So,
  this code can get a bit hard to understand and it may seem like magic.
  This is how OpenGL is. Typically, you will write your own renderer on top
  of OpenGL, which handles all this state management.

@author: Cade Brown <cade@kscript.org>
"""

# OpenGL bindings
import gl

# NumeriX, for specific datatypes
# This is part of the standard library
import nx

# Create a basic window from GLFW (part of the 'gl' package)
window = gl.glfw.Window("basic", (640, 480))

# Create a 'gl.Shader' object. This is how we tell the computer to render 
#   objects. The constructor takes the vertex shader source, then the fragment
#   shader source.
#
# SEE: GLSL (OpenGL Shading Language) https://www.khronos.org/opengl/wiki/OpenGL_Shading_Language
shader = gl.Shader("""#version 330 core
layout (location = 0) in vec3 vPos;

void main() {
    gl_Position = vec4(vPos.x, vPos.y, vPos.z, 1.0);
}
""", """#version 330 core
layout(location = 0) out vec4 oDiffuse;

void main() {
    oDiffuse = vec4(1.0, 0.5, 0.2, 1.0);
}
""")


# Create a 'gl.VAO' object. This is a state binding, which will capture the
#   buffers and atttributes we create within it
tri = gl.VAO()
tri.bind()

# Create a 'gl.VBO' object. This is a buffer on the GPU, which can hold any
#   data. We make sure to make it an array of 'nx.float', since we use 'gl.FLOAT'
#   when creating the attribute. You can upload any arbitrary bytes, though
#
# This data is an array of [x, y, z] coordinates for each vertex
#
# Due to OpenGL's global state, it is important that we create it within the 
#   'tri.bind()' and 'tri.unbind()' calls
tri_vbo = gl.VBO(nx.float([
    [-0.5, -0.5, 0.0],
    [0.5, -0.5, 0.0],
    [0.0, 0.5, 0.0],
]))

# Create a 'gl.EBO' object. This is also a buffer on the GPU, but it represents
#   indexes (in this case, the faces of triangles) within the VBO created above.
#   We make sure to use 'nx.u32', since we use 'gl.UNSIGNED_INT' when rendering
#
# Due to OpenGL's global state, it is important that we create it within the 
#   'tri.bind()' and 'tri.unbind()' calls
tri_ebo = gl.EBO(nx.u32([
    [0, 1, 2],
]))

# Now, create an attribute on the VAO. If we think of each vertex as being a struct,
#   as we do when we create the VBO, we can think of it as having attributes. Since
#   OpenGL is a graphics library, it allows vectorized types. Therefore, each attribute
#   is like an array of components. For example, our structure would be 'vec3 pos', which
#   would have 1 attribute, 3 components, of the 'float' type.
#
# The arguments are:
#   tri.attrib(index, size, type, normalize, stride, offset)
# Where:
#   * index: An integer key representing which attribute it is (start at 0, count up)
#   * size: Number of components in the vector. Should be 1, 2, 3, or 4
#   * type: The OpenGL datatype. We use 'gl.FLOAT', since we use 'nx.float' to create the array
#   * normalize: You shoudl pretty much always give 'gl.FALSE'. This is for normalizing fixed point data
#   * stride: The size of the structure. Since we have 3 floats per vertex point, it is 3 * nx.float.size
#   * offset: The offset from the beginning of the structure. There is none for the first attribute
tri.attrib(0, 3, gl.FLOAT, gl.FALSE, 3 * nx.float.size, 0)

# Make sure to enable it!
tri.enableAttrib(0)

# We are done, so unbind it. When we re-bind it, the VBO and EBO are loaded as the current
#   state
tri.unbind()


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


    ## Render Scene ##

    # Use the shader program
    shader.use()

    # Bind the VAO 
    tri.bind()
    # Now, draw all the elements of the VAO
    # '3' is the number of datapoints, and each triangle takes 3
    gl.drawElements(gl.TRIANGLES, 3, gl.UNSIGNED_INT, 0)

    # Done with the VAO
    tri.unbind()


    ## Finalize frame ##

    # Poll events
    gl.glfw.poll()

    # Swap buffers
    window.swap()
}

