-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)
mat5 = gr.material({0.7, 0.7, 0.7}, {0.0, 0.0, 0.0}, 25)
mat6 = gr.material({0.1, 0.1, 0.1}, {0.1, 0.1, 0.1}, 25, 0.9, 1.7)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 0, -400}, 100)
scene_root:add_child(s1)
s1:set_material(mat1)

s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
scene_root:add_child(s2)
s2:set_material(mat1)

s3 = gr.nh_sphere('s3', {0, -1200, -500}, 1000)
scene_root:add_child(s3)
s3:set_material(mat2)

b1 = gr.nh_box('b1', {-200, -125, 0}, 100)
scene_root:add_child(b1)
b1:set_material(mat4)

s4 = gr.nh_sphere('s4', {-100, 25, -300}, 50)
scene_root:add_child(s4)
s4:set_material(mat3)

s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
scene_root:add_child(s5)
s5:set_material(mat1)

--b2 = gr.nh_box('b2', {-1000, -1000, -1000}, 2000)
--scene_root:add_child(b2)
--b2:set_material(mat5)

s6 = gr.nh_sphere('s6', {0, 0, 400}, 100)
scene_root:add_child(s6)
s6:set_material(mat6)

-- A small stellated dodecahedron.

require('smstdodeca')

steldodec:set_material(mat3)
scene_root:add_child(steldodec)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'secondary.png', 500, 500,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
