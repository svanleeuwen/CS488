-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.5, 0.5, 0.5}, {0.1, 0.1, 0.1}, 25)
mat2 = gr.material({0.7, 0.0, 0.0}, {0.1, 0.1, 0.1}, 25)
mat3 = gr.material({0.0, 0.7, 0.0}, {0.1, 0.1, 0.1}, 25)
mat4 = gr.material({0.0, 0.0, 0.7}, {0.1, 0.1, 0.1}, 25)
mat5 = gr.material({0.7, 0.7, 0.0}, {0.1, 0.1, 0.1}, 25)
mat6 = gr.material({0.0, 0.7, 0.7}, {0.1, 0.1, 0.1}, 25)
mat7 = gr.material({0.7, 0.7, 0.7}, {0.1, 0.1, 0.1}, 25)
mat8 = gr.material({0.7, 0.35, 0.0}, {0.1, 0.1, 0.1}, 25)

scene_root = gr.node('root')

bump2 = gr.bump("bump2.jpg")
tex1 = gr.texture("blue.jpg")

game = gr.tetris('tetris')
scene_root:add_child(game)
game:scale(20, 20, 20)
game:rotate('x', 10)
game:rotate('y', -20)

border = gr.cube('border')
game:add_child(border)
border:set_material(mat1)
border:set_texture(tex1)

piece1 = gr.cube('piece1')
game:add_child(piece1)
piece1:set_material(mat2)
piece1:set_bump(bump2)

piece2 = gr.cube('piece2')
game:add_child(piece2)
piece2:set_material(mat3)
piece2:set_bump(bump2)

piece3 = gr.cube('piece3')
game:add_child(piece3)
piece3:set_material(mat4)
piece3:set_bump(bump2)

piece4 = gr.cube('piece4')
game:add_child(piece4)
piece4:set_material(mat5)
piece4:set_bump(bump2)

piece5 = gr.cube('piece5')
game:add_child(piece5)
piece5:set_material(mat6)
piece5:set_bump(bump2)

piece6 = gr.cube('piece6')
game:add_child(piece6)
piece6:set_material(mat7)
piece6:set_bump(bump2)

piece7 = gr.cube('piece7')
game:add_child(piece7)
piece7:set_material(mat8)
piece7:set_bump(bump2)

white_light = gr.light({-100.0, 300.0, 600.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, 'tetris.png', 300, 600,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
