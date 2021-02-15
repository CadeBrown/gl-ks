#!/usr/bin/env ks
""" basic.ks - Simple example showing the kscript OpenGL bindings


@author: Cade Brown <cade@kscript.org>
"""

# OpenGL, GLFW, etc. bindings
import gl

# NumeriX, used for efficient data transfer
import nx


# Create a basic window
window = gl.glfw.Window("basic", (640, 480))


# Create basic vertex pattern
vertex = nx.struct('vertex', [
    # Position (x, y, z)
    ('x', nx.float), ('y', nx.float), ('z', nx.float),

    # Texture coordinates (u, v)
    ('u', nx.float), ('v', nx.float),
])

# Triangle type
tri = nx.struct('tri', [
    # Three triangles (a, b, c)
    # These are an index into another array
    ('a', nx.s32), ('b', nx.s32), ('c', nx.s32),
])


# Create datapoints for the screen quad
quad_d = vertex([
    (-1, -1, 0, 0, 1),
    (-1, +1, 0, 0, 0),
    (+1, -1, 0, 1, 1),
    (+1, +1, 0, 1, 0),
])

# Indices of triangles
quad_i = tri([
    (0, 1, 2),
    (1, 3, 2),
])


# While it should stay rendering
while window {

    ## Logic



    ## Render
    
    # Query the size of the window
    (w, h) = window.size

    # Draw over the entire window
    gl.viewport(0, 0, w, h)

    # Clear the screen
    gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT)

    # Set background color
    gl.clearColor(0.1, 0.1, 0.1)


    ## Finalize frame

    # Poll events
    gl.glfw.poll()

    # Swap buffers
    window.swap()
}



